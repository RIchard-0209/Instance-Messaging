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

	Defer defer([this, &conn]() {
		pool_->returnConnection(std::move(conn));
		});

	try {
		if (conn == nullptr) {
			//	pool_->returnConnection(std::move(conn));
			return 0;	// return false;
		}


		// ׼�����ô洢����
		std::unique_ptr<sql::PreparedStatement> stmt(conn->_conn->prepareStatement("CALL reg_user(?,?,?,@result)"));
		stmt->setString(1, name);
		stmt->setString(2, email);
		stmt->setString(3, pwd);
		// ����PreparedStatement��ֱ��֧��ע�����������������Ҫʹ�ûỰ������������������ȡ���������ֵ

		  // ִ�д洢����
		stmt->execute();
		// ����洢���������˻Ự��������������ʽ��ȡ���������ֵ�������������ִ��SELECT��ѯ����ȡ����
	   // ���磬����洢����������һ���Ự����@result���洢������������������ȡ��
		const char* query = "SELECT @result AS result";

		std::unique_ptr<sql::Statement> stmtRes(conn->_conn->createStatement());
		std::unique_ptr<sql::ResultSet> res(stmtRes->executeQuery(query));

		if (res->next()) {
			int result = res->getInt("result");
			std::cout << "Result: " << result << std::endl;
			//pool_->returnConnection(std::move(conn));
			return result;
		}
		//pool_->returnConnection(std::move(conn));
		return -1;
	}
	catch (sql::SQLException e) {
		//pool_->returnConnection(std::move(conn));
		std::cerr << "SQLException is : " << e.what() << std::endl <<
			"(MySQL error code is : " << e.getErrorCode() <<
			", SQLState is : " << e.getSQLState() << ")" << std::endl;
		return -1;
	}
}

bool MysqlDao::CheckEmail(const std::string& name, const std::string& email)
{
	auto conn = pool_->getConnection();
	Defer defer([this, &conn]() {
		pool_->returnConnection(std::move(conn));
		});

	try {
		if (conn == nullptr) return false;

		// ׼����ѯ���
		std::unique_ptr<sql::PreparedStatement> pstmt(conn->_conn->prepareStatement("SELECT email from user WHERE name = ?"));

		// �󶨲���
		pstmt->setString(1, name);

		// ִ��
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

		while (res->next()) {
			std::cout << "Check Email�� " << res->getString("email") << std::endl;
			if (email != res->getString("email")) return false;
			return true;
		}

		return false;
	}
	catch (sql::SQLException& e) {

		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::UpdatePwd(const std::string& name, const std::string& pwd)
{
	auto conn = pool_->getConnection();
	Defer defer([this, &conn]() {
		pool_->returnConnection(std::move(conn));
		});

	try {
		if (conn == nullptr) {
			return false;
		}

		// ׼���������
		std::unique_ptr<sql::PreparedStatement> pstmt(
			conn->_conn->prepareStatement("UPDATE user SET pwd = ? where name = ?"));

		// �󶨲���
		pstmt->setString(1, pwd);
		pstmt->setString(2, name);

		// ִ��
		int updateCount = pstmt->executeUpdate();

		std::cout << "Update rows: " << updateCount << std::endl;

		return true;
	}
	catch (sql::SQLException& e) {

		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::CheckPwd(const std::string& name, const std::string& pwd, UserInfo& userInfo)
{
	auto conn = pool_->getConnection();

	Defer defer([this, &conn]() {
		pool_->returnConnection(std::move(conn));
		});

	try {
		if (conn == nullptr) {
			return false;
		}

		// ׼����ѯ���
		std::unique_ptr<sql::PreparedStatement> pstmt(conn->_conn->prepareStatement("SELECT * from user WHERE name = ?"));

		// �󶨲���
		pstmt->setString(1, name);

		// ִ��
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::string origin_pwd = "";

		while (res->next()) {
			origin_pwd = res->getString("pwd");
			// �ҵ��ĵ�һ������
			std::cout << "Password: " << origin_pwd << std::endl;
			break;
		}
		if (pwd != origin_pwd) return false;
		userInfo.name = name;
		userInfo.email = res->getString("email");
		userInfo.uid = res->getInt("uid");
		userInfo.pwd = origin_pwd;

		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

