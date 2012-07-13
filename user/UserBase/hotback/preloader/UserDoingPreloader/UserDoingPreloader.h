#ifndef __USERDOINGPRELOADER_H__
#define __USERDOINGPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <UserBase.h>

namespace xce {
namespace userdoingpreloader{

using namespace xce::userbase;

const std::string TABLE_USER_DOING = "user_doing";
const std::string DB_USER_DOING = "user_doing_preloader";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserDoingPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserDoingPreloader> {
public:
        UserDoingPreloader () {}
        virtual ~UserDoingPreloader () {}
};

class UserDoingPreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserDoingPreloaderFactory() {}
        virtual ~UserDoingPreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserDoingPreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserDoingPreloaderResultHandler (MyUtil::ObjectResultPtr& results);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};


}
}

#endif
