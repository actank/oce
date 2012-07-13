/*
 * =====================================================================================
 *
 *       Filename:  HomeUserVisitCountReaderI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月17日 17时02分46秒
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#ifndef __HOME_USER_VIEW_COUNT_READERI_H__
#define __HOME_USER_VIEW_COUNT_READERI_H__

#include "FootprintUnite.h"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "util/cpp/TimeCost.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include <boost/lexical_cast.hpp>
#include "Singleton.h"

#include <string>

namespace xce {
namespace footprint {
class HomeUserVisitCountReaderI : public HomeUserVisitCountReader {
public:
  HomeUserVisitCountReaderI() : _valid(false) {}
  virtual void setData(const MyUtil::ObjectResultPtr& homeFootprintResultPtr, const Ice::Current& = Ice::Current());
  virtual void setValid(bool valid, const Ice::Current& = Ice::Current());
  virtual bool isValid(const Ice::Current& = Ice::Current());
  virtual void setServerStatus() = 0;
  virtual string getIdentifier() = 0;
private:
  IceUtil::Mutex _mutex;
  bool _valid;
};

void HomeUserVisitCountReaderI::setData(const MyUtil::ObjectResultPtr& homeUserVisitCountResultPtr, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[HomeUserVisitCountReaderI::setData] size = "
      + boost::lexical_cast<string>(homeUserVisitCountResultPtr->data.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  if (homeUserVisitCountResultPtr->data.empty()) {
    return;
  }
  MyUtil::ObjectResultPtr ptr = UserVisitCountInfo::changeFromUserVisitCountPtrMap(homeUserVisitCountResultPtr);
  ServiceI::instance().getObjectCache()->addObjects(USER_VISIT_COUNT, ptr->data);
}

void HomeUserVisitCountReaderI::setValid(bool valid, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[HomeUserVisitCountReaderI::setValid] valid = " + boost::lexical_cast<string>(valid),
      MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  {
    IceUtil::Mutex::Lock lock(_mutex);
    _valid = valid;
    xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid, getIdentifier());
  }
  setServerStatus();
}

bool HomeUserVisitCountReaderI::isValid(const Ice::Current& current) {
  IceUtil::Mutex::Lock lock(_mutex);
  return _valid;
}

}
}

#endif
