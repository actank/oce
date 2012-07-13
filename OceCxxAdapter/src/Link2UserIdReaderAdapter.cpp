#include "Link2UserIdReaderAdapter.h"

using namespace xce::adapter::personalinfo;
using namespace xce::personalinfo;

Link2UserIdReaderAdapter::Link2UserIdReaderAdapter() :
    MyUtil::ReplicatedClusterAdapterI<Link2UserIdReaderPrx>
      ("ControllerLink2UserIdReader", 120, 300) {
}

void Link2UserIdReaderAdapter::add(const string& link, int userId) {
  if(userId < 0) {
    MCE_WARN("Link2UserIdReaderAdapter::add WRONG_USERID userId = " << userId);
    return;
  } else if (link == "") {
    MCE_WARN("Link2UserIdReaderAdapter::add WRONG_LINK link is null " << userId);
    return;
  }
  std::vector<Link2UserIdReaderPrx> allProxies = getAllProxySeq(0);
  for(std::vector<Link2UserIdReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it ) {
    try {
      (*it)->ice_timeout(10000)->add(link, userId);
    } catch(Ice::Exception& e) {
      MCE_WARN("Link2UserIdReaderAdapter::add Exception userId = " << userId << " " << e.what());
    }
  }  
}

void Link2UserIdReaderAdapter::remove(const string& link) {
  if(link == "") {
    return;
  }
  std::vector<Link2UserIdReaderPrx> allProxies = getAllProxySeq(0);
  for(std::vector<Link2UserIdReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it ) {
    try {
      (*it)->ice_timeout(10000)->remove(link);
    } catch(Ice::Exception& e) {
      MCE_WARN("Link2UserIdReaderAdapter::remove Exception link = " << link << " " << e.what());
    }
  }  
}

void Link2UserIdReaderAdapter::setData(int userId, const Ice::ObjectPtr& data) {
  if (!data) {
    return;
  }

  MyUtil::ObjectResultPtr dataMap = new MyUtil::ObjectResult;
  dataMap->data.insert(make_pair(userId, data));
  setData(dataMap);
}

void Link2UserIdReaderAdapter::setData(const MyUtil::ObjectResultPtr& dataMap) {
  if(dataMap->data.empty()) {
    return;
  }

  std::vector<Link2UserIdReaderPrx> allProxies = getAllProxySeq(0);
  for (std::vector<Link2UserIdReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
    try {
      (*it)->ice_timeout(10000)->setData(dataMap);
    } catch (Ice::Exception& e) {
      MCE_WARN("Link2UserIdReaderAdapter::setData Exception data size = " << dataMap->data.size() << " " << e.what());
    }
  }  
}
