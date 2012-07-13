#ifndef _SESSION_H_
#define _SESSION_H_

#include "Refactor.h"
#include "TalkCommon.h"
//#include "HttpParser.h"
#include "Request.h"
#include "ServiceI.h"

namespace talk {
namespace http {

using namespace com::xiaonei::talk::common;
using namespace MyUtil;
const static size_t MAX_MSG_SIZE = 50*1024;



class Session;
class HttpParser;
typedef IceUtil::Handle<Session> SessionPtr;
typedef IceUtil::Handle<HttpParser> HttpParserPtr;

class ConnectionStat {
public:
	void loadConfig(const string& path = "/opt/XNTalk/etc/XNHttpServer.conf");
	void changeStat(const SessionPtr& session, ConnectionStat* stat);
	virtual void recvRequest(const RequestPtr& request,
			const SessionPtr& session) {
		MCE_DEBUG("recvRequest --> in a wrong stat");
		responseClose(session,404,"");
	}
	;
	virtual void response(const RequestPtr& request, const SessionPtr& session) {
	}
	;
	virtual void deliverNotify(const SessionPtr& session, Ice::Long msgId) {
	}
	;
	virtual void responseClose(const SessionPtr& session, int status_code, const string& msg ); //{
	//}
	//;
protected:
	virtual void deliver(const SessionPtr& session, int status_code,
			int cache_time, bool is_gzip, bool keep_alive,
			const string& extra_header, const string& body,
			bool bin_data = false,
			const string& content_type = "text/plain;charset=UTF-8");

	struct ActionConfig {
		string type;
		bool has_index;
	};

	static map<string,ActionConfig> _config;
};

class ConnectingStat : public ConnectionStat, public Singleton<ConnectingStat> {
public:
	virtual void recvRequest(const RequestPtr& request,
			const SessionPtr& session);

};

class WapCometRecvStat : public ConnectionStat,
	public Singleton<WapCometRecvStat> {
public:
	virtual void response(const RequestPtr& request, const SessionPtr& session);
protected:
	virtual void deliver(const SessionPtr& session, int status_code,
			int cache_time, bool is_gzip, bool keep_alive,
			const string& extra_header, const string& body,
			bool bin_data = false,
			const string& content_type = "text/plain;charset=UTF-8");

};

class WapCometResponseStat : public ConnectionStat,
	public Singleton<WapCometResponseStat> {
public:

	virtual void deliverNotify(const SessionPtr& session, Ice::Long msgId);
	virtual void responseClose(const SessionPtr& session, int status_code , const string& msg );
protected:
	virtual void deliver(const SessionPtr& session, int status_code,
			int cache_time, bool is_gzip, bool keep_alive,
			const string& extra_header, const string& body,
			bool bin_data = false,
			const string& content_type = "text/plain;charset=UTF-8");

};

class WapBroadcastResponseStat : public ConnectionStat,
	public Singleton<WapBroadcastResponseStat> {
public:
	virtual void response(const RequestPtr& request, const SessionPtr& session);
};

class DefaultResponseStat : public ConnectionStat,
	public Singleton<DefaultResponseStat> {
public:
	virtual void response(const RequestPtr& request, const SessionPtr& session);
};

//-------------------------------------------
class Session : public ::xce::nio::AsioChannel {
public:
	Session();
	virtual ~Session();

	bool operator<(const Session& rhs) {
		return this < &rhs;
	}

	void clearCount() {
		_counter = 0;
	}

	bool bind(const string& ticket,bool flush, bool isWap );

	void jid(int userid, Ice::Long session_id) {

		JidPtr j = new Jid();
		j->userId = userid;
		j->endpoint = ServiceI::instance().getName();
		j->index = session_id;
		_jid = j;
	}
	JidPtr& jid() {
		return _jid;
	}
	void verify(bool v) {
		_verify = v;
	}
	bool verify() {
		return _verify;
	}
	bool isGzip() {
		return _isGzip;
	}
	void isGzip(bool isGzip) {
		_isGzip = isGzip;
	}
	void setCloseStat() {
		_isClose = true;
	}
	void needUnbind(bool need){
		_needUnbind = need;
	}

	virtual void stopped();
	void connectionStat(ConnectionStat* stat) {
		_stat = stat;
	}
	void recvRequest(const ::talk::http::RequestPtr& r) {
		//IceUtil::Mutex::Lock lock(_statMutex);
		_stat->recvRequest(r, this);
	}
	void response(const ::talk::http::RequestPtr& r) {
		//IceUtil::Mutex::Lock lock(_statMutex);
		_stat->response(r, this);
	}
	void deliverNotify(Ice::Long msgId, bool isSync = true) {
		if (isSync) {
			//IceUtil::Mutex::Lock lock(_statMutex);
			_stat->deliverNotify(this, msgId);
		} else {
			MCE_INFO("Session::deliverNotify --> ");
			_stat->deliverNotify(this, msgId);
		}
	}
	void responseClose(int status_code,const string& msg) {
		//IceUtil::Mutex::Lock lock(_statMutex);
		_stat->responseClose(this,status_code,msg);
	}
	bool isReadSomething(){
		return _isReadSomething;
	}
protected:
	void _response(int status_code, bool is_gzip, bool keep_alive,
			const string& extra_header, const string& body,
			bool is_push = false, int timeout = 0);
	virtual void started();

	virtual bool messaged(const string& message);
	virtual void finishWrite();

private:
	IceUtil::Mutex _statMutex;
	size_t _counter;
	HttpParserPtr _parser;

	JidPtr _jid;
	bool _verify;
	bool _isGzip;
	int _binding;
	bool _isClose;

	bool _needUnbind;
	bool _isReadSomething;

	ConnectionStat* _stat;

};

class SessionFactoryI : public xce::nio::AsioChannelFactory {
public:
	virtual xce::nio::AsioChannelPtr create() {
		return new Session;
	}
};

}
}

#endif /*SESSION_H_*/
