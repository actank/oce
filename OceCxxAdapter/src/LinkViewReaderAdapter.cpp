#include "LinkViewReaderAdapter.h"

using namespace xce::adapter::personalinfo;
using namespace xce::personalinfo;

LinkViewReaderAdapter::LinkViewReaderAdapter() :
    MyUtil::ReplicatedClusterAdapterI<LinkViewReaderPrx>("ControllerLinkViewReader", 120, 300) {
}

MyUtil::Str2StrMap LinkViewReaderAdapter::getLinkView(int userId) {
  return getProxy(userId)->getLinkView(userId);
}

MyUtil::Str2StrMap LinkViewReaderAdapter::getLinkViewSync(int userId) {
  return getProxy(userId)->getLinkViewSync(userId);
}

void LinkViewReaderAdapter::setLinkView(int userId, const MyUtil::Str2StrMap& props) {
  if(userId < 0) {
    MCE_WARN("LinkViewReaderAdapter::setLinkView WRONG_USERID userId = " << userId);
    return;
  }
  std::vector<LinkViewReaderPrx> allProxies = getAllProxySeq(userId);
  for(std::vector<LinkViewReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it ) {
    try {
      (*it)->ice_timeout(10000)->setLinkView(userId, props);
    } catch(Ice::Exception& e) {
      MCE_WARN("LinkViewReaderAdapter::setLinkView Exception userId = " << userId << " " << e.what());
    }
  }  
}

void LinkViewReaderAdapter::setData(int userId, const Ice::ObjectPtr& data) {
  if (!data) {
    return;
  }

  MyUtil::ObjectResultPtr dataMap = new MyUtil::ObjectResult;
  dataMap->data.insert(make_pair(userId, data));
  setData(dataMap);
}

void LinkViewReaderAdapter::setData(const MyUtil::ObjectResultPtr& dataMap) {
  if(dataMap->data.empty()) {
    return;
  }

  std::vector<LinkViewReaderPrx> allProxies = getAllProxySeq(0);
  for (std::vector<LinkViewReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
    try {
      (*it)->ice_timeout(10000)->setData(dataMap);
    } catch (Ice::Exception& e) {
      MCE_WARN("LinkViewReaderAdapter::setData Exception data size = " << dataMap->data.size() << " " << e.what());
    }
  }  
}
