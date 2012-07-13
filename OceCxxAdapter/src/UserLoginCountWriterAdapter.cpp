#include "UserLoginCountWriterAdapter.h"
#include "Date.h"
using namespace xce::adapter::userlogincount;
using namespace ::xce::userbase;

UserLoginCountWriterPrx UserLoginCountWriterAdapter::getUserLoginCountWriter(int id)
{
	return getProxy(id);
}

void UserLoginCountWriterAdapter::reload(int id)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(id);
	_mutex.notify();
}

void UserLoginCountWriterAdapter::incUserLoginCount(int id)
{
	UserLoginCountWriterPrx prx = getUserLoginCountWriter(id);
	if(prx)
	{
		try
		{
			prx->incUserLoginCount(id);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserLoginCountWriterAdapter::incUserLoginCount Exception id = " << id <<" " << e.what());
		}
	}
}

void UserLoginCountWriterAdapter::run()
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
			UserLoginCountWriterPrx prx = getUserLoginCountWriter(*it);
			if(prx)
			{
				try
				{
					prx->reload(*it);
				}catch(Ice::Exception& e)
				{
					MCE_WARN("UserLoginCountWriterAdapter::run reload Exception id = " << *it <<" " << e.what());
				}
			}
		}
	}
}


