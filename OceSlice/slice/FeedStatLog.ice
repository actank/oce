#ifndef __Feed_STAT_LOG_ICE__
#define  __Feed_STAT_LOG_ICE__
#include <Util.ice>
module xce{
  module feed {

    class FeedStatLog {
      void AddLog(MyUtil::StrSeq category, string log);
    };
  };
};
#endif
