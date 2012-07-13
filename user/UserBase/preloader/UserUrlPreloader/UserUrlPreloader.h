#ifndef __USERURLPRELOADER_H__
#define __USERURLPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <UserBase.h>

namespace xce {
namespace userurlpreloader{

using namespace xce::userbase;

const std::string TABLE_USER_URL = "user_url";
const std::string DB_USER_URL = "user_url_preloader";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserUrlPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserUrlPreloader> {
public:
        UserUrlPreloader () {}
        virtual ~UserUrlPreloader () {}
};

class UserUrlPreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserUrlPreloaderFactory() {}
        virtual ~UserUrlPreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserUrlPreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserUrlPreloaderResultHandler (MyUtil::ObjectResultPtr& results);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};


}
}

#endif
