/*
 * =====================================================================================
 *
 *       Filename:  DistCacheWrapper.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年01月26日 15时07分08秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __DIST_CACHE_WRAPPER_H__
#define __DIST_CACHE_WRAPPER_H__

#include "./distcache/DistCacheAdapter.h"
#include "LogWrapper.h"

namespace xce {
namespace distcache {

using namespace xce::distcache;

/**
 *通用cache封装基类
 */
template<class T, class TPtr>
class DistCacheWrapper : virtual public Ice::Object {
public:
	DistCacheWrapper(std::string name) : name_(name) {}
	~DistCacheWrapper() {}

	virtual TPtr create(int64_t id) = 0;

	void initialize() {
		try {
			while(NULL == (dist_cache_ = DistCacheAdapter::connect(name_))) {
				MCE_INFO("DistCacheReloader, name: " << name_ << " connecting distcache ...");
			}
			MCE_INFO("DistCacheReloader, name: " << name_ << " connecting distcache is success!");
		} catch (...) {
			MCE_WARN("DistCacheReloader::initialize is fail!");
		}
	}

	TPtr read (int64_t id) {
		DataPtr data = dist_cache_ ->get(id);
		if (0 == data->size()) {
			TPtr proto_data = create(id);
			std::string mem_data_str;
			proto_data->SerializeToString(&mem_data_str);
			DataPtr result(new Data( mem_data_str.begin(), mem_data_str.end() ));
			dist_cache_ -> set(id, result);
			return proto_data;
		} else {
			std::string data_str(data->begin(), data->end());
			TPtr proto_data = new T();
			proto_data->ParseFromString(data_str);
			return  proto_data;
		}
	}

	bool write (int64_t id, T data) {
		std::string mem_data_str;
		data->SerializeToString(&mem_data_str);
		DataPtr result(new Data( mem_data_str.begin(), mem_data_str.end() ));
		dist_cache_ -> set(id, result);
		return true;
	}

private:
	std::string name_;
	DistCacheAdapterPtr dist_cache_;
};//end DistCacheWrapper
//typedef IceUtil::Handle<DistCacheWrapper<T,TPtr>> DistCacheWrapperPtr;

}
}
#endif
