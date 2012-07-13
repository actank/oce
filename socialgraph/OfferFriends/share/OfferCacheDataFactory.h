#ifndef __OFFER_CACHE_DATA_FACTORY_H__
#define __OFFER_CACHE_DATA_FACTORY_H__

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "UtilCxx/src/Singleton.h"
#include "ReplicaCluster/CacheLoader/src/CacheLoaderFactory.h"

namespace xce {
namespace offerfriends {

/*****************************************************************************/
/*这个用来干嘛呢？？*/
class OfferCacheDataFactory: virtual public xce::cacheloader::MinMaxFactory, 
		virtual public MyUtil::Singleton<OfferCacheDataFactory>  {
protected:
        friend class MyUtil::Singleton<OfferCacheDataFactory>;
public:
        Ice::ObjectPtr create(Ice::Int id);
        virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
};

/*****************************************************************************/

class OfferResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        OfferResultHandler(const  MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~OfferResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

/*****************************************************************************/

}
}

#endif
