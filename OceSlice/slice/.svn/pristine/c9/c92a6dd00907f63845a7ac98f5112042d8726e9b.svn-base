#ifndef _SOCIALGRAPH_RELATION_H_
#define _SOCIALGRAPH_RELATION_H_

#include <Util.ice>

module xce {
module socialgraph {

  // Those constants are very important,
  // becuase those can lead the business logic.
  // Once those are defined, do never change the value of them!
  const int kFirstLevelError = -1;
  const int kFirstLevelEmpty = 0;
  const int kFirstLevelSingle = 1;
  const int kFirstLevelInlove = 2;
  const int kFirstLevelEngaged = 3;
  const int kFirstLevelMarried = 4;

  const int kSecondLevelError = -1;
  const int kSecondLevelEmpty = 0;
  const int kSingleDivorced = 1;
  const int kSingleCourt = 2;
  const int kSingleLike = 3;
  const int kInloveColdwar = 4;
  const int kMarriedHoneymoon = 5;
  const int kMarriedColdwar = 6;
  const int kSingleBeliked = 7;
  const int kSingleBeCourted = 8;

  struct LevelTuple {
    int first;
    int second;
  };

  struct EmotionTuple {
    int userId;
    LevelTuple lt;
  };

  struct SgRelationTuple {
    int host;
    int guest;
    LevelTuple lt;
  };

  struct EmotionState {
    EmotionTuple et;
    int court;
    int sweetheart;
    int engaged;
    int married;
    MyUtil::IntList likeList;
    MyUtil::IntList beLikedList;
    MyUtil::IntList beCourtedList;
  };

  ["java:type:java.util.ArrayList"]
  sequence<SgRelationTuple> SgRelationTupleSeq;
  
  // rpc interface
  interface SgRelationService {
     void reload(int userId);
     void deleteEmotionStateCache(int host);
     EmotionState getEmotionState(int userId);
     void setEmotionState(int userId, EmotionState stat);
  };

};
};


#endif
