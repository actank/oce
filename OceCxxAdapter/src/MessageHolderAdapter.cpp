#include "MessageHolderAdapter.h"

using namespace talk::adapter;
using namespace talk;
using namespace MyUtil;

HolderManagerPrx MessageHolderAdapter::getManagerOneway(int id) {
	return locate<HolderManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

HolderManagerPrx MessageHolderAdapter::getManager(int id) {
	//	if(timeout > 0){
	//		HolderManagerPrx prx = locate<HolderManagerPrx>(_managers, "M", id, TWO_WAY);
	//		return HolderManagerPrx::uncheckedCast(prx->ice_timeout(timeout));
	//	}
	return locate<HolderManagerPrx>(_managers, "M", id, TWO_WAY);

}

void MessageHolderAdapter::message(const MessagePtr& msg) {
	getManagerOneway(msg->to->userId)->message(msg);
}

void MessageHolderAdapter::send(int userid, Ice::Long messageKey) {
	getManagerOneway(userid)->send(userid, messageKey);
}

void MessageHolderAdapter::cleanOfflineMessageCount(int userid) {
	getManagerOneway(userid)->cleanOfflineMessageCount(userid);
}

int MessageHolderAdapter::getOfflineMessageCount(int userid) {
	return getManager(userid)->getOfflineMessageCount(userid);
}

void MessageHolderAdapter::loadOfflineMessage(const JidPtr& jid){
	getManagerOneway(jid->userId)->loadOfflineMessage(jid);
}
void MessageHolderAdapter::LoadUnreadNotifyInfoCount(const JidPtr& jid){//pager用户登录的时候，加载未读的Notify系统的消息(不在新留言以及回复提示那显示的)的数量.
	getManagerOneway(jid->userId)->LoadUnreadNotifyInfoCount(jid);
}
