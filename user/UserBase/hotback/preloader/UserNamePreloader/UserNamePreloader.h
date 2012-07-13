#ifndef __USERNAMEPRELOADER_H__
#define __USERNAMEPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <UserBase.h>

namespace xce {
namespace usernamepreloader{

using namespace xce::userbase;

const std::string TABLE_USER_NAME = "user_names";
const std::string DB_USER_NAME = "user_names_preloader";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserNamePreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserNamePreloader> {
public:
        UserNamePreloader () {}
        virtual ~UserNamePreloader () {}
};

class UserNamePreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserNamePreloaderFactory() {}
        virtual ~UserNamePreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserNamePreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserNamePreloaderResultHandler (MyUtil::ObjectResultPtr& results);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};


}
}

#endif
