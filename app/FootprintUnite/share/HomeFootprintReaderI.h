/*
 * =====================================================================================
 *
 *       Filename:  HomeFootprintReaderI.h
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

#ifndef __HOME_FOOTPRINT_READERI_H__
#define __HOME_FOOTPRINT_READERI_H__

#include "FootprintUnite.h"
#include "FootprintShare.h"
#include "ServiceI.h"

#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "util/cpp/TimeCost.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include <boost/lexical_cast.hpp>
#include "Singleton.h"
#include <string>

namespace xce {
namespace footprint {

using namespace MyUtil;
using namespace ::xce::log;
using namespace com::xiaonei::xce;
using namespace xce::serverstate;
using namespace xce::footprint;

class HomeFootprintReaderI : public HomeFootprintReader {
public:
  HomeFootprintReaderI() : _valid(false) {}
  virtual void setData(const MyUtil::ObjectResultPtr& homeFootprintResultPtr, const Ice::Current& = Ice::Current());
  virtual void setValid(bool valid, const Ice::Current& = Ice::Current());
  virtual bool isValid(const Ice::Current& = Ice::Current());
  virtual void setServerStatus() = 0;
  virtual string getIdentifier() = 0;
private:
  IceUtil::Mutex _mutex;
  bool _valid;
};

void HomeFootprintReaderI::setData(const MyUtil::ObjectResultPtr& homeFootprintResultPtr, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[HomeFootprintReaderI::setData] size = "
      + boost::lexical_cast<string>(homeFootprintResultPtr->data.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  if (homeFootprintResultPtr->data.empty()) {
    return;
  }
  MyUtil::ObjectResultPtr ptr = HomeFootprintSet::changeFromHomeVisitHistoryMap(homeFootprintResultPtr, CACHE_SIZE);
  ServiceI::instance().getObjectCache()->addObjects(FOOTPRINT_SET, ptr->data);
}

void HomeFootprintReaderI::setValid(bool valid, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[HomeFootprintReaderI::setValid] valid = " + boost::lexical_cast<string>(valid),
      MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  {
    IceUtil::Mutex::Lock lock(_mutex);
    _valid = valid;
    xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid, getIdentifier());
  }
  setServerStatus();
}

bool HomeFootprintReaderI::isValid(const Ice::Current& current) {
  IceUtil::Mutex::Lock lock(_mutex);
  return _valid;
}

}
}

#endif
