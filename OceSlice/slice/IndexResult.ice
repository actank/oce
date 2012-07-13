#ifndef _INDEXRESULT_ICE
#define _INDEXRESULT_ICE

#include <Util.ice>

module com{
  module xiaonei {
    module search5 {
      module result {
        struct IndexResult {
          int docId;
          float score;
          string resourceId;
          MyUtil::Str2StrMap info;
        };

        sequence<IndexResult> ResSeq;
        
        struct SearchResult {
          int hitsFromFriends;
          int hitsFromOthers;
          int totalHits;
          ResSeq results;
        };
      };
    };
  };
};

#endif
