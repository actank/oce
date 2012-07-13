#ifndef __TALK_LOGIC_ICE__
#define __TALK_LOGIC_ICE__

#include <TalkCache.ice>

module com
{
module xiaonei
{
module talk
{
	/*
	class LogicManager
	{
		void message(common::Jid jid, string msg);
		
		
		common::Passport verify(string email, string password);
		common::Jid bind(common::Jid j);
		void unbind(common::Jid j);
		
		
		void wBind(int userId);
        void webBind(int userId);
        void wUnbind(int userId);
        void webUnbind(int userId);

		void broadcastPresence(common::Jid j, string show, string ver, string ext,string status,string photo, string nick);
		void presence(common::Jid jid, string show, string ver, string ext);
		void presenceSubscribe(string id, int from,int to, string status);
		void presenceSubscribed(string id, int from,int to);
		void presenceUnsubscribed(string id, int from,int to);
		void offlineNotify(common::JidSeq seq);
		void webPresence(int userId);
		
		
		void deliverFeed(string msg, string type, MyUtil::IntSeq ids );
		void loadOfflineMessage(common::Jid jid);
		
		
		
		void removeBuddyNotify(int host, int guest);
		void notifySetPrivacy(common::Jid j , string msg);
		
		
		
	};*/
	enum NotifyType{AvatarToItsOtherAvatars, AvatarToAll, AllAvatarsToBuddies};
	class LoginHandler{
	
		//void message(common::Jid jid, string msg);
		common::Passport verify(string email, string password);
		void bind(common::Jid j);
		void unbind(common::Jid j);
	};
	
	class PresenceHandler{
		void message(common::Jid jid, string msg);
		//void broadcastPresence(common::Jid j, string show, string ver, string ext,string status,string photo, string nick);
		void presence(common::Jid jid, string show, string ver, string subver, string ext);
		void presenceWithSecver(common::Jid jid, string show, string ver, string subver, string ext, string secver);
		void presenceSubscribe(string id, int from,int to, string status);
		void presenceSubscribeWithPostscript(string id, int from,int to, string status, string postscript);
		void presenceSubscribed(string id, int from,int to);
		void presenceUnsubscribed(string id, int from,int to);
		void offlineNotify(common::JidSeq seq);
		void webPresence(int userId);
		void nodifyBuddiesOnlineStat(common::AvatarSeq activeAvatars, common::AvatarSeq selfOtherAvatars, common::AvatarMap buddies, NotifyType type, MyUtil::Str2StrMap paras);
		void mucOfflineNotify(common::JidSeq jids);
		void offline(common::Jid j);
		void userStatusChanged(int userId, int userStatus);
//		void ChangeNotifyData(string notifydata);
	};
	
	class MucPresenceHandler{
		void message(common::Jid jid, string msg);
		//void broadcastPresence(common::Jid j, string show, string ver, string ext,string status,string photo, string nick);
		void presence(common::Jid jid, string show, string ver, string subver, string ext);
		void presenceWithSecver(common::Jid jid, string show, string ver, string subver, string ext, string secver);
		void presenceSubscribe(string id, int from,int to, string status);
		void presenceSubscribeWithPostscript(string id, int from,int to, string status, string postscript);
		void presenceSubscribed(string id, int from,int to);
		void presenceUnsubscribed(string id, int from,int to);
		void offlineNotify(common::JidSeq seq);
		void webPresence(int userId);
		void nodifyBuddiesOnlineStat(common::AvatarSeq activeAvatars, common::AvatarSeq selfOtherAvatars, common::AvatarMap buddies, NotifyType type, MyUtil::Str2StrMap paras);
		void mucOfflineNotify(common::JidSeq jids);
		void offline(common::Jid j);
		void userStatusChanged(int userId, int userStatus);
		void ChangeNotifyData(string notifydata);
	};

	class MessageHandler{
		void deliverAppNotice(int toid,int appid,string appName, string appIcon, string msg);
		void sendMessage(int from, int to, string msg);
    void message(common::Jid jid, string msg);
		void deliverFeed(string msg, string type, MyUtil::IntSeq ids );
		//void loadOfflineMessage(common::Jid jid);
	};
	
	dictionary<int,MyUtil::StrSeq> BuddyItemMap;
	
	class BuddyItem{
		int host;
		BuddyItemMap items;
	};
	
	 
	class IqHandler{
		void RemoveNotifyBySource(int userId, int type, long source);
		void RemoveNotifyById(int userId, long notifyId);
		void NotifyBuddyApplyChange(int userId);
		void message(common::Jid jid, string msg);
		void removeBuddyNotify(int host, int guest);
		void changeBuddyGroupNotify(BuddyItem bi);
		void notifySetPrivacy(common::Jid j , string msg);
	};
	

};
};
};

#endif
