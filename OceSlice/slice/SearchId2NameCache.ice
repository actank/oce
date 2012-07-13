#ifndef _SEARCH_ID2NAME_CACHE_ICE
#define _SEARCH_ID2NAME_CACHE_ICE

module xce
{
module search
{
	dictionary<int, string> Int2StringMap;
  sequence<int> IntSeq;
	interface SearchId2NameCache
	{
		string GetName(int userId);
		Int2StringMap GetNames(IntSeq userIds);
	};
};
};
#endif
