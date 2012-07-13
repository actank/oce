#ifndef __DOUBLEACCOUNTID2RELATEDID_PRELOADER_H__
#define __DOUBLEACCOUNTID2RELATEDID_PRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "GeneralPreloader/src/GeneralPreloaderProducer.h"
#include <IdCache.h>

#include <user/IdCache/share/IdCacheDataI.h>

namespace idcache
{

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 1;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class DoubleAccountId2RelatedIdPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<DoubleAccountId2RelatedIdPreloader> 
{
};

class DoubleAccountIdInfo : public Ice::Object
{
	public:
		int userId0;
		int userId1;
};
typedef IceUtil::Handle<DoubleAccountIdInfo> DoubleAccountIdInfoPtr;

class DoubleAccountId2RelatedIdPreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory
{

public:

	virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchGroupIdProducer : public virtual xce::generalpreloader::Producer 
{
	virtual std::vector<int> produce(const int beginId, const int load_batch_size, int& maxId ,int& tableMod);
};

class BatchGroupIdHandlerI : public com::xiaonei::xce::ResultHandler
{
	public: 
		BatchGroupIdHandlerI(MyUtil::IntSeq& results, Ice::Int& maxId);
		
		virtual bool handle(mysqlpp::Row& row) const; 
	
	private: 
		
		MyUtil::IntSeq& _results;
		Ice::Int& _maxId;
};

class BatchDoubleAccountIdInfoHandlerI: public com::xiaonei::xce::ResultHandler
{
	public: 
		BatchDoubleAccountIdInfoHandlerI(MyUtil::ObjectResultPtr results);
		
		virtual bool handle(mysqlpp::Row& row) const; 
	
	private: 
		
		MyUtil::ObjectResultPtr _results;
};

class BatchVirtualIdInfoHandlerI: public com::xiaonei::xce::ResultHandler
{
	public: 
		BatchVirtualIdInfoHandlerI(MyUtil::ObjectResultPtr results);
		
		virtual bool handle(mysqlpp::Row& row) const; 
	
	private: 
		
		MyUtil::ObjectResultPtr _results;
};


}

#endif
