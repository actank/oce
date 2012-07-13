/*
 * =====================================================================================
 *
 *       Filename:  DemoDistCacheWrapper.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年01月27日 11时04分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __DEMO_DIST_CACHE_WRAPPER_H__
#define __DEMO_DIST_CACHE_WRAPPER_H__

#include "../src/DistCacheWrapper.h"

namespace xce {
namespace distcache {

using namespace xce::distcache;

static const std::string NAME = "DEMO";

class DemoData {
public:
	DemoData() {}
	~DemoData() {}

	void SerializeToString(std::string* data) {}
	void ParseFromString(std::string data) {}
};

class DemoDistCacheWrapper : virtual public DistCacheWrapper<DemoData>, public IceUtil::Shared {
public:
	DemoDistCacheWrapper() : DistCacheWrapper<DemoData>(NAME) {}

	virtual DemoData* create(int64_t id) {
	}
};//end DemoDistCacheReloader

typedef IceUtil::Handle<DemoDistCacheWrapper> DemoDistCacheWrapperPtr;

}
}
#endif
