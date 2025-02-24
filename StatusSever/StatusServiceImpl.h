#pragma once

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;
using message::LoginReq;
using message::LoginRsp;


struct ChatServer {
	std::string host;
	std::string port;
	std::string name;
	int con_count;
};


class StatusServiceImpl final : public StatusService::Service
{
public:
	StatusServiceImpl();
	virtual ~StatusServiceImpl() = default;

	Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
		GetChatServerRsp* reply) override;
	
	Status Login(ServerContext* context, const LoginReq* request,
		LoginRsp* reply) override;

private:
	void insertToken(int uid, std::string token);
	ChatServer getChatServer();
	std::unordered_map<std::string, ChatServer> _servers;
	std::unordered_map<int, std::string> _tokens;
	
	std::mutex _server_mtx;
	std::mutex _token_mtx;
	

	int _server_index;
};

