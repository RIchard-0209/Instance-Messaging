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
	VarifyExpired = 1003,		// 验证码过期
	VarifyCodeErr = 1004,			// 验证码错误
	UserExist = 1005,			// 用户已存在
	PasswdError = 1006,			// 密码错误
	EmailNotMatch = 1007,		// 邮箱不匹配
	PasswdUpFailed = 1008,		// 密码修改失败
	PasswdInvalid = 1009,		// 密码不合法
	TokenInvaild = 1010,		// Token违法
	UidInvalid = 1011,			// uid违法
};


// Defer 类
class Defer {
public:
	// 接受一个lambda表达式或函数指针
	Defer(std::function<void()> func) : _func(func) {}

	// 只有销毁的时候才会执行这个函数
	~Defer() { _func(); }

private:
	std::function<void()> _func;
};

#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"