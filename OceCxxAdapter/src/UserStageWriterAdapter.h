#ifndef __USER_STAGE_WRITER_ADAPTER_H__
#define __USER_STAGE_WRITER_ADAPTER_H__

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
namespace userstage{

using namespace ::xce::userbase;
using namespace __gnu_cxx;
class UserStageWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<UserStageWriterPrx>,
	public MyUtil::Singleton<UserStageWriterAdapter>, IceUtil::Thread
{
public:
	UserStageWriterAdapter() : MyUtil::ReplicatedClusterAdapterI<UserStageWriterPrx>("ControllerUserBaseWriter",120,300,new MyUtil::OceChannel,true,"USGM"){
		start();
	}

	void reload(int id);

protected:

	void run();

private:

	UserStageWriterPrx getUserStageWriter(int userId);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	hash_set<int> _idSet;
};

}
}
}

#endif

