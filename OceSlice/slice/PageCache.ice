#ifndef __PAGECACHE_ICE__
#define __PAGECACHE_ICE__

#include <Util.ice>

module talk
{
module http
{
	class Content
	{
		int statusCode;
		int timeout;
		bool isBin;
		string contentType;
		string data;
	};

	interface PageCacheManager 
	{
		Content GetContent(string path, MyUtil::Str2StrMap cookies, MyUtil::Str2StrMap parameter);		
		void MemberStatusChange(int groupid, int userid, int mucstatus);
		void UpdateRoomName(int groupid, string gorupname);
		void ReloadStaticPages();
		int GetVideoViewPower();
		void SetVideoViewPower(int value);
	};
};
};
#endif
