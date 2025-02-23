#ifndef ASIOIOSERVICEPOOL_H
#define ASIOIOSERVICEPOOL_H

#include <boost/asio.hpp>
#include <vector>
#include "Singleton.h"

class AsioIOServicePool : public Singleton<AsioIOServicePool> {
  friend class Singleton<AsioIOServicePool>;

 public:
  using IOService = boost::asio::io_context;
  using Work = boost::asio::io_context::work;
  using WorkPtr = std::unique_ptr<Work>;
  ~AsioIOServicePool();
  AsioIOServicePool(const AsioIOServicePool&) = delete;
  AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;
  boost::asio::io_context& GetIOService();
  void Stop();

 private:
  AsioIOServicePool(std::size_t size = 2);
  std::vector<IOService> ioServices_;
  std::vector<WorkPtr> works_;
  std::vector<std::thread> threads_;
  std::size_t nextIOService_;
};

#endif