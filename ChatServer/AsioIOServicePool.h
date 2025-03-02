#pragma once
#include <boost/asio.hpp>
#include <vector>

#include "Singleton.h"

using IOService = boost::asio::io_context;
using Work = boost::asio::executor_work_guard<IOService::executor_type>;
using WorkPtr = std::shared_ptr<Work>;

class AsioIOServicePool : public Singleton<AsioIOServicePool>
{
	friend class Singleton<AsioIOServicePool>;

public:

	~AsioIOServicePool();
	IOService& GetIOService();
	void Stop();
	
protected:
	AsioIOServicePool(const AsioIOServicePool&) = delete;
	AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;

private:
	AsioIOServicePool(std::size_t size = std::thread::hardware_concurrency());

	std::vector<IOService> _ioServices;
	std::vector<WorkPtr> _works;
	std::vector<std::thread> _threads;
	std::size_t _nextIOService;
};

