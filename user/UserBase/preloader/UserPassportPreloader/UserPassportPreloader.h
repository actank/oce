#ifndef __USERPASSPORTPRELOADER_H__
#define __USERPASSPORTPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <UserBase.h>

namespace xce {
namespace userpassportpreloader{

using namespace xce::userbase;

const std::string TABLE_USER_PASSPORT = "user_passport";
const std::string DB_USER_PASSPORT = "user_passport_preloader";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserPassportPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserPassportPreloader> {
public:
        UserPassportPreloader () {}
        virtual ~UserPassportPreloader () {}
};

class UserPassportPreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserPassportPreloaderFactory() {}
        virtual ~UserPassportPreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserPassportPreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserPassportPreloaderResultHandler (MyUtil::ObjectResultPtr& results);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};


}
}

#endif
