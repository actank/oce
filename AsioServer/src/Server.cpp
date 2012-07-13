#include "Server.h"

using namespace std;
using namespace MyUtil;
using namespace xce::nio;

Session::~Session() {
//	cout << "Session::~Session" << endl;
}

void Session::started() {
//	cout << "Session::started -> " << endl;
	Server::instance().join(this);
}

void Session::stopped() {
//	cout << "Session::stopped - > " << endl;
	Server::instance().leave(this);
}

void Session::handle(const string& message) {
	if (rand() % 100 == 0) {
//		cout << "Session::handle -> " << message << endl;
		deliver(message);
	}
}

void Server::join(const SessionPtr& session) {
	IceUtil::Mutex::Lock lock(*this);
	_sessions.insert(session);
	cout << "Session::join -> size: " << _sessions.size() << endl;
}

void Server::leave(const SessionPtr& session) {
	IceUtil::Mutex::Lock lock(*this);
	_sessions.erase(session);
	cout << "Session::leave -> size: " << _sessions.size() << endl;
}

int main(int argc, char* argv[]) {
	MCE_INIT("test_log", "WARN");
	cout << "##################" << endl;
	Server::instance().start();
	
	while (true) {
		sleep(1);
	}
	

	sleep(5);
	Server::instance().stop();
	sleep(2);
	return 0;
}

