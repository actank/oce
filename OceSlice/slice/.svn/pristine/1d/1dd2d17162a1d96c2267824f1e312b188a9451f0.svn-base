#ifndef __ONLINE_CENTER_ICE__
#define __ONLINE_CENTER_ICE__

#include <OnlineStatSync.ice>
#include <TalkCommon.ice>
module talk
{
module online
{
	//enum OnlineType{OFFLINE = 0, WEB_ONLINE = 1, WP_ONLINE = 2, IM_ONLINE = 4,};
	class UserOnlineType{
		int userId;
		int onlineType;
	};

	sequence<UserOnlineType> UserOnlineTypeSeq;
	
  const int kUserPoolCount = 100000;
  class SyncBitmapData {
    int userPoolIndex;
    UserOnlineTypeSeq stateSeq;
  };

	class OnlineManager
	{

		bool presence(com::xiaonei::talk::common::Jid jid, com::xiaonei::talk::common::OnlineStat stat, 
		string ver, string ext, MyUtil::Str2StrMap paras);
		
		void notify(BuddyOnlineNotifySeq notifies,int index);
		
		void offline(com::xiaonei::talk::common::Jid jid);

    void ReloadBitmapByUserPool(int poolIndex);
		//void updateOnlineBitmap(int serverId);
		
		int getBuddyCount(int userId);
		//------------- rename stat to onlinetype
		UserOnlineTypeSeq getBuddyStatSeq(int userId);
		UserOnlineTypeSeq getUsersStatSeq(MyUtil::IntSeq ids);	
		UserOnlineTypeSeq getBuddyOnlineTypeSeq(int userId);
		UserOnlineTypeSeq getUsersOnlineTypeSeq(MyUtil::IntSeq ids);
		
		UserOnlineTypeSeq getRandomOnlineUsers(int count);
			
		MyUtil::IntSeq getBuddyIds(int userId);
		
		com::xiaonei::talk::common::AvatarMap getBuddyAvatars(int userId, int onlineType);
		com::xiaonei::talk::common::AvatarMap getBuddyAvatarsAndSelf(int userId, int buddyOnlineType, int selfOnlineType);
		com::xiaonei::talk::common::AvatarSeq getUserAvatars(int userId, int onlineType);
		com::xiaonei::talk::common::Avatar getAvatar(com::xiaonei::talk::common::Jid jid);
		com::xiaonei::talk::common::AvatarSeq getAvatars(com::xiaonei::talk::common::JidSeq jids);
		
		com::xiaonei::talk::common::JidSeq getUserJids(int userId, int onlineType);
		com::xiaonei::talk::common::JidSeqMap getUsersJids(MyUtil::IntSeq ids, int onlineType);
		com::xiaonei::talk::common::AvatarMap getUsersAvatars(MyUtil::IntSeq ids, int onlineType);
		
		com::xiaonei::talk::common::JidSeqMap getBuddyJids(int userId, int onlineType);
		com::xiaonei::talk::common::JidSeqMap getBuddyJidsAndSelf(int userId, int buddyOnlineType, int selfOnlineType);
		
		void addBuddy(int host, int guest);
		void removeBuddy(int host, int guest);
		
		void closeProxyServer(string endpoint, bool isBroadcast);	

		com::xiaonei::talk::common::AvatarSeq getOldestAvatars(int index, int beg, int limit, int type);
	};


};
};

#endif
