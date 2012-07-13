#ifndef MESSAGEHOLDERADAPTER_H_
#define MESSAGEHOLDERADAPTER_H_

#include "MessageHolder.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk {
namespace adapter {

using namespace MyUtil;
using namespace ::talk;
using namespace com::xiaonei::talk::common;

class MessageHolderAdapter : public MyUtil::AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, MessageHolderAdapter> {
public:
	MessageHolderAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void message(const MessagePtr& msg);
	void send(int userid, Ice::Long messageKey);

	void cleanOfflineMessageCount(int userid);
	int getOfflineMessageCount(int userid);
	void loadOfflineMessage(const JidPtr& jid);


	void LoadUnreadNotifyInfoCount(const JidPtr& jid);//pager用户登录的时候，加载未读的Notify系统的消息(不在新留言以及回复提示那显示的)的数量.
protected:
	virtual string name() {
		return "MessageHolder";
	}
	virtual string endpoints() {
		return "@MessageHolder";
	}
	virtual size_t cluster() {
		return 10;
	}

	HolderManagerPrx getManager(int id);
	//HolderManagerPrx getManager(int id);
	HolderManagerPrx getManagerOneway(int id);

	vector<HolderManagerPrx> _managersOneway;
	vector<HolderManagerPrx> _managers;
};

}
}

#endif /*MESSAGEHOLDERADAPTER_H_*/
