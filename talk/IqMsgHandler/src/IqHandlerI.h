#ifndef __IQHANDLER_I__
#define __IQHANDLER_I__

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


class IqHandlerI : virtual public IqHandler,
		virtual public IceUtil::AbstractMutexI<IceUtil::Mutex>,
		virtual public Singleton<IqHandlerI> {
public:

	virtual void RemoveNotifyBySource(int userId, int type, Ice::Long resource, const Ice::Current& = Ice::Current());
	virtual void RemoveNotifyById(int userId, Ice::Long notifyId, const Ice::Current& = Ice::Current());
	void GetBuddyApply(const JidPtr& jid);
	virtual void NotifyBuddyApplyChange(int userId, const Ice::Current& = Ice::Current());
	virtual void message(const JidPtr& jid, const string& msg, const Ice::Current& = Ice::Current());
	virtual void notifySetPrivacy(const JidPtr& j, const string& msg,
			const Ice::Current& = Ice::Current());
	virtual void removeBuddyNotify(Ice::Int host, Ice::Int guest,
			const Ice::Current& = Ice::Current());
	virtual void changeBuddyGroupNotify(const BuddyItemPtr& bi, const Ice::Current& = Ice::Current());


//	string createJid(int userid, int resourceId = -1,
//			const string& endpoint = "");

};
typedef IceUtil::Handle<IqHandlerI> IqHandlerIPtr;

}
;
}
;
}
;

#endif
