#ifndef __USER_RIGHT_WRITER_ADAPTER_H__
#define __USER_RIGHT_WRITER_ADAPTER_H__

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
namespace userright{

using namespace ::xce::userbase;
using namespace __gnu_cxx;
class UserRightWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<UserRightWriterPrx>,
	public MyUtil::Singleton<UserRightWriterAdapter>, IceUtil::Thread
{
public:
	UserRightWriterAdapter() : MyUtil::ReplicatedClusterAdapterI<UserRightWriterPrx>("ControllerUserBaseWriter",120,300,new MyUtil::OceChannel,true,"URM"){
		start();
	}

	void reload(int id);

protected:

	void run();

private:

	UserRightWriterPrx getUserRightWriter(int userId);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	hash_set<int> _idSet;
};

}
}
}

#endif

