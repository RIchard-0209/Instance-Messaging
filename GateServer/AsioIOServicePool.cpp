#include "AsioIOServicePool.h"

#include <iostream>

AsioIOServicePool::~AsioIOServicePool()
{
	// RAII 机制，析构时自动调用
	Stop();
	std::cout << "AsioIOServicePool destructed" << std::endl;
}

boost::asio::io_context& AsioIOServicePool::GetIOService()
{
	auto& services = _ioServices[_nextIOService++];
	if (_nextIOService == _ioServices.size()) {
		_nextIOService = 0;
	}
	return services;
}

void AsioIOServicePool::Stop()
{
	//因为仅仅执行work.reset并不能让iocontext从run的状态中退出
	//当iocontext已经绑定了读或写的监听事件后，还需要手动stop该服务。
	for (auto& work : _works) {
		work->get_executor().context().stop();
		work->reset();
	}

	for (auto& thread : _threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}

AsioIOServicePool::AsioIOServicePool(std::size_t size)
	: _ioServices(size), _works(size), _nextIOService(0)
{
	for (std::size_t i = 0; i < size; ++i) {
		// 为每个 io_service 创建一个 work 对象，然后将这个 work 对象存储在 _works 向量中
		_works[i] = std::unique_ptr<Work>(new Work(_ioServices[i].get_executor()));
		_threads.emplace_back([this, i] {
			_ioServices[i].run();
			});
	}
}