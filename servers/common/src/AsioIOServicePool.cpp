#include "AsioIOServicePool.h"
#include <iostream>

AsioIOServicePool::~AsioIOServicePool() {
  Stop();
  std::cout << "AsioIOServicePool destruct" << std::endl;
}

boost::asio::io_context& AsioIOServicePool::GetIOService() {
  auto& service = ioServices_[nextIOService_++];
  if (nextIOService_ == ioServices_.size()) {
    nextIOService_ = 0;
  }
  return service;
}

void AsioIOServicePool::Stop() {
  for (auto& work : works_) {
    work->get_io_context().stop();
    work.reset();
  }

  for (auto& t : threads_) {
    t.join();
  }
}

AsioIOServicePool::AsioIOServicePool(std::size_t size)
    : ioServices_(size), nextIOService_(0) {

  for (std::size_t i = 0; i < size; ++i) {
    works_.emplace_back(std::make_unique<Work>(ioServices_[i]));
  }

  // 遍历多个ioservice，创建多个线程，每个线程内部启动ioservice
  for (std::size_t i = 0; i < ioServices_.size(); ++i) {
    threads_.emplace_back([this, i]() { ioServices_[i].run(); });
  }
}
