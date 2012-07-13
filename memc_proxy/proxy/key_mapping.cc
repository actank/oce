#include "key_mapping.h"

#include "consist_hash.h"

#include "ServiceI.h"
#include "LogWrapper.h"

using namespace std;
using namespace boost::asio;
using namespace MyUtil;

namespace xce {
namespace feed {

static map<string, Continuum *> g_clusters;
static string memcached_nodes =  "10.3.22.42:11211 6800;"\
                                "10.3.22.43:11211 6800;"\
                                "10.3.22.119:11211 6800;"\
                                "10.3.22.120:11211 6800;"\
                                "10.3.22.121:11211 6800;"\
                                "10.3.22.122:11211 6800;"\
                                "10.3.22.123:11211 6800;"\
                                "10.3.22.124:11211 6800;"\
                                "10.3.22.125:11211 6800;"\
                                "10.3.22.126:11211 6800";


bool InitCacheClusters() {
  string prefix = "Service." + ServiceI::instance().getName() + ".MemcachedList.";
  Ice::PropertyDict prop_dict = ServiceI::instance().getCommunicator()->getProperties()->getPropertiesForPrefix(prefix);

  for (Ice::PropertyDict::iterator it = prop_dict.begin(); it != prop_dict.end(); ++it) {
    MCE_DEBUG("加载 Continuum " << it->first << ":" << it->second);
    if (strcmp(it->first.c_str() + prefix.size(), "FEED") == 0) {
      //it->second = "10.3.17.128:11211 2800;10.3.16.210:11211 2800;10.3.16.211:11211 2800;10.3.17.149:11211 1500;10.3.20.44:11211 2700"\
                    ";10.3.20.45:11211 2800;10.3.20.46:11211 2800;10.3.20.47:11211 2800;10.3.20.48:11211 2800";
      it->second = memcached_nodes;
      MCE_DEBUG("set feed :" << it->second);
    }

    Continuum * c = new Continuum;
    if (c->SetCacheNodes(it->second)) {
      g_clusters.insert(make_pair(it->first.c_str() + prefix.size(), c));
    } else {
      MCE_WARN("加载 Continuum 失败 " << it->first << ":" << it->second);
      delete c;
      continue;
    }
  }

  string ns = "ADUP";
  //string memcached_nodes = "10.3.17.128:11211 2800;10.3.16.210:11211 2800;10.3.16.211:11211 2800;10.3.17.149:11211 1500;10.3.20.44:11211 2700"\
                    ";10.3.20.45:11211 2800;10.3.20.46:11211 2800;10.3.20.47:11211 2800;10.3.20.48:11211 2800";
  Continuum * c = new Continuum;
  if (c->SetCacheNodes(memcached_nodes)) {
    g_clusters.insert(make_pair(ns, c));
  } else {
    MCE_WARN("加载 Continuum 失败 : " << ns << "-" << memcached_nodes);
    delete c;
  }

  return true;
}

ip::tcp::endpoint GetEndpointByKey(const char * key, size_t len) {
  size_t delim_pos = 0;
  for (; delim_pos < len; ++ delim_pos) {
    if (key[delim_pos] == '#') {
      break;
    }
  }

  Continuum * c = 0;

  if (delim_pos != len) {
    map<string, Continuum *>::const_iterator it = g_clusters.find(string(key, delim_pos));
    if (it != g_clusters.end()) {
      c = it->second;
    }
  } else {
    delim_pos = 0;
  }

  if (!c) {
    c = g_clusters["DEFAULT"];
  }
  
  ip::tcp::endpoint ep = c->LocateCacheNode(key, len);

  MCE_DEBUG("key=" << string(key, len) << " cache_node=" << ep);

  return ep;
}

}
}

