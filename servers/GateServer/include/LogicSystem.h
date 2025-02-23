#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H

#include <functional>
#include <unordered_map>
#include "Singleton.h"

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem : public Singleton<LogicSystem> {
  friend class Singleton<LogicSystem>;

 public:
  ~LogicSystem();
  bool HandleGet(const std::string&, std::shared_ptr<HttpConnection>);
  bool HandlePost(const std::string&, std::shared_ptr<HttpConnection>);
  void RegGet(const std::string&, HttpHandler handler);
  void RegPost(const std::string&, HttpHandler handler);

 private:
  LogicSystem();
  std::unordered_map<std::string, HttpHandler> post_handlers_;
  std::unordered_map<std::string, HttpHandler> get_handlers_;
};

#endif