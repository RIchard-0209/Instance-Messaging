#include "CServer.h"
#include "HttpConnection.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& port) :
	_ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)), _socket(ioc) {
}

void CServer::Run() {
	auto self = shared_from_this();
	_acceptor.async_accept(_socket, [self](beast::error_code ec) {
		try {
			// ����������ӣ�����������������

			if (ec) {
				self->Run();
				return;
			}

			// ���������ӣ�������HttpConnection������������
			std::make_shared<HttpConnection>(std::move(self->_socket))->Run();

			// ��������
			self->Run();
		}
		catch (std::exception& e) {
			//std::cerr << "accept error: " << e.what() << std::endl;
		}
		});
}