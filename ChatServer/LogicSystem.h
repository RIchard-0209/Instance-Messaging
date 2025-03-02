#pragma once

#include "Singleton.h"
#include "const.h"
#include "CSession.h"
#include "dataStruct.h"

#include <map>
#include <functional>
#include <queue>
#include <unordered_map>

typedef std::function<void(std::shared_ptr<CSession>, const short& msg_id, const std::string& msg_data)> FunCallBack;

class LogicSystem : public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;

public:
	~LogicSystem();
	void PostMsgToQue(std::shared_ptr<LogicNode> msg);
private:
	LogicSystem();
	void DealMsg();
	void RegisterCallBacks();
	void LoginHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);

	std::thread _worker_thread;
	std::queue<std::shared_ptr<LogicNode>> _msg_que;
	std::mutex _mtx;
	std::condition_variable _cv;
	bool _b_stop;
	std::map<short, FunCallBack> _fun_callbacks;
	std::unordered_map<int, std::shared_ptr<UserInfo>> _users;
};

