#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOServicePool.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& port) : _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)) {}

void CServer::Run() {
	auto self = shared_from_this();

	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<HttpConnection> new_conn = std::make_shared<HttpConnection>(io_context);

	_acceptor.async_accept(new_conn->GetSocket(), [self, new_conn](beast::error_code ec) {
		try {
			// ����������ӣ�����������������

			if (ec) {
				self->Run();
				return;
			}

			// ���������ӣ�������HttpConnection������������
			//std::make_shared<HttpConnection>(std::move(self->_socket))->Run();
			
			new_conn->Run();

			// ��������
			self->Run();
		}
		catch (std::exception& e) {
			//std::cerr << "accept error: " << e.what() << std::endl;
		}
		});
}