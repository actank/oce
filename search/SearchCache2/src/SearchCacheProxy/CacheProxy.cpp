#include <execinfo.h>
#include "QueryRunner.h"
#include "../Util/DatabaseCacheFactory.h"
#include "CacheProxy.h"

//***************************************************************************

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	int cluster = service.getCommunicator()->getProperties()->getPropertyAsInt("SearchCache.Cluster");
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("SearchCache.Mod");
	string dsn  = service.getCommunicator()->getProperties()->getPropertyWithDefault("SearchCache.Dsn", TOKYOTYRANT_DSN_PREFIX"127.0.0.1:1981");

	TaskManager::instance().config(TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_UPDATE_DBCACHE, ThreadPoolConfig(16, 32));

	service.getAdapter()->add(&CacheProxyManagerI::instance(),service.createIdentity(SEARCH_CACHE_MANAGER, ""));

	CacheProxyManagerI::instance().bootstrap(cluster, mod, dsn);

	service.registerXceObserver(new ServiceSchemaI);
}

//***************************************************************************

CacheProxyManagerI::CacheProxyManagerI() {
	cluster_ = 0;
	mod_ = 0;
}

void CacheProxyManagerI::bootstrap(int cluster, int mod, const string &dsn) {
	cluster_ = cluster;
	mod_ = mod;
	dsn_ = dsn;

	vector<string> dsns(cluster);
	dsns.resize(cluster);
	dsns[mod % cluster] = dsn;

	//XceCacheAdapter::instance().DbInitialize(dsns);
}

SearchCacheResult CacheProxyManagerI::getSearchCache(::Ice::Int userId, const Ice::Current&) {
  //MCE_INFO("[TEMPLE] CacheProxyManagerI::getSearchCache, userId is "<<userId);
	std::string data; 
  XceCacheAdapter::instance().Get(userId, &data);

	SearchMemCacheData cache;
	cache.Unserialize(data.data(), data.size());

	return cache.getProperties();
}

Int2SearchCacheResultMap CacheProxyManagerI::getSearchCacheList(const ::MyUtil::IntSeq& userIds, const Ice::Current&) {
  //MCE_INFO("[TEMPLE] CacheProxyManagerI::getSearchCacheList, userIds.size() is "<<userIds.size());
	struct timeval tvStart;
	struct timeval tvEnd;
	//MyUtil::LongSeq uids;

	gettimeofday(&tvStart, NULL);

	Int2SearchCacheResultMap result;

	xce::Int2StrMap dataMap = XceCacheAdapter::instance().Gets(userIds);
	xce::Int2StrMap::const_iterator itData = dataMap.begin();
	xce::Int2StrMap::const_iterator itDataEnd = dataMap.end();
	for (; itData != itDataEnd; ++itData) {
		int userid = itData->first;
		const std::string &data = itData->second;

		SearchMemCacheData cache;

		if (cache.Unserialize(data.data(), data.size())) {
			result[userid] = cache.getProperties();
		}
	}

	gettimeofday(&tvEnd, NULL);
	int start = tvStart.tv_sec * 1000000 + tvStart.tv_usec;
	int end = tvEnd.tv_sec * 1000000 + tvEnd.tv_usec;

	if ((end - start) > 5000) {
		MCE_INFO("Get SearchCache List: time=" << end-start << "us total=" << userIds.size());
	}

  //MCE_INFO("[TEMPLE]CacheProxyManagerI::getSearchCacheList return value.size() is "<<result.size());
	return result;
}

Int2ByteSeq CacheProxyManagerI::getSearchCacheList2(const ::MyUtil::IntSeq& userIds, const Ice::Current& ic) {
    struct timeval tvStart;
    struct timeval tvEnd;

    gettimeofday(&tvStart, NULL);

    Int2ByteSeq result;

    xce::Int2StrMap dataMap = XceCacheAdapter::instance().Gets(userIds);
    //MCE_INFO("[TEMPLE]CacheProxyManagerI::getSearchCacheList2 Gets return value.size() is "<<dataMap.size());
    xce::Int2StrMap::const_iterator itData = dataMap.begin();
    xce::Int2StrMap::const_iterator itDataEnd = dataMap.end();
    for (; itData != itDataEnd; ++itData) {
      int userid = itData->first;
      const std::string &data = itData->second;
      //MCE_INFO("[TEMPLE]CacheProxyManagerI::getSearchCacheList2 value is "<<data);
        result[userid].assign(data.begin(), data.end());
    }

    gettimeofday(&tvEnd, NULL);
    int start = tvStart.tv_sec*1000000 + tvStart.tv_usec;
    int end = tvEnd.tv_sec*1000000 + tvEnd.tv_usec;
    if ((end - start) > 5000) {
        const Ice::ConnectionPtr con = ic.con;
        if (con.get() != NULL) {
            MCE_INFO(con->toString() << " Get SearchCache List2: time=" << end-start << "us " <<
                "us total=" << userIds.size());
        }
    }
    //MCE_INFO("[TEMPLE]CacheProxyManagerI::getSearchCacheList2 return value.size() is "<<result.size());
  return result;
}

void CacheProxyManagerI::load(const ::MyUtil::IntSeq& ids, const Ice::Current&) {
//	XceCacheAdapter::instance().Loads(ids);
  return;
}
void CacheProxyManagerI::update(const xce::searchcache::UpdateUnitSeq&units, const Ice::Current& ic) {
  return;
}
bool CacheProxyManagerI::puts(const xce::searchcache::Int2ByteSeq& id_2_byte_seq, const Ice::Current& ic) {
  return true;
}
bool CacheProxyManagerI::removes(const MyUtil::LongSeq& ids, const Ice::Current& ic) {
  return true;
}

bool CacheProxyManagerI::isValid(const Ice::Current&) {
	assert(cluster_ != 0);
	return XceCacheAdapter::instance().IsValid(mod_ % cluster_);
}

void CacheProxyManagerI::setValid(bool valid, const Ice::Current&) {
	assert(cluster_ != 0);
	XceCacheAdapter::instance().SetValid(mod_ % cluster_, valid);
}

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	TaskManager::instance().config(props, "Fill", TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
}
