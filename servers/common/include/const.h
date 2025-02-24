#ifndef CONST_H
#define CONST_H
#include <functional>
namespace ErrorCodes {
enum {
  Success = 0,
  Error_Json = 1001,      //Json解析错误
  RPCFailed = 1002,       //RPC请求错误
  VerifyExpired = 1003,   //验证码过期
  VerifyCodeErr = 1004,   //验证码错误
  UserExist = 1005,       //用户已经存在
  PasswdErr = 1006,       //密码错误
  EmailNotMatch = 1007,   //邮箱不匹配
  PasswdUpFailed = 1008,  //更新密码失败
  PasswdInvalid = 1009,   //密码更新失败
  TokenInvalid = 1010,    //Token失效
  UidInvalid = 1011,      //uid无效
};
};

class Defer {
 public:
  Defer(std::function<void()> func) : func_(func) {}
  ~Defer() { func_(); }

 private:
  std::function<void()> func_;
};

const auto MAX_LENGTH = 1024 * 2;
const auto HEAD_TOTAL_LEN = 4;
const auto HEAD_ID_LEN = 2;
const auto HEAD_DATA_LEN = 2;
const auto MAX_RECVQUE = 10000;
const auto MAX_SENDQUE = 1000;

namespace MSG_IDS {
enum {
  MSG_CHAT_LOGIN = 1005,               //用户登陆
  MSG_CHAT_LOGIN_RSP = 1006,           //用户登陆回包
  ID_SEARCH_USER_REQ = 1007,           //用户搜索请求
  ID_SEARCH_USER_RSP = 1008,           //搜索用户回包
  ID_ADD_FRIEND_REQ = 1009,            //申请添加好友请求
  ID_ADD_FRIEND_RSP = 1010,            //申请添加好友回复
  ID_NOTIFY_ADD_FRIEND_REQ = 1011,     //通知用户添加好友申请
  ID_AUTH_FRIEND_REQ = 1013,           //认证好友请求
  ID_AUTH_FRIEND_RSP = 1014,           //认证好友回复
  ID_NOTIFY_AUTH_FRIEND_REQ = 1015,    //通知用户认证好友申请
  ID_TEXT_CHAT_MSG_REQ = 1017,         //文本聊天信息请求
  ID_TEXT_CHAT_MSG_RSP = 1018,         //文本聊天信息回复
  ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019,  //通知用户文本聊天信息
};
};

const auto USERIPPREFIX = "uip_";
const auto USERTOKENPREFIX = "utoken_";
const auto IPCOUNTPREFIX = "ipcount_";
const auto USER_BASE_INFO = "ubaseinfo_";
const auto LOGIN_COUNT = "logincount";
const auto NAME_INFO = "nameinfo_";

#endif