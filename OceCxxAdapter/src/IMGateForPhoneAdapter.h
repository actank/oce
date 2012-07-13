#ifndef IMGATEFORPHONEADAPTER_H__
#define IMGATEFORPHONEADAPTER_H__

#include "Singleton.h"
#include "AdapterI.h"
#include "IMGateForPhone.h"

namespace com {
namespace xiaonei {
namespace talk {
namespace gate {


using namespace MyUtil;

class IMGateForPhoneAdapter: public MyUtil::AdapterI, public MyUtil::AdapterISingleton<MyUtil::TalkChannel, IMGateForPhoneAdapter> {
public:
  IMGateForPhoneAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

	void SendMessage(int phoneuserid, int touserid, const string& message);//手机用户发送聊天消息
	void PhoneUserOnline(int phoneuserid);//手机用户上线
	void PhoneUserOffline(int phoneuserid);//手机用户下线

	//void ServerDisable();//调用后，停止这个服务对IM上层服务的一切调用
	//void ServerEnable();//调用后，允许这个服务对IM上层服务进行调用


	IMGateForPhoneManagerPrx getManager(int id);
	IMGateForPhoneManagerPrx getManagerOneway(int id);
private:

  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@IMGateForPhone";
  }
  virtual size_t cluster() {
    return 1;
  }
  vector<IMGateForPhoneManagerPrx> _managersOneway;
  vector<IMGateForPhoneManagerPrx> _managers;
};

};
};
};
};


#endif
