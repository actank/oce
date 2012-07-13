#include "UserTimeWriterAdapter.h"
#include "Date.h"
using namespace xce::adapter::usertime;
using namespace ::xce::userdesc;

UserTimeWriterPrx UserTimeWriterAdapter::getUserTimeWriter(int userId)
{
	return getProxy(userId);
}

UserTimeWriterPrx UserTimeWriterAdapter::getUserTime10sWriter(int userId) {
	return getProxy(userId)->ice_timeout(10000);
}

void UserTimeWriterAdapter::setUserTime(int userId, const MyUtil::Str2StrMap& props) {
	if(userId <= 0) {
		return;
	}
	getUserTimeWriter(userId)->setUserTime(userId, props);
}

void UserTimeWriterAdapter::createUserTime(int userId, const MyUtil::Str2StrMap& props) {
	if(userId <= 0) {
		return;
	}
	getUserTime10sWriter(userId)->createUserTime(props);
}

void UserTimeWriterAdapter::reload(int userId)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(userId);
	_mutex.notify();
}

void UserTimeWriterAdapter::run()
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
			UserTimeWriterPrx prx = getUserTimeWriter(*it);
			if(prx)
			{
				try
				{
					prx->reload(*it);
				}catch(Ice::Exception& e)
				{
					MCE_WARN("UserTimeWriterAdapter::run reload Exception id = " << *it <<" " << e.what());
				}
			}
		}
	}
}


