#include "UserDoingWriterAdapter.h"
#include "Date.h"
using namespace xce::adapter::userdoing;
using namespace ::xce::userbase;

UserDoingWriterPrx UserDoingWriterAdapter::getUserDoingWriter(int id)
{
	return getProxy(id);
}

void UserDoingWriterAdapter::reload(int id)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(id);
	_mutex.notify();
}

void UserDoingWriterAdapter::run()
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
			UserDoingWriterPrx prx = getUserDoingWriter(*it);
			if(prx)
			{
				try
				{
					prx->reloadUserDoing(*it);
				}catch(Ice::Exception& e)
				{
					MCE_WARN("UserDoingWriterAdapter::run reload Exception id = " << *it <<" " << e.what());
				}
			}
		}
	}
}


