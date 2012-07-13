#ifndef __USER_LOGINCOUNT_READER_ADAPTER_H__
#define __USER_LOGINCOUNT_READER_ADAPTER_H__

#include "UserBase.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace adapter {
namespace userlogincount{

using namespace xce::userbase;
using namespace MyUtil;

const int CLUSTER = 100;

class UserLoginCountReaderAdapter : public MyUtil::ReplicatedClusterAdapterI<UserLoginCountReaderPrx>,
	public MyUtil::Singleton<UserLoginCountReaderAdapter>
{
public:
	UserLoginCountReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<UserLoginCountReaderPrx>("ControllerUserBaseReader",120,300,new MyUtil::OceChannel,true,"ULCM"){
	}
	
	int getUserLoginCount(int id, const Ice::Context& ctx = Ice::Context());
	void incUserLoginCount(int id);
	void setData(int id, const Ice::ObjectPtr& data);
private:
		

	UserLoginCountReaderPrx getUserLoginCountReaderPrx(int userId);

};

}
}
}

#endif

