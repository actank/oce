#ifndef __SERVER_H__
#define __SERVER_H__

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <IceUtil/IceUtil.h>
#include <IceUtil/Thread.h>
#include <Ice/Ice.h>
#include "Session.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Refactor.h"

namespace com {
namespace xiaonei {
namespace talk {

using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace std;



//----------------------------------------------------------------------
class Server : public Singleton<Server> {
public:
	void start(const Ice::PropertiesPtr& props);
	//void stop();

	//void apply(SessionPtr session);
	//void leave(SessionPtr session);
	//bool deliver(const JidPtr& jid, const string& message);

	//int applySize();

private:

	::xce::nio::RefactorPtr _refactor;
	IceUtil::Mutex _mutex;
	//set<SessionPtr> _applies;
	//	SessionMap _sessions; // userid-resourceid -> sessionPtr;
};
typedef IceUtil::Handle<Server> ServerPtr;
//---------------------------------------------



}
;
}
;
}
;
#endif

