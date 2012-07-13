#ifndef __XFEED_ICE__
#define __XFEED_ICE__

#include <Util.ice>
#include <Ice/BuiltinSequences.ice>

module xce
{
module xfeed
{
	const int XFeedMergeRemove = -1;
	const int XFeedMergeNone = 0;
	const int XFeedMergeReplace = 1;
	const int XFeedMergeAppend = 2;
	const int XFeedMergeReply = 3;
	const int XFeedMergeReplyReplace = 4;

	class XFeedLetter
	{
		MyUtil::Str2StrMap prop;
		string xml;
	
		long id;
		int time;
		int type;
		int stype;
		int actor;
		string resource;
		MyUtil::IntSeq target;
	};
	sequence<XFeedLetter> XFeedLetterSeq;
	
	class XFeedContent
	{
		long id;
		string xml;
		int comment;
		int type;
		int stype;
		int unread;
		int time;
		int weight;
	};
	sequence<XFeedContent> XFeedContentSeq;
	dictionary<long, XFeedContent> XFeedContentDict;
	
	class XFeedQuery
	{
		int total;
		int unread;
		XFeedContentSeq data;
	};
	
	/*
		the second para of dispatch must have the follow keys;
		"owner","type","targetsExp","policy","resource";
	*/	

	class XFeedDispatcher
	{
		void dispatch(string xml, MyUtil::Str2StrMap props);
		void dispatchAgain(long startId, long endId);
		void dispatchAgainByFid(long feedId);
		void deliverSupply(int supplier, int acceptor);
			
		void incCommentCount(long feedId,int step);
		void decCommentCount(long feedId,int step);
	};

	class XFeedMiniManager
	{
		XFeedQuery getXFeedSeqByType(int userId, MyUtil::IntSeq types, bool unread, int begin, int limit);
		XFeedQuery getXFeedSeqBySType(int userId, MyUtil::IntSeq stypes, bool unread, int begin, int limit);		
		void setXFeedRead(int userId, long xfeedId);
		void setXFeedAllRead(int userId);

		void deliverXFeed(XFeedLetter xfeed);
	};

	class XFeedNewsManager
	{
		XFeedQuery getXFeedSeqByType(int userId, MyUtil::IntSeq types, bool unread, int begin, int limit);
		XFeedQuery getXFeedSeqBySType(int userId, MyUtil::IntSeq stypes, bool unread, int begin, int limit);
		void setXFeedRead(int userId, long xfeedId);
		void setXFeedAllRead(int userId);

		void deliverXFeed(XFeedLetter xfeed);
	};
	class XFeedContentManager
	{		
		void incXFeedComment(long xfeedId, int count);
		void decXFeedComment(long xfeedId, int count);
		XFeedContentDict getXFeedDict(MyUtil::LongSeq xfeedIds);
		void deliverXFeed(XFeedContent xfeed);
		void removeXFeed(long xfeedId);
	};
	
	class XFeedSyncManager{
		void deliverNews(XFeedLetter xfeed);
		void deliverContent(XFeedLetter xfeed);	
	};
};
};
#endif
