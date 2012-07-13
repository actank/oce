/*
 * AdQualityI.h
 *
 *  Created on: 2011-04-12
 *      Author: zhizhao.sun
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
  //下面是主要接口
  virtual AdQualityUMap GetQualityUsByZone(Ice::Long zone_id, const ::Ice::Current& = ::Ice::Current());

  /***********************************************
   * 以下接口为了兼容老版本，保留，在以后重构时考虑去除
   ***********************************************/
  virtual AdQualityMap GetQualitys(const string& user_key, const ::Ice::Current& = ::Ice::Current());
  virtual AdQualityMap GetQualitysByZone(Ice::Long zone_id, const ::Ice::Current& = ::Ice::Current());
  virtual AdQualityUMap GetQualityUs(const string& user_key, const ::Ice::Current& = ::Ice::Current());

  virtual void update(const AdStatMap & stat_map, const ::Ice::Current& = ::Ice::Current());
  virtual void reload(const ::Ice::Current& = ::Ice::Current());
  virtual string GetLastUpdateTime(const ::Ice::Current& = ::Ice::Current());
};

}
}

#endif /* ADQUALITYI_MANAGER_H_ */
