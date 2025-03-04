#include "MySqlPool.h"


#include <chrono>

MySqlPool::~MySqlPool()
{
	std::unique_lock<std::mutex> lock(_mtx);
	while (!_pool.empty()) {
		_pool.pop();
	}
}

std::unique_ptr<sqlConnection> MySqlPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mtx);
	_cv.wait(lock, [this]() {
		if (_stop) return true;
		return !_pool.empty();
		});

	if (_stop) return nullptr;

	std::unique_ptr<sqlConnection> conn(std::move(_pool.front()));
	_pool.pop();
	return conn;
}

void MySqlPool::returnConnection(std::unique_ptr<sqlConnection> conn)
{
	std::lock_guard<std::mutex> lock(_mtx);
	if (_stop) return;
	_pool.push(std::move(conn));
	_cv.notify_one();
}

void MySqlPool::checkConnection()
{
	std::lock_guard<std::mutex> guard(_mtx);
	int size = _poolSize;

	// 获取当前时间戳
	auto currentTime = std::chrono::system_clock::now().time_since_epoch();
	// 转换为秒
	long long time_stamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();

	for (int i = 0; i < size; i++) {
		auto conn = std::move(_pool.front());
		_pool.pop();

		Defer defer([this, &conn]() {
			_pool.push(std::move(conn));
			});

		if (time_stamp - conn->_last_time < 5)  continue;

		try {
			std::unique_ptr<sql::Statement> stmt(conn->_conn->createStatement());
			stmt->executeQuery("SELECT 1");
			conn->_last_time = time_stamp;
			std::cout << "Execute timer alive query, cur is " << time_stamp << std::endl;

		}
		catch (sql::SQLException e) {
			std::cout << "Error keeping connection alive: " << e.what() << std::endl;

			// 重新创建连接并替换旧的连接
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			auto* newConn = driver->connect(_url, _user, _passwd);
			newConn->setSchema(_schema);
			conn->_conn.reset(newConn);
			conn->_last_time = time_stamp;
		}
	}
}

void MySqlPool::Close()
{
	_stop = true;
	_cv.notify_all();
}

MySqlPool::MySqlPool(const std::string& url, const std::string& user, const std::string& passwd, const std::string& schema, int poolSize) :
	_url(url), _user(user), _passwd(passwd), _schema(schema)   {
	try {
		for (int i = 0; i < poolSize; ++i) {
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			std::cout << "url is " << url << " user is " << user << " passwd is " << passwd << std::endl;
			auto* conn(driver->connect(url, user, passwd));
			conn->setSchema(schema);
			
			// 获取当前时间戳
			auto currentTime = std::chrono::system_clock::now().time_since_epoch();
			// 转换为秒
			long long time_stamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();

			_pool.push(std::make_unique<sqlConnection>(conn, time_stamp));
		}

		_check_thread = std::thread([this]() {
			while (_stop) {
				checkConnection();
				std::this_thread::sleep_for(std::chrono::seconds(60));
			}
			});

		_check_thread.detach();
	} 
	catch (sql::SQLException e) {
		std::cout << "mysql pool init failed, the problem is " << e.what() << std::endl;
	}
}
