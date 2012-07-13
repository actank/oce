#ifndef __USER_DOING_WRITER_ADAPTER_H__
#define __USER_DOING_WRITER_ADAPTER_H__

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
namespace userdoing{

using namespace ::xce::userbase;
using namespace __gnu_cxx;
class UserDoingWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<UserDoingWriterPrx>,
	public MyUtil::Singleton<UserDoingWriterAdapter>, IceUtil::Thread
{
public:
	UserDoingWriterAdapter() : MyUtil::ReplicatedClusterAdapterI<UserDoingWriterPrx>("ControllerUserBaseWriter",120,300,new MyUtil::OceChannel,true,"UDM"){
		start();
	}

	void reload(int id);

protected:

	void run();

private:

	UserDoingWriterPrx getUserDoingWriter(int userId);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	hash_set<int> _idSet;
};

}
}
}

#endif

