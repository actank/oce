#include "UserNetworkWriterAdapter.h"

using namespace mop::hi::svc::adapter;
using namespace std;

NetworkWriterPrx UserNetworkWriterAdapter::getNetworkWriter(int id)
{
	return getProxy(id);
}

void UserNetworkWriterAdapter::reloadAll(int id)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(id);
	_mutex.notify();
}

void UserNetworkWriterAdapter::run()
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
			NetworkWriterPrx prx = getNetworkWriter(*it);
			if(prx)
			{
				try
				{
					prx->reloadAll(*it);
				}catch(Ice::Exception& e)
				{
					MCE_WARN("UserNetworkWriterAdapter::run reloadAll Exception id = " << *it <<" " << e.what());
				}
			}
		}
	}
}
