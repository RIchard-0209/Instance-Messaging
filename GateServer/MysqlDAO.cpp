#include "MysqlDAO.h"
#include "ConfigMgr.h"

MysqlDao::MysqlDao()
{
	auto& cfg = ConfigMgr::GetInstance();
	const auto& host = cfg["Mysql"]["Host"];
	const auto& port = cfg["Mysql"]["Port"];
	const auto& pwd = cfg["Mysql"]["Passwd"];
	const auto& schema = cfg["Mysql"]["Schema"];
	const auto& user = cfg["Mysql"]["User"];
	pool_.reset(new MySqlPool(host + ":" + port, user, pwd, schema, 5));
}

MysqlDao::~MysqlDao()
{
	pool_->Close();
}

int MysqlDao::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	auto conn = pool_->getConnection();
	try {
		if (conn == nullptr) {
			pool_->returnConnection(std::move(conn));
			return 0;	// return false;
		}

		// 准备调用存储过程
		std::unique_ptr<sql::PreparedStatement> stmt(conn->_conn->prepareStatement("CALL reg_user(?,?,?,@result)"));
		stmt->setString(1, name);
		stmt->setString(2, email);
		stmt->setString(3, pwd);
		// 由于PreparedStatement不直接支持注册输出参数，我们需要使用会话变量或其他方法来获取输出参数的值

		  // 执行存储过程
		stmt->execute();
		// 如果存储过程设置了会话变量或有其他方式获取输出参数的值，你可以在这里执行SELECT查询来获取它们
	   // 例如，如果存储过程设置了一个会话变量@result来存储输出结果，可以这样获取：
		const char* query = "SELECT @result AS result";

		std::unique_ptr<sql::Statement> stmtRes(conn->_conn->createStatement());
		std::unique_ptr<sql::ResultSet> res(stmtRes->executeQuery(query));
		
		if (res->next()) {
			int result = res->getInt("result");
			std::cout << "Result: " << result << std::endl;
			pool_->returnConnection(std::move(conn));
			return result;
		}
		pool_->returnConnection(std::move(conn));
		return -1;
	}
	catch (sql::SQLException e) {
		pool_->returnConnection(std::move(conn));
		std::cerr << "SQLException is : " << e.what() << std::endl <<
			"(MySQL error code is : " << e.getErrorCode() <<
			", SQLState is : " << e.getSQLState() << ")" << std::endl;
		return -1;
	}
}

