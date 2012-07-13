#ifndef ZOIE_UPDATER_H__
#define ZOIE_UPDATER_H__
#include <vector>
#include <queue>
#include <boost/shared_ptr.hpp>
#include "ServiceI.h"
#include "IceUtil/IceUtil.h"
#include "QueryRunner.h"

namespace search {
namespace update {
class ZoieUpdater : public MyUtil::Singleton<ZoieUpdater>,IceUtil::Thread{
	public:
		ZoieUpdater();
		void insert(int,int);
		void insertBatch(const vector<long>&,int);
		void run();
	private:
		vector<pair<int,int> > buffer_;
		IceUtil::Monitor<IceUtil::Mutex> mutex_;
		time_t last_update_;
};

};
};
#endif
