#ifndef __USERFEEDS_ICE__
#define __USERFEEDS_ICE__

#include <RFeed.ice>

module xce{
module feed{

	const int FeedidPrefix = 0;
	const int UseridPrefix = 1;
	const int TypePrefix = 2;

	const string AppidContextPrefix = "AppId";
	const string UseridContextPrefix = "UserId";
	
	struct FeedMeta {
		long feedid;
		int userid;
		int type;
		int time;
		int weight;
		long mergeid;
	};
	sequence<FeedMeta> FeedMetaSeq;
	
	interface UserFeeds {
		void put(FeedMeta meta);

		FeedMetaSeq get(MyUtil::IntSeq userids, int begin, int limit);
		FeedMetaSeq getByTypes(MyUtil::IntSeq userids, int begin, int limit, MyUtil::IntSeq types);
		FeedMetaSeq getWithFilter(MyUtil::IntSeq userIds, int begin, int limit, MyUtil::StrSeq filter);
		FeedMetaSeq getByTypesWithFilter(MyUtil::IntSeq userIds, int begin, int limit, MyUtil::IntSeq types, MyUtil::StrSeq filter);

		FeedMetaSeq getLessThanFeedidByTypesWithFilter(MyUtil::IntSeq userids, long feedid, int limit, MyUtil::IntSeq types, MyUtil::StrSeq filter);

		void setLimit(long limit);

		int getUserCount(int userid);
		long getTotalCount();
	};

	interface FeedItemFacade {
		void put(FeedSeed seed);
		void del(FeedMeta meta);
	};

	interface FeedItemFilter {
		void set(int userid, long feedid);
		MyUtil::LongSeq get(int userid);

		void setLimit(long limit);
		long getTotal();
	};	

	interface FeedItemWrapper {
		FeedMetaSeq get(int userid, int begin, int limit);
    
    FeedMetaSeq	GetFeeds(int userid, int begin, int limit, MyUtil::IntSeq types, MyUtil::IntSeq ntypest);
    FeedMetaSeq GetFeedsByFocus(int userid, int begin, int limit, MyUtil::IntSeq types, MyUtil::IntSeq ntypes);
		FeedMetaSeq GetFeedsByGroupId(int userid, int group, int begin, int limit, MyUtil::IntSeq types, MyUtil::IntSeq ntypes);
		FeedMetaSeq GetFeedsByGroupName(int userid, string name, int begin, int limit, MyUtil::IntSeq types, MyUtil::IntSeq ntypes);
 
	};

	interface AppFeeds {
		void put(FeedMeta meta);

		FeedMetaSeq get(MyUtil::IntSeq userids, int begin, int limit);
		FeedMetaSeq getByTypes(MyUtil::IntSeq userids, int begin, int limit, MyUtil::IntSeq types);
		FeedMetaSeq getWithFilter(MyUtil::IntSeq userIds, int begin, int limit, MyUtil::StrSeq filter);
		FeedMetaSeq getByTypesWithFilter(MyUtil::IntSeq userIds, int begin, int limit, MyUtil::IntSeq types, MyUtil::StrSeq filter);

		FeedMetaSeq getGreaterThanFeedid(int appid, long feedid, int limit);
		FeedMetaSeq getLessThanFeedid(int appid, long feedid, int limit);

		FeedMetaSeq getGreaterThanFeedidWithUseridByType(int appid, long feedid, int limit, int userid, MyUtil::IntSeq types);

		void setLimit(long limit);

		int getUserCount(int userid);
		long getTotalCount();
	};

};
};

#endif
