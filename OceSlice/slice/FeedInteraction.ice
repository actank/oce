#ifndef __INTERACTION_FEEDINTERACTION_ICE__
#define __INTERACTION_FEEDINTERACTION_ICE__
#include <Util.ice>

module xce{
  module feed {
    class FeedInteractionInfo {
      long feed;
      int range;
      int actor;
      int time;
    };


    class  FeedInteractionCount {
      long feed;
      int clickCount;
      int replyCount;
      int viewCount;
      int range;
    };
   

    sequence<FeedInteractionCount> FeedInteractionCountSeq; 

    class FeedInteraction {
      void AddFeedInteraction(long feed, int actor, int range, int time);
      void Inc(int type, long feed, int count);
      void IncBatch(int type, MyUtil::Long2IntMap feedCounts);
      void IncHitCount(string content);
      FeedInteractionCount GetFeedInteractionCount(long feed, bool load); 
      FeedInteractionCountSeq GetFeedInteractionCountSeq(MyUtil::LongSeq feeds, bool load);
      string GetFeedInteractionSeq(string feeds, bool load);
    };
  };
};
#endif
