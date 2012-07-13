#ifndef IMWINDOWADAPTER_H__
#define IMWINDOWADAPTER_H__

#include "Singleton.h"
#include "AdapterI.h"
#include "IMWindow.h"

namespace talk {
namespace window{


using namespace MyUtil;
using namespace com::xiaonei::talk::common;

class IMWindowAdapter: public MyUtil::AdapterI, public MyUtil::AdapterISingleton<MyUtil::TalkChannel, IMWindowAdapter> {
public:
  IMWindowAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

	void UserLoginReminder(const JidPtr& jid);
	void UserAdReminder(const string& path, int userid);
	void UserAdReminderOnly(const string& path, const JidPtr& jid);
	void UsersAdReminder(const string& path, MyUtil::IntSeq& userids);
  void SetNotifyData(const string& notifydata);
  void NeedNotify(int userid, float ver);

	IMWindowManagerPrx getManager(int id);
	IMWindowManagerPrx getManagerOneway(int id);
private:

  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@IMWindow";
  }
  virtual size_t cluster() {
    return 1;
  }
  vector<IMWindowManagerPrx> _managersOneway;
  vector<IMWindowManagerPrx> _managers;
};

};
};


#endif
