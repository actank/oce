#include "IdCacheWriterAdapter.h"
#include "Date.h"

namespace idcache
{

IdCacheWriterPrx IdCacheWriterAdapter::getIdCacheWriterPrx(int id)
{
	return getProxy(abs(id));
}

void IdCacheWriterAdapter::reload(int id)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(id);
	_mutex.notify();
}

void IdCacheWriterAdapter::run()
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
			IdCacheWriterPrx prx = getIdCacheWriterPrx(*it);
			if(prx)
			{
				try
				{
					prx->reload(*it);
				}catch(Ice::Exception& e)
				{
					MCE_WARN("IdCacheWriterAdapter::run reload Exception id = " << *it <<" " << e.what());
				}
			}
		}
	}
}

}
