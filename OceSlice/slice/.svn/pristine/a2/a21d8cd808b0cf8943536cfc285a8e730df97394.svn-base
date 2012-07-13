#ifndef __TALK_CACHE_ICE__
#define __TALK_CACHE_ICE__

#include <ObjectCache.ice>
#include <TalkCommon.ice>

module com
{
module xiaonei
{
module talk
{
module cache
{
	class User
	{
		int id;
		string name;
		string statusOriginal; // web's doing == im's signature == xmpp's status
		string statusShifted;//
		string headurl;
		string tinyurl;
	};
	sequence<User> UserSeq;

	/*class ContactList
	{
		MyUtil::IntSeq contactIds;
	};*/
	
	class CacheManager{
		void updateUser(int id, MyUtil::Str2StrMap props);
	};

};
};
};
};

#endif
