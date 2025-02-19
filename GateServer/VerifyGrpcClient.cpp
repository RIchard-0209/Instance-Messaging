#include "VerifyGrpcClient.h"

RPConPool::RPConPool(std::size_t poolsize, std::string host, std::string port)
	: _poolSize(poolsize), _host(host), _port(port), _stop(false)
{
	for (size_t i = 0; i < _poolSize; ++i)
	{
		auto channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
		//_connection.push(std::move(VarifyService::NewStub(channel)));
		_connection.push(VarifyService::NewStub(channel));
	}
}

RPConPool::~RPConPool()
{
	std::lock_guard<std::mutex> lock(_mtx);
	// Close()
	_stop = true;
	_cv.notify_all();

	while (!_connection.empty())
	{
		_connection.pop();
	}
}

std::unique_ptr<VarifyService::Stub> RPConPool::GetConnection()
{
	std::unique_lock<std::mutex> lock(_mtx);

	// false½âËø£¬µÈ´ý»½ÐÑ
	_cv.wait(lock, [this]() {
		if (_stop) return true;
		return !_connection.empty();
		});

	if (_stop) return  nullptr;
	auto context = std::move(_connection.front());
	_connection.pop();
	return context;

}

void RPConPool::returnConnection(std::unique_ptr<VarifyService::Stub> context)
{
	std::lock_guard<std::mutex> lock(_mtx);
	if (_stop) return;

	_connection.push(std::move(context));
	_cv.notify_one();
}

VerifyGrpcClient::VerifyGrpcClient()
{
	//std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
	//stub_ = VarifyService::NewStub(channel);

	auto& g_cfg_mgr = ConfigMgr::GetInstance();
	std::string host = g_cfg_mgr["VarifyServer"]["Host"];
	std::string port = g_cfg_mgr["VarifyServer"]["Port"];
	std::cout << "host is " << host << " port is " << port << std::endl;
	//_pool = std::make_unique<RPConPool>(2, host, port);
	_pool.reset(new RPConPool(5, host, port));

}
