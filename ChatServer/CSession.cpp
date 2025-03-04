#include "CSession.h"
#include "CServer.h"
#include "LogicSystem.h"

#include <iostream>
#include <sstream>		// 使用字符串和数字数据互相转换

#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

CSession::CSession(boost::asio::io_context& ioc, CServer* server) :
	_socket(ioc), _server(server), _b_close(false), _b_head_parse(false)
{
	boost::uuids::uuid uuid = boost::uuids::random_generator()();
	_uuid = boost::uuids::to_string(uuid);
	_recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

CSession::~CSession()
{
	std::cout << "~CSession destruct" << std::endl;
}

tcp::socket& CSession::GetSocket()
{
	return _socket;
}

std::string& CSession::GetUuid()
{
	return _uuid;
}

std::shared_ptr<CSession> CSession::sharedSelf()
{
	return shared_from_this();
}

void CSession::Start()
{
	AsyncReadHead(HEAD_TOTAL_LEN);
}

void CSession::Close()
{
	_socket.close();
	_b_close = true;
}

// 没有搞懂 2025-3-2
void CSession::Send(std::string msg, short msg_id)
{
	std::lock_guard< std::mutex> lock(_send_mtx);
	int send_que_size = _send_que.size();

	if (send_que_size > MAX_SENDQUE) {
		// 如果超过最多发送队列
		std::cout << "Session: " << _uuid << " send que fulled, size is " << MAX_SENDQUE << std::endl;
		return;
	}

	// 显式转换 msg.length() 为 short
	_send_que.push(std::make_shared<SendNode>(
		msg.c_str(),
		(msg.length()),
		msg_id
	));

	// 这个是做什么用的: 如果队列非空，说明已有异步操作在处理，直接返回
	if (send_que_size > 0) return;

	auto& msg_node = _send_que.front();
	// boost::asio::async_write(
	//	_socket,
	//	boost::asio::buffer(msg_node->_data, msg_node->_total_len),
	//	[this, self = sharedSelf(), msg_node](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
	//		HandleWrite(ec, self);
	//	});
	boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->_total_len),
		std::bind(&CSession::HandleWrite, this, std::placeholders::_1, sharedSelf()));
}


// 这两个send不能使用模板吗？ 2025-3-2
// 一个使用string的函数，另一个是char*
// 先把功能实现，再说吧
// TODO
void CSession::Send(char* msg, short max_length, short msg_id)
{
	std::lock_guard< std::mutex> lock(_send_mtx);
	int send_que_size = _send_que.size();

	if (send_que_size > MAX_SENDQUE) {
		// 如果超过最多发送队列
		std::cout << "Session: " << _uuid << " send que fulled, size is " << MAX_SENDQUE << std::endl;
		return;
	}

	_send_que.push(std::make_shared<SendNode>(msg, max_length, msg_id));

	// 这个是做什么用的
	if (send_que_size > 0) return;

	auto& msg_node = _send_que.front();
	//boost::asio::async_write(
	//	_socket,
	//	boost::asio::buffer(msg_node->_data, msg_node->_total_len),
	//	[this, self = sharedSelf(), msg_node](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
	//		HandleWrite(ec, self);
	//	});
	boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->_total_len),
		std::bind(&CSession::HandleWrite, this, std::placeholders::_1, sharedSelf()));
}




void CSession::AsyncReadBody(int total_len)
{
	auto self = shared_from_this();
	asyncReadFull(total_len,
		[self, this, total_len](
			const boost::system::error_code& ec,
			std::size_t bytes_transfered
			) {
				try {
					if (ec) {
						std::cout << "handle read failed, error is : " << ec.what() << std::endl;
						Close();
						_server->ClearSession(_uuid);
						return;
					}

					if (bytes_transfered < total_len) {
						std::cout << "read length not match, read ["
							<< bytes_transfered << "] , total ["
							<< total_len << "]" << std::endl;
						Close();
						_server->ClearSession(_uuid);
						return;
					}

					memcpy(_recv_msg_node->_data, _data, bytes_transfered);
					_recv_msg_node->_cur_len += bytes_transfered;
					_recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
					std::cout << "receive data is " << _recv_msg_node->_data << std::endl;
					//此处将消息投递到逻辑队列中
					LogicSystem::GetInstance()->PostMsgToQue(
						std::make_shared<LogicNode>(shared_from_this(), _recv_msg_node)
					);
					//继续监听头部接受事件
					AsyncReadHead(HEAD_TOTAL_LEN);
				}
				catch (std::exception e) {
					std::cerr << "Exception code is " << e.what() << std::endl;
				}
		});
}

void CSession::AsyncReadHead(int total_len)
{
	auto self = sharedSelf();

	asyncReadFull(HEAD_TOTAL_LEN,
		[self, this](
			const boost::system::error_code& ec,
			std::size_t bytes_transfered
			) {
				try {
					if (ec) {
						std::cout << "handle read failed. error is " << ec.what() << std::endl;
						Close();
						_server->ClearSession(_uuid);
						return;
					}

					if (bytes_transfered < HEAD_TOTAL_LEN) {
						std::cout << "read length not match, read ["
							<< bytes_transfered << "] , total ["
							<< HEAD_TOTAL_LEN << "]" << std::endl;
						Close();
						_server->ClearSession(_uuid);
						return;
					}

					_recv_head_node->Clear();
					memcpy(_recv_head_node->_data, _data, bytes_transfered);

					// 获取头部MSG_ID数据
					short msg_id = 0;
					memcpy(&msg_id, _recv_head_node->_data, HEAD_ID_LEN);

					// 网络字节序转化为本地字节序
					msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
					std::cout << "msg_id is " << msg_id << std::endl;

					// if: id valid
					if (msg_id > MAX_LENGTH) {
						std::cout << "invalid msg_id is " << msg_id << std::endl;
						_server->ClearSession(_uuid);
						return;
					}

					short msg_len = 0;
					memcpy(&msg_len, _recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);

					//网络字节序转化为本地字节序
					msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
					std::cout << "msg_len is " << msg_len << std::endl;

					// 长度非法
					if (msg_len > MAX_LENGTH) {
						std::cout << "invalid data length is " << msg_len << std::endl;
						_server->ClearSession(_uuid);
						return;
					}

					_recv_msg_node = std::make_shared<RecvNode>(msg_len, msg_id);
					AsyncReadBody(msg_len);
				}
				catch (std::exception e) {
					std::cerr << "Exception code is " << e.what() << std::endl;
				}
		});
}

void CSession::HandleWrite(
	const boost::system::error_code& error,
	std::shared_ptr<CSession> shared_self
)
{
	try {
		if (!error) {
			std::lock_guard<std::mutex> lock(_send_mtx);

			_send_que.pop();
			if (!_send_que.empty()) {
				auto& msg_node = _send_que.front();
				//boost::asio::async_write(
				//	_socket,
				//	boost::asio::buffer(msg_node->_data, msg_node->_total_len),
				//	[this, self = sharedSelf(), msg_node](boost::system::error_code ec) {
				//		HandleWrite(ec, self);
				//	});
				boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->_total_len),
					std::bind(&CSession::HandleWrite, this, std::placeholders::_1, sharedSelf()));
			}
			else {
				std::cout << "handle write failed, error is " << error.what() << std::endl;
				Close();
				_server->ClearSession(_uuid);
			}
		}

	}
	catch (std::exception& e) {
		std::cerr << "handle write failed, error is :" << e.what() << std::endl;
		Close();
		_server->ClearSession(_uuid);
	}
}

void CSession::asyncReadFull(
	std::size_t maxLength,
	std::function<void(const boost::system::error_code&, std::size_t)> handler
)
{
	// 在 memset 前面加上 :: 时，表示显式调用全局命名空间中的 memset 函数，而不是其他可能存在的同名局部函数或类成员函数
	// 明确调用全局命名空间的 C 标准库函数，避免命名冲突
	::memset(_data, 0, MAX_LENGTH);
	asyncReadLen(0, maxLength, handler);
}

//读取指定字节数
void CSession::asyncReadLen(
	std::size_t read_len,
	std::size_t total_len,
	std::function<void(const boost::system::error_code&, std::size_t)> handler)
{
	auto self = sharedSelf();
	_socket.async_read_some(
		boost::asio::buffer(_data + read_len, total_len - read_len),
		[read_len, total_len, handler, self](
			const boost::system::error_code& ec,
			std::size_t bytesTransfered
			) {
				// 出现错误，调用回调函数
				if (ec) {
					handler(ec, read_len + bytesTransfered);
					return;
				}

				// 长度够了
				if (read_len + bytesTransfered >= total_len) {
					handler(ec, read_len + bytesTransfered);
					return;
				}

				// 都不满足，继续读取
				self->asyncReadLen(read_len + bytesTransfered, total_len, handler);
		});
}