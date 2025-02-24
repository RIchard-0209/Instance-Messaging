#include "StatusServiceImpl.h"
#include "const.h"
#include "ConfigMgr.h"

std::string generate_unique_string() {
	boost::uuids::uuid uuid = boost::uuids::random_generator()();
	return to_string(uuid);

}

StatusServiceImpl::StatusServiceImpl()
{
	auto& cfg = ConfigMgr::GetInstance();
	ChatServer server;
	server.port = cfg["ChatServer1"]["Port"];
	server.host = cfg["ChatServer1"]["Host"];
	server.con_count = 0;
	server.name = cfg["ChatServer1"]["Name"];
	_servers[server.name] = server;

	server.port = cfg["ChatServer2"]["Port"];
	server.host = cfg["ChatServer2"]["Host"];
	server.con_count = 0;
	server.name = cfg["ChatServer2"]["Name"];
	_servers[server.name] = server;
}

Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* reply)
{
	std::string prefix("IChat status server has received :  ");
	const auto& server = getChatServer();
	reply->set_host(server.host);
	reply->set_port(server.port);
	reply->set_error(ErrorCodes::Success);
	reply->set_token(generate_unique_string());
	insertToken(request->uid(), reply->token());
	return Status::OK;
}

Status StatusServiceImpl::Login(ServerContext* context, const LoginReq* request, LoginRsp* reply)
{
	auto uid = request->uid();
	auto token = request->token();
	std::lock_guard<std::mutex> lock(_token_mtx);
	auto it = _tokens.find(uid);
	if (it == _tokens.end()) {
		reply->set_error(ErrorCodes::UidInvalid);
		return Status::OK;
	}

	if (it->second != token) {
		reply->set_error(ErrorCodes::TokenInvaild);
		return Status::OK;
	}

	reply->set_error(ErrorCodes::Success);
	reply->set_uid(uid);
	reply->set_token(token);
	return Status::OK;
}

void StatusServiceImpl::insertToken(int uid, std::string token)
{
	std::lock_guard<std::mutex> lock(_token_mtx);
	_tokens[uid] = token;
}

ChatServer StatusServiceImpl::getChatServer()
{
	std::lock_guard<std::mutex> guard(_server_mtx);
	auto minServer = _servers.begin()->second;

	for (const auto& server : _servers) {
		if (server.second.con_count < minServer.con_count) minServer = server.second;
	}

	return minServer;
}


