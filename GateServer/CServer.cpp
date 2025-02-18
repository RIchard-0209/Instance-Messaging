#include "CServer.h"
#include "HttpConnection.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& port) :
	_ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)), _socket(ioc) {
}

void CServer::Run() {
	auto self = shared_from_this();
	_acceptor.async_accept(_socket, [self](beast::error_code ec) {
		try {
			// 出错放弃连接，继续监听其他连接

			if (ec) {
				self->Run();
				return;
			}

			// 创建新连接，并创建HttpConnection类管理这个连接
			std::make_shared<HttpConnection>(std::move(self->_socket))->Run();

			// 继续监听
			self->Run();
		}
		catch (std::exception& e) {
			//std::cerr << "accept error: " << e.what() << std::endl;
		}
		});
}