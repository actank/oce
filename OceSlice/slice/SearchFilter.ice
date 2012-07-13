#ifndef __SEARCHFILTER_ICE__
#define __SEARCHFILTER_ICE__

#include <Util.ice>
#include <SearchModel.ice>

module com
{
module xiaonei
{
module search2
{
module searchfilter 
{

interface SearchFilterManager{
	bool search(int userid,string query, int begin, int limit, string ip);	
	void block(int userid);
	MyUtil::Str2StrMapSeq getBlocked();
};

};
};
};
};
#endif
