#ifndef __USERCONFIGM2SADAPTER_H__
#define __USERCONFIGM2SADAPTER_H__

#include "UserConfig.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

#include <QueryRunner.h>
#include "CallGuarantee/src/CallGuaranteeAdapter.h"

namespace mop {
namespace hi {
namespace svc {
namespace model {

const std::string DB_USER_CONFIG = "user_config";

class CreateHandle : public callguarantee::GuaranteeHandle {
public:
	CreateHandle(int userId, const std::vector<UserConfigSlavePrx>& prxs);
	virtual bool doHandle();
	virtual bool reDoHandle();
	virtual void merge(const callguarantee::GuaranteeHandlePtr& newData) {}
private:
	int userId_;
	std::vector<UserConfigSlavePrx> prxs_;
	UserConfigDataPtr data_;
};

typedef IceUtil::Handle<CreateHandle> CreateHandlePtr;

class SetHandle : public callguarantee::GuaranteeHandle {
public:
	SetHandle(int userId, const MyUtil::Str2StrMap& props, const std::vector<UserConfigSlavePrx>& prxs):
		_userId(userId),_props(props), _prxs(prxs) {}
	virtual bool doHandle();
	virtual bool reDoHandle();
	virtual void merge(const callguarantee::GuaranteeHandlePtr& newData);
private:
	int _userId;
	MyUtil::Str2StrMap _props;
	std::vector<UserConfigSlavePrx> _prxs;
};

typedef IceUtil::Handle<SetHandle> SetHandlePtr;

class UserConfigResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	UserConfigResultHandlerI(const UserConfigDataPtr& props) : _props(props) {}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	UserConfigDataPtr _props;
};
	
class UserConfigM2SAdapter : public callguarantee::CallGuaranteeAdapterI<UserConfigSlavePrx>, public MyUtil::Singleton<mop::hi::svc::model::UserConfigM2SAdapter>
{
public:
	UserConfigM2SAdapter() : callguarantee::CallGuaranteeAdapterI<UserConfigSlavePrx>("ControllerUserConfigSlave", 120, 300, false, "C") {}

	void createUserConfig(int userId);

	void setUserConfig(int userId, const MyUtil::Str2StrMap& props);
};

}
}
}
}

#endif

