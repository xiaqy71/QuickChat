#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H

#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>
#include <unordered_map>
#include "Singleton.h"
class LogicNode;
class CSession;

typedef std::function<void(std::shared_ptr<CSession>, const short& msg_id,
                           const std::string& msg_data)>
    FunCallBack;

class LogicSystem : public Singleton<LogicSystem> {
  friend class Singleton<LogicSystem>;

 public:
  ~LogicSystem();
  void PostMsgToQue(std::shared_ptr<LogicNode> msg);

 private:
  LogicSystem();
  void DealMsg();
  void RegisterCallBacks();
  void LoginHandler(std::shared_ptr<CSession> session, const short& msg_id,
                    const std::string& msg_data);
  void SearchInfo(std::shared_ptr<CSession> session, const short& msg_id,
                  const std::string& msg_data);

  std::thread worker_thread_;
  std::queue<std::shared_ptr<LogicNode>> msg_que_;
  std::mutex mutex_;
  std::condition_variable consume_;
  bool b_stop_;
  std::unordered_map<short, FunCallBack> fun_callbacks_;
};

#endif