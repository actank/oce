#ifndef _FRIENDFINDER_BYACTIVEUSERWORKER_H_
#define _FRIENDFINDER_BYACTIVEUSERWORKER_H_

#include "FriendFinderByActiveUserWorker.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "ServiceI.h"
#include "PreloadLoginData.h"

namespace xce {
namespace friendfinder {

class FriendFinderByActiveUserWorkerI : virtual public FriendFinderByActiveUserWorker,
		virtual public MyUtil::Singleton<FriendFinderByActiveUserWorkerI> {
public:
	virtual void load(const Ice::Current&);

protected:
	friend class MyUtil::Singleton<FriendFinderByActiveUserWorkerI>;
};

}
}

#endif
