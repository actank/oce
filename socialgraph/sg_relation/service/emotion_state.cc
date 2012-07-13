#include "emotion_state.h"
#include <boost/foreach.hpp>
#include "permission.h"
#include "db_util.h"
#include "OceCxxAdapter/src/SocialGraphRelationAdapter.h"

namespace xce {
namespace socialgraph {

void EmptyState::SetOn() {
}
void EmptyState::SetOff() {

}

void SingleState::SetOn() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::SingleLevelTuple());

}
void SingleState::SetOff() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::EmptyLevelTuple());
}

void DivorcedState::SetOn() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::DivorcedLevelTuple());
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }
}

void DivorcedState::SetOff() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::EmptyLevelTuple());
}

void CourtState::SetOn() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::CourtLevelTuple());
  int guest = stat_.court;
  if (guest > 0 && host != guest) {
    DbUtil::UpdateSgRelation(host, guest, LevelTupleFactory::CourtLevelTuple());
    DbUtil::UpdateSgRelation(guest, host, LevelTupleFactory::BeCourtedLevelTuple());
    //delete guest cache
    try {
      SgRelationAdapter::instance().deleteEmotionStateCache(guest);
    } catch (...) {
      MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << guest << ") ... exception");
    }
  }
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }
}
void CourtState::SetOff() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::EmptyLevelTuple());
  int guest = stat_.court;
  if (guest > 0 && host != guest) {
    DbUtil::DeleteSgRelation(host, guest, LevelTupleFactory::CourtLevelTuple());
    DbUtil::DeleteSgRelation(guest, host, LevelTupleFactory::BeCourtedLevelTuple());
    //delete guest cache
    try {
      SgRelationAdapter::instance().deleteEmotionStateCache(guest);
    } catch (...) {
      MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << guest << ") ... exception");
    }
  }
}

void LikeState::SetOn() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::LikeLevelTuple());
  BOOST_FOREACH(int guest, stat_.likeList) {
    DbUtil::UpdateSgRelation(host, guest, LevelTupleFactory::LikeLevelTuple());
    DbUtil::UpdateSgRelation(guest, host, LevelTupleFactory::BeLikedLevelTuple());
    //delete guest cache
    try {
      SgRelationAdapter::instance().deleteEmotionStateCache(guest);
    } catch (...) {
      MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << guest << ") ... exception");
    }
  }
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }
}

void LikeState::SetOff() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::EmptyLevelTuple());
  BOOST_FOREACH(int guest, stat_.likeList) {
    DbUtil::DeleteSgRelation(host, guest, LevelTupleFactory::LikeLevelTuple());
    DbUtil::DeleteSgRelation(guest, host, LevelTupleFactory::BeLikedLevelTuple());
    //delete guest cache
    try {
      SgRelationAdapter::instance().deleteEmotionStateCache(guest);
    } catch (...) {
      MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << guest << ") ... exception");
    }
  }
}

void InloveState::SetOn() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::InloveLevelTuple());
  int guest = stat_.sweetheart;
  if (guest > 0 && host != guest) {
    DbUtil::UpdateSgRelation(host, guest, LevelTupleFactory::InloveLevelTuple());
  }
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }
}

void InloveState::SetOff() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::EmptyLevelTuple());
  int guest = stat_.sweetheart;
  if (guest > 0 && host != guest) {
    DbUtil::DeleteSgRelation(host, guest, LevelTupleFactory::InloveLevelTuple());
  }
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }
}

void InloveColdwarState::SetOn() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::InloveColdwarLevelTuple());
  int guest = stat_.sweetheart;
  if (guest > 0 && host != guest) {
    DbUtil::UpdateSgRelation(host, guest, LevelTupleFactory::InloveColdwarLevelTuple());
  }
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }
}
void InloveColdwarState::SetOff() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::EmptyLevelTuple());
  int guest = stat_.sweetheart;
  if (guest > 0 && host != guest) {
    DbUtil::DeleteSgRelation(host, guest, LevelTupleFactory::InloveColdwarLevelTuple());
  }
}

void EngagedState::SetOn() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::EngagedLevelTuple());
  int guest = stat_.engaged;
  if (guest > 0 && host != guest) {
    DbUtil::UpdateSgRelation(host, guest, LevelTupleFactory::EngagedLevelTuple());
  }
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }
}
void EngagedState::SetOff() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::EmptyLevelTuple());
  int guest = stat_.engaged;
  if (guest > 0 && host != guest) {
    DbUtil::DeleteSgRelation(host, guest, LevelTupleFactory::EngagedLevelTuple());
  }
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }

}

void MarriedState::SetOn() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::MarriedLevelTuple());
  int guest = stat_.married;
  if (guest > 0 && host != guest) {
    DbUtil::UpdateSgRelation(host, guest, LevelTupleFactory::MarriedLevelTuple());
  }
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }
}

void MarriedState::SetOff() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::EmptyLevelTuple());
  int guest = stat_.married;
  if (guest > 0 && host != guest) {
    DbUtil::DeleteSgRelation(host, guest, LevelTupleFactory::MarriedLevelTuple());
  }
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }
}

void MarriedHoneymoonState::SetOn() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::MarriedHoneymoonLevelTuple());
  int guest = stat_.married;
  if (guest > 0 && host != guest) {
    DbUtil::UpdateSgRelation(host, guest, LevelTupleFactory::MarriedHoneymoonLevelTuple());
  }
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }
}
void MarriedHoneymoonState::SetOff() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::EmptyLevelTuple());
  int guest = stat_.married;
  if (guest > 0 && host != guest) {
    DbUtil::DeleteSgRelation(host, guest, LevelTupleFactory::MarriedHoneymoonLevelTuple());
  }
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }

}

void MarriedColdwarState::SetOn() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::MarriedColdwarLevelTuple());
  int guest = stat_.married;
  if (guest > 0 && host != guest) {
    DbUtil::UpdateSgRelation(host, guest, LevelTupleFactory::MarriedColdwarLevelTuple());
  }
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }
}

void MarriedColdwarState::SetOff() {
  int host = stat_.et.userId;
  DbUtil::UpdateEmotionState(host, LevelTupleFactory::EmptyLevelTuple());
  int guest = stat_.married;
  if (guest > 0 && host != guest) {
    DbUtil::DeleteSgRelation(host, guest, LevelTupleFactory::MarriedColdwarLevelTuple());
  }
  //delete host cache
  try {
    SgRelationAdapter::instance().deleteEmotionStateCache(host);
  } catch (...) {
    MCE_WARN("SgRelationAdapter::instance().deleteEmotionStateCache(" << host << ") ... exception");
  }
}


}}

