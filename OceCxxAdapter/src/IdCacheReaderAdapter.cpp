#include "IdCacheReaderAdapter.h"
#include "Date.h"

namespace idcache
{

IdCacheReaderPrx IdCacheReaderAdapter::getIdCacheReaderPrx(int id)
{
	return getProxy(abs(id));
}

IdRelationInfoPtr IdCacheReaderAdapter::getIdRelationInfo(int id)
{
	IdCacheReaderPrx prx = getIdCacheReaderPrx(id);
	if(prx)
	{
		try
		{
			return prx->getIdRelationInfo(id);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("IdCacheReaderAdapter::getIdRelationInfo Exception id " << id << " " << e.what());
		}
	}
	return NULL;
}

int IdCacheReaderAdapter::getRelatedUserId(int id)
{
	IdCacheReaderPrx prx = getIdCacheReaderPrx(id);
	if(prx)
	{
		try
		{
			return prx->getRelatedUserId(id);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("IdCacheReaderAdapter::getRelatedUserId Exception id " << id << " " << e.what());
		}
	}
	return 0;
}

void IdCacheReaderAdapter::createIdRelation(int id, int transId, int virtualId) 
{
	std::vector<IdCacheReaderPrx> allProxies = getAllProxySeq(abs(id));
	for(std::vector<IdCacheReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->createIdRelation(id, transId, virtualId);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("IdCacheReaderAdapter::createIdRelation Exception id " << id << " transId " << transId << " virtualId " << virtualId << " " << e.what());
		}
	}
}

void IdCacheReaderAdapter::deleteIdRelation(int id, int virtualId) 
{
	std::vector<IdCacheReaderPrx> allProxies = getAllProxySeq(abs(id));
	for(std::vector<IdCacheReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->deleteIdRelation(id, virtualId);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("IdCacheReaderAdapter::deleteIdRelationException id " << id << " virtualId " << virtualId << " " << e.what());
		}
	}
}

void IdCacheReaderAdapter::setData(int id, const Ice::ObjectPtr& data) 
{
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(id, data));
	std::vector<IdCacheReaderPrx> allProxies = getAllProxySeq(abs(id));
	for(std::vector<IdCacheReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(res);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("IdCacheReaderAdapter::setData id " << id << " " << e.what());
		}
	}
}

bool IdCacheReaderAdapter::isVirtualId(int id)
{
	if(!isMaybeVirtualId(id))
	{
		return false;
	}
	IdCacheReaderPrx prx = getIdCacheReaderPrx(id);
	if(prx)
	{
		try
		{
			return prx->isVirtualId(id);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("IdCacheReaderAdapter::isVirtualId Exception id " << id << " " << e.what());
		}
	}
	return false;
}

bool IdCacheReaderAdapter::isMaybeVirtualId(int id)
{
	return id > MIN_FILTER_ID && id < MAX_FILTER_ID;
}

}

