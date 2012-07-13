#include "sg_relation_service.h"
#include <boost/foreach.hpp>

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "OceCxxAdapter/src/ActiveTrackAdapter.h"
#include "util/cpp/TimeCost.h"

#include "permission.h"
#include "db_util.h"
#include "emotion_state.h"


void MyUtil::initialize() {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  using namespace xce::serverstate;
  using namespace xce::socialgraph;
  using namespace xce::log;
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&SgRelationServiceImpl::instance(), service.createIdentity("SRSI", ""));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("SocialGraphRelationService.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("SocialGraphRelationService.Interval", 5);

  static const std::string kControllerName = "ControllerSocialGraphRelationService";

  ConnectionPoolManager::instance().initialize();

  service.registerObjectCache(kEmotionStateCacheData, "EMOTION_STATE_CACHE_DATA", new EmotionStateCacheDataFactory, true);

  // register to Controller Service
  ServerStateSubscriber::instance().initialize(kControllerName,  &SgRelationServiceImpl::instance(), mod, interval, new MyUtil::SocialGraphChannel());
  ServerStateSubscriber::instance().setStatus(1);

  ActiveTrack::instance().initialize();

}



namespace xce {
namespace socialgraph {

SgRelationServiceImpl::SgRelationServiceImpl() {
  MCE_INFO("SgRelationServiceImpl::SgRelationServiceImpl()");
}

void SgRelationServiceImpl::reload(Ice::Int user_id, const Ice::Current& current) {
  MCE_INFO("SgRelationServiceImpl::reload(" << user_id << ")");
  Load(user_id);
}

void SgRelationServiceImpl::deleteEmotionStateCache(Ice::Int user_id, const Ice::Current& current) {
  MCE_INFO("SgRelationServiceImpl::deleteEmotionStateCache(" << user_id << ")");
  Delete(user_id);
}

EmotionState SgRelationServiceImpl::getEmotionState(Ice::Int user_id, const Ice::Current& current) {
  using namespace MyUtil;
  MCE_INFO("SgRelationServiceImpl::getEmotionState(" << user_id << ")");
  EmotionStateCacheDataPtr object = ServiceI::instance().findObject<EmotionStateCacheDataPtr>(kEmotionStateCacheData, user_id);
  if (!object) { // miss
    // load from DB
    return Load(user_id);
  }
  else {
    IceUtil::Mutex::Lock lock(object->mutex_);
    return object->emotion_state_;
  }
}

void SgRelationServiceImpl::setEmotionState(Ice::Int user_id, const EmotionState& e_stat, const Ice::Current& current) {
  MCE_INFO("SgRelationServiceImpl::setEmotionState(" << user_id << ")");
  EmotionState e_stat_now = Load(user_id);
  AbsEmotionState* strategy_now = EmotionStateStrategy(e_stat_now);
  AbsEmotionState* strategy_will = EmotionStateStrategy(e_stat);
  if (strategy_now != NULL) {
    strategy_now->SetOff();
    delete strategy_now;
    strategy_now = NULL;
  }
  if (strategy_will != NULL) {
    strategy_will->SetOn();
    delete strategy_will;
    strategy_will = NULL;
  }
}

EmotionState SgRelationServiceImpl::Load(int user_id) {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  std::ostringstream oss;
  oss << "SgRelationServiceImpl::Load(" << user_id << ")";
  TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
  EmotionStateCacheDataPtr object = new EmotionStateCacheData();   
  EmotionStateRow esr = DbUtil::GetEmotionState(user_id);
  object->emotion_state_.et.userId = user_id;
  object->emotion_state_.et.lt.first = esr.first_level_;
  object->emotion_state_.et.lt.second = esr.second_level_;
  SgRelationRowSeq seq = DbUtil::GetSgRelation(user_id);
  BOOST_FOREACH(const SgRelationRow& row, seq) {
    SgRelationTuple tuple;
    tuple.host = row.host_;
    tuple.guest = row.guest_;
    tuple.lt.first = row.first_level_;
    tuple.lt.second = row.second_level_;
    InitSgRelation(tuple, object->emotion_state_);
  }
  ServiceI::instance().addObject(object, kEmotionStateCacheData, user_id);
  return object->emotion_state_;
}

void SgRelationServiceImpl::Delete(int user_id) {
  using namespace MyUtil;
  ServiceI::instance().removeObject(kEmotionStateCacheData, user_id);
}

void SgRelationServiceImpl::InitSgRelation(const SgRelationTuple& st, EmotionState& e_stat) {
  if (LevelTupleFactory::EmptyLevelTuple() == st.lt) {
    
  }
  else if (LevelTupleFactory::SingleLevelTuple() == st.lt) {

  }
  else if (LevelTupleFactory::DivorcedLevelTuple() == st.lt) {

  }
  else if (LevelTupleFactory::CourtLevelTuple() == st.lt) {
    e_stat.court = st.guest;
  }
  else if (LevelTupleFactory::BeCourtedLevelTuple() == st.lt) {
    e_stat.beCourtedList.push_back(st.guest);
  }
  else if (LevelTupleFactory::LikeLevelTuple() == st.lt) {
    e_stat.likeList.push_back(st.guest);
  }
  else if (LevelTupleFactory::BeLikedLevelTuple() == st.lt) {
    e_stat.beLikedList.push_back(st.guest);
  }
  else if (LevelTupleFactory::InloveLevelTuple() == st.lt) {
    e_stat.sweetheart = st.guest;
  }
  else if (LevelTupleFactory::InloveColdwarLevelTuple() == st.lt) {
    e_stat.sweetheart = st.guest;
  }
  else if (LevelTupleFactory::EngagedLevelTuple() == st.lt) {
    e_stat.engaged = st.guest;
  }
  else if (LevelTupleFactory::MarriedLevelTuple() == st.lt) {
    e_stat.married = st.guest;
  }
  else if (LevelTupleFactory::MarriedHoneymoonLevelTuple() == st.lt) {
    e_stat.married = st.guest;
  }
  else if (LevelTupleFactory::MarriedColdwarLevelTuple() == st.lt) {
    e_stat.married = st.guest;
  }
}

AbsEmotionState* SgRelationServiceImpl::EmotionStateStrategy(const EmotionState& e_stat) {
  if (LevelTupleFactory::EmptyLevelTuple() == e_stat.et.lt) {
    return new EmptyState(e_stat); 
  }
  else if (LevelTupleFactory::SingleLevelTuple() == e_stat.et.lt) {
    return new SingleState(e_stat);
  }
  else if (LevelTupleFactory::DivorcedLevelTuple() == e_stat.et.lt) {
    return new DivorcedState(e_stat);
  }
  else if (LevelTupleFactory::CourtLevelTuple() == e_stat.et.lt) {
    return new CourtState(e_stat);
  }
  else if (LevelTupleFactory::BeCourtedLevelTuple() == e_stat.et.lt) {
   
  }
  else if (LevelTupleFactory::LikeLevelTuple() == e_stat.et.lt) {
    return new LikeState(e_stat);
  }
  else if (LevelTupleFactory::BeLikedLevelTuple() == e_stat.et.lt) {

  }
  else if (LevelTupleFactory::InloveLevelTuple() == e_stat.et.lt) {
    return new InloveState(e_stat);
  }
  else if (LevelTupleFactory::InloveColdwarLevelTuple() == e_stat.et.lt) {
    return new InloveColdwarState(e_stat);
  }
  else if (LevelTupleFactory::EngagedLevelTuple() == e_stat.et.lt) {
    return new EngagedState(e_stat);
  }
  else if (LevelTupleFactory::MarriedLevelTuple() == e_stat.et.lt) {
    return new MarriedState(e_stat);
  }
  else if (LevelTupleFactory::MarriedHoneymoonLevelTuple() == e_stat.et.lt) {
    return new MarriedHoneymoonState(e_stat);
  }
  else if (LevelTupleFactory::MarriedColdwarLevelTuple() == e_stat.et.lt) {
    return new MarriedColdwarState(e_stat);
  }
  return NULL;
}


}};


//#include <iostream>
//int main() {
//  com::xiaonei::xce::ConnectionPoolManager::instance().initialize();
//  using namespace xce::socialgraph;
//  std::cout << "sg_relation_service main" << std::endl;
  /*
  EmotionStateRow esr = DbUtil::GetEmotionState(286162347);
  std::cout << "user_id:" << esr.user_id_ << " first:" << esr.first_level_ << " second:" << esr.second_level_ << std::endl;
  SgRelationRowSeq seq = DbUtil::GetSgRelation(286162347);
  BOOST_FOREACH(SgRelationRow& row, seq) {
    std::cout << "host:" << row.host_ 
              << "\tguest:" << row.guest_
              << "\tfirst:" << row.first_level_
              << "\tsecond:" << row.second_level_ 
              << std::endl;
  }
  */
//  EmotionState es;
//  es.et.userId = 286162347;
//  es.court = 27640369;
//  es.likeList.push_back(27640369);
//  es.likeList.push_back(269936619);
//  es.sweetheart = 27640369;
//  es.engaged = 269936619;
//  es.married = 269936619;
//  AbsEmotionState* em_stat = new MarriedColdwarState(es);
//  em_stat->SetOn();
//  em_stat->SetOff();
//  delete em_stat;
  /*
  CourtState court_state(es);
  court_state.SetOn();
  
  court_state.SetOff();
  */
  /*
  SingleState single_state(es);
  single_state.SetOn();
  single_state.SetOff();
  */
  /*
  LevelHolder::instance().Dump();
  for (int i=0; i < 5; ++i) {
    LevelTuple lt;
    lt.first = i;
    for (int j= 0; j < 5; ++j) {
      lt.second = j;
      std::cout << "LevelHolder::Valid() LevelTuple(" << lt.first << "," << lt.second << "):" << boolalpha << LevelHolder::instance().Valid(lt) << std::endl;
    }
  }
  */
  /*
  std::cout << "=================first level dict===============" << std::endl;
  FirstLevelRowSeq first_seq = DbUtil::GetFullFirstLevel();
  BOOST_FOREACH(FirstLevelRow& row, first_seq) {
    std::cout << "index_value:" << row.index_value_ << " desc_:" << row.desc_ << std::endl;
  } 
  std::cout << "=================second level dict===============" << std::endl;
  SecondLevelRowSeq second_seq = DbUtil::GetFullSecondLevel();
  BOOST_FOREACH(SecondLevelRow& row, second_seq) {
    std::cout << "index_value:" << row.index_value_ << " desc_:" << row.desc_ << " parent_level_:" << row.parent_level_ << std::endl;
  } 
  */

//}
  

