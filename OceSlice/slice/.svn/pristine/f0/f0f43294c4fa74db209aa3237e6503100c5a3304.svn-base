#ifndef __IM_WINDOW__
#define __IM_WINDOW__

#include <Util.ice>
#include <TalkCommon.ice>

module talk{
module window{
class IMWindowManager{
	void ReloadStaticPages();//通知服务重新加载tpl文件
	void ReloadPathConfig();//通知服务重新加载tpl文件的配置xml    IMWindow.conf
	void UserLoginReminder(com::xiaonei::talk::common::Jid jid);
	void UserAdReminder(string path, int userid);
	void UserAdReminderOnly(string path, com::xiaonei::talk::common::Jid jid);
	void UsersAdReminder(string path, MyUtil::IntSeq userid);
  void NeedNotify(int userid, float ver);
  void SetNotifyData(string notifydata);
};
};
};

#endif
