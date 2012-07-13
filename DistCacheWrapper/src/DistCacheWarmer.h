/*
 * =====================================================================================
 *
 *       Filename:  DistCacheWarmer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年02月25日 15时45分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __DIST_CACHE_WARMER_H__
#define __DIST_CACHE_WARMER_H__

#include "DistCacheIdProducer.h"
#include "DistCacheWrapper.h"

namespace xce {
namespace distcache {

using namespace xce::distcache;

const int NO_WARMER = 0;
const int ACTIVE_WARMER = 1;
const int FULL_WARMER = 2;

template <class T, class TPtr>
class DistCacheWarmer : virtual Ice::Object {
public:
	DistCacheWarmer(DistCacheWrapper<T, TPtr>* wrapper, int cluster, int mod, int warmer_mode = ACTIVE_WARMER) : wrapper_(wrapper), cluster_(cluster), mod_(mod), warmer_mode_(warmer_mode_) {}
	//~DistCacheWarmer() {}

	void warm() {
		IDProducerPtr producer_ = NULL;
		if (NO_WARMER == warmer_mode_) {
			return;
		} else if (ACTIVE_WARMER == warmer_mode_) {
			producer_ = new ActiveIDProducer(cluster_, mod_);
		} else {
			producer_ = new FullIDProducer(cluster_, mod_);
		}
		
		com::xiaonei::xce::ConnectionPoolManager::instance().initialize();
		vector<int> ids_;
		int begin_id_ = 0;
		int max_id_ = 0;
		do {
			begin_id_ = max_id_;
			ids_ = producer_ -> produce(begin_id_, max_id_);
			for(vector<int>::iterator it = ids_.begin(); it != ids_.end(); ++it) {
				wrapper_ -> read(*it);
			}
		} while (!ids_.empty());
	}
private:
	DistCacheWrapper<T, TPtr>* wrapper_;
	int cluster_;
	int mod_;
	int warmer_mode_;	

};

}
}
#endif
