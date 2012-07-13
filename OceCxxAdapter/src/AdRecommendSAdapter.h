/*
 * AdRecommendSAdapter.h
 *
 *  Created on: 2010-11-24
 *      Author: sunzz
 */

#ifndef ADRECOMMENDSADAPTER_H_
#define ADRECOMMENDSADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "AdRecommendationHandler.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class AdRecommendSAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::AdChannel, AdRecommendSAdapter> {
public:
	AdRecommendSAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

  AdResult GetAds(int userid, long zoneid);
  void Click(int userid, long zoneid, long groupid);
  bool AddUserListL(int userid, long zoneid, int sindex);
  bool AddUserListH(int userid, long zoneid, int sindex);
  void CleanUserClick(int userid, long zoneid);
  int GetIndex(int uid, long zid);

private:
  AdRecommendationHandlerPrx getManager(int id);
  AdRecommendationHandlerPrx getManagerOneway(int id);

  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@AdRecommendS";
  }
  virtual size_t cluster() {
    return 1;
  }

  vector<AdRecommendationHandlerPrx> _managersOneway;
  vector<AdRecommendationHandlerPrx> _managers;
};

}
}

#endif /* ADRECOMMENDADAPTER_H_ */
