#include "IMWindowAdapter.h"
#include <boost/lexical_cast.hpp>
#include <string>
using namespace MyUtil;
using namespace talk::window;
using namespace boost;
using namespace std;
IMWindowManagerPrx IMWindowAdapter::getManager(int id) {
  return locate<IMWindowManagerPrx> (_managers, "M", id, TWO_WAY);
}

IMWindowManagerPrx IMWindowAdapter::getManagerOneway(int id) {
  return locate<IMWindowManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}

void IMWindowAdapter::UserLoginReminder(const JidPtr& jid){
	getManagerOneway(jid->userId)->UserLoginReminder(jid);
}
void IMWindowAdapter::UserAdReminder(const string& path, int userid){
	getManagerOneway(userid)->UserAdReminder(path, userid);
}
void IMWindowAdapter::UserAdReminderOnly(const string& path, const JidPtr& jid){
	getManagerOneway(jid->userId)->UserAdReminderOnly(path, jid);
}
void IMWindowAdapter::UsersAdReminder(const string& path, MyUtil::IntSeq& userids){
	getManagerOneway(userids.at(0))->UsersAdReminder(path, userids);
}

void IMWindowAdapter::SetNotifyData(const string& notifydata) {
//	getManagerOneway()->SetNotifyData(notifydata);
}

void IMWindowAdapter::NeedNotify(int userid, float ver) {
	getManagerOneway(userid)->NeedNotify(userid, ver);
}
