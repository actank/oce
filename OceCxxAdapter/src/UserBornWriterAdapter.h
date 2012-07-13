#ifndef __USER_BORN_WRITER_ADAPTER_H__
#define __USER_BORN_WRITER_ADAPTER_H__

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
namespace userborn{

using namespace ::xce::userbase;
using namespace __gnu_cxx;
class UserBornWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<UserBornWriterPrx>,
	public MyUtil::Singleton<UserBornWriterAdapter>, IceUtil::Thread
{
public:
	UserBornWriterAdapter() : MyUtil::ReplicatedClusterAdapterI<UserBornWriterPrx>("ControllerUserBaseWriter",120,300,new MyUtil::OceChannel,true,"UBOM"){
		start();
	}

	void reload(int id);

protected:

	void run();

private:

	UserBornWriterPrx getUserBornWriter(int userId);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	hash_set<int> _idSet;
};

}
}
}

#endif

