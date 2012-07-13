/*
 * =====================================================================================
 *
 *       Filename:  DemoDistCacheWrapper.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年01月27日 11时21分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "DemoDistCacheWrapper.h"

int main() {
	xce::distcache::DemoDistCacheWrapperPtr cache_ = new xce::distcache::DemoDistCacheWrapper();
	cache_->initialize();

	cache_ -> read(233940269);
}
