
module com
{
module xiaonei
{
module services
{
module Suggestion
{
	sequence<int> IntSeq;
	dictionary<int, string> Int2StrMap;
	enum IndexType
	{
		App
	};
	interface SuggestionManager
	{
		void buildIndex(IndexType type);
		Int2StrMap search(IndexType type, string condition, int limit);
	};
};
};
};
};
