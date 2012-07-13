#ifndef CELL_SOCIALGRAPH_MCCLIENT_H__
#define CELL_SOCIALGRAPH_MCCLIENT_H__
#include "socialgraph/socialgraphutil/zkclient.h"
//#include "cache/client/consistenthash.h"

namespace xce {

class MemcachedClient {
public:
	static MemcachedClient& GetInstance();
	MemcachedClient() { }
	virtual ~MemcachedClient() { }
  // 使用distcache1.0 
	bool SetByDistCache(const std::string& mckey, const std::vector<unsigned char>& data);
  // 使用distcache1.0
	bool GetByDistCache(const std::string& mckey, std::vector<unsigned char>& data);

  // 使用distcache1.0 业务＋long 没有移位操作
	bool SetByDistCache2(const std::string& mckey, const std::vector<unsigned char>& data);
  // 使用distcache1.0 业务＋long 没有移位操作
	bool GetByDistCache2(const std::string& mckey, std::vector<unsigned char>& data);
};

}
#endif
