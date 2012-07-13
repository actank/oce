#include "CommonTask.h"
#include "IceLogger.h"
#include "PresenceMsgHandlerAdapter.h"
#include "OnlineManagerI.h"
#include "XmppTools.h"
#include "OnlineBitmapAdapter.h"


using namespace talk::adapter;
using namespace talk::online;
using namespace com::xiaonei::talk::util;
using namespace talk::onlinebitmap;

void PresenceTask::handle(){
	//MCE_WARN("PresenceTask::handle --> " << jidToString(_avatar->jid));
	MCE_DEBUG("OnlineStat: old " << _oldStat << ", new " << _newStat);

	UserPoolPtr pool = OnlineManagerI::instance().getPool(_avatar->jid->userId);
	if (!pool) {
		return;
	}

	NotifyType type;
	//AvatarPtr activeAvatar = _avatar;
	AvatarMap avatars;
	AvatarSeq self = OnlineManagerI::instance().getUserAvatars(_avatar->jid->userId,IM_ONLINE|WP_ONLINE|PHONE_ONLINE);
	//AvatarToItsOtherAvatars, AvatarToAll, AllAvatarsToBuddies
	if(_oldStat==INVISIABLE && _newStat==INVISIABLE){
		type = AvatarToItsOtherAvatars;
		//self = getUserAvatars(jid->userId);
	}else if(_oldStat!=INVISIABLE && _newStat!=INVISIABLE){
		type = AvatarToAll;
		//self = getUserAvatars(jid->userId);
		avatars = OnlineManagerI::instance().getBuddyAvatars(_avatar->jid->userId,IM_ONLINE);
	}else{
		type = AllAvatarsToBuddies;
		//self = getUserAvatars(jid->userId);
		avatars = OnlineManagerI::instance().getBuddyAvatars(_avatar->jid->userId,IM_ONLINE);
	}

	MCE_DEBUG("PresenceTask::handle --> self size=" << self.size() << " buddies=" << avatars.size());

	if(_avatar->stat != OFFLINE){
		for(AvatarSeq::iterator it=self.begin(); it!=self.end(); ++it){
			if((*it)->jid->index == _avatar->jid->index ){
				self.erase(it);
				break;
			}
		}
	}

	//排除webpagger用户
//	for(AvatarMap::iterator it=avatars.begin(); it!=avatars.end(); ){
//		for(AvatarSeq::iterator seqIt=it->second.begin(); seqIt!=it->second.end();){
//			if((*seqIt)->jid->endpoint.empty() || (*seqIt)->jid->endpoint.at(0)!='T'){
//				seqIt = it->second.erase(seqIt);
//			}else{
//				++seqIt;
//			}
//		}
//		if(it->second.empty()){
//			avatars.erase(it++);
//		}else{
//			++it;
//		}
//	}

	AvatarSeq seq;
	seq.push_back(_avatar);
	MCE_DEBUG("PresenceTask::handle --> self size=" << self.size() << " buddies=" << avatars.size());
	try{
		MyUtil::Str2StrMap::const_iterator it = _parameters.begin();
        	for(; it!=_parameters.end(); ++it){
                	MCE_INFO("##~~~~~~~~~~~~ " << " " << it->first << ":" << it->second);
        	}

		PresenceMsgHandlerAdapter::instance().notifyBuddiesOnlineStat(seq, self, avatars, type, _parameters);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceTask::handle --> notifyBuddiesOfOnlineStat error, " << e);
	}
}

