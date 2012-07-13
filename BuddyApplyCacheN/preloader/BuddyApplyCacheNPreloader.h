#ifndef _BUDDYAPPLYCACHENPRELOAD_H_
#define _BUDDYAPPLYCACHENPRELOAD_H_

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <BuddyApplyCache.h>
#include <BuddyApplyCacheN/share/BuddyApplyCacheNData.h>

namespace xce {
namespace buddyapplycache {

	const int PRODUCE_BATCH_SIZE = 1000;
	const int CONSUME_BATCH_SIZE = 2000;
	const int CONSUME_BATCH_THREAD_SIZE = 5;
	const int WRITE_BATCH_SIZE = 1000;
	const int PRX_DATAQUEUE_MAX_SIZE = 100;

	class BuddyApplyCacheNPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<BuddyApplyCacheNPreloader> 
	{
		public:
			BuddyApplyCacheNPreloader () {}
			virtual ~BuddyApplyCacheNPreloader () {}
	};

	class BuddyApplyCacheNPreloaderFactory : virtual public xce::generalpreloader::IdListFactory
	{
		public:
			BuddyApplyCacheNPreloaderFactory() {}
			virtual ~BuddyApplyCacheNPreloaderFactory() {}
			virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	};

	class BatchBuddyApplyCacheNPreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
		public:
			BatchBuddyApplyCacheNPreloaderResultHandler (MyUtil::ObjectResultPtr& results);
			virtual bool handle(mysqlpp::Row& row) const;
		private:
			MyUtil::ObjectResultPtr _results;
	};
}
}

#endif

