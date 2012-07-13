#ifndef __TALK_CACHE_LOADER_ICE__
#define __TALK_CACHE_LOADER_ICE__

#include <TalkCommon.ice>

module com
{
module xiaonei
{
module talk
{
module cache
{
module loader
{

	class TalkUser
	{
		int id;
		string name;
		string statusOriginal; // web's doing == im's signature == xmpp's status
		string statusShifted;//
		string headurl;
		string tinyurl;
	};
	sequence<TalkUser> TalkUserSeq;
  dictionary<int, TalkUser> TalkUserMap;

	class CacheLoaderManager{
		void updateUser(int userid, MyUtil::Str2StrMap props);
		TalkUser LoadById(int userid);
		TalkUserMap LoadBySeq(MyUtil::IntSeq userids);
	};

};
};
};
};
};

#endif
