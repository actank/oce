#ifndef __USER_TIME_WRITER_ADAPTER_H__
#define __USER_TIME_WRITER_ADAPTER_H__

#include <ext/hash_set>
#include "UserDesc.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <IceUtil/IceUtil.h>
#include <boost/lexical_cast.hpp>
#include "Markable.h"

namespace xce {
namespace adapter {
namespace usertime{

using namespace ::xce::userdesc;
using namespace __gnu_cxx;
class UserTimeWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<UserTimeWriterPrx>,
	public MyUtil::Singleton<UserTimeWriterAdapter>, IceUtil::Thread
{
public:
	UserTimeWriterAdapter() : MyUtil::ReplicatedClusterAdapterI<UserTimeWriterPrx>("ControllerUserDescWriter",120,300,new MyUtil::OceChannel,true,"T"){
		start();
	}

	void setUserTime(int userId, const MyUtil::Str2StrMap& props);
	void createUserTime(int userId, const MyUtil::Str2StrMap& props);
	void reload(int userId);

protected:

	void run();

private:

	UserTimeWriterPrx getUserTimeWriter(int userId);
	UserTimeWriterPrx getUserTime10sWriter(int userId);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	hash_set<int> _idSet;
};

}
}
}

#endif

