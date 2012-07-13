#ifndef __USERLOGINCOUNTPRELOADER_H__
#define __USERLOGINCOUNTPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <UserBase.h>

namespace xce {
namespace userlogincountpreloader{

using namespace xce::userbase;

const std::string TABLE_USER_LOGINCOUNT= "user_logincount";
const std::string DB_USER_LOGINCOUNT= "user_logincount_preloader";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserLoginCountPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserLoginCountPreloader> {
public:
        UserLoginCountPreloader () {}
        virtual ~UserLoginCountPreloader () {}
};

class UserLoginCountPreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserLoginCountPreloaderFactory() {}
        virtual ~UserLoginCountPreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserLoginCountPreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserLoginCountPreloaderResultHandler (MyUtil::ObjectResultPtr& results);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};


}
}

#endif
