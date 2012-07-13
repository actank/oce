#ifndef MESSAGETASK_H_
#define MESSAGETASK_H_

#include "TaskManager.h"
#include "TalkCommon.h"
#include "TalkUtil/src/pugixml.hpp"
#include "MessageHandlerI.h"
#include "ActiveTrackAdapter.h"

namespace com
{
namespace xiaonei
{
namespace talk
{

using namespace MyUtil;
using namespace pugi;
using namespace com::xiaonei::talk::common;

class TimeStat{
	timeval _begin, _end;
	public:
	TimeStat(){
		reset();
	}
	void reset(){
		gettimeofday(&_begin, NULL);
	}
	float getTime(){
		gettimeofday(&_end, NULL);
		float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
			-_begin.tv_usec;
		timeuse/=1000;
		return timeuse;
	}
};




class MessageTask : public Task
{
	JidPtr _jid;
	xml_document_ptr _doc;
	string _ip;
public:
	MessageTask(const JidPtr& jid, const xml_document_ptr& doc, const string& ip):_jid(jid),_doc(doc), _ip(ip){};
	virtual void handle();
	enum ReplyType{REPLY_NONE, REPLY_STATUS, REPLY_BLOG, REPLY_PHOTO, REPLY_ALBUM};
protected:
	void _processRoomTalk();
	void _processRoomTalkPrivateMsg();
	void _processInvite();
	void _processReply();
	void _processShare();
};

class FirstSend2WTimer : public Timer, public MyUtil::Singleton<FirstSend2WTimer>{
public:
  FirstSend2WTimer() : Timer(24*60*60*1000){}
  Ice::Long MakeKey(int from, int to){
    Ice::Long key = (((Ice::Long)from)<<32)|to;
    return key; 
  }
  void Push(int from, int to){
    IceUtil::Mutex::Lock lock(mutex_);
    keys_.insert(MakeKey(from, to));
  }
  bool IsSend(int from, int to){
    IceUtil::Mutex::Lock lock(mutex_);
    return keys_.find(MakeKey(from, to))==keys_.end();
  }
  virtual void handle(){
    set<Ice::Long> tmp;
    keys_.swap(tmp);
  }
public:
  IceUtil::Mutex        mutex_;
  set<Ice::Long>        keys_;
};



class MucTestTask : public MyUtil::Task{
public:
	MucTestTask(const JidPtr& jid) : _jid(jid){
	};
	virtual void handle();
private:
	JidPtr _jid;
};

};
};
};
#endif /*MESSAGETASK_H_*/
