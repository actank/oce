#include "Connection.h"
#include "Refactor.h"
#include "PerformanceStat.h"

namespace xce {
namespace nio {

//----------------------------------------------------------------------
Connection::Connection(Ice::Long connectionId, AsioChannelPtr& handler) :
	_channel(handler), _state(ASIO_PLAIN), _id(connectionId) {
}

Connection::~Connection() {
	MCE_DEBUG("Connection::~Connection");
}

void Connection::start(ConnectionPtr & conn) {
	MCE_DEBUG("Connection::start -> " << this << ",");
	socket().set_option(asio::socket_base::keep_alive(true));
	socket().set_option(asio::socket_base::reuse_address(true));

	//ConnectionPool::instance().addConnection(conn);
	_channel->getRefactor()->addConnection(conn);
	try{
		_channel->setRemoteAddr(socket().remote_endpoint().address().to_v4().to_ulong());
	}catch(std::exception& e){
		MCE_WARN("Connection::start --> set remote addr error, " << e.what());
	}catch(...){
		MCE_WARN("Connection::start --> set remote addr error");
	}
	if (_channel->notify(ASIO_STARTED)) {
		hookRead();
	} else {
		//ConnectionPool::instance().close(this->id());
		stop(true);
	}
}

void Connection::stop(bool isClose) {
	PerfromanceStat::instance().ConnectionStopped();
	{
		IceUtil::Mutex::Lock lock(_mutex);
		MCE_DEBUG("Connection::stop");
		if (_state == ASIO_SHUTDOWN) {
			MCE_DEBUG("Connection::stop --> stop yet");
			return;
		}
		try {
			if(isClose){
			socket().shutdown(asio::ip::tcp::socket::shutdown_both);
			}else{
						socket().shutdown(asio::ip::tcp::socket::shutdown_receive );
			}
		} catch (std::exception& e) {
			//IE每次刷新Connection reset by peer，之后调用shutdown会报Transport endpoint is not connected
			MCE_DEBUG("Connection::stop -> " << this << ", shutdown socket err -> " << e.what());
		} catch (...) {
			MCE_WARN("Connection::stop -> " << this << ", shutdown socket err -> unknown");
		}
		if(isClose){
		try {
				socket().close();
			} catch (std::exception& e) {
				MCE_WARN("Connection::stop -> " << this << ", close socket err -> " << e.what());
			} catch (...) {
				MCE_WARN("Connection::stop -> " << this << ", close socket err -> unknown");
			}
		}
		_state = ASIO_SHUTDOWN;

	}
	if (_channel) {
		_channel->notify(ASIO_STOPPED);
		//_channel = 0;
	}
}

void Connection::handshake() {

}

bool Connection::handle(const string& message) {
	//MCE_DEBUG("Connection::handle -> " << message);
	if (_channel->notify(ASIO_MSG, message)) {
		return true;
	} else {
		//ConnectionPool::instance().close(this->id());
		stop(true);
		return false;
	}
}

/*string Connection::getRemoteAddr(){
	asio::ip::tcp::socket& sock = this->socket();
	asio::ip::tcp::endpoint remoteEndpoint = sock.remote_endpoint();
	string ipStr = remoteEndpoint.address().to_string();
	int port = remoteEndpoint.port();
	ostringstream os;
	os << ipStr << ":" << port;
	return os.str();
}*/

bool Connection::deliver(const string& message) {
//	MCE_DEBUG("Connection::deliver -> " << this << "," << __LINE__);
	IceUtil::Mutex::Lock lock(_mutex);
	if (_state == ASIO_SHUTDOWN) {
		return false;
	}
	bool writing = !_outgoing.empty();
	_outgoing.push_back(message);
	if (!writing) {
		hookWrite();
	}
//	MCE_DEBUG("Connection::deliver -> " << this << "," << __LINE__);
	return true;
}

void Connection::handleRead(const asio::error_code& err, size_t length) {
	if (!err) {
		if (handle(string(_incoming, length))) {
			if (_state != ASIO_SHUTDOWN) {
				hookRead();
			}
		}
	} else {
		if (err != asio::error::eof) {
			//IE每次刷新Connection reset by peer
			MCE_DEBUG("Connection::handleRead -> " << _id << ", failed -> "
					<< err.message());
		}
		//ConnectionPool::instance().close(this->id());
		PerfromanceStat::instance().ConnectionStopped(false);
		stop(true);
	}
}

void Connection::handleWrite(const asio::error_code& err) {
	if (!err) {
		{
			IceUtil::Mutex::Lock lock(_mutex);
			_outgoing.pop_front();
			if (!_outgoing.empty() && _state != ASIO_SHUTDOWN) {
				hookWrite();
				return;
			}
		}

		_channel->finishWrite();

	} else {
		MCE_WARN("Connection::handleWrite -> " << this << ", failed -> "
				<< err.message());
		//ConnectionPool::instance().close(this->id());
		stop(true);
	}
}

//---------------------------------------------------------------------------
PlainConnection::PlainConnection(Ice::Long connectionId, asio::io_service& io,
		AsioChannelPtr& handler) :
	Connection(connectionId, handler), _socket(io) {

}

PlainConnection::~PlainConnection() {

}

void PlainConnection::hookRead() {
	socket().async_read_some(asio::buffer(_incoming, BUFFER_SIZE),
			boost::bind(&Connection::handleRead, shared_from_this(),
					asio::placeholders::error,
					asio::placeholders::bytes_transferred));
}

void PlainConnection::hookWrite() {
	asio::async_write(socket(), asio::buffer(_outgoing.front().c_str(), _outgoing.front().size()), boost::bind( &Connection::handleWrite,
			shared_from_this(), asio::placeholders::error));
}

//----------------------------------------------------------------------
SSLConnection::SSLConnection(Ice::Long connectionId, asio::io_service& io,
		asio::ssl::context& ctx, AsioChannelPtr& handler) :
	Connection(connectionId, handler), _socket(io, ctx) {

}

SSLConnection::~SSLConnection() {
	//MCE_DEBUG("SSLConnection::~Connection");
}

void SSLConnection::handshake() {
	MCE_DEBUG("SSLConnection::handshake --> begin");
	if (_state == ASIO_PLAIN) {
		MCE_DEBUG("SSLConnection::handshake --> stat="<<_state);
		_state = ASIO_HANDSHAKE;
		_socket.async_handshake(asio::ssl::stream_base::server, boost::bind(
				&SSLConnection::handleHandshake, shared_from_this(),
				asio::placeholders::error));
	}
}

void SSLConnection::handleHandshake(const asio::error_code& err) {
	if (!err) {
		MCE_DEBUG("SSLConnection::handleHandshake -> success");
		_state = ASIO_TLS;
		hookRead();
	} else {
		MCE_WARN("SSLConnection::handleHandshake -> failed -> "
				<< err.message());
		//ConnectionPool::instance().close(this->id());
		stop(true);
	}
}

void SSLConnection::hookRead() {
	if (_state == ASIO_PLAIN) {
		socket().async_read_some(asio::buffer(_incoming, BUFFER_SIZE),
				boost::bind(&SSLConnection::handleRead, shared_from_this(),
						asio::placeholders::error,
						asio::placeholders::bytes_transferred));
	} else if (_state == ASIO_TLS) {
		_socket.async_read_some(asio::buffer(_incoming, BUFFER_SIZE),
				boost::bind(&SSLConnection::handleRead, shared_from_this(),
						asio::placeholders::error,
						asio::placeholders::bytes_transferred));
	}
}

void SSLConnection::hookWrite() {
	if (_state == ASIO_PLAIN) {
		asio::async_write(socket(), asio::buffer(_outgoing.front().c_str(), _outgoing.front().size()), boost::bind( &SSLConnection::handleWrite,
				shared_from_this(), asio::placeholders::error));
	} else if (_state == ASIO_TLS) {
		asio::async_write(_socket, asio::buffer(_outgoing.front().c_str(), _outgoing.front().size()), boost::bind( &SSLConnection::handleWrite,
				shared_from_this(), asio::placeholders::error));
	}
}

//---------------------------------------------------------------------------
AsioChannel::AsioChannel() :
	_isConnected(true), _handling(false) {
	_ip = 0;
}

AsioChannel::~AsioChannel() {

}

//void AsioChannel::hook(ConnectionPtr conn) {
//	_conn = conn;
//}

void AsioChannel::shutdown(bool isClose) {
	//IceUtil::Mutex::Lock lock(*this);
	//	ConnectionPtr conn = ConnectionPool::instance().getConnection(connectionId());
	//	if (conn) {
	//		conn->stop();
	//	}
	_refactor->connectionShutdown(connectionId(),isClose);
	//notify(ASIO_STOPPED,"");
}

void AsioChannel::handshake() {
	//IceUtil::Mutex::Lock lock(*this);
	//	ConnectionPtr conn = ConnectionPool::instance().getConnection(connectionId());
	//	if (conn) {
	//		conn->handshake();
	//	}
	_refactor->connectionHandshake(connectionId());
}

bool AsioChannel::notify(AsioState state, const string& message) {

	if (state == ASIO_STARTED) {
		try {
			started();
		} catch(...) {
			MCE_WARN("AsioChannel::notify --> throw exception in started()");
			return false;
		}
		return true;
	} else if (state == ASIO_STOPPED) {
		if (!_isConnected) {
			return true;
		}
		_isConnected = false;
		try {
			stopped();
		} catch(...) {
			MCE_WARN("AsioChannel::notify --> throw exception in stopped()");
		}
		//ConnectionPool::instance().close(connectionId());
		_refactor->connectionRemove(connectionId());
		return true;
	} else if (state == ASIO_MSG) {
		//			IceUtil::Mutex::Lock lock(*this);
		//MCE_DEBUG("AsioChannel::notify --> msg:"<< message);
		//MCE_DEBUG("AsioChannel::notify --> CONN_ID:"<<connectionId());
		bool suc = false;
		try {
			suc = messaged(message);
		} catch(Ice::Exception& e) {
			MCE_WARN("AsioChannel::notify --> throw exception in messaged() err:"<<e);
		} catch(std::exception& e) {
			MCE_WARN("AsioChannel::notify --> throw exception in messaged() err:"<<e.what());
		} catch(...) {
			MCE_WARN("AsioChannel::notify --> throw exception in messaged()");
		}
		return suc;
	}
	return false;
}

string AsioChannel::getRemoteAddr(){
	return asio::ip::address_v4::address_v4(_ip).to_string();
}

bool AsioChannel::deliver(const string& message) {
	//IceUtil::Mutex::Lock lock(*this);
	if (_isConnected) {
		//if (ConnectionPool::instance().deliver(connectionId(), message)) {
		//		MCE_DEBUG("AsioChannel::deliver --> msg:"<<message);
		if (_refactor->deliver(connectionId(), message)) {
			return true;
		} else {
			shutdown();
		}
	}
	return false;
}

void AsioChannel::started() {

}

void AsioChannel::stopped() {

}

bool AsioChannel::messaged(const string& message) {
	return true;
}

void AsioChannel::execute(AsioActionPtr action, Ice::ObjectPtr data) {
	try {
		action->execute(this, data);
	} catch (...) {

	}
}

void AsioChannel::schedule(AsioActionPtr action, Ice::ObjectPtr data) {
	IceUtil::Mutex::Lock lock(*this);

	RequestPtr request(new Request);
	request->action = action;
	request->data = data;
	_requests.push_back(request);
	MCE_DEBUG("AsioChannel::schedule1");
	if (!_handling) {
		_handling = true;
		MCE_DEBUG("AsioChannel::schedule2");
		MyUtil::TaskManager::instance().execute(this);
	}
}

void AsioChannel::handle() {
	while (true) {
		RequestPtr request;
		{
			IceUtil::Mutex::Lock lock(*this);

			if (_requests.empty()|| (!_isConnected)) {
				_handling = false;
				return;
			}
			// _handling is true
			request = _requests.front();
			_requests.pop_front();
		}
		execute(request->action, request->data);
		IceUtil::Mutex::Lock lock(*this);
		if (_requests.empty()) {
			_handling = false;
			return;
		}
	}
}

}
}

