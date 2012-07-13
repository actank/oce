#ifndef FEEDDISPATCHERADAPTER_H_
#define FEEDDISPATCHERADAPTER_H_

#include "RFeed.h"
#include <vector>
#include "Singleton.h"
#include "AdapterI.h"
#include "FeedConfig.h"

namespace xce {
namespace feed {

class ConfigManagerAdapter: public MyUtil::AdapterI,
		public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel,  ConfigManagerAdapter> {
public:
	ConfigManagerAdapter(){
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}


  void SetSendConfig(int uid, int type, int appid, bool newsconfig);
  bool GetSendConfig(int uid, int type,int appid);

  void SetRecvConfig(int uid, int type, int appid, bool newsconfig);
  MyUtil::IntArray FilterRecvConfig(MyUtil::IntArray target,int type,int appid);


  UserConfigDict GetUserBlockedAppDict(int uid); 

protected:
	virtual string  name() {
		return "FeedConfig";
	}
	virtual string endpoints() {
		return "@FeedConfig";
	}
	virtual size_t cluster() {
		return 1;
	}

	ConfigManagerPrx getManager(int id);
	ConfigManagerPrx getManagerOneway(int id);

	vector<ConfigManagerPrx> _managersOneway;
	vector<ConfigManagerPrx> _managers;
};

};
};

#endif /*BUDDYCOREADAPTER_H_*/
