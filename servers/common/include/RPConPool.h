#ifndef RPCONPOOL_H
#define RPCONPOOL_H

#include <condition_variable>
#include <grpcpp/grpcpp.h>
#include <memory>
#include <mutex>
#include <queue>

using grpc::Channel;

template<class ServiceType> class RPConPool
{
    using StubType = typename ServiceType::Stub;

public:
    RPConPool(size_t poolSize, const std::string& host, const std::string& port)
        : poolSize_(poolSize)
        , host_(host)
        , port_(port)
        , b_stop_(false)
    {
        for (size_t i = 0; i < poolSize_; ++i) {
            std::shared_ptr<Channel> channel =
                grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
            connections_.emplace(ServiceType::NewStub(channel));
        }
    }

    ~RPConPool()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        Close();
        while (!connections_.empty()) { connections_.pop(); }
    }

    std::unique_ptr<StubType> getConnection()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] {
            if (b_stop_) { return true; }
            return !connections_.empty();
        });
        if (b_stop_) { return nullptr; }
        auto context = std::move(connections_.front());
        connections_.pop();
        return context;
    }

    void returnConnection(std::unique_ptr<StubType> context)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (b_stop_) { return; }
        connections_.emplace(std::move(context));
        cond_.notify_one();
    }
    void Close()
    {
        b_stop_ = true;
        cond_.notify_all();
    }

private:
    std::atomic<bool>                     b_stop_;
    std::size_t                           poolSize_;
    std::string                           host_;
    std::string                           port_;
    std::queue<std::unique_ptr<StubType>> connections_;
    std::mutex                            mutex_;
    std::condition_variable               cond_;
};

#endif