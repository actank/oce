#ifndef __TALK_STAT_ICE__
#define __TALK_STAT_ICE__

#include <TalkCommon.ice>

module talk
{
module stat
{
	class StatManager
	{
		void login(MyUtil::IntSeq ids);
		void wLogin(MyUtil::IntSeq ids);
			
		void totalMsgCount(int count);
		void chatingMsgCount(int count);
		void msgStat(com::xiaonei::talk::common::Jid from , com::xiaonei::talk::common::Jid to);
		void versionStat(int userId, string ver, string subver);
		
		void mucMsgCount(int msgCount, int msgBroadcastCount);
		
		void incSetStatusCount(int c);
		void incReplyStatusCount(int c);
		void incReplyPhotoCount(int c);
		void incReplyBlogCount(int c);
		void incReplyShareCount(int c);
	};

};
};

#endif
