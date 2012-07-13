#ifndef __USERSTAGEPRELOADER_H__
#define __USERSTAGEPRELOADER_H_

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <UserBase.h>

namespace xce {
namespace userstagepreloader{

using namespace xce::userbase;

const std::string TABLE_USER_STAGE = "user_stage";
const std::string DB_USER_STAGE = "user_stage_preloader";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserStagePreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserStagePreloader> {
public:
        UserStagePreloader () {}
        virtual ~UserStagePreloader () {}
};

class UserStagePreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserStagePreloaderFactory() {}
        virtual ~UserStagePreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserStagePreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserStagePreloaderResultHandler (MyUtil::ObjectResultPtr& results);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};


}
}

#endif
