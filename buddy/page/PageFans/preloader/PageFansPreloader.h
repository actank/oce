#ifndef _PAGEFANSPRELOAD_H_
#define _PAGEFANSPRELOAD_H_

#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "GeneralPreloader/src/GeneralPreloader.h"

#include <IceUtil/IceUtil.h>
#include "ServiceI.h"
#include "PageFans.h"
#include "../data/PageFansCommon.h"
namespace xce {
namespace buddy{

	const int PRODUCE_BATCH_SIZE = 1;
	const int CONSUME_BATCH_SIZE = 1;
	const int CONSUME_BATCH_THREAD_SIZE = 1;
	const int WRITE_BATCH_SIZE = 1;
	const int PRX_DATAQUEUE_MAX_SIZE = 100;


	class BatchPageIdHandlerI: public com::xiaonei::xce::ResultHandler {
			public:
					BatchPageIdHandlerI( std::vector<int>& results, Ice::Int& maxId ) :
							_results(results), _maxId(maxId) {
							}
					virtual bool handle(mysqlpp::Row& row) const {
							int id = (int) row["id"];
							_maxId = id > _maxId ? id : _maxId;
							_results.push_back( id );
							return true;
					}
			private:
					std::vector<int>& _results;
					Ice::Int& _maxId;
	};

	class BatchPageIdProducer : public virtual xce::generalpreloader::Producer {
			virtual std::vector<int> produce(int beginId, int load_batch_size, int& maxId , int& tableMod) {
					std::vector<int> results;
					Statement sql;
					sql << "SELECT id FROM page WHERE id > " << beginId
							<< " ORDER BY id LIMIT " << load_batch_size;
					BatchPageIdHandlerI handler( results, maxId );
					QueryRunner( "xce_page" , CDbRServer ).query( sql, handler );
					return results;
			}
	       ~BatchPageIdProducer() {}
	};



	class PageFansPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<PageFansPreloader> 
	{
		public:
			PageFansPreloader () {}
			virtual ~PageFansPreloader () {}
	};


	class PageFansPreloaderFactory : virtual public xce::generalpreloader::IdListFactory
	{
		public:
			PageFansPreloaderFactory() {}
			virtual ~PageFansPreloaderFactory() {}
			virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
			

	};


}
}

#endif

