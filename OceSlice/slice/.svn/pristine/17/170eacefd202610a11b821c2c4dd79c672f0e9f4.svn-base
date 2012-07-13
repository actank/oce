#ifndef FEED_SORT_ICE
#define FEED_SORT_ICE

#include <RFeed.ice>
//#include <Util.ice>
module xce {
module feed {


struct FeedWrapper{
	long feed;
	long merge;
	int typeinfo; // 包含 NewsMergeType(2位), MiniMergeType(2位), ReplyType(2位), 空两位，其余24位为type 和 Stype通过mod 100确定大类和小类
	int time;
	int weight;
	
	//高8位为 friendrank,
	//00000000 00000000 00000000 00000000
	//[frank]  [wid]    [view]   00000[radom][up]0
	//高8位为 friendrank,次高8位为baseWeightId, 低16位为weight
	
	int actor;
	
	//交互
	//00000000 00000000 00000000 00000000
	//         click	   reply    range
	int interaction; 
	
	//int click;
	//int reply;
	//int range;	
};
sequence<FeedWrapper> FeedWrapperSeq;

class SortParam{
	FeedWrapperSeq feeds;
	int begin;
	int limit;
	bool sort;
	bool showcomment;
	bool showrandom;
	bool showedm;
	bool gatherbytime;
	long lastGetTime; // -1代表用户刚加载，没有上次刷 新时间
};

sequence<MyUtil::LongSeq> LongSeqSeq;
sequence<LongSeqSeq> LongSeqSeqSeq;
class SortResult{
	LongSeqSeqSeq fids;
	int edmCount;
};

class FeedSort{
	SortResult Sort(int uid,SortParam param);	
};

};
};
#endif