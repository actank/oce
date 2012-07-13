#ifndef __USER_PASSPORT_WRITER_ADAPTER_H__
#define __USER_PASSPORT_WRITER_ADAPTER_H__

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
namespace userpassport{

using namespace ::xce::userbase;
using namespace __gnu_cxx;
class UserPassportWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<UserPassportWriterPrx>,
	public MyUtil::Singleton<UserPassportWriterAdapter>, IceUtil::Thread
{
public:
	UserPassportWriterAdapter() : MyUtil::ReplicatedClusterAdapterI<UserPassportWriterPrx>("ControllerUserBaseWriter",120,300,new MyUtil::OceChannel,true,"UPM"){
		start();
	}

	void reload(int id);

protected:

	void run();

private:

	UserPassportWriterPrx getUserPassportWriter(int userId);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	hash_set<int> _idSet;
};

}
}
}

#endif

