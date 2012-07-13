#ifndef __ICON2URL_DATA_READER_PRELOADER_H__
#define __ICON2URL_DATA_READER_PRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "GeneralPreloader/src/GeneralPreloaderProducer.h"
#include <VipInfo.h>

#include <user/VipInfo/share/VipInfoDataI.h>

namespace xce {
namespace icon2urldatareaderpreloader{

const std::string DB_MEMBERS = "vipmember_preloader";

const std::string TABLE_ICON_URL = "icon_url";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class Icon2UrlDataReaderPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<Icon2UrlDataReaderPreloader> 
{
};

class Icon2UrlDataReaderPreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory
{

public:

	virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchIconsIdProducer : public virtual xce::generalpreloader::Producer 
{
	virtual std::vector<int> produce(const int beginId, const int load_batch_size, int& maxId ,int& tableMod);
};

}
}

#endif
