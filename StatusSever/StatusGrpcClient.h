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

	// �����˿ں���ӷ���
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);

	// ����������gPRC������
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;

	// ����Boost::Asio��io_context
	boost::asio::io_context ioc;
	// ����signal_set���ڲ���SIGINT
	boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);

	// �����첽�ȴ�SIGINT�ź�
	signals.async_wait([&server](const boost::system::error_code& error, int signal_number) {
		if (!error) {
			std::cout << "Shutting down Server..." << std::endl;
			server->Shutdown();	// �ص�����
		}
		});

	// �����߳�����ioc
	std::thread([&ioc]() { ioc.run(); }).detach();

	// �ȴ��������ر�
	server->Wait();
	ioc.stop();
}