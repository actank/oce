#include "UserRightWriterAdapter.h"
#include "Date.h"
using namespace xce::adapter::userright;
using namespace ::xce::userbase;

UserRightWriterPrx UserRightWriterAdapter::getUserRightWriter(int id)
{
	return getProxy(id);
}

void UserRightWriterAdapter::reload(int id)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(id);
	_mutex.notify();
}

void UserRightWriterAdapter::run()
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
			UserRightWriterPrx prx = getUserRightWriter(*it);
			if(prx)
			{
				try
				{
					prx->reload(*it);
				}catch(Ice::Exception& e)
				{
					MCE_WARN("UserRightWriterAdapter::run reload Exception id = " << *it <<" " << e.what());
				}
			}
		}
	}
}


