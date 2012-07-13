#ifndef _EXTERNALSEARCHER_ICE
#define _EXTERNALSEARCHER_ICE

#include <Util.ice>
#include <SearchModel.ice>

module externalsearch 
{
	interface ExternalSearcher
	{
		com::xiaonei::search2::model::IndexResult search(MyUtil::Str2StrMap query,int begin,int limit);
	};
};

#endif
