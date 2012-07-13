#ifndef FEED_FOCUS_INVERT_ICE
#define FEED_FOCUS_INVERT_ICE

#include <Util.ice>
module xce {
module feed {

class FeedFocusInvert{
  void AddFocus(int source,int target);
  void DelFocus(int source,int target);
  MyUtil::IntSeq GetSources(int target);
};

};
};
#endif