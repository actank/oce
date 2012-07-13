#ifndef __LOGIN_MANAGER_I__
#define __LOGIN_MANAGER_I__

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
		float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
			-_begin.tv_usec;
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
		MCE_INFO("begin at -->"<<name);
	}
	;

	~StatFunc() {
		gettimeofday(&m_tpend, NULL);
		float timeuse=1000000*(m_tpend.tv_sec-m_tpstart.tv_sec)+m_tpend.tv_usec
				-m_tpstart.tv_usec;
		timeuse/=1000;

		MCE_INFO("end at -->"<<_name <<"  time:"<<timeuse<<" ms");
	}
	;
};

class LoginHandlerI : virtual public LoginHandler,
		virtual public IceUtil::AbstractMutexI<IceUtil::Mutex>,
		virtual public Singleton<LoginHandlerI> {
public:

	virtual PassportPtr verify(const string& email,const string& password, const Ice::Current& = Ice::Current());
	virtual void bind(const JidPtr& j, const Ice::Current& = Ice::Current());
	virtual void unbind(const JidPtr& j, const Ice::Current& = Ice::Current());

  PassportPtr verify_new(const string& email, const string& password);
	PassportPtr getPassport(const string& email, const string& password);
	void AddLoginScore(int userid);
};
typedef IceUtil::Handle<LoginHandlerI> LoginHandlerIPtr;

}
;
}
;
}
;

#endif
