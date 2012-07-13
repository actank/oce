#ifndef __REFACTOR_H__
#define __REFACTOR_H__

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <IceUtil/IceUtil.h>
#include <IceUtil/Thread.h>
#include <Ice/Ice.h>
#include "Connection.h"

namespace xce {
namespace nio {

using namespace std;
using asio::ip::tcp;

static const int MAX_SERVER_CLUSTER = 100;
static const int POOL_TUB_SIZE = 100;



//----------------------------------------------------------------------

typedef map<Ice::Long, ConnectionPtr> ConnectionMap;

class Refactor : public IceUtil::Thread {
public:
			Refactor(short port,  int index,
					const AsioChannelFactoryPtr& factory, size_t threads = 10,
					const string& certificate = "", int max_conn = -1);
	virtual ~Refactor();

	void start();
	void stop();

	bool deliver(Ice::Long connectionId, const string& msg);
	void connectionRemove(Ice::Long connectionId);
	void connectionShutdown(Ice::Long connectionId,bool isClose);
	void connectionHandshake(Ice::Long connectionId);
//	ConnectionPtr createPlainConnection(asio::io_service& io);
//	ConnectionPtr createSslConnection(asio::io_service& io,
//			asio::ssl::context& ctx);
	ConnectionPtr getConnection(Ice::Long connectionId);
	void addConnection(const ConnectionPtr& conn);
	int connectionPoolSize();

private:
	void accept(ConnectionPtr connection, const asio::error_code& error);

	ConnectionPtr create();

	virtual void run();

	short _port;
	AsioChannelFactoryPtr _factory;
	//ConnectionPoolPtr _pool;
	string _certificate;
	size_t _threads;
	acceptor_ptr _acceptor;

	struct IosCtx {
		io_service_ptr ios;
		work_ptr work;
		context_ptr ctx;
	};
	vector<IosCtx> _iosCtxs;
	IosCtx _acceptCtx;

	/// The next io_service to use for a connection.
	size_t _next;
	IosCtx& nextIosCtx();

	//-----------------------------
	Ice::Long getConnectionId() {
		IceUtil::Mutex::Lock lock(_idMutex);
		Ice::Long cc = _beginIndex + _count++ * MAX_SERVER_CLUSTER + _index;
		//MCE_DEBUG("getConnectionId() --> CONN_ID:"<<cc);
		return cc;
	}
	int poolIndex(Ice::Long connectionId) {
		int index = (int) (((connectionId - _index - _beginIndex)
				/ MAX_SERVER_CLUSTER) % POOL_TUB_SIZE);
		MCE_DEBUG("socket pool index:"<<index);
		if (index >= 0) {
			return index;
		}
		return -1;
	}

	//-----------------------------
	//connection pool
	ConnectionMap _pool[POOL_TUB_SIZE];
	IceUtil::Mutex _idMutex;
	IceUtil::RWRecMutex _rwmutex[POOL_TUB_SIZE];

	Ice::Long _count;
	Ice::Long _beginIndex;
	int _index;
	int _max_conn;
};
typedef IceUtil::Handle<Refactor> RefactorPtr;

}
}
#endif

