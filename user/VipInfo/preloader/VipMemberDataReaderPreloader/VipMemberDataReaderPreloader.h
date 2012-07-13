#ifndef __VIP_MEMBER_DATA_READER_PRELOADER_H__
#define __VIP_MEMBER_DATA_READER_PRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "GeneralPreloader/src/GeneralPreloaderProducer.h"
#include <VipInfo.h>

#include <user/VipInfo/share/VipInfoDataI.h>

namespace xce {
namespace vipmemberdatareaderpreloader{

const std::string DB_MEMBERS = "vipmember_preloader";

const std::string TABLE_VIP_MEMBER = "user_member_info";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 10;

class VipMemberDataReaderPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<VipMemberDataReaderPreloader> 
{
};

class VipMemberDataReaderPreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory
{

public:

	virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchVipMemberIdProducer : public virtual xce::generalpreloader::Producer 
{
	virtual std::vector<int> produce(const int beginId, const int load_batch_size, int& maxId ,int& tableMod);
};

class BatchVipMemberIdHandlerI: public com::xiaonei::xce::ResultHandler
{
	public: 
		BatchVipMemberIdHandlerI( std::vector<int>& results, Ice::Int& maxId );
		
		virtual bool handle(mysqlpp::Row& row) const; 
	
	private: 
		
		std::vector<int>& _results;
		Ice::Int& _maxId;
};

}
}

#endif
