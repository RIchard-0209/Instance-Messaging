#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <boost/asio.hpp>

#include "const.h"
#include "ConfigMgr.h"
#include "hiredis.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"
#include "AsioIOServicePool.h"
#include "StatusServiceImpl.h"

void RunServer() {
	auto& cfg = ConfigMgr::GetInstance();

	std::string server_address(cfg["StatusServer"]["Host"] + ":" + cfg["StatusServer"]["Port"]);
	StatusServiceImpl service;

	grpc::ServerBuilder builder;

	// 监听端口和添加服务
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);

	// 构建并启动gPRC服务器
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;

	// 创建Boost::Asio的io_context
	boost::asio::io_context ioc;
	// 创建signal_set用于捕获SIGINT
	boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);

	// 设置异步等待SIGINT信号
	signals.async_wait([&server](const boost::system::error_code& error, int signal_number) {
		if (!error) {
			std::cout << "Shutting down Server..." << std::endl;
			server->Shutdown();	// 关掉服务
		}
		});

	// 单独线程运行ioc
	std::thread([&ioc]() { ioc.run(); }).detach();

	// 等待服务器关闭
	server->Wait();
	ioc.stop();
}