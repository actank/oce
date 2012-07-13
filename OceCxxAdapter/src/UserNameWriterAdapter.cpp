#include "UserNameWriterAdapter.h"
#include "Date.h"
using namespace xce::adapter::username;
using namespace ::xce::userbase;

UserNameWriterPrx UserNameWriterAdapter::getUserNameWriter(int id)
{
	return getProxy(id);
}

void UserNameWriterAdapter::reload(int id)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(id);
	_mutex.notify();
}

void UserNameWriterAdapter::run()
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
			UserNameWriterPrx prx = getUserNameWriter(*it);
			if(prx)
			{
				try
				{
					prx->reload(*it);
				}catch(Ice::Exception& e)
				{
					MCE_WARN("UserNameWriterAdapter::run reload Exception id = " << *it <<" " << e.what());
				}
			}
		}
	}
}


