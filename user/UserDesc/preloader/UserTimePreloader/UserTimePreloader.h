#ifndef __USERTIMEPRELOADER_H__
#define __USERTIMEPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <UserDesc.h>

namespace xce {
namespace usertimepreloader{

using namespace xce::userdesc;

const std::string TABLE_USER_TIME = "user_time";
const std::string DB_USER_TIME = "user_time_preloader";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserTimePreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserTimePreloader> {
public:
        UserTimePreloader () {}
        virtual ~UserTimePreloader () {}
};

class UserTimePreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserTimePreloaderFactory() {}
        virtual ~UserTimePreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserTimePreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserTimePreloaderResultHandler (MyUtil::ObjectResultPtr& results);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};


}
}

#endif
