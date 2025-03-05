#pragma once

#include <boost/asio.hpp>

#include "Singleton.h"

class AsioIOServicePool : public Singleton<AsioIOServicePool> {
  friend class Singleton<AsioIOServicePool>;

 public:
  using IOService = boost::asio::io_context;
  using Work = boost::asio::executor_work_guard<IOService::executor_type>;
  using WorkPtr = std::shared_ptr<Work>;

  ~AsioIOServicePool();

  AsioIOServicePool(const AsioIOServicePool&) = delete;
  AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;

  boost::asio::io_context& GetIOService();
  void Stop();

 private:
  AsioIOServicePool(
      std::size_t size = 2 /*std::thread::hardware_concurrency()*/);

  std::vector<IOService> _ioServices;
  std::vector<WorkPtr> _works;
  std::vector<std::thread> _threads;
  std::size_t _nextIOService;  // 下一个io_service的索引
};
