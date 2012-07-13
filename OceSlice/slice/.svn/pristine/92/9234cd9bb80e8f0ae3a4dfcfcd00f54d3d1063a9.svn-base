#ifndef __DOING_SEARCH_ICE__
#define __DOING_SEARCH_ICE__

#include <Util.ice>
module xce
{
module doingsearch
{
	dictionary<string, string> Str2StrMap;

	interface DoingSearchFacade {
			void access(long doingID, long userID, long timestamp, string content);
	};

	struct DoingItem
	{
		long id;
		long userid;
		long time;
		string content;
	};

	sequence<DoingItem> DoingItemSeq;

	struct SearchResult
	{
		int current;
		int total;
		double cost;
		DoingItemSeq items;
	};

	interface DoingSearchManager
	{
		SearchResult search(Str2StrMap condition, int begin, int limit);
	};

	interface DoingSearchContainer
	{
		SearchResult search(Str2StrMap condition, int begin, int limit);
		void addDoing(DoingItem item);
	};
};
};

#endif
