#ifndef __USER_URL_WRITER_ADAPTER_H__
#define __USER_URL_WRITER_ADAPTER_H__

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
namespace userurl{

using namespace ::xce::userbase;
using namespace __gnu_cxx;
class UserUrlWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<UserUrlWriterPrx>,
	public MyUtil::Singleton<UserUrlWriterAdapter>, IceUtil::Thread
{
public:
	UserUrlWriterAdapter() : MyUtil::ReplicatedClusterAdapterI<UserUrlWriterPrx>("ControllerUserBaseWriter",120,300,new MyUtil::OceChannel,true,"U"){
		start();
	}

	void reload(int id);

protected:

	void run();

private:

	UserUrlWriterPrx getUserUrlWriter(int userId);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	hash_set<int> _idSet;
};

}
}
}

#endif

