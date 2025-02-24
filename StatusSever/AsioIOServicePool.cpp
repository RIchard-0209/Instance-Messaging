#include "AsioIOServicePool.h"

#include <iostream>

AsioIOServicePool::~AsioIOServicePool()
{
	// RAII ���ƣ�����ʱ�Զ�����
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
	//��Ϊ����ִ��work.reset��������iocontext��run��״̬���˳�
	//��iocontext�Ѿ����˶���д�ļ����¼��󣬻���Ҫ�ֶ�stop�÷���
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
		// Ϊÿ�� io_service ����һ�� work ����Ȼ����� work ����洢�� _works ������
		_works[i] = std::unique_ptr<Work>(new Work(_ioServices[i].get_executor()));
		_threads.emplace_back([this, i] {
			_ioServices[i].run();
			});
	}
}