/*
 * =====================================================================================
 *
 *       Filename:  HomeFootprintReplicaAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月30日 15时34分01秒
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#include "HomeFootprintReplicaAdapter.h"
#include "boost/lexical_cast.hpp"
#include "IceExt/src/Channel.h"
#include "ObjectCache.h"
#include "OceCxxAdapter/src/BatchUpdaterReplicaAdapter.h"
#include <iostream>

using namespace std;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace xce::footprint;

HomeFootprintReplicaPrx HomeFootprintReplicaAdapter::getHomeFootprintReplicaPrx(int id) {
  return getProxy(id);
}

vector<HomeFootprintReplicaPrx> HomeFootprintReplicaAdapter::getAllHomeFootprintReplicaPrx(int id) {
  return getAllProxySeq(id);
}

map<int, MyUtil::ObjectResultPtr> HomeFootprintReplicaAdapter::seperateObjectResultByMod(const MyUtil::ObjectResultPtr& ptr, int mod) {
  map<int, MyUtil::ObjectResultPtr> result;
  for (map<long, Ice::ObjectPtr>::const_iterator it = ptr->data.begin(); it != ptr->data.end(); ++it) {
    map<int, MyUtil::ObjectResultPtr>::iterator itp = result.find(it->first % mod);
    if (itp == result.end()) {
      MyUtil::ObjectResultPtr tempPtr = new MyUtil::ObjectResult;
      tempPtr->data.insert(make_pair<long, Ice::ObjectPtr>(it->first, it->second));
      result.insert(make_pair<int, MyUtil::ObjectResultPtr>(it->first % mod, tempPtr));
    } else {
      itp->second->data.insert(make_pair<long, Ice::ObjectPtr>(it->first, it->second));
    }
  }
  return result;
}

void HomeFootprintReplicaAdapter::setFootprint(const MyUtil::ObjectResultPtr& pData, int category) {
  map<int, MyUtil::ObjectResultPtr> seperatedResult = seperateObjectResultByMod(pData, getCluster());
  for (map<int, MyUtil::ObjectResultPtr>::const_iterator it = seperatedResult.begin(); it != seperatedResult.end(); ++it) {
    try {
      vector<HomeFootprintReplicaPrx> prxs = getAllHomeFootprintReplicaPrx(it->first);
      for (vector<HomeFootprintReplicaPrx>::iterator itp = prxs.begin(); itp != prxs.end(); ++itp) {
        try {
          (*itp)->setFootprint(it->second, category);
        } catch(Ice::Exception& e) {
          MCE_WARN("[HomeFootprintAdapter::setFootprint] setFootprint for mod = " << it->first << " Ice::Exception : " << e.what());
        } catch (std::exception& e) {
          MCE_WARN("[HomeFootprintAdapter::setFootprint] setFootprint for mod = " << it->first << " std::exception : " << e.what());
        } catch (...) {
          MCE_WARN("[HomeFootprintAdapter::setFootprint] setFootprint for mod = " << it->first << " UNKNOWN exception");
        }
      }
    } catch (Ice::Exception& e) {
      MCE_WARN("[HomeFootprintAdapter::setFootprint] getPrx for mod = " << it->first << " Ice::Exception : " << e.what());
    } catch (std::exception& e) {
      MCE_WARN("[HomeFootprintAdapter::setFootprint] getPrx for mod = " << it->first << " std::exception : " << e.what());
    } catch (...) {
      MCE_WARN("[HomeFootprintAdapter::setFootprint] getPrx for mod = " << it->first << " UNKNOWN exception");
    }
  }
}

void HomeFootprintReplicaAdapter::setUserVisitCount(const MyUtil::ObjectResultPtr& pData) {
  map<int, MyUtil::ObjectResultPtr> seperatedResult = seperateObjectResultByMod(pData, getCluster());
  for (map<int, MyUtil::ObjectResultPtr>::const_iterator it = seperatedResult.begin(); it != seperatedResult.end(); ++it) {
    try {
      vector<HomeFootprintReplicaPrx> prxs = getAllHomeFootprintReplicaPrx(it->first);
      for (vector<HomeFootprintReplicaPrx>::iterator itp = prxs.begin(); itp != prxs.end(); ++itp) {
        try {
          (*itp)->setUserVisitCount(it->second);
        } catch(Ice::Exception& e) {
          MCE_WARN("[HomeFootprintAdapter::getUserVisitCountObj] setUserVisitCount for mod = " << it->first << " Ice::Exception : " << e.what());
        } catch (std::exception& e) {
          MCE_WARN("[HomeFootprintAdapter::getUserVisitCountObj] setUserVisitCount for mod = " << it->first << " std::exception : " << e.what());
        } catch (...) {
          MCE_WARN("[HomeFootprintAdapter::getUserVisitCountObj] setUserVisitCount for mod = " << it->first << " UNKNOWN exception");
        }
      }
    } catch (Ice::Exception& e) {
      MCE_WARN("[HomeFootprintAdapter::getUserVisitCountObj] getPrx for mod = " << it->first << " Ice::Exception : " << e.what());
    } catch (std::exception& e) {
      MCE_WARN("[HomeFootprintAdapter::getUserVisitCountObj] getPrx for mod = " << it->first << " std::exception : " << e.what());
    } catch (...) {
      MCE_WARN("[HomeFootprintAdapter::getUserVisitCountObj] getPrx for mod = " << it->first << " UNKNOWN exception");
    }
  }
}

