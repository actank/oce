#ifndef _SG_RELATION_PERMISSION_H_
#define _SG_RELATION_PERMISSION_H_
#include <set>
#include "SocialGraphRelation.h"
#include "UtilCxx/src/Singleton.h"
namespace xce {
namespace socialgraph {

struct LevelTupleComp {
  bool operator() (const LevelTuple& lt1, const LevelTuple& lt2) const {
    if (lt1.first < lt2.first) {
      return false;
    }
    if (lt1.first == lt2.first) {
      return lt1.second < lt2.second;
    }
    return true;
  }
};


typedef std::set<LevelTuple, LevelTupleComp> LevelTupleSet;

class LevelHolder : virtual public MyUtil::Singleton<LevelHolder> {
 public:
   bool Valid(const LevelTuple&) const;
   void Dump() const;
 private:
   void Init();
   LevelHolder();
   friend class MyUtil::Singleton<LevelHolder>;
 private:
   LevelTupleSet lt_set_;
   IceUtil::Mutex mutex_;
};


struct LevelTupleFactory {
  static LevelTuple EmptyLevelTuple() {
    LevelTuple lt;
    lt.first = kFirstLevelEmpty;
    lt.second = kSecondLevelEmpty;
    return lt;
  }
  static LevelTuple SingleLevelTuple() {
    LevelTuple lt;
    lt.first = kFirstLevelSingle;
    lt.second = kSecondLevelEmpty;
    return lt;
  }
  static LevelTuple DivorcedLevelTuple() {
    LevelTuple lt;
    lt.first = kFirstLevelSingle;
    lt.second = kSingleDivorced;
    return lt;
  }
  static LevelTuple CourtLevelTuple() {
    LevelTuple lt;
    lt.first = kFirstLevelSingle;
    lt.second = kSingleCourt;
    return lt;
  }
  static LevelTuple BeCourtedLevelTuple() {
    LevelTuple lt;
    lt.first = kFirstLevelSingle;
    lt.second = kSingleBeCourted;
    return lt;
  }
  static LevelTuple LikeLevelTuple() {
    LevelTuple lt;
    lt.first = kFirstLevelSingle;
    lt.second = kSingleLike;
    return lt;
  }
  static LevelTuple BeLikedLevelTuple() {
    LevelTuple lt;
    lt.first = kFirstLevelSingle;
    lt.second = kSingleBeliked;
    return lt;
  }
  static LevelTuple InloveLevelTuple() {
    LevelTuple lt;
    lt.first = kFirstLevelInlove;
    lt.second = kSecondLevelEmpty;
    return lt;
  }
  static LevelTuple InloveColdwarLevelTuple() {
    LevelTuple lt;
    lt.first = kFirstLevelInlove;
    lt.second = kInloveColdwar;
    return lt;
  }
  static LevelTuple EngagedLevelTuple() {
    LevelTuple lt;
    lt.first = kFirstLevelEngaged;
    lt.second = kSecondLevelEmpty;
    return lt;
  }
  static LevelTuple MarriedLevelTuple() {
    LevelTuple lt;
    lt.first = kFirstLevelMarried;
    lt.second = kSecondLevelEmpty;
    return lt;
  }
  static LevelTuple MarriedHoneymoonLevelTuple() {
    LevelTuple lt;
    lt.first = kFirstLevelMarried;
    lt.second = kMarriedHoneymoon;
    return lt;
  }
  static LevelTuple MarriedColdwarLevelTuple() {
    LevelTuple lt;
    lt.first = kFirstLevelMarried;
    lt.second = kMarriedColdwar;
    return lt;
  }
};

}};
#endif

