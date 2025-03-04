// ChatServer.cpp： main

#include "LogicSystem.h"
#include "AsioIOServicePool.h"
#include "CServer.h"
#include "ConfigMgr.h"

#include <csignal>
#include <thread>
#include <mutex>
#include <condition_variable>

std::condition_variable _cv_quit;
std::mutex _mtx_quit;

int main() {
	try {
		auto& cfg = ConfigMgr::GetInstance();
		auto asio_pool = AsioIOServicePool::GetInstance();
		boost::asio::io_context ioc;
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);	// 中断和kiil
		signals.async_wait([&ioc, asio_pool](auto, auto) {
			ioc.stop();
			asio_pool->Stop();	// 析构的时候会自动调用stop;
			//delete asio_pool;
			});
		auto port = cfg["SelfServer"]["Port"];
		CServer s(ioc, atoi(port.c_str()));
		ioc.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception is : " << e.what() << std::endl;
	}
}