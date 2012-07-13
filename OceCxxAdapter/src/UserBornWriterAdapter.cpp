#include "UserBornWriterAdapter.h"
#include "Date.h"
using namespace xce::adapter::userborn;
using namespace ::xce::userbase;

UserBornWriterPrx UserBornWriterAdapter::getUserBornWriter(int id)
{
	return getProxy(id);
}

void UserBornWriterAdapter::reload(int id)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(id);
	_mutex.notify();
}

void UserBornWriterAdapter::run()
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
			UserBornWriterPrx prx = getUserBornWriter(*it);
			if(prx)
			{
				try
				{
					prx->reloadUserBorn(*it);
				}catch(Ice::Exception& e)
				{
					MCE_WARN("UserBornWriterAdapter::run reload Exception id = " << *it <<" " << e.what());
				}
			}
		}
	}
}


