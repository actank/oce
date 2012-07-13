#ifndef REST_LOGIC_ICE
#define REST_LOGIC_ICE

#include <TalkCommon.ice>

module talk
{
module rest
{
	class RestResult{
		int 	err;
		string 	errstr;
		string	xml;
	};
	
	interface RestLogicManager{
		void setStatus(com::xiaonei::talk::common::Jid jid, 
						com::xiaonei::talk::common::OnlineStat state, string status);
		void replyStatus(int userid, long statusId, int ownerId, int replieduserId, string content);
		void replyPhoto(int userid, long photoId, int ownerId, int replieduserId, string content, int type);
		void replyAlbum(int userid, long albumId, int ownerId, int replieduserId, string content, int type);
		void replyBlog(int userid, long blogId, int ownerId, int replieduserId, string content, int type);
		void useImNotify(int userid);
		void activeUserNotify(int userid);
		void activeUserNotify2(int userid);
		void sendNotice(int userid, string data, MyUtil::IntSeq users);
		void sendLobbyNotice(int userid, string data, MyUtil::IntSeq users);
		void notifyResult(long reqId, RestResult res);
		void getSessionKey(com::xiaonei::talk::common::Jid jid);
		void getSessionKey2(com::xiaonei::talk::common::Jid jid, int iqid);
	};
};
};

#endif
