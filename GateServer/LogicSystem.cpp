#include "LogicSystem.h"
#include "HttpConnection.h"


bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> conn)
{
	if (_get_handlers.find(path) != _get_handlers.end()) {
		_get_handlers[path](conn);
		return true;
	}
	return false;
}

void LogicSystem::RegGet(std::string url, HttpHandler handler) {
	_get_handlers.insert(make_pair(url, handler));
}

LogicSystem::LogicSystem() {
	RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
		beast::ostream(connection->_response.body()) << "receive";
		});
}