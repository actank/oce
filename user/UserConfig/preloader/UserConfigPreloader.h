#ifndef __USERCONFIGPRELOADER_H__
#define __USERCONFIGPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <UserConfig.h>

namespace xce{
namespace userconfigpreloader{

const std::string TABLE_USER_CONFIG = "user_config";
const std::string DB_USER_CONFIG = "user_config_preloader";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserConfigPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserConfigPreloader> {
public:
        UserConfigPreloader () {}
        virtual ~UserConfigPreloader () {}
};

class UserConfigPreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserConfigPreloaderFactory() {}
        virtual ~UserConfigPreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserConfigPreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserConfigPreloaderResultHandler (MyUtil::ObjectResultPtr& results);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};

}
}

#endif
