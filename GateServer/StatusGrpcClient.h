#pragma once

#include "const.h"
#include "Singleton.h"
#include "ConfigMgr.h"
#include <grpcpp/grpcpp.h> 
#include "message.grpc.pb.h"
#include "message.pb.h"


using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::StatusService;

class StatusConnPool {
public:
	StatusConnPool(std::size_t poolSize, std::string host, std::string port);

	~StatusConnPool();

	std::unique_ptr<StatusService::Stub> getConnection();

	void returnConnection(std::unique_ptr<StatusService::Stub> context);

	void close();

private:
	std::atomic<bool> _b_stop;
	std::size_t _poolSize;
	std::string _host;
	std::string _port;
	std::queue<std::unique_ptr<StatusService::Stub>> _connections;
	std::mutex _mtx;
	std::condition_variable _cv;

};

class StatusGrpcClient : public Singleton<StatusGrpcClient>
{
	friend class Singleton<StatusGrpcClient>;

public:
	~StatusGrpcClient() {};
	GetChatServerRsp GetChatServer(int uid);
	LoginRsp Login(int uid, std::string token);

private:
	StatusGrpcClient();
	std::unique_ptr<StatusConnPool> _pool;

};

