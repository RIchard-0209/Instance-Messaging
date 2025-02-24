#pragma once

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>


#include "hiredis.h"

#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

#include <memory>
#include <iostream>
#include <functional>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Singleton.h"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http; // from <boost/beast/http.hpp>
namespace net = boost::asio; // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,
	RPCFailed = 1002,
	VarifyExpired = 1003,		// ��֤�����
	VarifyCodeErr = 1004,			// ��֤�����
	UserExist = 1005,			// �û��Ѵ���
	PasswdError = 1006,			// �������
	EmailNotMatch = 1007,		// ���䲻ƥ��
	PasswdUpFailed = 1008,		// �����޸�ʧ��
	PasswdInvalid = 1009,		// ���벻�Ϸ�
	TokenInvaild = 1010,		// TokenΥ��
	UidInvalid = 1011,			// uidΥ��
};


// Defer ��
class Defer {
public:
	// ����һ��lambda���ʽ����ָ��
	Defer(std::function<void()> func) : _func(func) {}

	// ֻ�����ٵ�ʱ��Ż�ִ���������
	~Defer() { _func(); }

private:
	std::function<void()> _func;
};

#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"