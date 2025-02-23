#include "MysqlDao.h"
#include "ConfigMgr.h"
#include "const.h"

SqlConnection::SqlConnection(sql::Connection* con, int64_t lasttime)
    : con_(con), last_oper_time_(lasttime) {}

MySqlPool::MySqlPool(const std::string& url, const std::string& user,
                     const std::string& pass, const std::string& schema,
                     int poolSize)
    : url_(url),
      user_(user),
      pass_(pass),
      schema_(schema),
      poolSize_(poolSize),
      b_stop_(false) {
  try {
    for (int i = 0; i < poolSize_; ++i) {
      sql::mysql::MySQL_Driver* driver =
          sql::mysql::get_mysql_driver_instance();
      auto* con = driver->connect(url_, user_, pass_);
      con->setSchema(schema_);

      auto currentTime = std::chrono::system_clock::now().time_since_epoch();
      long long timestamp =
          std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
      pool_.emplace(std::make_unique<SqlConnection>(con, timestamp));
    }

    check_thread_ = std::thread([this]() {
      while (!b_stop_) {
        checkConnection();
        std::this_thread::sleep_for(std::chrono::seconds(60));
      }
    });
    check_thread_.detach();
  } catch (sql::SQLException& e) {
    std::cout << "mysql pool init failed, error is" << e.what() << std::endl;
  }
}

void MySqlPool::checkConnection() {
  std::lock_guard<std::mutex> guard(mutex_);
  int poolsize = pool_.size();
  auto currentTime = std::chrono::system_clock::now().time_since_epoch();
  long long timestamp =
      std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
  for (int i = 0; i < poolsize; ++i) {
    auto con = std::move(pool_.front());
    pool_.pop();
    Defer defer([this, &con]() { pool_.emplace(std::move(con)); });
    if (timestamp - con->last_oper_time_ < 5) {
      continue;
    }
    try {
      std::unique_ptr<sql::Statement> stmt(con->con_->createStatement());
      stmt->executeQuery("SELECT 1");
      con->last_oper_time_ = timestamp;
    } catch (sql::SQLException& e) {
      std::cout << "Error keeping connection alive: " << e.what() << std::endl;
      sql::mysql::MySQL_Driver* driver =
          sql::mysql::get_mysql_driver_instance();
      auto* newcon = driver->connect(url_, user_, pass_);
      newcon->setSchema(schema_);
      con->con_.reset(newcon);
      con->last_oper_time_ = timestamp;
    }
  }
}

std::unique_ptr<SqlConnection> MySqlPool::getConnection() {
  std::unique_lock<std::mutex> lock(mutex_);
  cond_.wait(lock, [this]() {
    if (b_stop_) {
      return false;
    }
    return !pool_.empty();
  });
  if (b_stop_) {
    return nullptr;
  }
  std::unique_ptr<SqlConnection> con(std::move(pool_.front()));
  pool_.pop();
  return con;
}

void MySqlPool::returnConnection(std::unique_ptr<SqlConnection> con) {
  std::unique_lock<std::mutex> lock(mutex_);
  if (b_stop_) {
    return;
  }
  pool_.emplace(std::move(con));
  cond_.notify_one();
}

void MySqlPool::Close() {
  b_stop_ = true;
  cond_.notify_all();
}

MySqlPool::~MySqlPool() {
  std::unique_lock<std::mutex> lock(mutex_);
  while (!pool_.empty()) {
    pool_.pop();
  }
}

MysqlDao::MysqlDao() {
  auto& cfg = ConfigMgr::Inst();
  const auto& host = cfg["Mysql"]["Host"];
  const auto& port = cfg["Mysql"]["Port"];
  const auto& pwd = cfg["Mysql"]["Passwd"];
  const auto& schema = cfg["Mysql"]["Schema"];
  const auto& user = cfg["Mysql"]["User"];
  std::cout << schema << std::endl;
  pool_.reset(new MySqlPool(host + ":" + port, user, pwd, schema, 5));
}

MysqlDao::~MysqlDao() {
  pool_->Close();
}

int MysqlDao::RegUser(const std::string& name, const std::string& email,
                      const std::string& pwd) {
  auto con = pool_->getConnection();
  try {
    if (con == nullptr) {
      return false;
    }
    std::unique_ptr<sql::PreparedStatement> stmt(
        con->con_->prepareStatement("CALL reg_user(?,?,?,@result)"));
    stmt->setString(1, name);
    stmt->setString(2, email);
    stmt->setString(3, pwd);
    stmt->execute();
    std::unique_ptr<sql::Statement> stmtResult(con->con_->createStatement());
    std::unique_ptr<sql::ResultSet> res(
        stmtResult->executeQuery("SELECT @result AS result"));
    if (res->next()) {
      int result = res->getInt("result");
      std::cout << "Result: " << result << std::endl;
      pool_->returnConnection(std::move(con));
      return result;
    }
    pool_->returnConnection(std::move(con));
    return -1;
  } catch (sql::SQLException& e) {
    pool_->returnConnection(std::move(con));
    std::cerr << "SQLException: " << e.what();
    std::cerr << " (MYSQL error code: " << e.getErrorCode();
    std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    return -1;
  }
}

bool MysqlDao::CheckEmail(const std::string& name, const std::string& email) {
  auto con = pool_->getConnection();
  try {
    if (con == nullptr) {
      pool_->returnConnection(std::move(con));
      return false;
    }

    std::unique_ptr<sql::PreparedStatement> pstmt(
        con->con_->prepareStatement("SELECT email FROM user WHERE name = ?"));
    pstmt->setString(1, name);
    std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

    while (res->next()) {
      std::cout << "Check Email: " << res->getString("email") << std::endl;
      if (email != res->getString("email")) {
        pool_->returnConnection(std::move(con));
        return false;
      }
      pool_->returnConnection(std::move(con));
      return true;
    }
  } catch (sql::SQLException& e) {
    pool_->returnConnection(std::move(con));
    std::cout << "SQLException: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    return false;
  }
  return true;
}

bool MysqlDao::UpdatePwd(const std::string& name, const std::string& newpwd) {
  auto con = pool_->getConnection();
  try {
    if (con == nullptr) {
      pool_->returnConnection(std::move(con));
      return false;
    }

    std::unique_ptr<sql::PreparedStatement> pstmt(
        con->con_->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));

    pstmt->setString(2, name);
    pstmt->setString(1, newpwd);

    int updateCount = pstmt->executeUpdate();

    std::cout << "Update rows: " << updateCount << std::endl;
    pool_->returnConnection(std::move(con));
    return true;
  } catch (sql::SQLException& e) {
    pool_->returnConnection(std::move(con));
    std::cout << "SQLException: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    return false;
  }
}

bool MysqlDao::CheckPwd(const std::string& name, const std::string& pwd,
                        UserInfo& userInfo) {
  auto con = pool_->getConnection();
  Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });

  try {
    if (con == nullptr) {
      return false;
    }

    std::unique_ptr<sql::PreparedStatement> pstmt(
        con->con_->prepareStatement("SELECT * FROM user WHERE name = ?"));
    pstmt->setString(1, name);

    std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
    std::string origin_pwd = "";
    while (res->next()) {
      origin_pwd = res->getString("pwd");
      std::cout << "Password: " << origin_pwd << std::endl;
      break;
    }

    if (pwd != origin_pwd) {
      return false;
    }
    userInfo.name = name;
    userInfo.email = res->getString("email");
    userInfo.uid = res->getInt("uid");
    userInfo.pwd = origin_pwd;
    return true;
  } catch (sql::SQLException& e) {
    std::cerr << "SQLException: " << e.what();
    std::cerr << " (MySQL error code: " << e.getErrorCode();
    std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    return false;
  }
}
