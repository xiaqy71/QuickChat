#ifndef MYSQLDAO_H
#define MYSQLDAO_H

#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class SqlConnection {
 public:
  SqlConnection(sql::Connection* con, int64_t lasttime);
  std::unique_ptr<sql::Connection> con_;
  int64_t last_oper_time_;
};

class MySqlPool {
 public:
  MySqlPool(const std::string& url, const std::string& user,
            const std::string& pass, const std::string& schema, int poolSize);
  void checkConnection();
  std::unique_ptr<SqlConnection> getConnection();
  void returnConnection(std::unique_ptr<SqlConnection> con);
  void Close();
  ~MySqlPool();

 private:
  std::string url_;
  std::string user_;
  std::string pass_;
  std::string schema_;
  int poolSize_;
  std::queue<std::unique_ptr<SqlConnection>> pool_;
  std::mutex mutex_;
  std::condition_variable cond_;
  std::atomic<bool> b_stop_;
  std::thread check_thread_;
};

struct UserInfo {
  std::string name;
  std::string pwd;
  int uid;
  std::string email;
};

class MysqlDao {
 public:
  MysqlDao();
  ~MysqlDao();
  int RegUser(const std::string& name, const std::string& email,
              const std::string& pwd);
  bool CheckEmail(const std::string& name, const std::string& email);
  bool UpdatePwd(const std::string& name, const std::string& newpwd);

 private:
  std::unique_ptr<MySqlPool> pool_;
};

#endif