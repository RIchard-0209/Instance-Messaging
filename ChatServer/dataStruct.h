#pragma once
#include <string>

class CSession;

struct UserInfo {
	std::string name;
	std::string pwd;
	std::string email;
	int uid;
};


//struct CSessionMessage {
//	std::shared_ptr<CSession> session;
//	short id;
//	std::string data;
//};