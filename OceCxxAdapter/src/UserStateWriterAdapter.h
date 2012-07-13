#ifndef __USER_STATE_WRITER_ADAPTER_H__
#define __USER_STATE_WRITER_ADAPTER_H__

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
namespace userstate{

using namespace ::xce::userbase;
using namespace __gnu_cxx;
class UserStateWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<UserStateWriterPrx>,
	public MyUtil::Singleton<UserStateWriterAdapter>, IceUtil::Thread
{
public:
	UserStateWriterAdapter() : MyUtil::ReplicatedClusterAdapterI<UserStateWriterPrx>("ControllerUserBaseWriter",120,300,new MyUtil::OceChannel,true,"USM"){
		start();
	}

	void reload(int id);

protected:

	void run();

private:

	UserStateWriterPrx getUserStateWriter(int userId);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	hash_set<int> _idSet;
};

}
}
}

#endif

