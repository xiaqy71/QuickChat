#ifndef USERMGR_H
#define USERMGR_H

#include <memory>
#include <unordered_map>
#include "Singleton.h"

class CSession;
class UserMgr : public Singleton<UserMgr> {
  friend class Singleton<UserMgr>;

 public:
  ~UserMgr() = default;
  std::shared_ptr<CSession> GetSession(int uid);
  void SetUserSession(int uid, std::shared_ptr<CSession> session);
  void RmvUserSession(int uid);

 private:
  UserMgr();
  std::mutex session_mtx_;
  std::unordered_map<int, std::shared_ptr<CSession>> uid_to_session_;
};

#endif