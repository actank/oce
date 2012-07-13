#ifndef __USERCONTACTPRELOADER_H__
#define __USERCONTACTPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <UserDesc.h>

namespace xce {
namespace usercontactpreloader{

using namespace xce::userdesc;

const std::string TABLE_USER_CONTACT = "user_contact";
const std::string DB_USER_CONTACT = "user_contact_preloader";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserContactPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserContactPreloader> {
public:
        UserContactPreloader () {}
        virtual ~UserContactPreloader () {}
};

class UserContactPreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserContactPreloaderFactory() {}
        virtual ~UserContactPreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserContactPreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserContactPreloaderResultHandler (MyUtil::ObjectResultPtr& results);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};


}
}

#endif
