#ifndef __CACHE_ADAPTER_H__
#define __CACHE_ADAPTER_H__

#include <SearchCache.h>
#include "../Util/SearchCacheData.h"
#include <ServiceI.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <XceCacheAdapter.h>

using namespace MyUtil;
using namespace IceUtil;
using namespace com::xiaonei::xce;
using namespace xce::searchcache;

namespace xce {
namespace searchcache {

const Ice::Int SEARCH_CACHE = 0;
const Ice::Int BUCKET = 10;

const std::string SEARCH_CACHE_MANAGER = "M";

const int TASK_LEVEL_FILL = 1;
const int TASK_LEVEL_UPDATE_DBCACHE = 9;

//***************************************************************************

class CacheProxyManagerI: virtual public SearchCacheManager,
		public MyUtil::Singleton<CacheProxyManagerI> {
public:
	CacheProxyManagerI();
	virtual SearchCacheResult getSearchCache(::Ice::Int, const Ice::Current&);
	virtual Int2SearchCacheResultMap getSearchCacheList(const ::MyUtil::IntSeq&,
			const Ice::Current&);

  virtual Int2ByteSeq getSearchCacheList2(const ::MyUtil::IntSeq& userIds, 
      const Ice::Current& ic);
	virtual void load(const ::MyUtil::IntSeq&, const Ice::Current&);
	virtual bool isValid(const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const Ice::Current& = Ice::Current());
  virtual void update(const xce::searchcache::UpdateUnitSeq&, const Ice::Current&);
  virtual bool puts(const xce::searchcache::Int2ByteSeq&, const Ice::Current&);
  virtual bool removes(const MyUtil::LongSeq&, const Ice::Current&);

	void bootstrap(int cluster, int mod, const string &dsn);

private:

	int cluster_;
	int mod_;
	string dsn_;
	
};

class ServiceSchemaI: public MyUtil::Descriptor {
	public:
		virtual void configure(const Ice::PropertiesPtr& props);
};

//***************************************************************************

}
}
#endif
