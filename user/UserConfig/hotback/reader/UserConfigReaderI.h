#ifndef __USER_CONFIG_READER_I_H__
#define __USER_CONFIG_READER_I_H__

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

const int TASK_LEVEL_FILL = 1;
const Ice::Int USER_CONFIG = 0;
const string USER_CONFIG_READER = "UCR";
const string TABLE_USER_CONFIG = "user_config";

//UserConfigReaderI
//--------------------------------------------------------------
class UserConfigReaderI: virtual public UserConfigReader, public MyUtil::Singleton<UserConfigReaderI> {
public:
	virtual MyUtil::Str2StrMap getUserConfig(Ice::Int uid, const Ice::Current& = Ice::Current());

	virtual void setUserConfig(Ice::Int uid, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());

	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());

private:
	UserConfigDataIPtr _getUserConfig(int userId);
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

//FillTask
//--------------------------------------------------------------
template<class T, class P> class FillTask: virtual public ::MyUtil::Task {
public:
	FillTask(const ::MyUtil::ObjectResultPtr& userData) :
		_userData(userData) {
	}
	virtual void handle(){
		for (MyUtil::ObjectMap::iterator it = _userData->data.begin(); it != _userData->data.end(); ++it) {
			T obj = new P;
			obj->parse(it->second);
			MyUtil::ServiceI::instance().getObjectCache()->addObject(USER_CONFIG, it->first, obj);
		}
	}

private:
	::MyUtil::ObjectResultPtr _userData;
};

}
}
}
}
#endif

