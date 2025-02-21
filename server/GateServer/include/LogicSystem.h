#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H

#include "Singleton.h"
#include <functional>
#include <unordered_map>

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem : public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;

public:
    ~LogicSystem();
    bool HandleGet(const std::string&, std::shared_ptr<HttpConnection>);
    void RegGet(const std::string&, HttpHandler handler);

private:
    LogicSystem();
    std::unordered_map<std::string, HttpHandler> post_handlers_;
    std::unordered_map<std::string, HttpHandler> get_handlers_;
};

#endif