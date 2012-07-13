#include "UserStageWriterAdapter.h"
#include "Date.h"
using namespace xce::adapter::userstage;
using namespace ::xce::userbase;

UserStageWriterPrx UserStageWriterAdapter::getUserStageWriter(int id)
{
	return getProxy(id);
}

void UserStageWriterAdapter::reload(int id)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(id);
	_mutex.notify();
}

void UserStageWriterAdapter::run()
{
	while(true)
	{
		MyUtil::IntSeq ids;
		{
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
			if(_idSet.empty())
			{
				_mutex.wait();
			}
			ids.insert(ids.end(), _idSet.begin(), _idSet.end());
			_idSet.clear();
		}
		for(MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it)
		{
			UserStageWriterPrx prx = getUserStageWriter(*it);
			if(prx)
			{
				try
				{
					prx->reloadUserStage(*it);
				}catch(Ice::Exception& e)
				{
					MCE_WARN("UserStageWriterAdapter::run reload Exception id = " << *it <<" " << e.what());
				}
			}
		}
	}
}


