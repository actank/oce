#ifndef __USER_LOGINCOUNT_WRITER_ADAPTER_H__
#define __USER_LOGINCOUNT_WRITER_ADAPTER_H__

#include <ext/hash_set>
#include "UserBase.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <IceUtil/IceUtil.h>
#include <boost/lexical_cast.hpp>
#include "Markable.h"

namespace xce {
namespace adapter {
namespace userlogincount{

using namespace ::xce::userbase;
using namespace __gnu_cxx;
class UserLoginCountWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<UserLoginCountWriterPrx>,
	public MyUtil::Singleton<UserLoginCountWriterAdapter>, IceUtil::Thread
{
public:
	UserLoginCountWriterAdapter() : MyUtil::ReplicatedClusterAdapterI<UserLoginCountWriterPrx>("ControllerUserBaseWriter",120,300,new MyUtil::OceChannel,true,"ULCM"){
		start();
	}

	void reload(int id);
	
	void incUserLoginCount(int id);

protected:

	void run();

private:

	UserLoginCountWriterPrx getUserLoginCountWriter(int userId);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	hash_set<int> _idSet;
};

}
}
}

#endif

