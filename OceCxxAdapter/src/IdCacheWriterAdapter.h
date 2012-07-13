#ifndef __IDCACHE_WRITER_ADAPTER_H__
#define __IDCACHE_WRITER_ADAPTER_H__

#include <ext/hash_set>
#include "IdCache.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <IceUtil/IceUtil.h>
#include <boost/lexical_cast.hpp>
#include "Markable.h"

namespace idcache 
{

class IdCacheWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<IdCacheWriterPrx>,
	public MyUtil::Singleton<IdCacheWriterAdapter>, IceUtil::Thread
{
public:
	IdCacheWriterAdapter() : MyUtil::ReplicatedClusterAdapterI<IdCacheWriterPrx>("ControllerIdCacheWriter",120,300)
	{
		start();
	}

	void reload(int id);

protected:

	void run();

private:

	IdCacheWriterPrx getIdCacheWriterPrx(int id);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	__gnu_cxx::hash_set<int> _idSet;
};

}

#endif

