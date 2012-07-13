
#ifndef _PERSONAL_SEARCHER_ICE
#define _PERSONAL_SEARCHER_ICE
#include <Util.ice>
module com
{
		module xiaonei
		{
				module search3
				{
						module server
						{
								dictionary<string,MyUtil::Str2StrMap> QueryMap;
								interface PersonalSearcher
								{
										MyUtil::IntList search(int userId, QueryMap query, int begin, int limit);
								};
						};
				};
		};
};
#endif
