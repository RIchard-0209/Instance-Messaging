#pragma once
#include <string>
#include <iostream>
#include <memory>

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"

#include "const.h"
#include "Singleton.h"
#include "ConfigMgr.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;


class RPConPool {
public:
	RPConPool(std::size_t poolsize, std::string host, std::string port);
	~RPConPool();

	std::unique_ptr<VarifyService::Stub> GetConnection();
	void returnConnection(std::unique_ptr<VarifyService::Stub> context);


private:
	std::atomic<bool> _stop;
	std::size_t _poolSize;
	std::string _host;
	std::string _port;

	// Tip:可以优化这个地方
	std::queue<std::unique_ptr<VarifyService::Stub>> _connection;
	std::condition_variable _cv;
	std::mutex _mtx;

};

class VerifyGrpcClient :public Singleton<VerifyGrpcClient>
{
	friend class Singleton<VerifyGrpcClient>;
public:

	GetVarifyRsp GetVarifyCode(std::string email) {
		ClientContext context;
		GetVarifyRsp reply;
		GetVarifyReq request;
		request.set_email(email);


		auto stub = _pool->GetConnection();
		//  Status status = stub_->GetVarifyCode(&context, request, &reply);
		Status status = stub->GetVarifyCode(&context, request, &reply);

		// Tip:这个代码感觉能优化
		if (status.ok()) {
			// 回收连接
			_pool->returnConnection(std::move(stub));
			return reply;
		}
		else {
			_pool->returnConnection(std::move(stub));
			reply.set_error(ErrorCodes::RPCFailed);
			return reply;
		}
	}

private:
	VerifyGrpcClient();

	//std::unique_ptr<VarifyService::Stub> stub_;
	std::unique_ptr<RPConPool> _pool;
};

