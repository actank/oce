#ifndef CELL_SOCIALGRAPH_MCADAPTER_H__
#define CELL_SOCIALGRAPH_MCADAPTER_H__

#include <boost/thread/mutex.hpp>

#include "socialgraph/socialgraphutil/zkclient.h"
#include "distcache/DistCacheAdapter.h"

namespace xce {
namespace socialgraph {


struct BizType {
  BizType(std::string& biz, int type, xce::distcache::DistCacheAdapterPtr& adapter) {
		biz_ = biz;
    type_ = type;
    dist_cache_adapter_ = adapter;
  }
	std::string biz_;
  int type_;
	xce::distcache::DistCacheAdapterPtr dist_cache_adapter_;
};

class MemcachedAdapter : public xce::Observer {
public:
	static MemcachedAdapter& GetInstance();
  MemcachedAdapter();
  virtual ~MemcachedAdapter();

  bool Set(const std::string&, const std::vector<unsigned char>&);
  bool Get(const std::string&, std::vector<unsigned char>&);

  /* @brief Set2没有移位操作key = 业务＋long */
  bool Set2(const std::string&, const std::vector<unsigned char>&);

  /* @brief Get2没有移位操作key = 业务＋long */
  bool Get2(const std::string&, std::vector<unsigned char>&);

	virtual void Update(const std::string&, const std::string&);

private:
  void Init();
private:
  std::map<std::string, BizType> biz_map_;
  boost::shared_mutex rw_mutex_;
};

//----------------  
class BusinessCacheAdapter {
 public:
  static BusinessCacheAdapter& GetInstance();
  BusinessCacheAdapter();
  virtual ~BusinessCacheAdapter();
  bool Get(const std::string& biz, int64_t key, std::vector<unsigned char>& data);
  bool Set(const std::string& biz,int64_t key, const std::vector<unsigned char>& data);
 private:
  void Init();
  void InitBiz();
 private:
  typedef std::map<std::string, xce::distcache::DistCacheAdapterPtr> DC_AdapterMap; 
  DC_AdapterMap dc_adapters_;
  std::vector<std::string> bizs_;
};


}}
#endif

