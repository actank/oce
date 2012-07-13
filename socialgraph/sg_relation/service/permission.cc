#include "permission.h"
#include <boost/foreach.hpp>
#include "db_util.h"

namespace xce {
namespace socialgraph {

LevelHolder::LevelHolder() {
  Init();
  Dump();
}

void LevelHolder::Init() {
  IceUtil::Mutex::Lock lock(mutex_);
  FirstLevelRowSeq first_seq = DbUtil::GetFullFirstLevel();
  BOOST_FOREACH(const FirstLevelRow& r, first_seq) {
    LevelTuple lt;
    lt.first = r.index_value_;
    lt.second = kSecondLevelEmpty;
    lt_set_.insert(lt);
  }
  SecondLevelRowSeq second_seq = DbUtil::GetFullSecondLevel();  
  BOOST_FOREACH(const SecondLevelRow& r, second_seq) {
    LevelTuple lt;
    lt.first = r.parent_level_;
    lt.second = kSecondLevelEmpty;
    LevelTupleSet::const_iterator it = lt_set_.find(lt);
    if (it != lt_set_.end()) {
      lt.second = r.index_value_;
      lt_set_.insert(lt);
    }
  }
}

bool LevelHolder::Valid(const LevelTuple& lt) const {
  IceUtil::Mutex::Lock lock(mutex_);
  LevelTupleSet::const_iterator it = lt_set_.find(lt);
  return (it != lt_set_.end());
}

void LevelHolder::Dump() const {
  IceUtil::Mutex::Lock lock(mutex_);
  BOOST_FOREACH(const LevelTuple& lt, lt_set_) {
    MCE_INFO("LevelHolder::Dump() LevelTuple(" << lt.first << "," << lt.second << ")");
  }
}

}}


