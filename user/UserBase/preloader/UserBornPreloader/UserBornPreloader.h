#ifndef __USERBORNPRELOADER_H__
#define __USERBORNPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <UserBase.h>

namespace xce {
namespace userbornpreloader{

using namespace xce::userbase;

const std::string TABLE_USER_BORN = "user_born";
const std::string DB_USER_BORN = "user_born_preloader";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserBornPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserBornPreloader> {
public:
        UserBornPreloader () {}
        virtual ~UserBornPreloader () {}
};

class UserBornPreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserBornPreloaderFactory() {}
        virtual ~UserBornPreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserBornPreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserBornPreloaderResultHandler (MyUtil::ObjectResultPtr& results);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};


}
}

#endif
