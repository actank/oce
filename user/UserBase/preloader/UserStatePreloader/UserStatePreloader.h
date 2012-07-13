#ifndef __USERSTATEPRELOADER_H__
#define __USERSTATEPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <UserBase.h>

namespace xce {
namespace userstatepreloader{

using namespace xce::userbase;

const std::string TABLE_USER_STATE = "user_state";
const std::string DB_USER_STATE = "user_state_preloader";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserStatePreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserStatePreloader> {
public:
        UserStatePreloader () {}
        virtual ~UserStatePreloader () {}
};

class UserStatePreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserStatePreloaderFactory() {}
        virtual ~UserStatePreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserStatePreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserStatePreloaderResultHandler (MyUtil::ObjectResultPtr& results);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};


}
}

#endif
