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
#include "TempMutex.h"

using namespace xce::tempmutex;

namespace mop {
namespace hi {
namespace svc {
namespace model {

const int TASK_LEVEL_FILL = 1;
const Ice::Int USER_CONFIG = 0;
const string USER_CONFIG_WRITER = "UCW";
const string TABLE_USER_CONFIG = "user_config";

//UserConfigWriterI
//--------------------------------------------------------------
class UserConfigWriterI: virtual public UserConfigWriter, public MyUtil::Singleton<UserConfigWriterI> {
public:
	UserConfigWriterI();
	virtual ~UserConfigWriterI();
	virtual void createUserConfig(const Str2StrMap& properties, const Ice::Current& = Ice::Current());
	virtual void setUserConfig(Ice::Int userId, const MyUtil::Str2StrMap& properties, const Ice::Current& = Ice::Current());
	virtual void reload(Ice::Int userId, const Ice::Current& = Ice::Current());

	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());

private:
	TempMutexManagerPtr _tempMutexManager;
};

//UserConfigFactoryI
//--------------------------------------------------------------
class UserConfigFactoryI: public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

//ResultHandler
//--------------------------------------------------------------
class BatchConfigResHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchConfigResHandlerI(UserConfigDataIPtr& rs) :
		m_rs(rs) {
	}
	virtual bool handle(mysqlpp::Row& row) const ;

private:
	UserConfigDataIPtr m_rs;
};

}
}
}
}
#endif

