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
		std::unique_ptr<sql::PreparedStatement> stmt(conn->_conn->prepareStatement(CALL_REG_USER));
		stmt->setString(1, name);
		stmt->setString(2, email);
		stmt->setString(3, pwd);
		// ����PreparedStatement��ֱ��֧��ע�����������������Ҫʹ�ûỰ������������������ȡ���������ֵ

		  // ִ�д洢����
		stmt->execute();
		// ����洢���������˻Ự��������������ʽ��ȡ���������ֵ�������������ִ��SELECT��ѯ����ȡ����
	   // ���磬����洢����������һ���Ự����@result���洢������������������ȡ��

		std::unique_ptr<sql::Statement> stmtRes(conn->_conn->createStatement());
		std::unique_ptr<sql::ResultSet> res(stmtRes->executeQuery(REG_USER_QUERY));
		
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
		std::unique_ptr<sql::PreparedStatement> pstmt(conn->_conn->prepareStatement(SELECT_USER_EMAIL_BY_NAME));

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
			conn->_conn->prepareStatement(UPDATE_USER_PWD_BY_NAME));

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
		std::unique_ptr<sql::PreparedStatement> pstmt(conn->_conn->prepareStatement(SELECT_USER_ALL_BY_NAME));

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

std::shared_ptr<UserInfo> MysqlDao::GetUser(int uid)
{
	auto con = pool_->getConnection();
	if (con == nullptr) return nullptr;

	Defer defer([this, &con]() {
		pool_->returnConnection(std::move(con));
		});

	try {
		// ׼��SQL���
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_conn->prepareStatement(SELECT_USER_ALL_BY_UID));
		pstmt->setInt(1, uid);

		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;

		// ���������
		while (res->next()) {
			user_ptr.reset(new UserInfo);
			user_ptr->pwd = res->getString("pwd");
			user_ptr->email = res->getString("email");
			user_ptr->name = res->getString("name");
			user_ptr->uid = uid;
			break;
		}
		return user_ptr;
	}
	catch (sql::SQLException e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return nullptr;
	}
}

