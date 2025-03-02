#pragma once

#include "Singleton.h"
#include "string"
#include "MysqlDAO.h"


// 再次封装一层，对外使用这一层
class MysqlMgr : public Singleton<MysqlMgr> 
{
	friend class Singleton<MysqlMgr>;

public:
	~MysqlMgr();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);

	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePwd(const std::string& name, const std::string& pwd);
	bool CheckPwd(const std::string& name, const std::string& pwd, UserInfo& userInfo);
	std::shared_ptr<UserInfo> GetUser(int uid);
private:
	MysqlMgr();
	MysqlDao _dao;

};

