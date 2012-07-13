#ifndef __USERRIGHTPRELOADER_H__
#define __USERRIGHTPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <UserBase.h>

namespace xce {
namespace userrightpreloader{

using namespace xce::userbase;

const std::string TABLE_USER_RIGHT = "user_right";
const std::string DB_USER_RIGHT = "user_right_preloader";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserRightPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserRightPreloader> {
public:
        UserRightPreloader () {}
        virtual ~UserRightPreloader () {}
};

class UserRightPreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserRightPreloaderFactory() {}
        virtual ~UserRightPreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserRightPreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserRightPreloaderResultHandler (MyUtil::ObjectResultPtr& results);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};


}
}

#endif
