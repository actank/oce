/*
 * XiaoNei search interface definition.
 * Written by bochun.bai@dudu-inc.com 
 * History: 
 *   2007-11-06 13:56 Create
 *
 */
module com
{
module xiaonei
{
module search2
{
module model
{
	// Define some basic types
	sequence<string> StrSeq;

	dictionary<string, string> Str2StrMap;
	sequence<int> IntSeq;

	// Define IndexType
	enum IndexType
	{
		User, Tribe, Page, App, Share
	};

	enum GroupType
	{
		GroupSearch,GroupBrowse,GroupGuide,TestGroupSearch,TestGroupBrowse,TestGroupGuide,Group22Search,Group65Search,Group22Browse,Group65Browse,Group22Guide,Group65Guide,GroupShare,TestGroupShare
	};

	// Define conditions related object
	enum IndexConditionType
	{
		Browse, FilterSearch, Guide, OpenSearch, ProfileNav, OpenTribeSearch, OpenPeopleSearch, Public,Flyer, OpenPageSearch, OpenAppSearch, NetbarSearch, OfferFriends, EDMSearch, UserSearch, ShareSearch,OpenPageSearchForWap,ExternalSearch,ExternalPeopleSearch
	};
	struct IndexCondition
	{
		IndexConditionType type;
		Str2StrMap queryCondition;
		Str2StrMap opUserCondition;
	};

	// Define result related object
	struct IndexResultItem
	{
		IndexType type;
		int id;
		Str2StrMap content;
	};
	sequence<IndexResultItem> IndexResultItemSeq;
	
  struct IndexShareResultItem
	{
		string md5;
		Str2StrMap content;
	};
	sequence<IndexShareResultItem> IndexShareResultItemSeq;
	
  dictionary<IndexType, int> Type2IntMap;
	
  class IndexShareResult
	{
		int currentPos;
		int totalCount;
		double timeCost;
		int Count;
		IndexShareResultItemSeq contents;
	};
	
	class IndexResult
	{
		int currentPos;
		int totalCount;
		double timeCost;
		Type2IntMap typedCount;
		IndexResultItemSeq contents;
	};
};
};
};
};

