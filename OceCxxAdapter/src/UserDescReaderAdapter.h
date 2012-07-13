#ifndef __USER_DESC_READER_ADAPTER_H__
#define __USER_DESC_READER_ADAPTER_H__

#include "UserDesc.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace adapter {
namespace userdesc {

class UserDescReaderAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::userdesc::UserDescReaderPrx>,
	public MyUtil::Singleton<UserDescReaderAdapter>
{
public:
	UserDescReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<xce::userdesc::UserDescReaderPrx>("ControllerUserDescReader",120,300,new MyUtil::OceChannel,true,"M"){
	}

	xce::userdesc::UserDescDataPtr getUserDesc(int userid, const Ice::Context& ctx = Ice::Context());
	xce::userdesc::UserDescDataNPtr getUserDescN(int userid, const Ice::Context& ctx = Ice::Context());

private:

	xce::userdesc::UserDescReaderPrx getUserDescReader(int userId);
	xce::userdesc::UserDescReaderPrx getUserDescReaderOneway(int userId);

};

}
}
}

#endif

