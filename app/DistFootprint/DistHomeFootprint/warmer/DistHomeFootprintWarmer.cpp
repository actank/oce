/*
 * =====================================================================================
 *
 *       Filename:  DistHomeFootprintWarmer.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年02月25日 19时17分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "../wrapper/DistHomeFootprintWrapper.h"
#include "./DistCacheWrapper/src/DistCacheWarmer.h"
using namespace xce::footprint;
using namespace xce::distcache;

int main() {
	try {
		HomeFootprintWrapper* wrapper_ = new HomeFootprintWrapper();
		DistCacheWarmer<HomeFootprintDataI, HomeFootprintDataIPtr>* warmer_ = new DistCacheWarmer<HomeFootprintDataI, HomeFootprintDataIPtr>(wrapper_, 10, 0);
		warmer_ -> warm();
	} catch (...) {
	}
	exit(0);
}
