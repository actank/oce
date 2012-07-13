#ifndef _SEARCH_LOGGER_ICE
#define _SEARCH_LOGGER_ICE

#include <Util.ice>

module com 
{
module renren 
{
module search2 
{
module logger 
{
	interface SearchLogger
	{
		void access(MyUtil::Str2StrMap log);
		string getExplain(MyUtil::Str2StrMap log);
	};
};
};
};
};

#endif
