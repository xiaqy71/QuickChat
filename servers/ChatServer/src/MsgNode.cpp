#include "MsgNode.h"
#include <boost/asio.hpp>
#include <cstring>
#include <iostream>
#include "const.h"
MsgNode::MsgNode(short max_len) : total_len_(max_len), cur_len_(0) {
  data_ = new char[total_len_ + 1];
  data_[total_len_] = '\0';
}

MsgNode::~MsgNode() {
  std::cout << "destruct MsgNode" << std::endl;
  delete[] data_;
}

void MsgNode::Clear() {
  ::memset(data_, 0, total_len_);
  cur_len_ = 0;
}

RecvNode::RecvNode(short max_len, short msg_id)
    : MsgNode(max_len), msg_id_(msg_id) {}

SendNode::SendNode(const char* msg, short max_len, short msg_id)
    : MsgNode(max_len + HEAD_TOTAL_LEN), msg_id_(msg_id) {
  short msg_id_host =
      boost::asio::detail::socket_ops::host_to_network_short(msg_id);
  memcpy(data_, &msg_id_host, HEAD_ID_LEN);

  short max_len_host =
      boost::asio::detail::socket_ops::host_to_network_short(max_len);
  memcpy(data_ + HEAD_ID_LEN, &max_len_host, HEAD_DATA_LEN);
  memcpy(data_ + HEAD_ID_LEN + HEAD_DATA_LEN, msg, max_len);
}
