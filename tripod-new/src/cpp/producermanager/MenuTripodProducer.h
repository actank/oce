/*
 * =====================================================================================
 *
 *       Filename:  MenuCacheTripodProducer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年12月13日 16时56分41秒
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#ifndef __MENU_TRIPOD_PRODUCER_H__
#define __MENU_TRIPOD_PRODUCER_H__
 
#include "TripodProducer.h"
#include "boost/lexical_cast.hpp"
#include "app/MenuCache/MenuTripod/MenuTripodShare.h"
#include "ServiceI.h"

namespace xce {
namespace menutripod {

using namespace std;
using namespace com::renren::tripod;
using namespace MyUtil;
using namespace com::xiaonei::xce;

class MenuTripodProducer : virtual public TripodProducer {
public:
  MenuTripodProducer() {
  }
  virtual ~MenuTripodProducer() {}
  virtual DataMap create(const KeySeq& keys, bool isMaster);
  virtual KeySeq createHotKeys(const string& beginKey, int limit);
  virtual KeySeq createKeys(const string& beginKey, int limit);
private:
  void fillDataMap(DataMap& dataMap, const Statement& sql, CacheType cacheType);
};

}
}

#endif
