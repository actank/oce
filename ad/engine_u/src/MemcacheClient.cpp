#include <boost/algorithm/string/regex.hpp>
#include <boost/lexical_cast.hpp>
#include "MemcacheClient.h"
#include "IceLogger.h"
#include "AdConfig.h"

namespace xce {
namespace ad {
using namespace std;

MemcacheClient::MemcacheClient():init_(false), local_cache_(10000) {
}

MemcacheClient::~MemcacheClient() {
}

bool MemcacheClient::Init() {
    return true;
}

bool MemcacheClient::Filter(int uid, std::set<AdGroupPtr>& groups) {
  timeval start;
  gettimeofday(&start, NULL);
  //MCE_INFO("MemcacheClient::Filter --> uid = " << uid);
  try {
    std::set<Ice::Long> not_like;
    if(get(boost::lexical_cast<string>(uid), not_like)) {
      for(std::set<Ice::Long>::iterator it = not_like.begin(); it != not_like.end(); ++it) {
        try {
          Ice::Long group_id = (*it);
          //MCE_DEBUG("MemcacheClient::Filter() --> uid = " << uid << ", closed ad = " << group_id);
          for(std::set<AdGroupPtr>::iterator git = groups.begin(); git != groups.end(); ++git) {
            if((*git)->group_id() == group_id) {
              //MCE_DEBUG("MemcacheClient::Filter() --> uid = " << uid << ", closed ad = " << group_id << " filtered!!!");
              groups.erase(git);
              break;
            }
          }
        } catch(...) {
          MCE_DEBUG("MemcacheClient::Filter() --> uid = " << uid << ", bad value in memory: " << *it);
        }
      }
      timeval end;
      gettimeofday(&end, NULL);
      MCE_INFO("MemcacheClient::Filter --> complete uid = " << uid << ", time = " << 1000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000 << " ms");
      return true;
    } else {
      timeval end;
      gettimeofday(&end, NULL);
      MCE_INFO("MemcacheClient::Filter --> get no value uid = " << uid << ", time = " << 1000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000 << " ms");
      return false;
    }
  } catch(...) {
    MCE_INFO("MemcacheClient::Init() --> unknown error");
  }
  return false;
}

bool MemcacheClient::append(const string& key, const string& append_value) {
  //MCE_DEBUG("MemcacheClient::append --> key = " << key << ", append_value = " << append_value);
  bool ret = false;
  char* result;
  memcached_return rc;
  uint32_t  flags;
  size_t value_length;
  string servers = EngineConfig::instance().memcached();
  //MCE_DEBUG("MemcacheClient::append --> servers = " << servers);
  memcached_server_st *server = NULL;
  memcached_st* memc;
  try {
    memc = memcached_create(NULL);
    server = memcached_servers_parse(servers.c_str());
    rc = memcached_server_push(memc,server);
    memcached_server_list_free(server);

    result = memcached_get(memc, key.c_str(), key.length(), &value_length, &flags, &rc);
    if(rc == MEMCACHED_SUCCESS) {
      ret = true;
      ostringstream oss;
      oss << result << '_' << append_value;
      string value = oss.str();
      rc = memcached_set(memc, key.c_str(), key.length(), value.c_str(), value.length(), 10*60*1000, flags);
      {
        int int_key = boost::lexical_cast<int>(key);
        //IceUtil::RWRecMutex::WLock lock(mutex_);
        std::set<Ice::Long> values;
        values = local_cache_.locate(int_key, values);
        if(values.empty()) {
          values.insert(0);
          //插入memcache里的值 result
          vector<string> dead_groups;
          boost::algorithm::split_regex(dead_groups, result, boost::regex("_"));
          for(vector<string>::iterator vit = dead_groups.begin(); vit != dead_groups.end(); ++vit) {
            try {
              //MCE_DEBUG("MemcacheClient::get --> dead_group " << *vit);
              values.insert(boost::lexical_cast<Ice::Long>(*vit));
            } catch(...) {
              MCE_WARN("MemcacheClient::append --> parse error: dead_group = " << *vit);
            }
          }
        } else {
          values.insert(boost::lexical_cast<Ice::Long>(append_value));
        }
        local_cache_.add(values, int_key);
      }
    } else if(rc == MEMCACHED_NOTFOUND) {
      ret = true;
      rc = memcached_set(memc, key.c_str(), key.length(), append_value.c_str(), append_value.length(), 10*60*1000, flags);
      {
        //IceUtil::RWRecMutex::WLock lock(mutex_);
        std::set<Ice::Long> values;
        values.insert(0);
        values.insert(boost::lexical_cast<Ice::Long>(append_value));
        local_cache_.add(values, boost::lexical_cast<int>(key));
      }
    }

    //free
    memcached_free(memc);
  } catch(...) {
    MCE_INFO("MemcacheClient::get() --> unknown error");
  }

}

bool MemcacheClient::get(const string& key, std::set<Ice::Long>& value) {
  value.clear();
  {
    //IceUtil::RWRecMutex::RLock lock(mutex_);
    value = local_cache_.locate(boost::lexical_cast<int>(key), value);
    if(value.size() == 1) { //本地有缓存, memcache中没有
      //value.clear();
      return true;
    } else if(!value.empty()) { //有缓存, 且不是默认值
      return true;
    }
  }
  //本地没有缓存，尝试去memcache取
  value.insert(0);
  bool ret = false;
  char* result;
  memcached_return rc;
  uint32_t  flags;
  size_t value_length;
  string servers = EngineConfig::instance().memcached();
  memcached_server_st *server = NULL;
  memcached_st* memc;
  try {
    memc = memcached_create(NULL);
    server = memcached_servers_parse(servers.c_str());
    rc = memcached_server_push(memc,server);
    memcached_server_list_free(server);

    result = memcached_get(memc, key.c_str(), key.length(), &value_length, &flags, &rc);
    //free
    memcached_free(memc);
  } catch(...) {
    MCE_INFO("MemcacheClient::get() --> unknown error");
  }

  if(rc == MEMCACHED_SUCCESS) {
    ret = true;
    vector<string> dead_groups;
    boost::algorithm::split_regex(dead_groups, result, boost::regex("_"));
    for(vector<string>::iterator vit = dead_groups.begin(); vit != dead_groups.end(); ++vit) {
      try {
        //MCE_DEBUG("MemcacheClient::get --> dead_group " << *vit);
        value.insert(boost::lexical_cast<Ice::Long>(*vit));
      } catch(...) {
        MCE_WARN("MemcacheClient::get --> parse error: dead_group = " << *vit);
      }
    }
    {
      //IceUtil::RWRecMutex::WLock lock(mutex_);
      local_cache_.add(value, boost::lexical_cast<int>(key));
    }
  } else if(rc == MEMCACHED_NOTFOUND) {
    ret = true;
    {
      //IceUtil::RWRecMutex::WLock lock(mutex_);
      local_cache_.add(value, boost::lexical_cast<int>(key));
    }
  }
  return ret;
}

} // end namespace ad
} // end namespace xce
