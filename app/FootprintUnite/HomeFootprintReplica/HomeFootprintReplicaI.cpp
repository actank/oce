/*
 * =====================================================================================
 *
 *       Filename:  HomeFootprintReplicaI.cpp
 *
 *    Description:  replica for HomeFootprintReplica.
 *
 *        Version:  1.0
 *        Created:  2011年09月14日 17时16分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "app/FootprintUnite/share/FootprintShare.h"
#include "HomeFootprintReplicaI.h"
#include "HomeFootprintAdapter.h"
#include "util/cpp/InvokeMonitor.h"
#include "util/cpp/TimeCost.h"
#include <Monitor/client/service/ServiceMonitor.h>

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::footprint;
using namespace xce::serverstate;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();

  service.getAdapter()->add(&HomeFootprintReplicaI::instance(), service.createIdentity("HFR", ""));
  service.getAdapter()->add(&HomeFootprintReplicaReaderI::instance(), service.createIdentity("HFRR", ""));
  service.getAdapter()->add(&HomeFootprintBufReplicaReaderI::instance(), service.createIdentity("HFBRR", ""));
  service.getAdapter()->add(&HomeUserVisitCountReplicaReaderI::instance(), service.createIdentity("HUVCRR", ""));
  service.registerObjectCache(FOOTPRINT_SET, "HFS", new HomeFootprintFactoryI,false);
  service.registerObjectCache(FOOTPRINT_BUF_SET, "HFBS", new HomeFootprintBufFactoryI);
  service.registerObjectCache(USER_VISIT_COUNT, "HUVCS", new HomeUserVisitCountFactoryI,false);

  // 加入向Controller注册
  int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("HomeFootprintReplica.Mod", 10);
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("HomeFootprintReplica.Interval", 5);
  map<string, Ice::ObjectPtr> mapNameAndService;
  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("HFRR", &HomeFootprintReplicaReaderI::instance()));
  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("HFBRR", &HomeFootprintBufReplicaReaderI::instance()));
  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("HUVCRR", &HomeUserVisitCountReplicaReaderI::instance()));
  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("HFR", &HomeFootprintReplicaI::instance()));

  string ctrEndpoints = service.getCommunicator()->getProperties()
    ->getPropertyWithDefault("HomeFootprintReplica.ControllerEndpoint", "ControllerHomeFootprintReplica");
  ServerStateSubscriber::instance().initialize(ctrEndpoints,
      mapNameAndService, mod, interval);
  {
    int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("HomeFootprintReplica.Status", 0);
    ServerStateSubscriber::instance().setStatus(status, "HFRR");
    ServerStateSubscriber::instance().setStatus(status, "HFBRR");
    ServerStateSubscriber::instance().setStatus(status, "HUVCRR");
    ServerStateSubscriber::instance().setStatus(status, "HFR");
    ServerStateSubscriber::instance().setStatus(status);

    std::ostringstream os;
    os << "MyUtil::initialize set status : " << status;
    MCE_WARN(os.str());
  }
  //统计log
  ServiceMonitorManager::instance().start();
}

HomeFootprintReplicaI::HomeFootprintReplicaI() {
}

void HomeFootprintReplicaI::setFootprint(const MyUtil::ObjectResultPtr& pData, int category, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[HomeFootprintReplicaI::setFootprint] size = "
      + boost::lexical_cast<string>(pData->data.size()) + ", category = " + boost::lexical_cast<string>(category),
      MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  if (pData->data.empty()) {
    return;
  }
  int cacheSize = getHomeFootprintCacheSize(category);
  MyUtil::ObjectResultPtr ptr = HomeFootprintSet::changeFromHomeVisitHistoryMap(pData, cacheSize);
  ServiceI::instance().getObjectCache()->addObjects(category, ptr->data);
}

void HomeFootprintReplicaI::setUserVisitCount(const MyUtil::ObjectResultPtr& pData, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[HomeFootprintReplicaI::setData] size = "
      + boost::lexical_cast<string>(pData->data.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  if (pData->data.empty()) {
    return;
  }
  MyUtil::ObjectResultPtr ptr = UserVisitCountInfo::changeFromUserVisitCountPtrMap(pData);
  ServiceI::instance().getObjectCache()->addObjects(USER_VISIT_COUNT, ptr->data);
}

HomeInfoPtr HomeFootprintReplicaI::getAll(long id, int begin, int nLimit, int tLimit, const Ice::Current& current) {
	ostringstream oss;
	oss << "[HomeFootprintReplicaI::getAll] owner = " << id << ", limit = " << nLimit << ", begin = " << begin;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	HomeInfoPtr res = new HomeInfo;

	HomeFootprintSetPtr p = getFootprintSet(id);
	if(begin < CACHE_SIZE){
		if(p){
			res->steps = p->get(begin, nLimit, tLimit);
		}
	}
	if ((begin + nLimit) > CACHE_SIZE) { 
		HomeFootprintSetPtr p2 = getFootprintBufSet(id);
		if(p2){
      if (begin >= CACHE_SIZE) { 
        vector<HomeStepPtr> vec = p2->get(begin - CACHE_SIZE, nLimit, tLimit);
        res->steps.insert(res->steps.end(),vec.begin(),vec.end());
      } else {
        vector<HomeStepPtr>  vec = p2->get(0,nLimit+begin-CACHE_SIZE,tLimit);
        res->steps.insert(res->steps.end(),vec.begin(),vec.end());
      }
		}
	}
	res->visitcount = getUserVisitCount(id);
	MCE_INFO("HomeFootprintReplicaI::getAll id: " << id << " limit: " << nLimit << " getSize: " << res->steps.size() << " visitcount: " << res->visitcount );
	return res;
}

int HomeFootprintReplicaI::getUserVisitCount(long uid, const Ice::Current& current) {
  MCE_INFO("[HomeFootprintReplicaI::getUserVisitCount] userId = " << uid);
  ostringstream oss;
  oss<< "[HomeFootprintReplicaI::getUserVisitCount] userId = " << uid;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);
  UserVisitCountInfoPtr p = locateUserVisitCount(uid);
  if (p) {
    return p->getCount();
  } else {
    return 0;
  }
}

int HomeFootprintReplicaI::getSize(long id, const Ice::Current& current) {
  ostringstream oss;
  oss << "[HomeFootprintReplicaI::getSize] userId = "<< id;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  HomeFootprintSetPtr p = getFootprintSet(id);
  if(p == 0) {
    return 0;
  }
  HomeFootprintSetPtr p2 = getFootprintBufSet(id);
  if(p2 == 0) {
    return p->getSize();
  }
  return p->getSize() + p2->getSize();
}

bool HomeFootprintReplicaI::isValid(const Ice::Current& current) {
  MCE_INFO("[HomeFootprintReplicaI::isValid]");
  IceUtil::RWRecMutex::RLock lock(validMutex_);
  return isValid_;
}

void HomeFootprintReplicaI::setValid(bool newState, const Ice::Current& current) {
  IceUtil::RWRecMutex::WLock lock(validMutex_);
  int state = 0;
  if (HomeFootprintReplicaReaderI::instance().isValid() && HomeFootprintBufReplicaReaderI::instance().isValid()
      && HomeUserVisitCountReplicaReaderI::instance().isValid()) {
    state = 1;
  }
  MCE_INFO("[HomeFootprintReplicaI::setValid] valid = " << state);
  ServerStateSubscriber::instance().setStatus(state, "HFR");
  ServerStateSubscriber::instance().setStatus(state);
}

HomeFootprintSetPtr HomeFootprintReplicaI::getFootprintSet(long id) {
  return ServiceI::instance().locateObject<HomeFootprintSetPtr>(FOOTPRINT_SET, id);
} 

HomeFootprintSetPtr HomeFootprintReplicaI::getFootprintBufSet(long id) {
  return ServiceI::instance().locateObject<HomeFootprintSetPtr>(FOOTPRINT_BUF_SET, id);
}

UserVisitCountInfoPtr HomeFootprintReplicaI::locateUserVisitCount(long userId) {
  UserVisitCountInfoPtr countPtr = ServiceI::instance().locateObject<UserVisitCountInfoPtr> (USER_VISIT_COUNT, userId);
  return countPtr;
} 

Ice::ObjectPtr HomeFootprintFactoryI::create(long id) {
  MCE_INFO("[HomeFootprintFactoryI::create] id = " << id); 
  return new HomeFootprintSet(HomeFootprintAdapter::instance().getHomeFootprintObj(id, FOOTPRINT_SET), CACHE_SIZE);
}

Ice::ObjectPtr HomeFootprintBufFactoryI::create(long id) {
  MCE_INFO("[HomeFootprintBufFactoryI::create] id = " << id); 
  return new HomeFootprintSet(HomeFootprintAdapter::instance().getHomeFootprintObj(id, FOOTPRINT_BUF_SET), CACHE_BUF_SIZE);
}

Ice::ObjectPtr HomeUserVisitCountFactoryI::create(long id) {
  MCE_INFO("[HomeUserVisitCountFactoryI::create] id = " << id);
  return new UserVisitCountInfo(HomeFootprintAdapter::instance().getUserVisitCountObj(id));
}
