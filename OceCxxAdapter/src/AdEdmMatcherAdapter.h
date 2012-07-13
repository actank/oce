#ifndef _AD_EDM_MATCHER_H_
#define _AD_EDM_MATCHER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "AdNotify.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class AdEdmMatcherAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::NotifyChannel, AdEdmMatcherAdapter> {
public:
  AdEdmMatcherAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

  void Login(int uid);
  void Block(int uid, int promotion);
  void SetInterval(int uid, IntervalType time);
  void MarkReadByUid(int uid);          //将uid用户的所有message设为已读
  void LoadOnePromotion(int promotion); //将一种promotion的信息加载到内存
  void DelPromotion(int promotion);     //删除一种promotion
  AdPartners GetAdPartners(int uid, MyUtil::IntSeq ads);
  HotAdInfoSeq GetAdInfoSeq();
  void Reload();
protected:
  AdNotifierPrx getManager(int id);
  AdNotifierPrx getManagerOneway(int id);

  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@AdEdmMatcher";
  }
  virtual size_t cluster() {
    return 1;
  }

  vector<AdNotifierPrx> _managersOneway;
  vector<AdNotifierPrx> _managers;
};

}
}

#endif // _AD_NOTIFY_ADAPTER_H_
