#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <map>
#include <mutex>
#include <iostream>

#include "CSession.h"

using boost::asio::ip::tcp;

class CServer
{
public:
	CServer(boost::asio::io_context& ioc, short port);
	~CServer();
	void ClearSession(std::string uuid);

private:
	void HandleAccept(std::shared_ptr<CSession> session,
		const boost::system::error_code& error);
	void StartAccept();

	boost::asio::io_context& _ioc;
	tcp::acceptor _acceptor;
	std::map<std::string, std::shared_ptr<CSession>> _sessions;
	std::mutex _mtx;
	short _port;
};

