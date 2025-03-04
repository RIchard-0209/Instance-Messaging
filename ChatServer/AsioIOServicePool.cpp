#include "AsioIOServicePool.h"

AsioIOServicePool::AsioIOServicePool(std::size_t size)
	: _ioServices(size), _works(size), _nextIOService(0)
{
	for (std::size_t i = 0; i < size; ++i) {
		_works[i] = std::unique_ptr<Work>(new Work(_ioServices[i].get_executor()));
		// 为每个 io_service 创建一个 work 对象，然后将这个 work 对象存储在 _works 向量中
		for (std::size_t i = 0; i < _ioServices.size(); ++i) {
			_threads.emplace_back([this, i]() {
				_ioServices[i].run();
				});
		}
	}
}


AsioIOServicePool::~AsioIOServicePool() {
	Stop();
	std::cout << "AsioIOServicePool destructed" << std::endl;
}

IOService& AsioIOServicePool::GetIOService() {
	auto& services = _ioServices[_nextIOService++];
	if (_nextIOService == _ioServices.size()) _nextIOService = 0;
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
