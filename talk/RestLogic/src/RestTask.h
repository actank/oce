#ifndef RESTTASK_H_
#define RESTTASK_H_

//#include "TaskManager.h"
#include <sys/time.h>
#include <string>
#include <IceUtil/Shared.h>
#include <map>
#include <Ice/Ice.h>
#include "TalkCommon.h"
#include "RestLogic.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "Date.h"

using namespace com::xiaonei::talk::common;
using namespace std;
using namespace MyUtil;
using namespace talk::rest;

class RestTask : public IceUtil::Shared{
public:
	RestTask(int retryCount=1) : _retryCount(retryCount){
		_timeStamp = time(NULL);
	}
	//virtual void handle() = 0;
	virtual void result(const RestResultPtr& content) = 0;
	virtual void timeout() = 0;
	//virtual string path() = 0;
	virtual map<string, string> parameters() = 0;
	bool validContent(const RestResultPtr& content);
	string makeSig(const map<string, string>& paras);

	Ice::Long id(){
		return _id;
	}

	void id(Ice::Long id){
		_id = id;
	}

	bool isRetry(){
		return _retryCount>0;
	}

	bool isTimeout(){
		if(time(NULL) - _timeStamp > TIMEOUT){
			_timeStamp = time(NULL);
			return true;
		}
		return false;
	}
	void 	_handle();
private:
	static const int TIMEOUT = 10;//超时太短的话可能会重复调用
	int 		_retryCount;
	int			_timeStamp;
	Ice::Long	_id;
};
typedef IceUtil::Handle<RestTask> RestTaskPtr;


class SetStatusTask : public RestTask{
public:
	SetStatusTask(const JidPtr& jid, OnlineStat state, const string& status):
				_jid(jid), _state(state), _status(status){

	}
	void timeout();
	void result(const RestResultPtr& content);
	map<string, string> parameters();
public:
private:
	JidPtr 		_jid;
	OnlineStat 	_state;
	string		_status;
};

class ReplyStatusTask : public RestTask{
public:
	ReplyStatusTask(int userid, long statusId, int ownerId,
					int replieduserId, const string& content):
		_userid(userid), _statusId(statusId), _ownerId(ownerId),
		_replieduserId(replieduserId), _content(content){

	}
	map<string, string> parameters();
	void timeout();
	void result(const RestResultPtr& content);
public:
private:
	int			_userid;
	Ice::Long 	_statusId;
	int 		_ownerId;
	int 		_replieduserId;
	string 		_content;
};

class UseImNotifyTask : public RestTask{
public:
	UseImNotifyTask(int userid):
		_userid(userid){

	}
	map<string, string> parameters();
	void timeout();
	void result(const RestResultPtr& content);
public:
private:
	int			_userid;
};

class ActiveUserNotifyTask : public RestTask{
public:
	ActiveUserNotifyTask(int userid):
		_userid(userid){
	}
	map<string, string> parameters();
	void timeout();
	void result(const RestResultPtr& content);
public:
private:
	int			_userid;
};

class ActiveUserNotifyTask2 : public RestTask{
public:
	ActiveUserNotifyTask2(int userid):
		_userid(userid){
	}
	map<string, string> parameters();
	void timeout();
	void result(const RestResultPtr& content);
public:
private:
	int			_userid;
};

/*class LastWeekLoginUser : public MyUtil::Timer, public MyUtil::Singleton<LastWeekLoginUser>{
public:
	LastWeekLoginUser():Timer(60 * 1000){
		_inited = false;
		_day = Date(time(NULL)).day();
	}
	void handle();
	bool isLogin(int userid){
		if(!_inited){
			return true;
		}
		IceUtil::Mutex::Lock lock(_mutex);
		return (_users.find(userid)!=_users.end());
	}
private:
	int		_day;
	set<int> 	_users;
	bool		_inited;
	IceUtil::Mutex 	_mutex;
};*/


class SendNoticeTask : public RestTask{
public:
	SendNoticeTask(int userid, const string& data, MyUtil::IntSeq& userids):
		_userid(userid), _data(data){
		_toIds.swap(userids);
	}
	map<string, string> parameters();
	void timeout();
	void result(const RestResultPtr& content);
private:
	int			_userid;
	string			_data;
	MyUtil::IntSeq		_toIds;
};


class SendNoticeLobbyTask : public RestTask{
public:
  SendNoticeLobbyTask(int userid, const string& data, MyUtil::IntSeq& userids):
    _userid(userid), _data(data){
    _toIds.swap(userids);
  }
  map<string, string> parameters();
  void timeout();
  void result(const RestResultPtr& content);
private:
  int             _userid;
  string          _data;
  MyUtil::IntSeq  _toIds;
};








//photo album blog
class ReplyTask : public RestTask{
public:
	ReplyTask(const string& method, int userid, long resId, int ownerId,
			int replieduserId, const string& content, int replyType):
		_method(method), _userid(userid), _resId(resId), _ownerId(ownerId),
		_replieduserId(replieduserId), _content(content), _replyType(replyType){

	}
	map<string, string> parameters();
	void timeout();
	void result(const RestResultPtr& content);
private:
	string		_method;
	int		_userid;
	Ice::Long 	_resId;
	int 		_ownerId;
	int 		_replieduserId;
	string 		_content;
	int		_replyType;
};


class GetSessionKeyTask : public RestTask{
public:
	GetSessionKeyTask(const JidPtr& jid, int iqid) : jid_(jid), iqid_(iqid){
	}
	map<string, string> parameters();
	void timeout();
	void result(const RestResultPtr& content);
private:
	JidPtr jid_;
	int iqid_;
};

#endif
