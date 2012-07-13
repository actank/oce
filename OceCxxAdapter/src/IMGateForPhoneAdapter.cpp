#include "IMGateForPhoneAdapter.h"
#include <boost/lexical_cast.hpp>
#include <string>
using namespace MyUtil;
using namespace com::xiaonei::talk::gate;
using namespace boost;
using namespace std;
IMGateForPhoneManagerPrx IMGateForPhoneAdapter::getManager(int id) {
  return locate<IMGateForPhoneManagerPrx> (_managers, "M", id, TWO_WAY);
}

IMGateForPhoneManagerPrx IMGateForPhoneAdapter::getManagerOneway(int id) {
  return locate<IMGateForPhoneManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}


void IMGateForPhoneAdapter::SendMessage(int phoneuserid, int touserid, const string& message){//手机用户发送聊天消息
	getManagerOneway(phoneuserid)->SendMessage(phoneuserid, touserid, message);
}
void IMGateForPhoneAdapter::PhoneUserOnline(int phoneuserid){//手机用户上线
	getManagerOneway(phoneuserid)->PhoneUserOnline(phoneuserid);
}
void IMGateForPhoneAdapter::PhoneUserOffline(int phoneuserid){//手机用户下线
	getManagerOneway(phoneuserid)->PhoneUserOffline(phoneuserid);
}
/*
void IMGateForPhoneAdapter::ServerDisable(){//调用后，停止这个服务对IM上层服务的一切调用
	getManagerOneway(0)->ServerDisable();
}
void IMGateForPhoneAdapter::ServerEnable(){//调用后，允许这个服务对IM上层服务进行调用
	getManagerOneway(0)->ServerEnable();
}
*/
