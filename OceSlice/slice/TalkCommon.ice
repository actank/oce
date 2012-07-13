#ifndef __TALK_COMMON_ICE__
#define __TALK_COMMON_ICE__

#include <Util.ice>
//#include <IMSession.ice>

module com
{
module xiaonei
{
module talk
{
module common
{
	const string TALKDOMAIN = "talk.xiaonei.com";
	


	class Passport
	{
		int userId;
		int result; // -1：用户名密码错误    -2：用户状态受限   大于等于0为正常
	};

	class Jid
	{
		int userId;
		string endpoint;  //endpoint为web表示WEB
		long index;
	};
	sequence<Jid> JidSeq;
	dictionary<int, Jid> JidMap;
	dictionary<int, JidSeq> JidSeqMap;
	
	class SysJid
	{
		string node;
		string category;
	};
	
	
	enum OnlineStat{OFFLINE, INVISIABLE, NOTAVAILABLE, BUSY, AWAY, ONLINE};
	class Avatar
	{
		com::xiaonei::talk::common::Jid	jid;
		OnlineStat stat;
		string ver;
		string ext;
		int stamp;
	};
	sequence<Avatar> AvatarSeq;
	dictionary<int, AvatarSeq> AvatarMap;
	
	class OnlineInfo
	{
		int userId;
		string endpoint;
		long index;
		string show;
		string ver;
		string ext;
		int stamp;
	};
	
	sequence<OnlineInfo> OnlineInfoSeq;
	dictionary<int, OnlineInfoSeq> OnlineInfoMap;
	
	
	class Message
	{
		Jid from;
		Jid to;
		string msg;
		int type;
		long msgKey;
	};
	sequence<Message> MessageSeq;
	dictionary<long,Message> MessageMap;
	
//-----------------------------------
//---- Multi User Chat

		
	class RoomId{
		string name;
		string domain;
	};
	sequence<RoomId> RoomIdSeq;
	
	class RoomJid
	{
		RoomId rid;
		string nick;
	};
	
	enum Affiliation {Outcast,None,Member,Admin,Owner};
	enum RoomRole {NoneRole,Visitor,Participant,Moderator};
	
	class RoomMember{
		RoomId rid;
		int userId;
		Affiliation aType;
		
	};
	sequence<RoomMember> RoomMemberSeq;
	
	class Occupant{
		RoomJid rJid;
		Jid 	fJid;
		Affiliation aType;
		RoomRole role;
	};
	sequence<Occupant> OccupantSeq;
	class RoomType
	{
		bool hidden; // hidden = 0; public = 1
		bool memberOnly; // open = 0; memberonly = 1;
		bool moderated;  // Unmoderated  = 0; Moderated = 1
		int anonymousType;  // Non-Anonymous = 0; Semi-Anonymous = 1; Fully-Anonymous Room = 2;
		bool secured;	// Unsecured Room = 0; Password-Protected = 1;
		bool persistent; // Temporary Room = 0; Persistent Room = 1;
	};
	
	class Room{
		string title;
		string descr;
		RoomType type;
		string password;
		int maxOccupantsCount;
		bool allowUserToInvite;// not allow = 0; allow = 1;
	};
	sequence<Room> RoomSeq;
	
	
};
};
};
};

#endif
