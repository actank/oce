#include "UserUrlWriterAdapter.h"
#include "Date.h"
using namespace xce::adapter::userurl;
using namespace ::xce::userbase;

UserUrlWriterPrx UserUrlWriterAdapter::getUserUrlWriter(int id)
{
	return getProxy(id);
}

void UserUrlWriterAdapter::reload(int id)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(id);
	_mutex.notify();
}

void UserUrlWriterAdapter::run()
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
			UserUrlWriterPrx prx = getUserUrlWriter(*it);
			if(prx)
			{
				try
				{
					prx->reload(*it);
				}catch(Ice::Exception& e)
				{
					MCE_WARN("UserUrlWriterAdapter::run reload Exception id = " << *it <<" " << e.what());
				}
			}
		}
	}
}


