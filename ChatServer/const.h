#pragma once
#include <functional>


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

#define MAX_LENGTH  1024*2

#define HEAD_TOTAL_LEN 4

#define HEAD_ID_LEN 2

#define HEAD_DATA_LEN 2
#define MAX_RECVQUE  10000
#define MAX_SENDQUE 1000


enum MSG_IDS {
	MSG_CHAT_LOGIN = 1005,
	MSG_CHAT_LOGIN_RSP = 1006, 
};


