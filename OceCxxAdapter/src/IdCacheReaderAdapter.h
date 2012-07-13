#ifndef __ID_CACHE_ADAPTER_H__
#define __ID_CACHE_ADAPTER_H__

#include "IdCache.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <boost/lexical_cast.hpp>
#include "Markable.h"

namespace idcache
{

const int MIN_FILTER_ID = 2000000000;
const int MAX_FILTER_ID = 2100000000;

class IdCacheReaderAdapter : virtual public MyUtil::ReplicatedClusterAdapterI<IdCacheReaderPrx>,
	virtual public MyUtil::Singleton<IdCacheReaderAdapter>
{
public:
	IdCacheReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<IdCacheReaderPrx>( "ControllerIdCacheReader",120,300,new MyUtil::OceChannel,true,"M"){

	}
	IdRelationInfoPtr getIdRelationInfo(int id);
	int getRelatedUserId(int id);
	void createIdRelation(int id, int transId, int virtualId);
	void deleteIdRelation(int id, int virtualId);
	bool isVirtualId(int id);
	void setData(int id, const Ice::ObjectPtr& data);

private:

	bool isMaybeVirtualId(int id);
	IdCacheReaderPrx getIdCacheReaderPrx(int id);

};

}

#endif

