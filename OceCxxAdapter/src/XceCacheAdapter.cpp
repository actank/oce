//#include <boost/scoped_ptr.hpp>

//#include "SearchCache2/src/Util/TokyoTyrantCache.h"
//#include "SearchCache2/src/Util/DatabaseCacheFactory.h"

#include "XceCacheAdapter.h"

using namespace xce;
using namespace xce::searchcache;

CommonCacheChannel::CommonCacheChannel(){
  Ice::PropertiesPtr properties = Ice::createProperties();
  properties->setProperty("Ice.Override.ConnectTimeout", "300");
  properties->setProperty("IceGrid.InstanceName", "XiaoNeiSearchCache");

  properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
  properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

  properties->setProperty("Ice.Default.Host", ::MyUtil::IpAddr::inner());
        //                      properties->setProperty("Ice.Trace.Protocol", "3");
        //                      properties->setProperty("Ice.Trace.Network", "3");

        //                      properties->setProperty("Ice.ProgramName", "ice");
        //                      properties->setProperty("Ice.UseSyslog", "1");

        //      properties->setProperty(
        //                      "Ice.Default.Locator",
        //                      "XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

  properties->setProperty("Ice.Default.Locator", "XiaoNeiSearchCache/Locator:tcp -h XiaoNeiSearchCacheMaster -p 4061:tcp -h XiaoNeiSearchCacheReplica1 -p 4061:tcp -h XiaoNeiSearchCacheReplica2 -p 4061");
        Ice::InitializationData id;
        id.properties = properties;
        _communicator = Ice::initialize(id);

        _query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XiaoNeiSearchCache/Query"));
}

CommonCacheChannel::~CommonCacheChannel() {

}
XceCacheAdapter::XceCacheAdapter()
{
  //_cache_proxies.resize(cluster() > 0 ? cluster() : 1);  
  //_db_loader.start(128 * 1024).detach();
}

/*void XceCacheAdapter::DbInitialize(const vector<string> & dbs)
{
  _dbcaches.resize(4); //cluster() size
  
  for(size_t i = 0; i < dbs.size(); ++i)
  {
    _dbcaches[i] = DatabaseCacheFactory::instance().getDbCache(dbs[i]);
    
    if(_dbcaches[i])
    {
      _dbcaches[i]->Initialize();
    }
  }
}*/

XceCacheAdapter::~XceCacheAdapter()
{
/*
  for(size_t i = 0; i < _dbcaches.size(); ++i)
  {
    if(_dbcaches[i])
    {
      _dbcaches[i]->Close();
    }
  }
*/
}
 
bool XceCacheAdapter::isValid(const Ice::ObjectPrx& proxy) {
  return CachePrx::uncheckedCast(proxy)->isValid();
}


/*void XceCacheAdapter::Load(int id) {
  _db_loader.Load(id);
}

void XceCacheAdapter::Loads(const IntSequence& ids) {  
  _db_loader.Loads(ids);
}

void XceCacheAdapter::XceCacheAdapter::doLoads(const IntSequence& ids)
{
  SearchCacheData data;
  char * p;
  int  len = 0;
  
  CachePrx xcache_prx;
  
  for(size_t i = 0; i < ids.size(); ++i)
  {
    if(_dbcaches[ ids[i] % 4 ]->Query(ids[i], &data))
    {
      BinStringPtr pbs = data.Serialize();
      pbs->getBuf(&p, &len);
      if(len <= 0)
      {
        MCE_WARN("user data empty in db " << ids[i]);
        continue;
      }
      
      // locate中没指定 endpoint 参数，则使用endpoints() 函数的返回值
      xcache_prx = locate<CachePrx>(_cache_proxies, "cache", ids[i] % cluster(), MyUtil::TWO_WAY);
      if(xcache_prx)
      {
        xcache_prx->PutByInt(ids[i], Buffer(p, p + len));
      }
      else
      {
        MCE_WARN("locate proxy error for user " << ids[i]);
      }
    }
    else
    {
      MCE_WARN("null DB handle" << ids[i]);
    }
  }
}
*/
bool XceCacheAdapter::Get(int userid, std::string* value) {
  if (!value)
    return true;
  try {
    CachePrx proxy = locate<CachePrx>(_cache_proxies, "cache", userid % cluster(), MyUtil::TWO_WAY);
    *value =  proxy->Get(std::string((const char*)&userid, sizeof(userid)));
    return true;
  } catch (Ice::Exception& e) {
    MCE_ERROR("Get error: userid="<< userid << e);
  }
  return false;
}

Int2StrMap XceCacheAdapter::Gets(const IntSequence& ids) {
  // 1 %
  // 2 call one by one

  typedef std::map< ::Ice::Int, StringList> GetsMap;
  GetsMap gets_map;

  for(IntSequence::const_iterator i = ids.begin(); i != ids.end(); ++i) {
    int index = *i % cluster();
    gets_map[index].push_back(std::string(
      (const char*)&(*i), sizeof(Ice::Int)
      ));
  }

  // 2 串行调用 Gets 
  Int2StrMap result;  
  int i=0;
  for (GetsMap::const_iterator gmi = gets_map.begin(); gmi != gets_map.end(); ++gmi, ++i) {
    try {
      CachePrx proxy = locate<CachePrx>(_cache_proxies, "cache", gmi->first, MyUtil::TWO_WAY);
      
      StringMap subr = proxy->Gets(gmi->second);

      for (StringMap::const_iterator i=subr.begin(); i!=subr.end(); ++i) {
        int uid = *(int*)(i->first.data()); // 
        result.insert(std::make_pair(uid, i->second));
      }
    } catch (Ice::Exception& e) {
      MCE_ERROR("Gets error: cluster="<< i << e);
      continue;
    }
  }
  
  return result;
}

bool XceCacheAdapter::Put(int userid, const std::string & value) {  
  try {
    CachePrx proxy = locate<CachePrx>(_cache_proxies, "cache", userid % cluster(), MyUtil::TWO_WAY);
    proxy->Put(std::string((const char*)&userid, sizeof(userid)), value);
    return true;
  } catch (Ice::Exception& e) {
    MCE_ERROR("Put error: userid="<< userid << e);
    //std::cout << "put is error" << e << std::endl;
  }
  return false;
}

void XceCacheAdapter::Puts(const IntSequence & uids, const StringList & values) {
  if(uids.empty() || (uids.size() != values.size()))
    return;
  typedef std::pair<StringList, StringList> PutsPair;
  std::vector<PutsPair> puts_pairs;
  puts_pairs.resize(cluster());
  
  CachePrx proxy;

  for(size_t i = 0; i < uids.size(); ++i) {
    puts_pairs[uids[i] % cluster()].first.push_back(std::string((const char *)&uids[i], sizeof(::Ice::Int)));
    puts_pairs[uids[i] % cluster()].second.push_back(values[i]);
  }
  
  for(size_t i = 0; i < puts_pairs.size(); ++i) {
    if(puts_pairs[i].first.empty())
      continue;
    try {
      proxy = locate<CachePrx>(_cache_proxies, "cache", i, MyUtil::TWO_WAY);
      proxy->Puts(puts_pairs[i].first, puts_pairs[i].second);
    } catch (Ice::Exception& e) {
      MCE_ERROR("Puts error: cluster=" << i << " error:" << e);
    }
  }
}

bool XceCacheAdapter::Remove(int userid) {
   try {
     CachePrx proxy = locate<CachePrx>(_cache_proxies, "cache", userid % cluster(), MyUtil::TWO_WAY);
     proxy->Remove(std::string((const char*)&userid, sizeof(userid)));
     return true;
   } catch (Ice::Exception& e) {
     MCE_ERROR("Remove error: userid=" << userid << e);
   }
   return false;
}

void XceCacheAdapter::Removes(const IntSequence& uids) {
  if(uids.empty())
    return;
  std::vector<StringList> rm_ids;
  rm_ids.resize(cluster());

  CachePrx proxy;

  for(size_t i = 0; i < uids.size(); ++i)
  {
    rm_ids[uids[i] % cluster()].push_back(std::string((const char *)&uids[i], sizeof(::Ice::Int)));
  }
  for(size_t i = 0; i < rm_ids.size(); ++i)
  {
    if(rm_ids[i].empty())
      continue;
   try {
      proxy = locate<CachePrx>(_cache_proxies, "cache", i, MyUtil::TWO_WAY);
      proxy->Removes(rm_ids[i]);
    } catch (Ice::Exception& e) {
      MCE_ERROR("Removes error: cluster=" << i << e);
    }
  }
}

bool XceCacheAdapter::IsValid(int cluster_id) {
  try {
    CachePrx proxy = locate<CachePrx>(_cache_proxies, "cache", cluster_id % cluster(), MyUtil::TWO_WAY);
    return proxy->isValid();
  } catch (Ice::Exception& e) {
    MCE_ERROR("IsValid error: cluster=" << cluster_id << e);
    std::cout << "IsValid error: cluster=" << cluster_id << e;
  }
  return false;
}

bool XceCacheAdapter::SetValid(int cluster_id, bool v) {
  try {
    CachePrx proxy = locate<CachePrx>(_cache_proxies, "cache", cluster_id % cluster(), MyUtil::TWO_WAY);
    proxy->setValid(v);
    return true;
  } catch (Ice::Exception& e) {
    MCE_ERROR("SetValid error: cluster=" << cluster_id << e);
    std::cout << "SetValid error: cluster=" << cluster_id << e;
  }
  return false;
}

bool XceCacheAdapter::State(int cluster_id, CacheState* cs) {
  if (!cs)
    return true;
  try {
    CachePrx proxy = locate<CachePrx>(_cache_proxies, "cache", cluster_id % cluster(), MyUtil::TWO_WAY);
     *cs = proxy->State();
     return true;
  } catch (Ice::Exception& e) {
    MCE_ERROR("State error: cluster=" << cluster_id << e);
  }
  return false;
}

bool XceCacheAdapter::SetState(int cluster_id, const CacheState & state) {
  try {
    CachePrx proxy = locate<CachePrx>(_cache_proxies, "cache", cluster_id % cluster(), MyUtil::TWO_WAY);
    proxy->SetState(state);
    return true;
  } catch (Ice::Exception& e) {
    MCE_ERROR("SetState error: cluster=" << cluster_id << e);
  }
  return false;
}
