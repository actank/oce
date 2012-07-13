#include <iostream>
#include <vector>
#include <map>
#include <boost/utility.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <ifaddrs.h>
#include <string.h>
#include <iostream>
#include <string>
#include <math.h>
#include <map>
#include <set>
#include <IceUtil/Time.h>
#include "TaskManager.h"
#include "LogWrapper.h"
#include "Refactor.h"

#define MALLOC_CHECK_ 3

using namespace std;
using namespace MyUtil;
using namespace xce::nio;

class Server;
class Session;
class SessionFactoryI;

class Session : public AsioAction {
public:
	~Session();

	virtual void started();

	virtual void stopped();

	virtual void handle(const string& message);
};
typedef IceUtil::Handle<Session> SessionPtr;

class SessionFactoryI : public SessionFactory {
public:
	virtual AsioActionPtr create() {
		return new Session;
	}
};

class Server : public IceUtil::Mutex, public Singleton<Server> {
public:
	Server() {
		_refactor = new Refactor(8888, new SessionFactoryI, 1, "/opt/XiaoNei/etc/server.pem");
		//		_refactor = new Refactor(8888, new SessionFactoryI, 1);
	}
	virtual ~Server() {
		cout << "Server::~Server" << endl;
		_sessions.clear();
	}

	void start() {
		_refactor->start();
	}

	void stop() {
		for (SessionSet::iterator it = _sessions.begin(); it != _sessions.end(); ++it) {
			(*it)->shutdown();
		}
		_refactor->stop();
		_refactor = 0;
	}

	void join(const SessionPtr& session);

	void leave(const SessionPtr& session);
private:
	typedef set<SessionPtr> SessionSet;
	SessionSet _sessions;
	RefactorPtr _refactor;
};

