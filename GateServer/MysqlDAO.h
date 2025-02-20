#pragma once

#include "MySqlPool.h"
#include <memory>
#include <string>


class MysqlDao
{
public:
	MysqlDao();
	~MysqlDao();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
private:
	std::unique_ptr<MySqlPool> pool_;
};

