#pragma once
#include "const.h"
#include "LogicSystem.h"


class HttpConnection :public std::enable_shared_from_this<HttpConnection>

{
public:
	friend class LogicSystem;
	HttpConnection(tcp::socket socket);
	void Run();


private:
	void CheckDeadline();
	void WriteResponse();
	void HandleReq();

	tcp::socket _socket;
	beast::flat_buffer _buffer{ 8192 };
	http::request<http::dynamic_body> _request;

	http::response<http::dynamic_body> _response;
	net::steady_timer deadline_{
		_socket.get_executor(), std::chrono::seconds(60)
	};
};

