#include "LogicSystem.h"
#include "StatusGrpcClient.h"
#include "MysqlMgr.h"


#include <json/reader.h>
#include <json/json.h>
#include <json/value.h>


LogicSystem::LogicSystem() : _b_stop(false)
{
	RegisterCallBacks();
	_worker_thread = std::thread(&LogicSystem::DealMsg, this);
}

LogicSystem::~LogicSystem()
{
	_b_stop = true;
	_cv.notify_all();
	_worker_thread.join();
}

void LogicSystem::DealMsg()
{
	for (;;) {
		std::unique_lock<std::mutex> unique_lock(_mtx);
		//�ж϶���Ϊ�������������������ȴ������ͷ���
		while (_msg_que.empty() && _b_stop) _cv.wait(unique_lock);

		// �ж��Ƿ�ر�״̬���������߼�ִ������˳�
		if (_b_stop) {
			while (!_msg_que.empty()) {
				auto msg_node = _msg_que.front();
				std::cout << "recv_msg id is : " << msg_node->_recvNode->_msg_id << std::endl;
				
				auto call_back_iter = _fun_callbacks.find(msg_node->_recvNode->_msg_id);
				if (call_back_iter == _fun_callbacks.end()) {
					_msg_que.pop();
					return;
				}

				call_back_iter->second(
					msg_node->_session,
					msg_node->_recvNode->_msg_id,
					std::string(
						msg_node->_recvNode->_data,
						msg_node->_recvNode->_cur_len)
				);

				_msg_que.pop();
			}
			break;	// for(;;)
		}

		// ��������
		auto msg_node = _msg_que.front();
		std::cout << "recv_msg id is :" << msg_node->_recvNode->_msg_id << std::endl;

		auto call_back_iter = _fun_callbacks.find(msg_node->_recvNode->_msg_id);
		if (call_back_iter == _fun_callbacks.end()) {
			_msg_que.pop();
			std::cout << "msg id [" << msg_node->_recvNode->_msg_id << "] handler not found" << std::endl;
			continue;
		}

		call_back_iter->second(
			msg_node->_session,
			msg_node->_recvNode->_msg_id,
			std::string(
				msg_node->_recvNode->_data,
				msg_node->_recvNode->_cur_len)
		);
		_msg_que.pop();
	}
}

void LogicSystem::RegisterCallBacks()
{

	_fun_callbacks[MSG_CHAT_LOGIN] = [this](auto&& session, auto&& msg_id, auto&& msg_data) {
		LoginHandler(std::forward<decltype(session)>(session),
			std::forward<decltype(msg_id)>(msg_id),
			std::forward<decltype(msg_data)>(msg_data));
		};
}

void LogicSystem::LoginHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data)
{
	Json::Reader reader;
	Json::Value root;
	reader.parse(msg_data, root);
	auto uid = root["uid"].asInt();
	auto token = root["token"].asString();
	std::cout << "user login uid is  " << uid << ", user token  is "
		<< token << std::endl;

	// ��״̬��������ȡtokenƥ���Ƿ�׼ȷ
	auto rsp = StatusGrpcClient::GetInstance()->Login(uid, token);
	Json::Value rtvalue;
	Defer defer([this, &rtvalue, session]() {
		std::string return_str = rtvalue.toStyledString();
		session->Send(return_str, MSG_CHAT_LOGIN_RSP);
		});

	rtvalue["error"] = rsp.error();
	if (rsp.error() != ErrorCodes::Success) return;

	auto find_it = _users.find(uid);
	std::shared_ptr<UserInfo> user_info = nullptr;

	if (find_it == _users.end()) {
		// ���û�����ڴ��У��͵����ݿ�����
		user_info = MysqlMgr::GetInstance()->GetUser(uid);
		if (user_info == nullptr) {
			rtvalue["error"] = ErrorCodes::UidInvalid;
			return;
		}
		_users[uid] = user_info;
	}
	else {
		user_info = find_it->second;
	}

	rtvalue["uid"] = uid;
	rtvalue["token"] = rsp.token();
	rtvalue["name"] = user_info->name;
}

void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> msg)
{
	std::unique_lock<std::mutex> unique_lock(_mtx);
	_msg_que.push(msg);

	// ��0��1����֪ͨ�ź�
	if (_msg_que.size() == 1) {
		unique_lock.unlock();
		_cv.notify_one();
	}
}