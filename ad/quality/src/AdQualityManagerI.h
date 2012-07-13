/*
 * AdQualityI.h
 *
 *  Created on: 2010-12-6
 *      Author: zhaohui.tang
 */

#ifndef ADQUALITYI_MANAGER_H_
#define ADQUALITYI_MANAGER_H_

#include "AdQuality.h"
#include <string>
#include <map>
#include <Ice/Ice.h>
#include <boost/lexical_cast.hpp>
#include "TaskManager.h"
#include "Singleton.h"
#include "../../util/src/AdCommonUtil.h"

namespace xce {
namespace ad {
using namespace MyUtil;
using namespace std;

class AdQualityManagerI : public AdQualityManager, public MyUtil::Singleton<AdQualityManagerI> {
public:
  AdQualityManagerI() :
    init_ok_(false) {
  }
  virtual AdQualityMap GetQualitys(const string& user_key, const ::Ice::Current& = ::Ice::Current());
  virtual AdQualityMap GetQualitysByZone(Ice::Long zone_id, const ::Ice::Current& = ::Ice::Current());
  virtual AdQualityUMap GetQualityUs(const string& user_key, const ::Ice::Current& = ::Ice::Current());
  virtual AdQualityUMap GetQualityUsByZone(Ice::Long zone_id, const ::Ice::Current& = ::Ice::Current());
  virtual void update(const AdStatMap & stat_map, const ::Ice::Current& = ::Ice::Current());
  virtual void reload(const ::Ice::Current& = ::Ice::Current());
  virtual string GetLastUpdateTime(const ::Ice::Current& = ::Ice::Current());
  void Init();
  bool init_ok() const {
    return init_ok_;
  }
  void set_init_ok(bool init_ok) {
    this->init_ok_ = init_ok;
  }
private:
  bool init_ok_;
};

class InitTask : virtual public MyUtil::Task {
public:
  InitTask() {
  }
  virtual void handle();
};

}
}

#endif /* ADQUALITYI_MANAGER_H_ */
