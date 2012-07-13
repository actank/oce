#include "consist_hash.h"
#include <algorithm>

#include <cmath>
#include <cstdio>
#include <cstring>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread/locks.hpp>

#include "doobs_hash.h"
#include "LogWrapper.h"
using namespace MyUtil;

namespace xce {
namespace feed {

bool Continuum::RebuildCachePoints()
{
  if(cache_nodes_.empty())
    return false;

  cache_points_mutex_.lock();
  cache_points_.clear();

  unsigned long total_points = 0;
  CacheNodeMap::const_iterator it = cache_nodes_.begin();
  for(; it != cache_nodes_.end(); ++it)
    total_points += it->second;

  cache_points_.reserve(total_points);

  for(it = cache_nodes_.begin(); it != cache_nodes_.end(); ++it)
  {
    char ss[64];
    //unsigned char digest[16];
    for(size_t k = 0; k < it->second; ++k)
    {
      snprintf(ss, 63, "%lu-%lu-%u", k, it->first.address().to_v4().to_ulong(), it->first.port());
      ss[63] = 0;
      // TODO : use murmur
      //md5_digest(ss, digest);
      //uint32_t hash_point = (digest[3] << 24)
      //              | ( digest[2] << 16 )
      //              | ( digest[1] <<  8 )
      //              |   digest[0];

      //murmur hash; 
      //uint32_t hash_point = murmur()(ss, strlen(ss));
      uint32_t hash_point = doobs_hash(ss, strlen(ss));

      cache_points_.push_back(CachePoint(hash_point, it->first));
    }
  }

  std::sort(cache_points_.begin(), cache_points_.end());
  cache_points_mutex_.unlock();
  return true;
}

ip::tcp::endpoint Continuum::LocateCacheNode(const char * key, size_t len) const
{
  uint32_t hash = doobs_hash(key, len);
  ip::tcp::endpoint ep;

  cache_points_mutex_.lock_shared();

  CachePointSeq::const_iterator it = std::lower_bound(cache_points_.begin(), cache_points_.end(), CachePoint(hash, ep));
  if (it != cache_points_.end()) {
    ep = it->endpoint;
  } else {
    if(!cache_points_.empty()) {
      ep = cache_points_.front().endpoint;
    }
  }
  cache_points_mutex_.unlock_shared();
  return ep;
}

bool Continuum::ParseNodesConfig(const std::string & config, CacheNodeMap * parsed) const
{
  std::vector<std::string> splited;
  boost::split(splited, config, boost::is_any_of(",;: "));
  if(splited.back().empty()) {
    splited.pop_back();
  }

  if(splited.empty() || splited.size() % 3 != 0)
  {
    MCE_WARN("incorrect cache nodes config: " << config);
    return false;
  }

  parsed->clear();

  int port, size;
  for(size_t i = 0; i < splited.size(); i += 3)
  {
    try {
      port = boost::lexical_cast<int>(splited[i + 1]);
      size = boost::lexical_cast<int>(splited[i + 2]);
    } catch(boost::bad_lexical_cast&) {
      MCE_WARN("incorrect cache nodes config: " << config);
      return false;
    }

    if(port <= 0 || size <= 0)
    {
      MCE_WARN("incorrect cache nodes config: " << config);
      return false;
    }
    parsed->insert(std::make_pair(ip::tcp::endpoint(ip::address_v4::from_string(splited[i]), port), size));
  }
  return true;
}

bool Continuum::SetCacheNodes(const std::string & cache_nodes)
{
  CacheNodeMap cache_node_map;
  if(!ParseNodesConfig(cache_nodes, &cache_node_map)) {
    return false;
  }
  cache_nodes_.swap(cache_node_map);
  return RebuildCachePoints();
}

//std::vector<std::pair<uint32_t, uint32_t> > Continuum::GetServerRanges(const std::string & endpoint)
//{
//  std::vector<std::pair<uint32_t, uint32_t> > ranges;  
//  cache_points_mutex_.lock_shared();
//  if(cache_nodes_.find(endpoint) != cache_nodes_.end())
//  {
//    std::pair<uint32_t, uint32_t> r;
//    
//    bool contains = false;
//    CachePointSeq::const_iterator it = cache_points_.begin();
//    if(it->endpoint == endpoint)
//    {
//      contains = true;
//      r.first = 0;//cache_points_.back().hash_point;
//    }
//  
//    for(++it; it != cache_points_.end(); ++it)
//    {
//      if(contains)
//      {
//        if(it->endpoint != endpoint)
//        {
//          contains = false;
//          r.second = (it - 1)->hash_point;
//          ranges.push_back(r);
//        }
//      }
//      else
//      {
//        if(it->endpoint == endpoint)
//        {
//          contains = true;
//          r.first = (it - 1)->hash_point;
//        }
//      }
//    }    
//    if(contains)
//    {
//      r.second = (it - 1)->hash_point;
//      ranges.push_back(r);
//    }
//    
//    //TODO : 最后一段的边界, 有问题.
//  }
//  cache_points_mutex_.unlock_shared();
//  return ranges;
//}

void Continuum::Dump()
{
  for(CachePointSeq::const_iterator it = cache_points_.begin(); it != cache_points_.end(); ++it)
  {
    MCE_INFO("cache point dump - " << it->endpoint << " : " << it->hash_point);
  }
}

}
}

