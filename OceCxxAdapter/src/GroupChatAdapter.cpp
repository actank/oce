#include "GroupChatAdapter.h"

namespace xce {
namespace talk{
namespace groupchat{
GroupChatManagerPrx GroupChatAdapter::getManagerOneway() {
	return GroupChatManagerPrx::uncheckedCast(locate<GroupChatManagerPrx> ("M", ONE_WAY));
}

bool GroupChatAdapter::join(::Ice::Long feedId, ::Ice::Int userId,
		const ::Ice::Context* __context){
	return getManagerOneway()->join(feedId, userId);
}
bool GroupChatAdapter::kickMember(::Ice::Long feedId, ::Ice::Int userId,
		const ::Ice::Context* __context){
	return getManagerOneway()->kickMember(feedId, userId);
}
bool GroupChatAdapter::sendMsg(::Ice::Long feedId, ::Ice::Int userId,
		const ::std::string& msg, const ::Ice::Context* __context){
	return getManagerOneway()->sendMsg(feedId, userId,msg);
}


}
;
}
;
}
;
