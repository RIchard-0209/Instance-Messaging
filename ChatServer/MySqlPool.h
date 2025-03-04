#pragma once

//#include <memory>
//#include <atomic>
//#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "const.h"
#include "Singleton.h"

// Connector c++
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>
#include "data.h"

class sqlConnection {
public:
	sqlConnection(sql::Connection* conn, int64_t last_time) :_conn(conn), _last_time(last_time) {}

	std::unique_ptr<sql::Connection> _conn;
	int64_t _last_time;
};

class MySqlPool {
public:
	~MySqlPool();
	MySqlPool(const std::string&, const std::string&, const std::string&, const std::string&, int);

	std::unique_ptr<sqlConnection> getConnection();
	void returnConnection(std::unique_ptr<sqlConnection>);
	void checkConnection();
	void Close();

private:

	std::string _url;
	std::string _user;
	std::string _passwd;
	std::string _schema;
	int _poolSize;
	std::queue<std::unique_ptr<sqlConnection>> _pool;
	std::mutex _mtx;
	std::condition_variable _cv;
	std::atomic<bool> _stop;
	std::thread _check_thread;
};

//struct UserInfo {
//	std::string name;
//	std::string pwd;
//	int uid;
//	std::string email;
//};