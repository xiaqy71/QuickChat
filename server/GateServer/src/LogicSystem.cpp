#include "LogicSystem.h"
#include "HttpConnection.h"

void LogicSystem::RegGet(const std::string& url, HttpHandler handler)
{
    get_handlers_.insert(std::make_pair(url, handler));
}

LogicSystem::LogicSystem()
{
    RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->response_.body()) << "receieve get_test req" << std::endl;
        int i = 0;
        for (auto& elem: connection->get_params_) {
            ++i;
            beast::ostream(connection->response_.body()) << "param" << i << "key is" << elem.first;
            beast::ostream(connection->response_.body()) << ", " << "value is " << elem.second << std::endl;
        }
    });
}

bool LogicSystem::HandleGet(const std::string& path, std::shared_ptr<HttpConnection> con)
{
    if (get_handlers_.find(path) == get_handlers_.end()) { return false; }
    get_handlers_[path](con);
    return true;
}

LogicSystem::~LogicSystem() {}