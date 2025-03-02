#include "MsgNode.h"
#include <iostream>

MsgNode::MsgNode(short max_len) : _total_len(max_len),
_cur_len(0)
{
	_data = new char[_total_len + 1]();
	_data[_total_len] = '\0';
}

MsgNode::~MsgNode()
{
	std::cout << "destruct MsgNode" << std::endl;
	delete[] _data;
}

void MsgNode::Clear()
{
	memset(_data, 0, sizeof _data);
}

RecvNode::RecvNode(short max_len, short msg_id) :
	MsgNode(max_len), _msg_id(msg_id)
{
}

SendNode::SendNode(const char* msg, short max_len, short msg_id) :
	MsgNode(max_len + HEAD_TOTAL_LEN), _msg_id(msg_id)
{
	// TODO: 可以使用结构体封装头部，避免硬编码
	// ​转换消息ID并拷贝到头部​
	short msg_id_host = boost::asio::detail::socket_ops::host_to_network_short(msg_id);
	memcpy(_data, &msg_id_host, HEAD_ID_LEN);
	// ​转换数据最大长度并拷贝
	short max_len_host = boost::asio::detail::socket_ops::host_to_network_short(max_len);
	memcpy(_data + HEAD_ID_LEN, &max_len_host, HEAD_DATA_LEN);
	// 拷贝实际消息内容
	memcpy(_data + HEAD_ID_LEN + HEAD_DATA_LEN, msg, max_len);
}