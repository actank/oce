/*
 * AdNotifyAdapter.h
 *
 *  Created on: Jul 2, 2010
 *      Author: yejingwei
 */

#ifndef ADNOTIFYADAPTER_H_
#define ADNOTIFYADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "AdMatch.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class AdMatchAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::AdChannel, AdMatchAdapter> {
public:
  AdMatchAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

  AdInfoSeq GetAds(string & user_profile, int count,const string & referer);
  void Click(int uid, const string & ip, string & text,const string & referer,const string & click_url);

//  void Suspend(Ice::Long group_id);
//  void Resume(Ice::Long group_id);
  void LoadOneGroup(Ice::Long group_id);
  void DelOneGroup(Ice::Long group_id);
  void ReloadOneGroup(Ice::Long group_id);

  void ReportDefault(int uid,const string & ip,int count,const DefaultSeq & ads);
  void GetAvailableAds(vector<Ice::Long> & ids);
  AdMatchPrx getManager(int id);
  AdMatchPrx getManagerOneway(int id);

protected:
  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@AdMatch";
  }
  virtual size_t cluster() {
    return 10;
  }

  vector<AdMatchPrx> _managersOneway;
  vector<AdMatchPrx> _managers;
};

}
;
}
;

#endif /* ADNOTIFYADAPTER_H_ */
