#include "UserStateWriterAdapter.h"
#include "Date.h"
using namespace xce::adapter::userstate;
using namespace ::xce::userbase;

UserStateWriterPrx UserStateWriterAdapter::getUserStateWriter(int id)
{
	return getProxy(id);
}

void UserStateWriterAdapter::reload(int id)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(id);
	_mutex.notify();
}

void UserStateWriterAdapter::run()
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
			UserStateWriterPrx prx = getUserStateWriter(*it);
			if(prx)
			{
				try
				{
					prx->reload(*it);
				}catch(Ice::Exception& e)
				{
					MCE_WARN("UserStateWriterAdapter::run reload Exception id = " << *it <<" " << e.what());
				}
			}
		}
	}
}


