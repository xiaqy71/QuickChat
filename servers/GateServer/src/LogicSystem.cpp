#include "LogicSystem.h"
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include "ConfigMgr.h"
#include "HttpConnection.h"
#include "MysqlMgr.h"
#include "RedisMgr.h"
#include "VerifyGrpcClient.h"
#include "const.h"

void LogicSystem::RegGet(const std::string& url, HttpHandler handler) {
  get_handlers_.insert(std::make_pair(url, handler));
}

void LogicSystem::RegPost(const std::string& url, HttpHandler handler) {
  post_handlers_.insert(std::make_pair(url, handler));
}

LogicSystem::LogicSystem() {
  RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
    beast::ostream(connection->response_.body())
        << "receieve get_test req" << std::endl;
    int i = 0;
    for (auto& elem : connection->get_params_) {
      ++i;
      beast::ostream(connection->response_.body())
          << "param" << i << "key is" << elem.first;
      beast::ostream(connection->response_.body())
          << ", "
          << "value is " << elem.second << std::endl;
    }
  });

  RegPost("/get_verifycode", [](std::shared_ptr<HttpConnection> connection) {
    auto body_str =
        boost::beast::buffers_to_string(connection->request_.body().data());
    std::cout << "receive body is " << body_str << std::endl;
    connection->response_.set(http::field::content_type, "text/json");
    Json::Value root;
    Json::Reader reader;
    Json::Value src_root;
    bool parse_success = reader.parse(body_str, src_root);
    if (!parse_success) {
      std::cout << "Failed to parse JSON data!" << std::endl;
      root["error"] = ErrorCodes::Error_Json;
      std::string json_str = root.toStyledString();
      beast::ostream(connection->response_.body()) << json_str;
      return true;
    }
    auto email = src_root["email"].asString();
    GetVerifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVerifyCode(email);
    std::cout << "email is " << email << std::endl;
    root["error"] = rsp.error();
    root["email"] = src_root["email"];
    std::string json_str = root.toStyledString();
    beast::ostream(connection->response_.body()) << json_str;
    return true;
  });

  RegPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
    auto body_str =
        boost::beast::buffers_to_string(connection->request_.body().data());
    std::cout << "receive body is " << body_str << std::endl;
    connection->response_.set(http::field::content_type, "text/json");
    Json::Value root;
    Json::Reader reader;
    Json::Value src_root;
    bool parse_success = reader.parse(body_str, src_root);
    if (!parse_success) {
      std::cout << "Failed to parse JSON data !" << std::endl;
      root["error"] = ErrorCodes::Error_Json;
      std::string jsonstr = root.toStyledString();
      beast::ostream(connection->response_.body()) << jsonstr;
      return true;
    }

    auto email = src_root["email"].asString();
    auto name = src_root["user"].asString();
    auto pwd = src_root["passwd"].asString();
    auto confirm = src_root["confirm"].asString();
    // auto icon = src_root["icon"].asString();

    if (pwd != confirm) {
      std::cout << "password err " << std::endl;
      root["error"] = ErrorCodes::PasswdErr;
      std::string json_str = root.toStyledString();
      beast::ostream(connection->response_.body()) << json_str;
    }

    // 先查找redis中email对应的验证码是否合理
    std::string verify_code;
    bool b_get_verify = RedisMgr::GetInstance()->Get(
        ConfigMgr::Inst()["Redis"]["verify_prefix"] +
            src_root["email"].asString(),
        verify_code);
    if (!b_get_verify) {
      std::cout << "get verify code expired" << std::endl;
      root["error"] = ErrorCodes::VerifyExpired;
      std::string json_str = root.toStyledString();
      beast::ostream(connection->response_.body()) << json_str;
      return true;
    }

    if (verify_code != src_root["verifycode"].asString()) {
      std::cout << "verify code error" << std::endl;
      root["error"] = ErrorCodes::VerifyCodeErr;
      std::string json_str = root.toStyledString();
      beast::ostream(connection->response_.body()) << json_str;
      return true;
    }

    // 查找数据库判断用户是否存在
    int uid = MysqlMgr::GetInstance()->RegUser(name, email, pwd);
    if (uid == 0 || uid == -1) {
      std::cout << "user or email exist" << std::endl;
      root["error"] = ErrorCodes::UserExist;
      std::string json_str = root.toStyledString();
      beast::ostream(connection->response_.body()) << json_str;
      return true;
    }

    root["error"] = 0;
    root["uid"] = uid;
    root["email"] = email;
    root["user"] = name;
    root["passwd"] = pwd;
    root["confirm"] = confirm;
    root["verifycode"] = src_root["verifycode"].asString();
    std::string json_str = root.toStyledString();
    beast::ostream(connection->response_.body()) << json_str;
    return true;
  });
}

bool LogicSystem::HandleGet(const std::string& path,
                            std::shared_ptr<HttpConnection> con) {
  if (get_handlers_.find(path) == get_handlers_.end()) {
    return false;
  }
  get_handlers_[path](con);
  return true;
}

bool LogicSystem::HandlePost(const std::string& path,
                             std::shared_ptr<HttpConnection> con) {
  if (post_handlers_.find(path) == post_handlers_.end()) {
    return false;
  }
  post_handlers_[path](con);
  return true;
}

LogicSystem::~LogicSystem() {}