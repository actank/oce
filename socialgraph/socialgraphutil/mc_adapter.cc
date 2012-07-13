#include "socialgraph/socialgraphutil/mc_adapter.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>

#include "base/logging.h"
#include "base/once.h"


namespace xce {
namespace socialgraph {

static const std::string kZNodeReconnect = "/BIZ";

static MemcachedAdapter* ma_ = 0;
static void InitMemcachedAdapter() {
  ma_ = new MemcachedAdapter;
}

MemcachedAdapter& MemcachedAdapter::GetInstance() {
  using namespace xce;
  static XCE_DECLARE_ONCE(once_guard_);
  XceOnceInit(&once_guard_, &InitMemcachedAdapter);
  return *ma_;
}

MemcachedAdapter::MemcachedAdapter() {
	xce::ZooConfig::GetInstance().Attach(this);
	Init();
}

MemcachedAdapter::~MemcachedAdapter() {
	xce::ZooConfig::GetInstance().Detach(this);
}

void MemcachedAdapter::Init() {
	std::string v;
  if (!xce::ZooConfig::GetInstance().Get(kZNodeReconnect, &v)) {
		LOG(LERROR) << "MemcachedAdapter::Init() get zk path:/BIZ failed!";
		sleep(1);
		Init();
	}
  else {
		Update(kZNodeReconnect, v);
	}
}
//------------------------------------
bool MemcachedAdapter::Set(const std::string& mckey, const std::vector<unsigned char>& data) {
  bool ret = false;
  std::vector<std::string> units;
	boost::split(units, mckey, boost::is_any_of("_"));
  if (units.size() != 2)
		return false;
  std::string biz_name;
  int userId;
  const int shift = 32;  // long 高4位放BizType.type_ 低4位放user id

  boost::shared_lock<boost::shared_mutex> read_lock(rw_mutex_);
  try {
    biz_name = units[0]; 
		std::map<std::string, BizType>::const_iterator cit = biz_map_.find(biz_name);
    if (biz_map_.end() == cit)
			throw 47;
    long type = (long)(cit->second.type_);
		userId = boost::lexical_cast<int>(units[1]);
		long k = (type << shift) + userId; //注意: 为了distcache使用!
    
		xce::distcache::DataPtr data_ptr(new std::vector<unsigned char>(data));
    ret = cit->second.dist_cache_adapter_->set(k, data_ptr);
	} catch (...) {
    ret = false;
	}
	return ret;
	
}

bool MemcachedAdapter::Get(const std::string& mckey, std::vector<unsigned char>& data) {
  bool ret = false;
  std::vector<std::string> units;
	boost::split(units, mckey, boost::is_any_of("_"));
  if (units.size() != 2)
		return ret;
  std::string biz_name;
  int userId;
  const int shift = 32;  // long 高4位放BizType.type_ 低4位放user id

  boost::shared_lock<boost::shared_mutex> read_lock(rw_mutex_);
  try {
    biz_name = units[0]; 
		std::map<std::string, BizType>::const_iterator cit = biz_map_.find(biz_name);
    if (biz_map_.end() == cit)
			throw 47;
    long type = (long)(cit->second.type_);
		userId = boost::lexical_cast<int>(units[1]);
		long k = (type << shift) + userId; //注意: 为了distcache使用!

		xce::distcache::DataPtr data_ptr = cit->second.dist_cache_adapter_->get(k);
    if (!data_ptr) {
      return false;
    }
		data_ptr->swap(data);

    ret = true;
	} catch (...) {
	  ret = false;
	}
	return ret;
}

bool MemcachedAdapter::Set2(const std::string& mckey, const std::vector<unsigned char>& data) {
  bool ret = false;
  std::vector<std::string> units;
	boost::split(units, mckey, boost::is_any_of("_"));
  if (units.size() != 2)
		return false;
  std::string biz_name;
  long userId;

  boost::shared_lock<boost::shared_mutex> read_lock(rw_mutex_);
  try {
    biz_name = units[0]; 
		std::map<std::string, BizType>::const_iterator cit = biz_map_.find(biz_name);
    if (biz_map_.end() == cit)
			throw 47;
		userId = boost::lexical_cast<long>(units[1]);
    
		xce::distcache::DataPtr data_ptr(new std::vector<unsigned char>(data));
    ret = cit->second.dist_cache_adapter_->set(userId, data_ptr);
	} catch (...) {
    ret = false;
	}
	return ret;
}

bool MemcachedAdapter::Get2(const std::string& mckey, std::vector<unsigned char>& data) {
  bool ret = false;
  std::vector<std::string> units;
	boost::split(units, mckey, boost::is_any_of("_"));
  if (units.size() != 2)
		return ret;
  std::string biz_name;
  long userId;

  boost::shared_lock<boost::shared_mutex> read_lock(rw_mutex_);
  try {
    biz_name = units[0]; 
		std::map<std::string, BizType>::const_iterator cit = biz_map_.find(biz_name);
    if (biz_map_.end() == cit)
			throw 47;
		userId = boost::lexical_cast<long>(units[1]);

		xce::distcache::DataPtr data_ptr = cit->second.dist_cache_adapter_->get(userId);
    if (!data_ptr) {
      return false;
    }
		data_ptr->swap(data);

    ret = true;
	} catch (...) {
	  ret = false;
	}
	return ret;
}

void MemcachedAdapter::Update(const std::string& key, const std::string& value) {
 	if (key == kZNodeReconnect) {
		LOG(INFO) << "MemcachedAdapter::Update() key:" << key << " value:" << value;

		std::vector<std::string> units;
		boost::split(units, value, boost::is_any_of("#"));
		bool ec = false;
		std::vector<std::string>::iterator iter = units.begin();
		std::map<std::string, BizType> target;
		for (; iter != units.end(); ++iter) {
			std::vector<std::string> line;
			boost::split(line, *iter, boost::is_any_of(","));
			try {
				std::string biz = line[0];
        int type = boost::lexical_cast<int>(line[1]);
				xce::distcache::DistCacheAdapterPtr dist_cache_adapter((xce::distcache::DistCacheAdapter*)NULL);
				int loop = 0;
				while (NULL == dist_cache_adapter.get()) {
					dist_cache_adapter = xce::distcache::DistCacheAdapter::connect(biz);
					if (NULL == dist_cache_adapter.get()) {
						LOG(LERROR) << "DistCacheAdapter::connect() failed! To reconnect is in 50ms from now! biz:" << biz;
						boost::this_thread::sleep(boost::posix_time::milliseconds(50));	
					}
          else {
            BizType bt(biz, type, dist_cache_adapter);
            target.insert(std::make_pair(biz, bt));
          }
					if (++loop > 10) 	
						throw 47;
				}	
			} catch (int) {
    		//...
				continue;
      } catch (...) {
				ec = true;
				break;
			}
		}
		if (!ec && target.size() != 0) {
      boost::unique_lock<boost::shared_mutex> write_lock(rw_mutex_);
			biz_map_.swap(target);
			LOG(INFO) << "MemcachedAdapter::Update() rebuild";
		}
	} 
}

//---------------------------------------------------------
static BusinessCacheAdapter* biz_cache_adapter_instance = 0;
static void InitBusinessCacheAdapter() {
  biz_cache_adapter_instance = new BusinessCacheAdapter();
}

BusinessCacheAdapter& BusinessCacheAdapter::GetInstance() {
  using namespace xce;
  static XCE_DECLARE_ONCE(once_guard_);
  XceOnceInit(&once_guard_, &InitBusinessCacheAdapter);
  return *biz_cache_adapter_instance;
}

BusinessCacheAdapter::BusinessCacheAdapter() {
  Init();
}

void BusinessCacheAdapter::Init() {
  InitBiz();
  BOOST_FOREACH(const std::string& biz, bizs_) {
    xce::distcache::DistCacheAdapterPtr dc_adapter = xce::distcache::DistCacheAdapter::connect(biz);
    assert(dc_adapter.get() != NULL);
    dc_adapters_.insert(std::make_pair(biz, dc_adapter));
  }
}

BusinessCacheAdapter::~BusinessCacheAdapter() {

}

void BusinessCacheAdapter::InitBiz() {
  bizs_.push_back("RCD");
  bizs_.push_back("FFUCR");
  bizs_.push_back("FFW");
  bizs_.push_back("FFNCW");
  bizs_.push_back("FFWR");
  bizs_.push_back("FFAG");
  bizs_.push_back("OFSA");
  bizs_.push_back("BLOG");
  bizs_.push_back("BLOGTAG");
  bizs_.push_back("BLOGACTION");
  bizs_.push_back("PAGE");
  bizs_.push_back("HOTPAGE");
  bizs_.push_back("RCDBLOG");
  bizs_.push_back("RCDVIDEO");
  bizs_.push_back("PUK");		//People U may know module in aggregation show of recommendation
  //bizs_.push_back("USERFEATURE");
}

bool BusinessCacheAdapter::Get(const std::string& biz, int64_t key, std::vector<unsigned char>& data) {
  DC_AdapterMap::iterator iter = dc_adapters_.find(biz);
  if (iter == dc_adapters_.end()) {
    return false;
  } 
  try {
    xce::distcache::DataPtr data_ptr = iter->second->get(key);
    if (!data_ptr) {
      return false;
    }
    data_ptr->swap(data);
    return true;
  } catch (...) {
    return false;
  }
}

bool BusinessCacheAdapter::Set(const std::string& biz,int64_t key, const std::vector<unsigned char>& data) {
  DC_AdapterMap::iterator iter = dc_adapters_.find(biz);
  if (iter == dc_adapters_.end()) {
    return false;
  } 
  xce::distcache::DataPtr data_ptr(new std::vector<unsigned char>(data));
  try {
    return iter->second->set(key, data_ptr);
  } catch (...) {
    return false;
  }
}

}} // end


