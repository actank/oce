#include "Refactor.h"
#include "PerformanceStat.h"

namespace xce {
namespace nio {

//----------------------------------------------------------------------
Refactor::Refactor(short port, int index, const AsioChannelFactoryPtr& factory,
		size_t threads, const string& certificate, int max_conn) :
	_port(port), _factory(factory),_certificate(certificate), _threads(threads),
			 _next(0), _count(0), _beginIndex((Ice::Long)time(NULL)
					*10000), _index(index), _max_conn(max_conn){
	//	cout << "Refactor::Refactor -> " << certificate << endl;
	//	cout.flush();

	// Give all the io_services work to do so that their run() functions will not
	// exit until they are explicitly stopped.


	for (std::size_t i = 0; i < threads+1; ++i) {
		IosCtx ic;
		io_service_ptr io(new asio::io_service);
		ic.ios = io;
		cout << "Refactor::Refactor -> io_service -> " << io << endl;
		work_ptr work(new asio::io_service::work(*io));
		cout << "Refactor::Refactor -> work -> " << work << endl;
		ic.work = work;
		if (!certificate.empty()) {
			context_ptr ctx(new asio::ssl::context(*io, asio::ssl::context::sslv23));
			ctx->set_options(asio::ssl::context::default_workarounds
					| asio::ssl::context::no_sslv2
					| asio::ssl::context::single_dh_use);
			//	context->set_options(asio::ssl::context::default_workarounds);
			//	context->set_password_callback(boost::bind(&server::get_password, this));
			ctx->use_certificate_chain_file(certificate);
			ctx->use_private_key_file(certificate, asio::ssl::context::pem);
			//	context.use_tmp_dh_file("dh512.pem");
			cout << "Refactor::Refactor -> ctx -> " << ctx << endl;
			cout << "Refactor::Refactor -> #####################" << endl;
			ic.ctx = ctx;
		}
		if (i) {
			_iosCtxs.push_back(ic);
		} else {
			_acceptCtx = ic;
		}
	}
	PerfromanceStat::instance().SetRefactor(this);
	TaskManager::instance().scheduleRepeated(&PerfromanceStat::instance());
}

Refactor::~Refactor() {

}

void Refactor::start() {
	IosCtx& ic = _acceptCtx;
	tcp::endpoint endpoint(tcp::v4(), _port);
	_acceptor.reset(new asio::ip::tcp::acceptor(*ic.ios));
	_acceptor->open(endpoint.protocol());
	_acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(1));
	_acceptor->bind(endpoint);
	_acceptor->listen(1000);

	for (size_t i = 0; i < 20; ++i) {
		ConnectionPtr conn = create();
		_acceptor->async_accept(conn->socket(), boost::bind(&Refactor::accept,
				this, conn, asio::placeholders::error));
	}
	IceUtil::Thread::start(131072).detach();
}

void Refactor::stop() {
	// Explicitly stop all io_services.
	for (std::size_t i = 0; i < _threads; ++i) {
		_iosCtxs[i].ios->stop();
	}
	_acceptor.reset();
	_iosCtxs.clear();
}

void Refactor::run() {
	// Create a pool of threads to run all of the io_services.
	std::vector<boost::shared_ptr<asio::thread> > threads;

	boost::shared_ptr<asio::thread> thread(new asio::thread(
			boost::bind(&asio::io_service::run, _acceptCtx.ios)));
	threads.push_back(thread);

	for (std::size_t i = 0; i < _threads; ++i) {
		boost::shared_ptr<asio::thread> thread(new asio::thread(
				boost::bind(&asio::io_service::run, _iosCtxs[i].ios)));
		threads.push_back(thread);
	}

	// Wait for all threads in the pool to exit.
	for (std::size_t i = 0; i < threads.size(); ++i) {
		threads[i]->join();
	}
}

void Refactor::accept(ConnectionPtr connection, const asio::error_code& error) {
	MCE_INFO("Refactor::accept --> _max_conn:" << _max_conn << " connectionPoolSize:" << connectionPoolSize());
	if (!error && (_max_conn <= 0 || (connectionPoolSize() < _max_conn))) {
		PerfromanceStat::instance().ConnectionStarted();
		connection->start(connection);
	} else {
		connection->stop(true);
	}
	ConnectionPtr conn = create();
	_acceptor->async_accept(conn->socket(), boost::bind(&Refactor::accept,
			this, conn, asio::placeholders::error));
}

ConnectionPtr Refactor::create() {
	Ice::Long id = getConnectionId();
	AsioChannelPtr handler = _factory->create();
	handler->initialize(id, this);
	IosCtx& ic = nextIosCtx();

	ConnectionPtr conn;
	if (_certificate.empty()) {
		conn = ConnectionPtr(new PlainConnection(id,*ic.ios, handler));
	} else {
		conn = ConnectionPtr(new SSLConnection(id,*ic.ios, *ic.ctx, handler));
	}
	return conn;
}
void Refactor::addConnection(const ConnectionPtr& conn) {

	int index = poolIndex(conn->id());
	if (index < 0) {
		MCE_WARN("ConnectionPool::addConnection --> add connection err");
		conn->stop(true);
		return;
	}
	{
		IceUtil::RWRecMutex::WLock lock(_rwmutex[index]);
		_pool[index][conn->id()] = conn;
	}
}

Refactor::IosCtx& Refactor::nextIosCtx() {
	// Use a round-robin scheme to choose the next io_service to use.
	IosCtx& result = _iosCtxs[_next];
	++_next;
	if (_next == _threads) {
		_next = 0;
	}
	return result;
}

bool Refactor::deliver(Ice::Long connectionId, const string& msg) {
	//MCE_DEBUG("ConnectionPool::deliver --> connectionId:"<<connectionId
	//		<<"   msg:"<<msg);
	int index = poolIndex(connectionId);
	if (index<0) {
		MCE_DEBUG("ConnectionPool::deliver --> index :"<<index);
		return false;
	}
	ConnectionPtr conn;
	{
		IceUtil::RWRecMutex::RLock lock(_rwmutex[index]);
		ConnectionMap::iterator it = _pool[index].find(connectionId);

		if (it != _pool[index].end()) {
			MCE_DEBUG("ConnectionPool::deliver --> deliver");
			conn = it->second;

		} else {
			return false;
		}
	}
	return conn->deliver(msg);
}
void Refactor::connectionRemove(Ice::Long connectionId) {

	int index = poolIndex(connectionId);
	if (index < 0) {
		return;
	}

	IceUtil::RWRecMutex::WLock lock(_rwmutex[index]);
	ConnectionMap::iterator it = _pool[index].find(connectionId);
	if (it != _pool[index].end()) {
		//it->second->stop();
		_pool[index].erase(it);
	}
}

void Refactor::connectionShutdown(Ice::Long connectionId, bool isClose) {

	PerfromanceStat::instance().ConnectionStopped(true);
	int index = poolIndex(connectionId);
	if (index < 0) {
		return;
	}
	ConnectionPtr conn;
	{
		IceUtil::RWRecMutex::RLock lock(_rwmutex[index]);
		ConnectionMap::iterator it = _pool[index].find(connectionId);
		if (it != _pool[index].end()) {
			conn = it->second;
		} else {
			return;
		}
	}
	if (conn) {
		conn->stop(isClose);
	}
}
ConnectionPtr Refactor::getConnection(Ice::Long connectionId) {
	ConnectionPtr conn;
	int index = poolIndex(connectionId);
	if (index < 0) {
		return conn;
	}
	{
		IceUtil::RWRecMutex::RLock lock(_rwmutex[index]);
		ConnectionMap::iterator it = _pool[index].find(connectionId);
		if (it != _pool[index].end()) {
			conn = it->second;
		}
	}
	return conn;
}
void Refactor::connectionHandshake(Ice::Long connectionId) {
	int index = poolIndex(connectionId);
	if (index < 0) {
		return;
	}
	ConnectionPtr conn;
	{
		IceUtil::RWRecMutex::RLock lock(_rwmutex[index]);
		ConnectionMap::iterator it = _pool[index].find(connectionId);
		if (it != _pool[index].end()) {
			conn = it->second;
		} else {
			return;
		}
	}
	if (conn) {
		conn->handshake();
	}
}

int Refactor::connectionPoolSize() {
	int count = 0;
	for (size_t i = 0; i < (unsigned)POOL_TUB_SIZE; ++i) {
		IceUtil::RWRecMutex::RLock lock(_rwmutex[i]);
		count += _pool[i].size();
	}
	//MCE_INFO("ConnectionPool::size --> POOL_SIZE:"<< count);
	return count;
}

}
}
