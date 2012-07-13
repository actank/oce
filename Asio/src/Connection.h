#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <list>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "asio.hpp"
#include "asio/ssl.hpp"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "LogWrapper.h"
#include "TaskManager.h"
#include "Singleton.h"


namespace xce {
namespace nio {

using namespace std;
using asio::ip::tcp;


class Connection;
class AsioChannel;
class AsioAction;
class Refactor;


typedef asio::ssl::stream<asio::ip::tcp::socket> ssl_socket;
typedef boost::shared_ptr<ssl_socket> ssl_socket_ptr;
typedef boost::shared_ptr<asio::io_service::strand> strand_ptr;
typedef boost::shared_ptr<asio::io_service> io_service_ptr;
typedef boost::shared_ptr<asio::io_service::work> work_ptr;
typedef boost::shared_ptr<asio::ssl::context> context_ptr;
typedef boost::shared_ptr<asio::ip::tcp::acceptor> acceptor_ptr;
typedef boost::shared_ptr<Connection> ConnectionPtr;
typedef IceUtil::Handle<AsioChannel> AsioChannelPtr;
typedef IceUtil::Handle<AsioAction> AsioActionPtr;
typedef IceUtil::Handle<Refactor> RefactorPtr;



enum AsioState {
	ASIO_STARTED,
	ASIO_STOPPED,
	ASIO_PLAIN,
	ASIO_HANDSHAKE,
	ASIO_TLS,
	ASIO_MSG,
	ASIO_SHUTDOWN
};

//----------------------------------------------------------------------
class Connection {
public:
	Connection(Ice::Long connectionId, AsioChannelPtr& channel);
	virtual ~Connection();

	virtual asio::ip::tcp::socket& socket() = 0;

	void start(ConnectionPtr& conn);
	void stop(bool isClose);

	virtual void handshake();

	bool handle(const string& message);
	bool deliver(const string& message);
	//string getRemoteAddr();
	virtual void hookRead() = 0;
	virtual void hookWrite() = 0;

	void handleRead(const asio::error_code& err, size_t length);
	void handleWrite(const asio::error_code& err);
	AsioChannelPtr channel(){
		return _channel;
	};

	Ice::Long id(){
		return _id;
	}
protected:
	IceUtil::Mutex _mutex;
	const static size_t BUFFER_SIZE = 512;
	char _incoming[BUFFER_SIZE];
	deque<string> _outgoing;
	size_t _counter;

	AsioChannelPtr _channel;
	AsioState _state;

	Ice::Long _id;
};

class PlainConnection : public Connection,
	public boost::enable_shared_from_this<PlainConnection> {
public:
	PlainConnection(Ice::Long connectionId,asio::io_service& io, AsioChannelPtr& handler);
	virtual ~PlainConnection();

	virtual asio::ip::tcp::socket& socket() {
		return _socket;
	}

private:
	virtual void hookRead();
	virtual void hookWrite();

	asio::ip::tcp::socket _socket;
};

class SSLConnection : public Connection,
	public boost::enable_shared_from_this<SSLConnection> {
public:
	SSLConnection(Ice::Long connectionId,asio::io_service& io, asio::ssl::context& ctx,
			AsioChannelPtr& handler);
	virtual ~SSLConnection();

	virtual asio::ip::tcp::socket& socket() {
		return _socket.next_layer();
	}

private:
	virtual void hookRead();
	virtual void hookWrite();

	virtual void handshake();
	void handleHandshake(const asio::error_code& err);

	ssl_socket _socket;
};

class AsioAction : public IceUtil::Shared {
public:
	virtual void execute(const AsioChannelPtr& channel,
			const Ice::ObjectPtr& data) = 0;
};

class AsioChannel : public MyUtil::Task, public IceUtil::Mutex {

public:
	AsioChannel();
	virtual ~AsioChannel();

	void initialize(Ice::Long connectionId, RefactorPtr refactor){
		_connectionId = connectionId;
		_refactor = refactor;
	}
	Ice::Long connectionId(){
		return _connectionId;
	}
	string getRemoteAddr();
	void setRemoteAddr(unsigned int addr){
		_ip = addr;
	}
	void shutdown(bool isClose = false);
	void handshake();

	bool notify(AsioState state, const string& message = "");
	bool deliver(const string& message);

	void execute(AsioActionPtr action, Ice::ObjectPtr data);
	void schedule(AsioActionPtr action, Ice::ObjectPtr data);

	virtual void handle();
	RefactorPtr getRefactor(){
		return _refactor;
	}

	virtual void finishWrite(){};
protected:
	virtual void started();
	virtual void stopped();

	virtual bool messaged(const string& message);


	bool _isConnected;
private:

	struct Request {
		Ice::ObjectPtr data;
		AsioActionPtr action;
	};
	typedef boost::shared_ptr<Request> RequestPtr;
	deque<RequestPtr> _requests;
	bool _handling;

	Ice::Long _connectionId;
	RefactorPtr _refactor;
	unsigned int _ip;
};

class AsioChannelFactory : public IceUtil::Shared {
public:
	virtual AsioChannelPtr create() = 0;
};
typedef IceUtil::Handle<AsioChannelFactory> AsioChannelFactoryPtr;

}
}

#endif
