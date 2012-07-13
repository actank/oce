#include "socialgraph/socialgraphutil/mcclient.h"
#include "socialgraph/socialgraphutil/mc_adapter.h"
#include "base/once.h"
#include "base/logging.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <libmemcached/memcached.h>

namespace xce {

static const uint32_t kFlags = 4096;

static const std::string kZNodeMemchaced = "/MEMCACHED";

static MemcachedClient* mc = 0;
static void InitMemcachedClient() {
  mc = new MemcachedClient;
}

MemcachedClient& MemcachedClient::GetInstance() {
  static XCE_DECLARE_ONCE(once_guard_); XceOnceInit(&once_guard_, &InitMemcachedClient);
  return *mc;
}

//使用distcache后的Set包装方法
bool MemcachedClient::SetByDistCache(const std::string& mckey, const std::vector<unsigned char>& data) {
	return xce::socialgraph::MemcachedAdapter::GetInstance().Set(mckey, data);
}

//使用distcache后的Get包装方法
bool MemcachedClient::GetByDistCache(const std::string& mckey, std::vector<unsigned char>& data) {
	return xce::socialgraph::MemcachedAdapter::GetInstance().Get(mckey, data);
}

// 使用distcache1.0 业务＋long 没有移位操作
bool MemcachedClient::SetByDistCache2(const std::string& mckey, const std::vector<unsigned char>& data) {
	return xce::socialgraph::MemcachedAdapter::GetInstance().Set2(mckey, data);
}

// 使用distcache1.0 业务＋long 没有移位操作
bool MemcachedClient::GetByDistCache2(const std::string& mckey, std::vector<unsigned char>& data) {
	return xce::socialgraph::MemcachedAdapter::GetInstance().Get2(mckey, data);
} 

} //namespace xce end!


