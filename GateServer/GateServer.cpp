#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

#include "CServer.h"
#include "ConfigMgr.h"

int main()
{
	ConfigMgr g_config_mgr;
	std::string gate_port_str = g_config_mgr["GateServer"]["port"];
	unsigned short gate_por = atoi(gate_port_str.c_str());


	try {
		unsigned short port = static_cast<unsigned short>(8080);

		net::io_context ioc{ 1 };

		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
			if (error) {
				return;
			}
			ioc.stop();

			});
		std::make_shared<CServer>(ioc, port)->Run();
		std::cout << "GateServer is open, the port is 8080\n";
		ioc.run();
	}

	catch (std::exception& e) {
		std::cerr << "exception: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}