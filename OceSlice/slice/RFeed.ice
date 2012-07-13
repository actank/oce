#ifndef __RFEED_ICE__
#define __RFEED_ICE__

#include <Util.ice>

module xce
{
module feed
{
	//const int FeedMergeNone = 0;
	const int FeedMergeAppend = 2;
	const int FeedMergeReplace = 1;
	//const int FeedMergeRemove = 3;
	
	const int NewFeedReply = 0;
	const int SideFeedReply = 1;
	class FeedSeed{
		long feed;
		long miniMerge;
		long newsMerge;
		long source;
		long psource;	//更高
		int actor;
		int type;	// 包含 NewsMergeType(2位), MiniMergeType(2位), ReplyType(2位), 空两位，其余24位为type 和 Stype通过mod 100确定大类和小类
		int ptype;
		int time;
		int baseWeight;	//高8位为 friendrank,次高8位为baseWeightId, 低16位为weight
		//int baseWeightId; //曲线的分类
		string xml ;
		MyUtil::Str2StrMap extraProps;
	};
	
	struct FeedItem{
		long feed;
		long merge;
		int type;	// 包含 NewsMergeType(2位), MiniMergeType(2位), ReplyType(2位), 空两位，其余24位为type 和 Stype通过mod 100确定大类和小类
		int time;
		int weight;	//高8位为 friendrank,次高8位为baseWeightId, 低16位为weight
		int actor;	//from
	};
	sequence<FeedItem> FeedItemSeq;
	dictionary<int,FeedItem> Int2FeedItemMap;
	
	class FeedReply{
		long oldReplyId;
		string oldReply;
		long newReplyId;
		string newReply;
		int count;
	};

	class ReplyData{
	      long source;
	      int stype;
	      int actor;
	      int replyCount;
	      long replyId;
	      int time;
	      string xml;
	};
	//sequence<FeedReply> FeedReplySeq;

	class FeedData{
	    long feed;		// type and stype 都在ID的高位指定
		long source;  //可以考虑不要
		int actor;
		int type;	// 包含 NewsMergeType(2位), MiniMergeType(2位), ReplyType(2位), 空两位，其余24位为type 和 Stype通过mod 100确定大类和小类
		string xml;
		int time;
		int weight; 	//高8位暂时无用,次高8位为baseWeightId, 低16位为weight
//		int commentCount;
	};


	class FeedContent{
		FeedData data;	
		FeedReply reply;
	};
	
	sequence<FeedContent> FeedContentSeq;
	sequence<FeedData> FeedDataSeq;
	dictionary<long,FeedContent> FeedContentDict;
	
	
	class FeedConfig{
		string expr;
		bool updateTime;
		bool sendConfig;
		bool sendNewsConfig;
		bool sendMiniConfig;
		//bool publishNews;
		//bool publishMini;
	}; 
	
	
	enum LoadType { Friend,App,AppFan,Page,HighSchool,Follow,MiniGroup,XiaoZu,MiniSite };//表达式的类型
	class FeedDispatcher
	{ 
		void dispatch(FeedSeed seed,FeedConfig config);
        void dispatchReply( ReplyData data, FeedConfig config);
		void Reload(LoadType  type , int id);
		void ReloadBatch(LoadType  type , MyUtil::IntSeq ids);
        
        void addBuddyNotify(int host, int guest);
        void buddyApplyNotify(int host, int guest);
        void joinPageNotify(int page, int fans);
        void guideBuddyReplyNotify(int newUser, int buddy);

	};


	sequence<FeedItemSeq> FeedItemSeqSeq;	
	class FeedItemManager{
		FeedDataSeq getFeedDataByStype(int uid, MyUtil::IntSeq stypes, int begin, int limit);
		FeedDataSeq getFeedDataByType(int uid, MyUtil::IntSeq stypes, int begin, int limit);
		FeedDataSeq getFeedData(int uid, int begin, int limit);
		FeedDataSeq getFeedData2(int uid, int begin, int limit);
		
		FeedDataSeq getFeedDataByIds(MyUtil::LongSeq fids);
		 
		void addFeed(FeedItem item, FeedConfig config, MyUtil::Int2IntMap id2weight);
		void setRead(int uid, int stype, long merge);
		void setReadById(int uid,long feedId);
		void setReadAll(int uid);

		bool hasFeed(int uid);

		FeedItemSeqSeq GetFeedItemSeqSeqByUsers( MyUtil::IntSeq uids);
		void PressureTest(int uid, int begin, int limit);
    FeedItemSeq GetOriginalFeedData(int uid);
    void Reload(int uid);
    //void test(int uid, int begin, int limit);	
	};
	dictionary<long, int> Feed2WeightMap;

  class FeedGroupManager {
    FeedDataSeq GetFeedData(int gid, int memberid, int begin, int limit);
    FeedDataSeq GetFeedDataByType(int gid, int memberid, MyUtil::IntSeq types, int begin, int limit);
    FeedDataSeq GetFeedDataByStype(int gid, int memberid, MyUtil::IntSeq stypes, int begin, int limit);
    
    FeedItemSeqSeq GetFeedItemSeqSeqByGroups(MyUtil::IntSeq groups);
    FeedItemSeq GetOriginalFeedData(int gid);
     
    void AddFeed(FeedItem item, FeedConfig config, MyUtil::Int2IntMap id2weight);
    void RemoveFeed(int gid, int stype, long merge);
    void RemoveFeedById(int gid,long feedId);
    void RemoveAllFeed(int gid);
    void RefreshFeed(int gid, long feedid, int timestamp);
    void ReplyFeed(int gid, long feedid, int timestamp, int owner, int replier);


    // key为group_id, value为未读个数
    MyUtil::Int2IntMap GetGroupsUnreadCount(int uid, MyUtil::IntSeq groups);
    void AddMember(int groupid, int memberid);
    void RemoveMember(int groupid, int memberid);
	MyUtil::Int2IntSeqMap GetMembers(MyUtil::IntSeq gids);
	void GetMembersTest(MyUtil::IntSeq gids);//测试接口
	
    bool HasFeed(int gid);
	void ReadAllFeed(int gid,int uid);
    void Test(int gid, int mid);  
    
    
  };
  class FeedGroupCount {
    MyUtil::Int2IntMap GetUnread(int uid, MyUtil::IntSeq groups);
  };
	
	
	class FeedContentManager{
		FeedContentDict getFeedDict(MyUtil::LongSeq feedIds);
		//Feed2WeightMap getWeight(MyUtil::LongSeq feedIds);
		
		//FeedContentDict getFeedDict2(UpdateTimeSeq seq);
		//Feed2WeightMap getWeight2(UpdateTimeSeq seq);
		
		int addFeedContent(FeedData data);
		void replaceFeedReply(MyUtil::LongSeq feedIds, FeedReply reply);
		void removeFeed(long feedId);
		void removeFeeds(MyUtil::LongSeq feedIds);
		void recoveryContent(long feedId);
		//void addFeedReply(long feedId, FeedReply reply);
		//void removeFeedReply(long feedId, long replyId);
		//void removeFeedReply(MyUtil::LongSeq feedIds, FeedReply reply);

		void ReplaceXML(MyUtil::LongSeq feedIds,string xml);
	};
	

//---------------------------------------------------------------------------

class FeedIndex
{
	long feed;
	long newsMerge;
	long miniMerge;
	long source;
	long psource;
	int  actor;
	int  time;
	int  stype;
	int  ptype;
};

sequence<FeedIndex> FeedIndexSeq;

class FeedIndexManager
{
	void add(FeedIndex index);
 	MyUtil::LongSeq removeBySource(long source, int stype);				
 	MyUtil::LongSeq removeByParent(long parent, int ptype);
 	MyUtil::LongSeq remove(long source, int stype, int actor); 	
	FeedIndexSeq getBySourceAndStype(long source, int stype, int size);
	FeedIndexSeq getByParent(long parent, int ptype, int size);
	MyUtil::LongSeq get(long source, int stype, int actor);
	FeedIndex getByFeed(long feed);
	
	
};

class FeedAssistant{
	void removeFeedById(long feedId);
	void removeFeedBySource(long source, int stype);
	void removeFeedByParent(long parent, int ptype);
	void removeFeed(long source, int stype, int actor);
	
	void removeFeedReply(long source, int stype, int actor, long removeReplyId, FeedReply reply);
	void removeFeedReplyById(int stype, long feedid, long removeReplyId, FeedReply reply);
	
	void readNews(int user, long feed);
	void readAllNews(int user);
	void readMini(int user, long feed);
	void readAllMini(int user);
	
	void dispatch(FeedSeed seed);
	void dispatchEDM(FeedSeed seed);
	void addFeedReplyById(int stype, long feedid, FeedReply reply);
	
	void dispatchWithReply(FeedSeed seed, FeedReply reply);
	void addFeedReply(long source, int stype, int actor, FeedReply reply);
	//void updateFeedReply(long source, int stype, int actor, MyUtil::Str2StrMap newReplyMap, MyUtil::Str2StrMap oldReplyMap, long replyId, bool isDel);
	
	void addFeedReplyWithConfig(long source, int stype, int actor, FeedReply reply,MyUtil::Str2StrMap replyConfig);
	
	long getFeedId(long source, int stype, int actor);

	void cacheSync(long source, int stype, int actor);

	long createFeedId();
	
	void dispatchAgain(long source, int stype, int actor, int toid);
	
	void ReplaceXML(long source,int stype,int actor, string xml)	;
	//void SetAsync(int i);
	void DirectedSend(long source, int stype, int actor, MyUtil::IntSeq targets);
};



/*class FeedReplyManager{
      FeedReply getOldFeedReply(int actor, long source, int stype);
      FeedReply getNewFeedReply(int actor, long source, int stype);
      FeedReply getFeedReplyPair(int actor, long source, int stype);	
      void reload();
};*/

class FocusManager{
  FeedDataSeq GetFocusFeedData(int user, int begin , int limit);
  FeedDataSeq GetFocusFeedDataByStype(int user, int begin , int limit,MyUtil::IntSeq stypes);
  bool AddFocuses(int user,int target);
  void DelFocuses(int user,int target);
  MyUtil::IntSeq GetFocuses(int user);
  int GetFocusFeedIncCount(int user);
  
  int AddFocusWithCheck(int user,int target);//return value. 0. success 1. exceed limit. 2.focused already. 3. not friend. 4.other reason
};

class FeedDataRes{
  int UnviewedCount;
  FeedDataSeq data;
};

class FeedNewsManager{
	FeedDataRes GetLiveFeedData(int uid, int begin, int limit);
	FeedDataRes GetLiveFeedDataByStype(int uid, MyUtil::IntSeq stypes, int begin, int limit);
	FeedDataRes GetLiveFeedDataByType(int uid, MyUtil::IntSeq stypes, int begin, int limit);
	
	FeedDataRes GetHotFeedData(int uid, int begin, int limit);
	FeedDataRes GetHotFeedDataByStype(int uid, MyUtil::IntSeq stypes, int begin, int limit);
	FeedDataRes GetHotFeedDataByType(int uid, MyUtil::IntSeq stypes, int begin, int limit);
    
    FeedDataSeq GetFeedDataByIds(MyUtil::LongSeq fids);
    FeedDataSeq GetFeedDataByIdsWithUid(int uid,MyUtil::LongSeq fids,long first,int stype);
		
    FeedItemSeq GetOriginalFeedData(int uid);

    int         GetUnviewedCount(int uid, int type);
	MyUtil::Int2IntMap	GetCountMapByStype(int uid, MyUtil::IntSeq stypes, bool unviewed);
		
    void AddFeed(FeedItem item, FeedConfig config, MyUtil::Int2IntMap id2weight);
	void SetRead(int uid, int stype, long merge);
	void SetReadById(int uid,long feedId);
	void SetReadAll(int uid);
	bool HasFeed(int uid);
	void load(int userid);
	FeedItemSeqSeq GetFeedItemSeqSeqByUsers( MyUtil::IntSeq uids); 
		
	void EraseUser(int uid);	
};


};
};

#endif
