#ifndef __USER_CONFIG_WRITER_I_H__
#define __USER_CONFIG_WRITER_I_H__

#include "UserConfig.h"
#include <Ice/Ice.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include "UserConfigCommon.h"
#include "TaskManager.h"
#include "ObjectCacheI.h"
#include "ServiceI.h"
#include "QueryRunner.h"

namespace mop {
namespace hi {
namespace svc {
namespace model {

const string USER_CONFIG_WRITER = "UCW";

//UserConfigWriterI
//--------------------------------------------------------------
class UserConfigWriterI: virtual public UserConfigWriter, public MyUtil::Singleton<UserConfigWriterI> {
public:
	virtual void createUserConfig(const Str2StrMap& properties, const Ice::Current& = Ice::Current());
	virtual void setUserConfig(Ice::Int userId, const MyUtil::Str2StrMap& properties, const Ice::Current& = Ice::Current());
	virtual void reload(Ice::Int userId, const Ice::Current& = Ice::Current());

	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());

};

}
}
}
}
#endif

