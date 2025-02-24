#ifndef MSGNODE_H
#define MSGNODE_H

class LogicSystem;
class MsgNode {
 public:
  MsgNode(short max_len);
  MsgNode(const MsgNode&) = delete;
  MsgNode& operator=(const MsgNode&) = delete;
  ~MsgNode();
  void Clear();

  short cur_len_;
  short total_len_;
  char* data_;
};

class RecvNode : public MsgNode {
  friend class LogicSystem;

 public:
  RecvNode(short max_len, short msg_id);

 private:
  short msg_id_;
};

class SendNode : public MsgNode {
  friend class LogicSystem;

 public:
  SendNode(const char* msg, short max_len, short msg_id);

 private:
  short msg_id_;
};

#endif