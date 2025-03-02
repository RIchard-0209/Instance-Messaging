#include "CServer.h"
#include "AsioIOServicePool.h"


CServer::CServer(boost::asio::io_context& ioc, short port):_ioc(ioc), _port(port), _acceptor(ioc, tcp::endpoint(tcp::v4(), port))
{
	std::cout << "Server start Success, listen port :" << port << std::endl;
	StartAccept();
}

CServer::~CServer()
{
	std::cout << "Server Destruct listen on port : " << _port << std::endl;
}

/// <summary>
/// Delete Session by uuid
/// </summary>
/// <param name="uuid"></param>
void CServer::ClearSession(std::string uuid)
{
	std::lock_guard<std::mutex> lock(_mtx);
	_sessions.erase(uuid);
}

/// <summary>
/// ���������߼�
/// </summary>
/// <param name="session"></param>
/// <param name="error"></param>
void CServer::HandleAccept(std::shared_ptr<CSession> session, const boost::system::error_code& error)
{
	if (!error) {
		// ���error Ϊ���򴴽�һ���µ�session
		session->Start();
		std::lock_guard<std::mutex> lock(_mtx);
		_sessions.insert(std::make_pair(session->GetUuid(), session));
	}
	else {
		std::cout << "session accept failed, error is " << error.what() << std::endl;
	}
	// ����������һ������

	StartAccept();
}

void CServer::StartAccept()
{
	auto& ioc = AsioIOServicePool::GetInstance()->GetIOService();	// �ӳ������ó�һ��ioc
	std::shared_ptr<CSession> new_session = std::make_shared<CSession>(ioc, this);

	//_acceptor.async_accept(new_session->GetSocket(), std::bind(&CServer::HandleAccpet, this, new_session, std::placeholders::_1));
	_acceptor.async_accept(
		new_session->GetSocket(),
		[this, new_session](const boost::system::error_code& error) {
			HandleAccept(new_session, error);
		});
}
