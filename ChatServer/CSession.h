#pragma once

#include "const.h"
#include "MsgNode.h"

#include <queue>
#include <mutex>
#include <memory>
#include <boost/asio.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class CServer;
class LogicSystem;

class CSession : public std::enable_shared_from_this<CSession>
{
public:
	CSession(boost::asio::io_context& ioc, CServer* server);
	~CSession();

	tcp::socket& GetSocket();
	std::string& GetUuid();
	std::shared_ptr<CSession> sharedSelf();

	void Start();
	void Close();
	void Send(char* msg, short max_length, short msg_id);
	void Send(std::string msg, short msg_id);
	void AsyncReadBody(int total_len);
	void AsyncReadHead(int total_len);

private:
	void HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self);

	void asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code&, std::size_t)> handler);
	void asyncReadLen(std::size_t read_len, std::size_t total_len,
		std::function<void(const boost::system::error_code&, std::size_t)> handler);

	tcp::socket _socket;
	std::string _uuid;
	char _data[MAX_LENGTH];
	CServer* _server;
	bool _b_close;
	bool _b_head_parse;
	std::mutex _send_mtx;
	std::queue<std::shared_ptr<SendNode>> _send_que;
	std::shared_ptr<MsgNode> _recv_head_node;
	std::shared_ptr<RecvNode> _recv_msg_node;
};

class LogicNode {
	friend class LogicSystem;

public:
	LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recvNode)
		: _session(session), _recvNode(recvNode) {
	};

private:
	std::shared_ptr<CSession> _session;
	std::shared_ptr<RecvNode> _recvNode;
};