#ifndef FEED_NAIVEBAYES_ICE
#define FEED_NAIVEBAYES_ICE


#include <Util.ice>
module xce {
module feed {


class FeedNaiveBayes{
	MyUtil::Int2IntMap GetScoreDict(int fromid, MyUtil::IntSeq toids, int stype);
	MyUtil::Int2IntMap GetScoreDictReverse(int to, MyUtil::IntSeq fromids, int stype);
	int GetSendFrequency(int fromid);
	void MergeNew(string score,string dump);
};

};
};
#endif
