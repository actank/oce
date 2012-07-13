#ifndef __VIRTUALID2RELATEDID_PRELOADER_H__
#define __VIRTUALID2RELATEDID_PRELOADER_H__

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
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class VirtualId2RelatedIdPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<VirtualId2RelatedIdPreloader> 
{
};

class VirtualId2RelatedIdPreloaderFactory : virtual public xce::generalpreloader::IdListFactory
{

public:

	virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserIdProducer : public virtual xce::generalpreloader::Producer 
{
	virtual std::vector<int> produce(const int beginId, const int load_batch_size, int& maxId ,int& tableMod);
};

class BatchUserIdHandlerI : public com::xiaonei::xce::ResultHandler
{
	public: 
		BatchUserIdHandlerI(MyUtil::IntSeq& results, Ice::Int& maxId);
		
		virtual bool handle(mysqlpp::Row& row) const; 
	
	private: 
		
		MyUtil::IntSeq& _results;
		Ice::Int& _maxId;
};

class BatchFilterDoubleAccountIdInfoHandlerI: public com::xiaonei::xce::ResultHandler
{
	public: 
		BatchFilterDoubleAccountIdInfoHandlerI(MyUtil::Int2IntMap& results);
		
		virtual bool handle(mysqlpp::Row& row) const; 
	
	private: 
		
		MyUtil::Int2IntMap& _results;
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
