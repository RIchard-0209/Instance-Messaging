#include "StatusGrpcClient.h"

StatusConnPool::StatusConnPool(std::size_t poolSize, std::string host, std::string port)
	: _poolSize(poolSize), _host(host), _port(port), _b_stop(false)
{
	for (std::size_t i = 0; i < _poolSize; ++i) {
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
			grpc::InsecureChannelCredentials());

		_connections.push(StatusService::NewStub(channel));
	}
}

StatusConnPool::~StatusConnPool()
{
	std::lock_guard<std::mutex> lock(_mtx);
	close();
	// 可以通过与临时队列交换，进行清空。效率O(1)
	//while (!_connections.empty()) _connections.pop();
	/// 仅交换指针
	///	交换后 _connections 将被清空，临时队列 temp 拥有原队列的所有元素。
	/// 随后 temp 在栈区销毁时自动释放内存（通过 unique_ptr 的析构函数）
	std::queue<std::unique_ptr<StatusService::Stub>> temp;
	std::swap(_connections, temp);
}

std::unique_ptr<StatusService::Stub> StatusConnPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mtx);
	_cv.wait(lock, [this]() {
		if (_b_stop) return true;
		return !_connections.empty();
		});

	if (_b_stop) return nullptr;
	
	auto context = std::move(_connections.front());
	_connections.pop();
	return context;

}

void StatusConnPool::returnConnection(std::unique_ptr<StatusService::Stub> context)
{
	std::lock_guard<std::mutex> lock(_mtx);
	if (_b_stop) return;
	_connections.push(std::move(context));
	_connections.pop();
	_cv.notify_one();
}

void StatusConnPool::close()
{
	_b_stop = true;
	_cv.notify_all();
}

GetChatServerRsp StatusGrpcClient::GetChatServer(int uid)
{
	ClientContext context;
	GetChatServerReq request;
	GetChatServerRsp reply;
	request.set_uid(uid);

	auto stub = _pool->getConnection();
	Status status = stub->GetChatServer(&context, request, &reply);
	Defer defer([&stub, this]() {
		_pool->returnConnection(std::move(stub));
		});
	if (!status.ok()) reply.set_error(ErrorCodes::RPCFailed);

	return reply;

}

LoginRsp StatusGrpcClient::Login(int uid, std::string token)
{
	ClientContext context;
	LoginRsp reply;
	LoginReq request;
	request.set_uid(uid);
	request.set_token(token);

	auto stub = _pool->getConnection();
	Status status = stub->Login(&context, request, &reply);
	Defer defer([&stub, this]() {
		_pool->returnConnection(std::move(stub));
		});
	if(!status.ok()) reply.set_error(ErrorCodes::RPCFailed);
	return reply;
}

StatusGrpcClient::StatusGrpcClient() {
	auto& gCfgMgr = ConfigMgr::GetInstance();
	std::string host = gCfgMgr["StatusServer"]["Host"];
	std::string port = gCfgMgr["StatusServer"]["Port"];
	_pool.reset(new StatusConnPool(5, host, port));
}