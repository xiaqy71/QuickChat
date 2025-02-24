#include "UserMgr.h"

std::shared_ptr<CSession> UserMgr::GetSession(int uid) {
  std::lock_guard<std::mutex> lock(session_mtx_);
  auto iter = uid_to_session_.find(uid);
  if (iter == uid_to_session_.end()) {
    return nullptr;
  }
  return iter->second;
}

void UserMgr::SetUserSession(int uid, std::shared_ptr<CSession> session) {
  std::lock_guard<std::mutex> lock(session_mtx_);
  uid_to_session_[uid] = session;
}

void UserMgr::RmvUserSession(int uid) {
  auto uid_str = std::to_string(uid);

  {
    std::lock_guard<std::mutex> lock(session_mtx_);
    uid_to_session_.erase(uid);
  }
}

UserMgr::UserMgr() {}
