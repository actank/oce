#ifndef __PRESENCEHANDLER_I__
#define __PRESENCEHANDLER_I__

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

namespace com {
namespace xiaonei {
namespace talk {
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::cache;
//using namespace mop::hi::svc::adapter;


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
		float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec -_begin.tv_usec;
		timeuse/=1000;
		return timeuse;
	}
};





class StatFunc {
	string _name;
	timeval m_tpstart, m_tpend;
public:
	StatFunc(string name) :
		_name(name) {
		gettimeofday(&m_tpstart, NULL);
		//MCE_INFO("begin at -->"<<name);
	}
	;

	~StatFunc() {
		gettimeofday(&m_tpend, NULL);
		float timeuse=1000000*(m_tpend.tv_sec-m_tpstart.tv_sec)+m_tpend.tv_usec
				-m_tpstart.tv_usec;
		timeuse/=1000;

		//MCE_INFO("end at -->"<<_name <<"  time:"<<timeuse<<" ms");
	}
	;
};

const float FUNTIMEOUT = 250.0;

class PresenceHandlerI : virtual public PresenceHandler,
		virtual public IceUtil::AbstractMutexI<IceUtil::Mutex>,
		virtual public Singleton<PresenceHandlerI> {
public:

	virtual void presence(const JidPtr& jid, const string& show,
			const string& ver, const string& subver, const string& ext,
			const Ice::Current& = Ice::Current());
	virtual void presenceWithSecver(const JidPtr& jid, const string& show,
			const string& ver, const string& subver, const string& ext, const string& secver,
			const Ice::Current& = Ice::Current());
	virtual void message(const JidPtr& jid, const string& msg,
			const Ice::Current& = Ice::Current());
	virtual void presenceSubscribe(const string& id, Ice::Int from,
			Ice::Int to, const string& status,
			const Ice::Current& = Ice::Current());
	virtual void presenceSubscribeWithPostscript(const string& id, Ice::Int from,
			Ice::Int to, const string& status, const string& postscript,
			const Ice::Current& = Ice::Current());
	virtual void presenceSubscribed(const string& id, Ice::Int from,
			const Ice::Int to, const Ice::Current& = Ice::Current());
	virtual void presenceUnsubscribed(const string& id, Ice::Int from,
			Ice::Int to, const Ice::Current& = Ice::Current());

	virtual void offlineNotify(const JidSeq& seq,
			const Ice::Current& = Ice::Current());

	virtual void webPresence(Ice::Int userId,
			const Ice::Current& = Ice::Current());

	virtual void mucOfflineNotify(const JidSeq& jids,
				const Ice::Current& = Ice::Current());
//	virtual void broadcastPresence(const JidPtr& jid, const string& show,
//			const string& ver, const string& ext, const string& status,
//			const string& photo, const string& nick,
//			const Ice::Current& = Ice::Current());


	virtual void nodifyBuddiesOnlineStat(const AvatarSeq& activeAvatars, const AvatarSeq& selfOtherAvatars,
					const AvatarMap& buddies, NotifyType type, const Str2StrMap& paras, const Ice::Current& = Ice::Current());

	virtual void offline(const JidPtr& jid, const Ice::Current& = Ice::Current());
//	string createJid(int userid, int resourceId = -1,
//			const string& endpoint = "");
	virtual void userStatusChanged(int userId, int userStatus, const Ice::Current& = Ice::Current());
	//---------------------------------------------------
	
//	virtual void ChangeNotifyData(const string& notifydata, const Ice::Current& = Ice::Current());

private:

	//IceUtil::Mutex _parseMutes;

};
typedef IceUtil::Handle<PresenceHandlerI> PresenceHandlerIPtr;

}
;
}
;
}
;

#endif
