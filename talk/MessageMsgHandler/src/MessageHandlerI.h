#ifndef __MESSAGEHANDLER_I__
#define __MESSAGEHANDLER_I__

#include <math.h>
#include <Ice/Ice.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include "ServiceI.h"
#include "LogWrapper.h"
#include "Singleton.h"
#include "TalkLogic.h"
#include "TalkProxy.h"
#include <sys/time.h>
//#include "TalkLogicAdapter.h"
//#include "TalkMessageAdapter.h"
#include "MessageMsgHandlerAdapter.h"
#include "NoticePassage.h"
namespace com {
namespace xiaonei {
namespace talk {
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::cache;
//using namespace mop::hi::svc::adapter;
using namespace ::talk::adapter;



class PipeFactoryI : public PipeFactory {
	public:
  virtual PipePtr create(const string& type, int index) {
   return new NoticePipe(type,index);
	}
};

class StatFunc {
	string _name;
	timeval m_tpstart, m_tpend;
public:
	StatFunc(string name) :
		_name(name) {
		gettimeofday(&m_tpstart, NULL);
		MCE_DEBUG("begin at -->"<<name);
	}
	;

	~StatFunc() {
		gettimeofday(&m_tpend, NULL);
		float timeuse=1000000*(m_tpend.tv_sec-m_tpstart.tv_sec)+m_tpend.tv_usec
				-m_tpstart.tv_usec;
		timeuse/=1000;

		MCE_DEBUG("end at -->"<<_name <<"  time:"<<timeuse<<" ms");
	}
	;
};

class Leaveword : public IceUtil::Shared {
public:
	int from;
	int to;
	string msg;
	int time;
};

typedef IceUtil::Handle<Leaveword> LeavewordPtr;
typedef vector<LeavewordPtr> LeavewordSeq;

class MessageHandlerI : virtual public MessageHandler,
		virtual public IceUtil::AbstractMutexI<IceUtil::Mutex>,
		virtual public Singleton<MessageHandlerI> {
public:
	MessageHandlerI() :
		_messageCount(0) {
		int cluster;
		try{
		  cluster = MessageMsgHandlerAdapter::instance().getCluster();
		}
		catch(Ice::Exception& e){
			MCE_WARN("call MessageMsgHandlerAdapter::getCluster err" << e);	
		}
		_beginIndex = time(NULL)*(Ice::Long)pow((double)cluster,7);
	}
	;
	virtual void deliverAppNotice(Ice::Int toid,Ice::Int appid,const string& appName, const string& appIcon, const string& msg, const Ice::Current& = Ice::Current());
  virtual void sendMessage(int from, int to, const string& msg, const Ice::Current& = Ice::Current());
	virtual void message(const JidPtr& jid, const string& msg,
			const Ice::Current& = Ice::Current());
	virtual void deliverFeed(const string& msg, const string& type,
			const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());
	//virtual void loadOfflineMessage(const JidPtr& jid,
	//		const Ice::Current& = Ice::Current());



	//---------------------------------------------------
	void addLeaveword(LeavewordPtr& leaveword);
	void swapLeaveword(LeavewordSeq& seq);

	void setIndex(int index) {
		_index = index;
	}
	;
	Ice::Long getMessageKey() {
		int cluster;
		try{
		  cluster = MessageMsgHandlerAdapter::instance().getCluster();
		}
		catch(Ice::Exception& e){
		  MCE_WARN("call MessageMsgHandlerAdapter::getCluster err" << e);
		}
		IceUtil::Mutex::Lock lock(_msgKeyMutex);
		if (_messageCount > pow((double)cluster, 6)) {
			_messageCount = 0;
			_beginIndex = time(NULL)*(Ice::Long)pow((double)cluster,7);
		}

		++_messageCount;
		return _beginIndex + _messageCount * cluster + _index;

		//return _messageCount * cluster + _index;
	}
	;
private:

	LeavewordSeq _leavewordSeq;

	IceUtil::Mutex _leavewordMutex;

	IceUtil::Mutex _msgKeyMutex;

	int _messageCount;
	int _index;
	Ice::Long _beginIndex;

};
typedef IceUtil::Handle<MessageHandlerI> MessageHandlerIPtr;

}
;
}
;
}
;

#endif
