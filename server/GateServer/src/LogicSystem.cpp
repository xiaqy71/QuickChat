#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"
#include "const.h"
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>

void LogicSystem::RegGet(const std::string& url, HttpHandler handler)
{
    get_handlers_.insert(std::make_pair(url, handler));
}

void LogicSystem::RegPost(const std::string& url, HttpHandler handler)
{
    post_handlers_.insert(std::make_pair(url, handler));
}

LogicSystem::LogicSystem()
{
    RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->response_.body()) << "receieve get_test req" << std::endl;
        int i = 0;
        for (auto& elem : connection->get_params_) {
            ++i;
            beast::ostream(connection->response_.body()) << "param" << i << "key is" << elem.first;
            beast::ostream(connection->response_.body()) << ", "
                                                         << "value is " << elem.second << std::endl;
        }
    });

    RegPost("/get_verifycode", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->request_.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->response_.set(http::field::content_type, "text/json");
        Json::Value  root;
        Json::Reader reader;
        Json::Value  src_root;
        bool         parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"]        = ErrorCodes::Error_Json;
            std::string json_str = root.toStyledString();
            beast::ostream(connection->response_.body()) << json_str;
            return true;
        }
        auto         email = src_root["email"].asString();
        GetVerifyRsp rsp   = VerifyGrpcClient::GetInstance()->GetVerifyCode(email);
        std::cout << "email is " << email << std::endl;
        root["error"]        = rsp.error();
        root["email"]        = src_root["email"];
        std::string json_str = root.toStyledString();
        beast::ostream(connection->response_.body()) << json_str;
        return true;
    });
}

bool LogicSystem::HandleGet(const std::string& path, std::shared_ptr<HttpConnection> con)
{
    if (get_handlers_.find(path) == get_handlers_.end()) { return false; }
    get_handlers_[path](con);
    return true;
}

bool LogicSystem::HandlePost(const std::string& path, std::shared_ptr<HttpConnection> con)
{
    if (post_handlers_.find(path) == post_handlers_.end()) { return false; }
    post_handlers_[path](con);
    return true;
}

LogicSystem::~LogicSystem() {}