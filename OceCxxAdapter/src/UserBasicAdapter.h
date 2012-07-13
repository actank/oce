#ifndef __USER_BASIC_ADAPTER_H__
#define __USER_BASIC_ADAPTER_H__

#include "UserBase.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace adapter {
namespace userbasic {

using namespace ::xce::userbase;
using namespace MyUtil;

class UserBasicAdapter : public MyUtil::ReplicatedClusterAdapterI<UserBasicManagerPrx>,
	public MyUtil::Singleton<UserBasicAdapter>
{
public:
	UserBasicAdapter() : MyUtil::ReplicatedClusterAdapterI<UserBasicManagerPrx>("ControllerUserBase",120,300,new MyUtil::OceChannel,true,"UBM") {
	}
	UserBasicDataPtr getUserBasic(int id, const Ice::Context& ctx = Ice::Context());
		
	UserBasicManagerPrx getUserBasicManager(int userId);
	UserBasicManagerPrx getUserBasicManagerOneway(int userId);

};

}
}
}

#endif

