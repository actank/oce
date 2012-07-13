#include "BuddyCoreI.h"
#include "ServiceI.h"
#include "SyncAdapter.h"
#include "BuddyCoreLogicAdapter.h"
#include <boost/lexical_cast.hpp>
#include "ConnectionQuery.h"
#include "QueryRunner.h"
#include "Date.h"
#include "ActiveTrackAdapter.h"
#include "UserCountAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "util/cpp/InvokeMonitor.h"

using namespace MyUtil;
using namespace com::xiaonei::service::buddy;
using namespace com::xiaonei::xce;
using namespace ::xce::log;
using namespace xce::serverstate;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&BuddyCoreManagerI::instance(), service.createIdentity(
        "M", ""));
  service.getAdapter()->add(&BuddyCoreSyncManagerI::instance(),
      service.createIdentity("S", ""));
  service.registerObjectCache(ENTRY_SEGMENT, "ES", new SyncServantFactory(new EntryListHolderFactory));

  TaskManager::instance().config(TASK_LEVEL_SYNCUSERCOUNT, MyUtil::ThreadPoolConfig(1, 10));
  TaskManager::instance().config(TASK_LEVEL_BATCHPROCESS, ThreadPoolConfig(1,1));
  TaskManager::instance().config(TASK_LEVEL_MISSEDIDPROCESS, ThreadPoolConfig(1,1));
  ActiveTrack::instance().initialize();

  //----------------------------Monitor-----------------------------------------
  MyUtil::ServiceMonitorManager::instance().start();
  //----------------------------HA----------------------------------------------

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyCore.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyCore.Interval", 5);

  // �~P~QController注�~F~L
  map<string,Ice::ObjectPtr> mapNameAndService;
  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(BUDDYCORE_MANAGER,&BuddyCoreManagerI::instance()));
  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(BUDDYCORESYNC_MANAGER,&BuddyCoreSyncManagerI::instance()));

  ServerStateSubscriber::instance().initialize("ControllerBuddyCore",
      mapNameAndService , mod, interval);

  ServerStateSubscriber::instance().setStatus(1,BUDDYCORE_MANAGER);
  ServerStateSubscriber::instance().setStatus(1,BUDDYCORESYNC_MANAGER);
  ServerStateSubscriber::instance().setStatus(1);	
}

void BuddyCoreManagerI::addApplyBatch(const ApplyshipMap& applies,
    const ApplyInfoMap& infos, const Ice::Current& current) {
  ostringstream msg;
  msg << "size = " << applies.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  for (ApplyshipMap::const_iterator applyIt = applies.begin(); applyIt
      != applies.end(); ++applyIt) {
    Applyship apply = applyIt->second;
    ApplyInfo info;
    ApplyInfoMap::const_iterator infoIt = infos.find(applyIt->first);
    if (infoIt == infos.end()) {
      MCE_WARN("can not find matched ApplyInfo for Applyship: "
          << applyIt->first);
      continue;
    } else {
      info = infoIt->second;
    }
    MCE_DEBUG("addApplyBatch: accept " << applyIt->first);

    // 记录LOG
    ostringstream object;
    object << apply.accepter;
    string ref = "UNKNOWN";
    map<string, string>::const_iterator itRef = current.ctx.find("REF");
    if (itRef != current.ctx.end()) {
      ref = (*itRef).second;
    }
    ActiveTrack::instance().action(apply.applicant, ref, "BUDDYAPPLYBATCH",
        object.str());

    addApply(apply, info, current);
  }
}
void BuddyCoreManagerI::addApply(const Applyship& apply, const ApplyInfo& info,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  TaskManager::instance().execute(new AddApplyTask(this, apply, info, current));
}
void AddApplyTask::handle() {
  _bm->addApplyWithState(_apply, _info, _current);
}
OpState BuddyCoreManagerI::addApplyWithState(const Applyship& apply,
    const ApplyInfo& applyInfo, const Ice::Current& current) {
  ostringstream msg;
  msg << "accepter=" << apply.accepter << " applicant=" << apply.applicant;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  OpState result;
  if(apply.accepter == apply.applicant){
    MCE_WARN("addApply accepter == applicant, do nothing!");
    result.first = NoPath;
    result.second = Self;
    return result;
  }


  Entry key(apply.accepter, DESC_APPLY);
  Entry mask(MAX_INT, 0);
  EntryList entry = locateEntryListHolder(apply.applicant)->get(key);

  // 记录LOG
  ostringstream object;
  object << apply.accepter;
  string ref = "UNKNOWN";
  map<string, string>::const_iterator itRef = current.ctx.find("REF");
  if (itRef != current.ctx.end()) {
    ref = (*itRef).second;
  }
  ActiveTrack::instance().action(apply.applicant, ref, "BUDDYAPPLY",
      object.str());

  MCE_INFO("[BuddyApplyInfo]" <<current.con->toString().substr(current.con->toString().find("remote",25)+17) << " " << apply.applicant
      << " " << apply.accepter << " " << ref);	

  if (entry.empty()) { // empty() check must be the first checking.
    result.first = NoPath;
    result.second = Apply;
    MyUtil::Str2StrMap newApplyInfoProps;
    MyUtil::Str2StrMap::iterator it;
    it = const_cast<MyUtil::Str2StrMap&>(applyInfo.props).find("STAT_FROM");
    if( it != applyInfo.props.end()){
      newApplyInfoProps["STAT_FROM"] = it->second;
      const_cast<MyUtil::Str2StrMap&>(applyInfo.props).erase(it);
    }
    it = const_cast<MyUtil::Str2StrMap&>(applyInfo.props).find("GROUP_NAME");
    if(it != applyInfo.props.end()){
      newApplyInfoProps["GROUP_NAME"] = it->second; 
      const_cast<MyUtil::Str2StrMap&>(applyInfo.props).erase(it);
    }
    it = const_cast<MyUtil::Str2StrMap&>(applyInfo.props).find("TIME");
    if(it != applyInfo.props.end()){
      newApplyInfoProps["TIME"] = it->second;
    }
    it = const_cast<MyUtil::Str2StrMap&>(applyInfo.props).find("WHY");
    if(it != applyInfo.props.end()){
      newApplyInfoProps["WHY"] = it->second;
    }
    std::stringstream ss;
    ss << apply.applicant;
    ss >> newApplyInfoProps["APPLICANT"];

    Ice::Current current1(current);
    buildContext(current1.ctx, newApplyInfoProps);

    /*BuddyDatabaseHelper::instance().insertBuddyApply(apply.applicant,
      apply.accepter, applyInfo.props);
     */
    BuddyDatabaseHelper::instance().insertBuddyApplyN(apply.applicant,
        apply.accepter, newApplyInfoProps);
    SyncAdapter::instance().syncAddApply(apply, current1);
    MCE_INFO("addApply success.  accepter=" << apply.accepter
        << " applicant=" << apply.applicant);
    return result;
  }

  if (entry.size() > 1) {
    result.first = NoPath;
    result.second = NoPath;
    MCE_WARN("AddApply found " << entry.size()
        << " entries. This value should always be 1. I will reload "
        << key << " for sync");
    SyncAdapter::instance().reload(apply.applicant);
    return result;
  }

  result.first = translate(entry.at(0).desc);
  result.second = result.first;
  switch (result.first) {
    case Applied:
      MCE_DEBUG("Found Applied, confirm it. " << "applicant: " <<apply.applicant << " accepter: " << apply.accepter);

      // 记录LOG
      ActiveTrack::instance().action(apply.applicant, ref, "BUDDYAPPLYAPPLIED",
          object.str());

      result.second = Friend;
      /*BuddyDatabaseHelper::instance().deleteBuddyApply(apply.accepter,
        apply.applicant);*/
      BuddyDatabaseHelper::instance().deleteBuddyApplyN(apply.accepter,
          apply.applicant);
      BuddyDatabaseHelper::instance().insertRelationBi(apply.applicant,
          apply.accepter);
      Applyship confirm;
      confirm.applicant = apply.accepter;
      confirm.accepter = apply.applicant;
      SyncAdapter::instance().syncAcceptApply(confirm, current);
      return result;
    case Block:
      {
        MCE_DEBUG("AddApply found Block. remove it and add apply. " << "applicant: " <<apply.applicant << " accepter: " << apply.accepter);
        // 记录LOG
        ActiveTrack::instance().action(apply.applicant, ref, "BUDDYAPPLYBLOCK",
            object.str());
        result.second = Apply;
        BuddyDatabaseHelper::instance().deleteBlock(apply.applicant,
            apply.accepter);
        MyUtil::Str2StrMap newApplyInfoProps;
        MyUtil::Str2StrMap::iterator it;
        it = const_cast<MyUtil::Str2StrMap&>(applyInfo.props).find("STAT_FROM");
        if( it != applyInfo.props.end()){
          (newApplyInfoProps)["STAT_FROM"] = it->second;
          const_cast<MyUtil::Str2StrMap&>(applyInfo.props).erase(it);
        }
        it = const_cast<MyUtil::Str2StrMap&>(applyInfo.props).find("GROUP_NAME");
        if(it != applyInfo.props.end()){
          newApplyInfoProps["GROUP_NAME"] = it->second;
          const_cast<MyUtil::Str2StrMap&>(applyInfo.props).erase(it);
        }
        it = const_cast<MyUtil::Str2StrMap&>(applyInfo.props).find("TIME");
        if(it != applyInfo.props.end()){
          newApplyInfoProps["TIME"] = it->second;
        }
        it = const_cast<MyUtil::Str2StrMap&>(applyInfo.props).find("WHY");
        if(it != applyInfo.props.end()){
          newApplyInfoProps["WHY"] = it->second;
        }

        std::stringstream ss;
        ss << apply.applicant;
        ss >> newApplyInfoProps["APPLICANT"];

        Ice::Current current1(current);
        buildContext(current1.ctx, newApplyInfoProps);

        /*BuddyDatabaseHelper::instance().insertBuddyApply(apply.applicant,
          apply.accepter, applyInfo.props);*/
        BuddyDatabaseHelper::instance().insertBuddyApplyN(apply.applicant,
            apply.accepter, newApplyInfoProps);
        SyncAdapter::instance().syncAddApply(apply, current1);
      }
      break;
    case BiBlock:
      MCE_DEBUG("AddApply found BiBlock. remove block and exit. " << "applicant: " <<apply.applicant << " accepter: " << apply.accepter);

      // 记录LOG
      ActiveTrack::instance().action(apply.applicant, ref, "BUDDYAPPLYBIBLOCK",
          object.str());

      result.second = Blocked;
      BuddyDatabaseHelper::instance().deleteBlock(apply.applicant,
          apply.accepter);
      Blockship removeBiBlock;
      removeBiBlock.from = apply.applicant;
      removeBiBlock.to = apply.accepter;
      SyncAdapter::instance().syncRemoveBlock(removeBiBlock, SYNC_BIBLOCK,
          current);
      return result;
    case Apply:
      {
        // no needs to apply two times, return.
        MCE_DEBUG("Already apply, addapply again. " << "applicant: " <<apply.applicant << " accepter: " << apply.accepter);

        // 记录LOG
        ActiveTrack::instance().action(apply.applicant, ref, "BUDDYAPPLYAPPLY",
            object.str());

        MyUtil::Str2StrMap newApplyInfoProps;
        MyUtil::Str2StrMap::iterator it;
        it = const_cast<MyUtil::Str2StrMap&>(applyInfo.props).find("STAT_FROM");
        if( it != applyInfo.props.end()){
          (newApplyInfoProps)["STAT_FROM"] = it->second;
          const_cast<MyUtil::Str2StrMap&>(applyInfo.props).erase(it);
        }
        it = const_cast<MyUtil::Str2StrMap&>(applyInfo.props).find("GROUP_NAME");
        if(it != applyInfo.props.end()){
          newApplyInfoProps["GROUP_NAME"] = it->second;
          const_cast<MyUtil::Str2StrMap&>(applyInfo.props).erase(it);
        }
        it = const_cast<MyUtil::Str2StrMap&>(applyInfo.props).find("TIME");
        if(it != applyInfo.props.end()){
          newApplyInfoProps["TIME"] = it->second;
        }
        it = const_cast<MyUtil::Str2StrMap&>(applyInfo.props).find("WHY");
        if(it != applyInfo.props.end()){
          newApplyInfoProps["WHY"] = it->second;
        }

        std::stringstream ss;
        ss << apply.applicant;
        ss >> newApplyInfoProps["APPLICANT"];

        Ice::Current current1(current);
        buildContext(current1.ctx, newApplyInfoProps);

        /*BuddyDatabaseHelper::instance().insertBuddyApply(apply.applicant,
          apply.accepter, applyInfo.props);*/
        BuddyDatabaseHelper::instance().insertBuddyApplyN(apply.applicant,
            apply.accepter, newApplyInfoProps);
        SyncAdapter::instance().syncAddApply(apply, current1);
        //add again, make sure the accepter see the request
      }
      break;
    case Blocked:
      MCE_DEBUG("Apply cannot overwrite Blocked. " << "applicant: " <<apply.applicant << " accepter: " << apply.accepter);
      // 记录LOG
      ActiveTrack::instance().action(apply.applicant, ref, "BUDDYAPPLYBLOCKED",
          object.str());
      return result;
    case Friend:
      MCE_DEBUG("Already Friend. " << "applicant: " <<apply.applicant << " accepter: " << apply.accepter);
      // 记录LOG
      ActiveTrack::instance().action(apply.applicant, ref, "BUDDYAPPLYFRIEND",
          object.str());
      return result;
    default:
      MCE_WARN("AddApply says: 'this should no be seen'. result.first = "
          << result.first);
  }

  // Sync
  //SyncAdapter::instance().syncAddApply(apply, current);

  return result;
}
OpStateMap BuddyCoreManagerI::addApplyWithStateBatch(
    const ApplyshipMap& applies, const ApplyInfoMap& infos,
    const Ice::Current& current) {
  ostringstream msg;
  msg << "size = " << applies.size();
  OpStateMap opstates;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  for (ApplyshipMap::const_iterator applyIt = applies.begin(); applyIt
      != applies.end(); ++applyIt) {
    Applyship apply = applyIt->second;
    ApplyInfo info;
    ApplyInfoMap::const_iterator infoIt = infos.find(applyIt->first);
    if (infoIt == infos.end()) {
      MCE_WARN("can not find matched ApplyInfo for Applyship: "
          << applyIt->first);
      continue;
    } else {
      info = infoIt->second;
    }
    MCE_DEBUG("addApplyBatch: accept " << applyIt->first);
    // 记录LOG
    ostringstream object;
    object << apply.accepter;
    string ref = "UNKNOWN";
    map<string, string>::const_iterator itRef = current.ctx.find("REF");
    if (itRef != current.ctx.end()) {
      ref = (*itRef).second;
    }
    ActiveTrack::instance().action(apply.applicant, ref, "BUDDYAPPLYWITHSTATEBATCH",
        object.str());

    opstates[applyIt->first] = addApplyWithState(apply, info, current);
  }
  return opstates;
}

void BuddyCoreManagerI::acceptAllApplied(::Ice::Int from,
    const Ice::Current& current) {
  MyUtil::IntSeq ids = getAppliedSeq(from, 0, -1, current);
  ostringstream msg;
  msg << from <<" acceptAllApplied: size = " << ids.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  //记录LOG	
  string ref = "UNKNOWN";
  map<string, string>::const_iterator itRef = current.ctx.find("REF");
  if (itRef != current.ctx.end()) {
    ref = (*itRef).second;
  }

  for (MyUtil::IntSeq::iterator it = ids.begin(); it != ids.end(); ++it) {
    Applyship apply;
    apply.applicant = *it;
    apply.accepter = from;
    MCE_DEBUG( "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] " << from <<" acceptAllApplied: accept " << *it);
    //acceptApply(apply, current);

    // 记录LOG
    ostringstream object;
    object << apply.accepter;
    ActiveTrack::instance().action(apply.applicant, ref, "BUDDYACCEPTALLAPPLIED",
        object.str());

    TaskManager::instance().execute(new AcceptApplyTask(this, apply, TASK_LEVEL_BATCHPROCESS, current));
  }
}
void BuddyCoreManagerI::acceptApply(const Applyship& apply,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  TaskManager::instance().execute(new AcceptApplyTask(this, apply, 0, current));
}
void AcceptApplyTask::handle() {
  _bm->acceptApplyWithState(_apply, _current);
}
OpState BuddyCoreManagerI::acceptApplyWithState(const Applyship& apply,
    const Ice::Current& current) {
  ostringstream msg;
  msg << "accepter=" << apply.accepter << " applicant=" << apply.applicant;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  OpState result;

  if( apply.accepter == apply.applicant){
    MCE_WARN("acceptApply accepter == applicant, do nothing! accepter:" << apply.accepter);
    result.first = NoPath;
    result.second = Self;
    return result;
  }

  Entry key(apply.applicant, DESC_APPLIED);
  Entry mask(MAX_INT, 0);
  EntryList entry = locateEntryListHolder(apply.accepter)->get(key);

  // 记录LOG
  ostringstream object;
  object << apply.applicant;
  string ref = "UNKNOWN";
  map<string, string>::const_iterator itRef = current.ctx.find("REF");
  if (itRef != current.ctx.end()) {
    ref = (*itRef).second;
  }
  ActiveTrack::instance().action(apply.accepter, ref, "BUDDYACCEPTAPPLY",
      object.str());

  /* We need this line below locateEntryListHolder */
  /*BuddyDatabaseHelper::instance().deleteBuddyApply(apply.applicant,
    apply.accepter);*/
  BuddyDatabaseHelper::instance().deleteBuddyApplyN(apply.applicant,
      apply.accepter);
  if (entry.empty()) { // empty() check must be the first checking.
    result.first = NoPath;
    result.second = NoPath;
    MCE_WARN("no apply to accept. applicant=" << apply.applicant
        << " accepter=" << apply.accepter);
    SyncAdapter::instance().reload(apply.accepter);
    return result;
  }

  if (entry.size() > 1) {
    result.first = NoPath;
    result.second = NoPath;
    MCE_WARN("acceptApply found " << entry.size()
        << " entries. This value should always be 1. I will reload "
        << key << " for sync");
    SyncAdapter::instance().reload(apply.accepter);
    return result;
  }

  result.first = translate(entry.at(0).desc);
  result.second = result.first;
  switch (result.first) {
    case Applied:
      /*
       * Done at the very beginning.
       BuddyDatabaseHelper::instance().deleteBuddyApply(apply.applicant,
       apply.accepter);
       */
      BuddyDatabaseHelper::instance().insertRelationBi(apply.applicant,
          apply.accepter);
      result.second = Friend;
      SyncAdapter::instance().syncAcceptApply(apply, current);
      MCE_INFO("acceptApply success.  accepter=" << apply.accepter
          << " applicant=" << apply.applicant);
      return result;
    case Apply:
    case Block:
    case BiBlock:
    case Blocked:
    case Friend:
      ActiveTrack::instance().action(apply.accepter, ref, "BUDDYACCEPTAPPLYNOTAPPLIED",
          object.str());
      MCE_WARN("acceptApply not applied, just return. accepter="
          << apply.accepter << " applicant=" << apply.applicant);
      return result;
    default:
      MCE_WARN("acceptApply says: 'this should no be seen'. accepter="
          << apply.accepter << " applicant=" << apply.applicant
          << "result.first = " << result.first);
  }
  return result;
}

void BuddyCoreManagerI::denyAllApplied(::Ice::Int from,
    const Ice::Current& current) {
  MyUtil::IntSeq ids = getAppliedSeq(from, 0, -1, current);
  ostringstream msg;
  msg << "denyAllApplied: size = " << ids.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  // 记录LOG
  string ref = "UNKNOWN";
  map<string, string>::const_iterator itRef = current.ctx.find("REF");
  if (itRef != current.ctx.end()) {
    ref = (*itRef).second;
  }

  for (MyUtil::IntSeq::iterator it = ids.begin(); it != ids.end(); ++it) {
    Applyship apply;
    apply.applicant = *it;
    apply.accepter = from;

    //记录LOG	
    ostringstream object;
    object << apply.applicant;	
    ActiveTrack::instance().action(apply.accepter, ref, "BUDDYDENYALLAPPLIED",
        object.str());

    MCE_DEBUG( "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "]" << from <<" denyAllApplied: deny " << *it);
    //denyApply(apply, current);
    TaskManager::instance().execute(new DenyApplyTask(this, apply, TASK_LEVEL_BATCHPROCESS, current));
  }
}

void BuddyCoreManagerI::denyAllApply(::Ice::Int from,
    const Ice::Current& current) {
  MyUtil::IntSeq ids = getApplySeq(from, 0, -1, current);
  ostringstream msg;
  msg << "size = " << ids.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  // 记��~ULOG
  string ref = "UNKNOWN";
  map<string, string>::const_iterator itRef = current.ctx.find("REF");
  if (itRef != current.ctx.end()) {
    ref = (*itRef).second;
  }

  for (MyUtil::IntSeq::iterator it = ids.begin(); it != ids.end(); ++it) {
    Applyship apply;
    apply.applicant = from;
    apply.accepter = *it;

    //记��~ULOG    
    ostringstream object;
    object << apply.applicant;
    ActiveTrack::instance().action(apply.accepter, ref, "BUDDYDENYALLAPPLY",
        object.str());

    MCE_DEBUG( "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "]" << from <<" denyAllApply: deny " << *it);
    //denyApply(apply, current);
    TaskManager::instance().execute(new DenyApplyTask(this, apply, TASK_LEVEL_BATCHPROCESS, current));
  }
}

void BuddyCoreManagerI::denyApply(const Applyship& apply,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  TaskManager::instance().execute(new DenyApplyTask(this, apply, 0, current));
}
void DenyApplyTask::handle() {
  _bm->denyApplyWithState(_apply, _current);
}
OpState BuddyCoreManagerI::denyApplyWithState(const Applyship& apply,
    const Ice::Current& current) {
  ostringstream msg;
  msg << "accepter=" << apply.accepter << " applicant="
    << apply.applicant;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  OpState result;

  if( apply.accepter == apply.applicant){
    MCE_WARN("denyApply accepter == applicant, do nothing! accepter:" << apply.accepter);
    result.first = NoPath;
    result.second = Self;
    return result;
  }

  Entry key(apply.applicant, DESC_APPLIED);
  Entry mask(MAX_INT, 0);
  EntryList entry = locateEntryListHolder(apply.accepter)->get(key);

  // 记录LOG
  ostringstream object;
  object << apply.applicant;
  string ref = "UNKNOWN";
  map<string, string>::const_iterator itRef = current.ctx.find("REF");
  if (itRef != current.ctx.end()) {
    ref = (*itRef).second;
  }
  ActiveTrack::instance().action(apply.accepter, ref, "BUDDYDENYAPPLY",
      object.str());

  /* We need this line below locateEntryListHolder */
  /*BuddyDatabaseHelper::instance().deleteBuddyApply(apply.applicant,
    apply.accepter);*/
  BuddyDatabaseHelper::instance().deleteBuddyApplyN(apply.applicant,
      apply.accepter);
  if (entry.empty()) { // empty() check must be the first checking.
    MCE_WARN("no applied to deny. applicant=" << apply.applicant
        << " accepter=" << apply.accepter);
    result.first = NoPath;
    result.second = NoPath;
    SyncAdapter::instance().reload(apply.accepter);
    return result;
  }

  if (entry.size() > 1) {
    MCE_WARN("denyApplied found " << entry.size()
        << " entries. This value should always be 1. I will reload "
        << key << " for sync");
    result.first = NoPath;
    result.second = NoPath;
    SyncAdapter::instance().reload(apply.accepter);
    return result;
  }

  result.first = translate(entry.at(0).desc);
  result.second = result.first;
  switch (result.first) {
    case Applied:
      /*
       * Done at the very beginning.
       BuddyDatabaseHelper::instance().deleteBuddyApply(apply.applicant,
       apply.accepter);
       */
      result.second = NoPath;
      SyncAdapter::instance().syncDenyApply(apply, current);
      MCE_INFO("denyApply success.  accepter=" << apply.accepter
          << " applicant=" << apply.applicant);
      return result;
    case Apply:
    case Block:
    case Blocked:
    case BiBlock:
    case Friend:
      ActiveTrack::instance().action(apply.accepter, ref, "BUDDYDENYAPPLYNOTAPPLIED",
          object.str());
    default:
      MCE_WARN("denyApplied says: 'this should no be seen'. result.first = "
          << result.first);
  }
  return result;
}

::Ice::Int BuddyCoreManagerI::getAppliedCount(::Ice::Int from,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  return getAppliedSeq(from, 0, -1, current).size();
}

::MyUtil::IntSeq BuddyCoreManagerI::getAppliedSeq(::Ice::Int from,
    ::Ice::Int begin, ::Ice::Int limit, const Ice::Current& current) {
  ostringstream msg;
  msg << from ; 
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Entry key(0, DESC_APPLIED);
  Entry mask(0, DESC_MASK_APPLY);
  /*
     EntryList entryResult = locateEntryListHolder(from)->get(key, mask);
     MyUtil::IntSeq result;
     for (EntryList::iterator it = entryResult.begin(); it < entryResult.end(); ++it) {
  //TODO check if have better option.
  if (it->desc != DESC_APPLIED) {
  MCE_WARN("getAppliedSeq duplicate check is needed. id=" << from);
  continue;
  }
  result.push_back(it->to);
  }
   */
  EntryListResultBuilderPtr builder = new EntryListResultBuilder;
  locateEntryListHolder(from)->get(key, mask, builder);
  MyUtil::IntSeq result = builder->result();
  MCE_INFO("[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] BuddyCoreManagerI::getAppliedSeq -> " << from << " -> "
      << result.size());
  return result;
}

::Ice::Int BuddyCoreManagerI::getApplyCount(::Ice::Int from,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  return getApplySeq(from, 0, -1, current).size();
}

::MyUtil::IntSeq BuddyCoreManagerI::getApplySeq(::Ice::Int from,
    ::Ice::Int begin, ::Ice::Int limit, const Ice::Current& current) {
  ostringstream msg;
  msg << from ;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  Entry key(0, DESC_APPLY);
  Entry mask(0, DESC_MASK);
  /*
     EntryList entryResult = locateEntryListHolder(from)->get(key, mask);
     MyUtil::IntSeq result;
     for (EntryList::iterator it = entryResult.begin(); it < entryResult.end(); ++it) {
  //TODO check if have better option.
  if (it->desc != DESC_APPLY) {
  MCE_WARN("getApplySeq duplicate check is needed. id=" << from);
  continue;
  }
  result.push_back(it->to);
  }
   */
  EntryListResultBuilderPtr builder = new EntryListResultBuilder;
  locateEntryListHolder(from)->get(key, mask, builder);
  MyUtil::IntSeq result = builder->result();
  MCE_INFO("[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] BuddyCoreManagerI::getApplySeq -> " << from << " -> "
      << result.size());
  return result;
}
::MyUtil::Int2IntSeqMap BuddyCoreManagerI::getApplySeqBatch(
    const ::MyUtil::IntSeq& userIds, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  ::MyUtil::Int2IntSeqMap result;
  for (IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
    IntSeq sublist = getApplySeq(*it, 0, -1, current);
    result.insert(make_pair(*it, sublist));
  }
  return result;
}

void BuddyCoreManagerI::removeFriend(const Friendship& buddy,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  TaskManager::instance().execute(new RemoveFriendTask(this, buddy, current));
}
void RemoveFriendTask::handle() {
  _bm->removeFriendWithState(_buddy, _current);
}
OpState BuddyCoreManagerI::removeFriendWithState(const Friendship& buddy,
    const Ice::Current& current) {
  ostringstream msg;
  msg << "from=" << buddy.from << " to=" << buddy.to;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  OpState result;

  if( buddy.from == buddy.to){
    MCE_WARN("removeFriend from == to, do nothing! from:" << buddy.from);
    result.first = NoPath;
    result.second = Self;
    return result;
  }

  Entry key(buddy.to, DESC_FRIEND);
  Entry mask(MAX_INT, 0);
  EntryList entry = locateEntryListHolder(buddy.from)->get(key);

  // 记录LOG
  ostringstream object;
  object << buddy.to;
  string ref = "UNKNOWN";
  map<string, string>::const_iterator itRef = current.ctx.find("REF");
  if (itRef != current.ctx.end()) {
    ref = (*itRef).second;
  }
  ActiveTrack::instance().action(buddy.from, ref, "BUDDYREMOVEFRIEND",
      object.str());

  if (entry.empty()) { // empty() check must be the first checking.
    MCE_WARN("removeFriend found nothing to do. from=" << buddy.from
        << " to=" << buddy.to);
    result.first = NoPath;
    result.second = NoPath;
    SyncAdapter::instance().reload(buddy.from);
    return result;
  }

  if (entry.size() > 1) {
    MCE_WARN("removeFriend found " << entry.size()
        << " entries. This value should always be 1. I will reload "
        << key << " for sync");
    result.first = NoPath;
    result.second = NoPath;
    SyncAdapter::instance().reload(buddy.from);
    return result;
  }

  result.first = translate(entry.at(0).desc);
  result.second = result.first;
  switch (result.first) {
    case Friend:
      result.second = NoPath;
      BuddyDatabaseHelper::instance().deleteRelationBi(buddy.from, buddy.to);
      SyncAdapter::instance().syncRemoveFriend(buddy, current);
      MCE_INFO("removeFriend success.  from=" << buddy.from << " to="
          << buddy.to);
      return result;
    case Apply:
    case Applied:
    case Blocked:
    case Block:
    case BiBlock:
      ActiveTrack::instance().action(buddy.from, ref, "BUDDYREMOVEFRIENDNOTFRIEND",
          object.str());
      SyncAdapter::instance().reload(buddy.from);
      break;
    default:
      MCE_WARN("removeFriend says: 'this should no be seen'. result.first = "
          << result.first);
  }
  return result;
}

::Ice::Int BuddyCoreManagerI::getFriendCount(::Ice::Int userId,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  return getFriendSeq(userId, 0, -1, current).size();
}

::MyUtil::IntSeq BuddyCoreManagerI::getFriendSeq(::Ice::Int userId,
    ::Ice::Int begin, ::Ice::Int limit, const Ice::Current& current) {
  ostringstream msg;
  msg << userId ;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Entry info(0, DESC_FRIEND);
  Entry mask(0, DESC_MASK_FRIEND);
  /*
     EntryList entryResult = locateEntryListHolder(userId)->get(info, mask);
     MyUtil::IntSeq result;
     for (EntryList::iterator it = entryResult.begin(); it < entryResult.end(); ++it) {
  //TODO check if has better option
  if (it->desc != DESC_FRIEND) {
  MCE_WARN("getFriendSeq duplicate check is needed. id=" << userId
  << " " << *it);
  continue;
  }
  result.push_back(it->to);
  }
   */
  EntryListResultBuilderPtr builder = new EntryListResultBuilder;
  locateEntryListHolder(userId)->get(info, mask, builder);
  MyUtil::IntSeq result = builder->result();
  MCE_INFO("[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] BuddyCoreManagerI::getFriendSeq -> " << userId << " -> "
      << result.size());
  return result;
}
::MyUtil::Int2IntSeqMap BuddyCoreManagerI::getFriendSeqBatch(
    const ::MyUtil::IntSeq& userIds, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  ::MyUtil::Int2IntSeqMap result;
  for (IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
    IntSeq sublist = getFriendSeq(*it, 0, -1, current);
    result.insert(make_pair(*it, sublist));
  }
  return result;
}

void BuddyCoreManagerI::addBlock(const Blockship& block,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  TaskManager::instance().execute(new AddBlockTask(this, block, current));
}
void AddBlockTask::handle() {
  _bm->addBlockWithState(_block, _current);
}
OpState BuddyCoreManagerI::addBlockWithState(const Blockship& block,
    const Ice::Current& current) {
  ostringstream msg;
  msg << "from=" << block.from << " to=" << block.to;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  OpState result;

  if(block.from == block.to){
    MCE_WARN("addBlock to == from, do nothing!from:" << block.from);
    result.first = NoPath;
    result.second = Self;
    return result;
  }

  Entry key(block.to, DESC_BLOCK);
  Entry mask(MAX_INT, 0);
  EntryList entry = locateEntryListHolder(block.from)->get(key);

  // 记录LOG
  ostringstream object;
  object << block.to;
  string ref = "UNKNOWN";
  map<string, string>::const_iterator itRef = current.ctx.find("REF");
  if (itRef != current.ctx.end()) {
    ref = (*itRef).second;
  }
  ActiveTrack::instance().action(block.from, ref, "BUDDYADDBLOCK",
      object.str());

  if (entry.empty()) { // empty() check must be the first checking.
    result.first = NoPath;
    result.second = Block;
    BuddyDatabaseHelper::instance().insertBlock(block.from, block.to);
    SyncAdapter::instance().syncAddBlock(block, SYNC_NORMALBLOCK, current);
    return result;
  }

  if (entry.size() > 1) {
    result.first = NoPath;
    result.second = NoPath;
    MCE_WARN("AddBlock found " << entry.size()
        << " entries. This value should always be 1. I will reload "
        << key << " for sync");
    SyncAdapter::instance().reload(block.from);
    return result;
  }

  result.first = translate(entry.at(0).desc);
  result.second = Block;
  bool biBlock = SYNC_NORMALBLOCK;
  switch (result.first) {
    case Apply:
      /*BuddyDatabaseHelper::instance().deleteBuddyApply(block.from, block.to);*/
      BuddyDatabaseHelper::instance().deleteBuddyApplyN(block.from, block.to);
      break;
    case Applied:
      /*BuddyDatabaseHelper::instance().deleteBuddyApply(block.to, block.from);*/
      BuddyDatabaseHelper::instance().deleteBuddyApplyN(block.to, block.from);
      break;
    case BiBlock:
    case Block:
      // no needs to block two times, return.
      MCE_DEBUG("Already Blocked");
      return result;
    case Blocked:
      // BiBlock:
      MCE_DEBUG("BiBlock");
      result.second = BiBlock;
      biBlock = SYNC_BIBLOCK;
      break;
    case Friend:
      BuddyDatabaseHelper::instance().deleteRelationBi(block.from, block.to);
      break;
    default:
      MCE_WARN("AddBlock says: 'this should no be seen'. result.first = "
          << result.first);
  }

  // Sync
  BuddyDatabaseHelper::instance().insertBlock(block.from, block.to);
  SyncAdapter::instance().syncAddBlock(block, biBlock, current);
  MCE_INFO("addBlock success.  from=" << block.from << " to=" << block.to);

  return result;
}

void BuddyCoreManagerI::removeBlock(const Blockship& block,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  TaskManager::instance().execute(new RemoveBlockTask(this, block, current));
}
void RemoveBlockTask::handle() {
  _bm->removeBlockWithState(_block, _current);
}
OpState BuddyCoreManagerI::removeBlockWithState(const Blockship& block,
    const Ice::Current& current) {
  ostringstream msg;
  msg << "from=" << block.from << " to=" << block.to;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  OpState result;

  if( block.from == block.to){
    MCE_WARN("removeBlock from == to, do nothing! from:" << block.from);
    result.first = NoPath;
    result.second = Self;
    return result;
  }

  Entry info(block.to, DESC_BLOCK);
  Entry mask(MAX_INT, 0);
  EntryList entry = locateEntryListHolder(block.from)->get(info);

  // 记录LOG
  ostringstream object;
  object << block.to;
  string ref = "UNKNOWN";
  map<string, string>::const_iterator itRef = current.ctx.find("REF");
  if (itRef != current.ctx.end()) {
    ref = (*itRef).second;
  }
  ActiveTrack::instance().action(block.from, ref, "BUDDYREMOVEBLOCK",
      object.str());

  if (entry.empty()) { // empty() check must be the first checking.
    MCE_WARN("removeBlock found nothing to do. from=" << block.from
        << " to=" << block.to);
    result.first = NoPath;
    result.second = NoPath;
    SyncAdapter::instance().reload(block.from);
    return result;
  }

  if (entry.size() > 1) {
    result.first = NoPath;
    result.second = NoPath;
    MCE_WARN("removeBlock found " << entry.size()
        << " entries. This value should always be 1. I will reload "
        << info << " for sync. from=" << block.from << " to="
        << block.to);
    SyncAdapter::instance().reload(block.from);
    return result;
  }

  result.first = translate(entry.at(0).desc);
  result.second = result.first;
  bool biBlock = SYNC_NORMALBLOCK;
  switch (result.first) {
    case Apply:
    case Applied:
    case Blocked:
    case Friend:
      MCE_DEBUG("removeBlock do nothing for " << result.first);
      return result;
    case Block:
      result.second = NoPath;
      break;
    case BiBlock:
      result.second = Blocked;
      biBlock = SYNC_BIBLOCK;
      break;
    default:
      MCE_WARN("AddBlock says: 'this should no be seen'. result.first = "
          << result.first);
  }
  BuddyDatabaseHelper::instance().deleteBlock(block.from, block.to);
  SyncAdapter::instance().syncRemoveBlock(block, biBlock, current);
  MCE_INFO("removeBlock success.  from=" << block.from << " to=" << block.to);
  return result;
}

::Ice::Int BuddyCoreManagerI::getBlockCount(::Ice::Int from,
    const Ice::Current& current) {
  return getBlockSeq(from, 0, -1, current).size();
}

::MyUtil::IntSeq BuddyCoreManagerI::getBlockSeq(::Ice::Int from,
    ::Ice::Int begin, ::Ice::Int limit, const Ice::Current& current) {
  ostringstream msg;
  msg << from ;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Entry info(0, DESC_BLOCK);
  Entry mask(0, DESC_MASK_BLOCK);
  /*
     EntryList entryResult = locateEntryListHolder(from)->get(info, mask);
     MyUtil::IntSeq result;
     for (EntryList::iterator it = entryResult.begin(); it < entryResult.end(); ++it) {
  //TODO check if has better option
  if (it->desc != DESC_BLOCK) {
  MCE_WARN("getBlockSeq duplicate check is needed. id=" << from);
  continue;
  }
  result.push_back(it->to);
  }
   */
  EntryListResultBuilderPtr builder = new EntryListResultBuilder;
  locateEntryListHolder(from)->get(info, mask, builder);
  MyUtil::IntSeq result = builder->result();
  MCE_INFO("[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] BuddyCoreManagerI::getBlockSeq -> " << from << " -> "
      << result.size());
  return result;
}

::MyUtil::Int2IntSeqMap BuddyCoreManagerI::getBlockSeqBatch(
    const ::MyUtil::IntSeq& userIds, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  ::MyUtil::Int2IntSeqMap result;
  for (IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
    IntSeq sublist = getBlockSeq(*it, 0, -1, current);
    result.insert(make_pair(*it, sublist));
  }
  return result;
}

void BuddyCoreManagerI::addFriend(const Ice::Int from, const Ice::Int to,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  TaskManager::instance().execute(new AddFriendTask(this, from, to, current));
}
void AddFriendTask::handle() {
  _bm->addFriendWithState(_from, _to, _current);
}
OpState BuddyCoreManagerI::addFriendWithState(const Ice::Int from, const Ice::Int to,
    const Ice::Current& current) {
  ostringstream msg;
  msg << "from=" << from << " to=" << to;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  OpState result;
 
  if( from == to){
    MCE_WARN("addFriend from == to, do nothing! from:" << from);
    result.first = NoPath;
    result.second = Self;
    return result;
  }  

  Entry info(to, NoPath); 
  EntryList entry = locateEntryListHolder(from)->get(info);
  if(entry.size()>0){
    result.first = translate(entry.at(0).desc);
    result.second = result.first;
    return result;
  }
  BuddyDatabaseHelper::instance().insertRelationBi(from,to);
  result.first = NoPath;
  result.second = Friend;
  SyncAdapter::instance().syncAddFriend(from, to, current);
  MCE_INFO("addFriend success.  from=" << from
      << " to=" << to);
  return result;
}

::BuddyDesc BuddyCoreManagerI::getRelation(const Relationship& relation,
    const Ice::Current& current) {
  if (relation.from == relation.to) {
    return ::Self;
  }
  Entry key(relation.to, 0);
  Entry mask(MAX_INT, 0);

  ostringstream msg;
  msg << key << " / " << mask ;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  EntryList entryResult = locateEntryListHolder(relation.from)->get(key);
  if (entryResult.empty()) {
    MCE_DEBUG("BuddyCoreManagerI::getRelation found empty.");
    return ::NoPath;
  }

  if (entryResult.size() != 1) {
    MCE_WARN("GetRelation should found only one entry. But found "
        << entryResult.size() << " entries");
  }
  return translate(entryResult.at(0).desc);
}

void BuddyCoreManagerI::reload(::Ice::Int userId, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  SyncAdapter::instance().reload(userId);
}

BuddyDesc BuddyCoreManagerI::translate(uint32_t desc) {
  return BuddyDescHelper::translateDesc(desc);
}

EntryListHolderPtr BuddyCoreManagerI::locateEntryListHolder(uint32_t fromId) {
  return ServiceI::instance().locateObject<EntryListHolderPtr> (
      ENTRY_SEGMENT, fromId);

}

void BuddyCoreManagerI::buildContext( Ice::Context& ctx, MyUtil::Str2StrMap& map ) {
  for(MyUtil::Str2StrMap::const_iterator it =  map.begin(); it != map.end(); ++it){
    ctx.insert(make_pair(it->first, it->second));
  }
}

/* --------------------------------------------------------------------------------------------- */

void BuddyCoreSyncManagerI::reload(::Ice::Int id, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  removeEntryListHolder(id);
}

void BuddyCoreSyncManagerI::syncAddFriend(const Ice::Int from, const Ice::Int to,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  //TODO back door has not been completed yet...haha
  uint32_t host;
  Entry entry;
  if (isLocalSync(current)) {
    host = from;
    entry.to = to;
    entry.desc = DESC_FRIEND;
    MCE_INFO("**SyncAddFriend Local**" << entry);
  } else {
    host = to;
    entry.to = from;
    entry.desc = DESC_FRIEND;
    MCE_INFO("**SyncAddFriend Remote**" << entry);
  }

  EntryListHolderPtr holder = findEntryListHolder(host);
  if (holder) {
    holder->add(entry);
  }else{
    MCE_WARN("syncAddFriend : " << host << "not in cache");
  }

}

void BuddyCoreSyncManagerI::syncAddApply(const Applyship& ship,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  uint32_t from;
  Entry entry;

  if (isLocalSync(current)) {
    from = ship.applicant;
    entry.to = ship.accepter;
    entry.desc = DESC_APPLY;
    MCE_INFO("**SyncAddApply Local**" << entry);
  } else {
    from = ship.accepter;
    entry.to = ship.applicant;
    entry.desc = DESC_APPLIED;
    MCE_INFO("**SyncAddApply Remote**" << entry);
  }

  EntryListHolderPtr holder = findEntryListHolder(from);
  if (holder) {
    holder->add(entry);
    if (!isLocalSync(current)) {
      syncUserCountGuestRequest(ship.accepter);
    }
  }else{
    MCE_WARN("syncAddApply : " << from << "not in cache");
    TaskManager::instance().execute(new MissedIdProcessTask(from));
  }
}

void BuddyCoreSyncManagerI::syncAcceptApply(const Applyship& ship,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  uint32_t from;
  Entry key;
  if (isLocalSync(current)) {
    from = ship.accepter;
    key.to = ship.applicant;
    key.desc = DESC_APPLIED;
    MCE_INFO("**SyncAcceptApply**Local " << key);
  } else {
    from = ship.applicant;
    key.to = ship.accepter;
    key.desc = DESC_APPLY;
    //        key.ext = EXT_UNKNOWN;
    MCE_INFO("**SyncAcceptApply**Remote " << key);
  }

  EntryListHolderPtr holder = findEntryListHolder(from);
  if (holder) {
    Entry value(key);
    value.desc = DESC_FRIEND;

    Entry keyMask(MAX_INT, DESC_MASK_APPLY);
    Entry valueMask(MAX_INT, (DESC_FRIEND | DESC_MASK_APPLY));
    holder->set(key, keyMask, value, valueMask);
    if(isLocalSync(current)) {
      syncUserCountGuestRequest(ship.accepter);
    }
  }else{
    MCE_WARN("syncAcceptApply : " << from << "not in cache");
  }
}

void BuddyCoreSyncManagerI::syncDenyApply(const Applyship& ship,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  uint32_t from;
  Entry key;

  if (isLocalSync(current)) {
    from = ship.accepter;
    key.to = ship.applicant;
    key.desc = DESC_APPLIED;
    MCE_INFO("**SyncDenyApply**Local " << key);
  } else {
    from = ship.applicant;
    key.to = ship.accepter;
    key.desc = DESC_APPLY;
    MCE_INFO("**SyncDenyApply**Remote " << key);
  }
  EntryListHolderPtr holder = findEntryListHolder(from);
  if (holder) {
    Entry mask(MAX_INT, DESC_MASK_APPLY);
    holder->remove(key, mask);
    if (isLocalSync(current)) {
      syncUserCountGuestRequest(ship.accepter);
    }
  }else{
    MCE_WARN("syncDenyApply : " << from << "not in cache");
  }
}

void BuddyCoreSyncManagerI::syncRemoveFriend(const Friendship& ship,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  uint32_t from;
  Entry key;
  if (isLocalSync(current)) {
    from = ship.from;
    key.to = ship.to;
    MCE_INFO("**SyncRemoveFriend**Local " << key);
  } else {
    from = ship.to;
    key.to = ship.from;
    MCE_INFO("**SyncRemoveFriend**Remote " << key);
  }
  EntryListHolderPtr holder = findEntryListHolder(from);
  if (holder) {
    key.desc = DESC_FRIEND;

    Entry mask(MAX_INT, DESC_MASK_FRIEND);
    holder->remove(key, mask);
  }else{
    MCE_WARN("syncRemoveFriend : " << from << "not in cache");
  }
}

void BuddyCoreSyncManagerI::syncAddBlock(const Blockship& ship,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  uint32_t from;
  Entry entry;

  if (isLocalSync(current)) {
    from = ship.from;
    entry.to = ship.to;
    if (isBiblock(current)) {
      entry.desc = DESC_BLOCKED | DESC_BLOCK;
    } else {
      entry.desc = DESC_BLOCK;
    }
    MCE_INFO("**SyncAddBlock Local**" << entry);
  } else {
    from = ship.to;
    entry.to = ship.from;
    if (isBiblock(current)) {
      entry.desc = DESC_BLOCK | DESC_BLOCKED;
    } else {
      entry.desc = DESC_BLOCKED;
    }
    MCE_INFO("**SyncAddBlock Remote**" << entry);
  }
  EntryListHolderPtr holder = findEntryListHolder(from);
  if (holder) {
    holder->add(entry);
    syncUserCountGuestRequest(from);
  }else{
    MCE_WARN("syncAddBlock : " << from << "not in cache");
  }
}

void BuddyCoreSyncManagerI::syncRemoveBlock(const Blockship& ship,
    const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  if (isLocalSync(current)) {
    uint32_t from = ship.from;
    EntryListHolderPtr holder = findEntryListHolder(from);
    if (!holder) {
      MCE_WARN("syncRemoveBlock : " << from << "not in cache");
      return;
    }
    Entry key(ship.to, DESC_BLOCK);
    Entry mask(MAX_INT, DESC_MASK_BLOCK);
    MCE_INFO("**SyncRemoveBlock**Local key=" << key << " mask=" << mask);
    if (!isBiblock(current)) {
      holder->remove(key, mask);
      MCE_INFO("**SyncRemoveBlock**A " << key << " / " << mask);
    } else {
      Entry value(key);
      value.desc = key.desc & ~DESC_BLOCK;

      Entry valueMask(0, DESC_MASK_BLOCK);
      holder->set(key, mask, value, valueMask);
    }
  } else {
    uint32_t from = ship.to;
    EntryListHolderPtr holder = findEntryListHolder(from);
    if (!holder) {
      MCE_WARN("syncRemoveBlock : " << from << "not in cache");
      return;
    }
    Entry key(ship.from, DESC_BLOCKED);
    Entry mask(MAX_INT, DESC_MASK_BLOCKED);
    MCE_INFO("**SyncRemoveBlock**Remote key=" << key << " mask=" << mask);
    if (!isBiblock(current)) {
      holder->remove(key, mask);
      MCE_INFO("**SyncRemoveBlock**B " << key << " / " << mask);
    } else {
      Entry value(key);
      value.desc = key.desc & ~DESC_BLOCKED;

      Entry valueMask(0, DESC_MASK_BLOCKED);
      holder->set(key, mask, value, valueMask);
    }
  }
}

bool BuddyCoreSyncManagerI::isLocalSync(const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  return !current.ctx.count(SYNC_REMOTE_KEY);
}

bool BuddyCoreSyncManagerI::isBiblock(const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  return current.ctx.count(SYNC_BIBLOCK_KEY);
}

EntryListHolderPtr BuddyCoreSyncManagerI::findEntryListHolder(uint32_t fromId) {
  return ServiceI::instance().findObject<EntryListHolderPtr> (ENTRY_SEGMENT,
      fromId);
}

void BuddyCoreSyncManagerI::removeEntryListHolder(uint32_t fromId) {
  return ServiceI::instance().removeObject(ENTRY_SEGMENT, fromId);
}

void BuddyCoreSyncManagerI::syncUserCountGuestRequest(uint32_t userId) {
  TaskManager::instance().execute(new SyncUserCountGuestRequestTask(userId));
  return;
}
//---------------------------------------------------------------------------
bool BuddyDatabaseHelper::insertBuddyApply(int applicant, int accepter,
    const MyUtil::Str2StrMap& props) {
  MCE_INFO("insertBuddyApply applicant=" << applicant << " accepter="
      << accepter);
  const_cast<MyUtil::Str2StrMap&> (props)["APPLICANT"] = boost::lexical_cast<
    string>(applicant);
  const_cast<MyUtil::Str2StrMap&> (props)["ACCEPTER"] = boost::lexical_cast<
    string>(accepter);
  QueryRunner(DB_BUDDY_APPLY, CDbWServer).execute(Statement::buildReplace(
        TABLE_APPLY, props));
  return true;
}

bool BuddyDatabaseHelper::deleteBuddyApply(int applicant, int accepter) {
  MCE_INFO("deleteBuddyApply applicant=" << applicant << " accepter="
      << accepter);
  MyUtil::Str2StrMap filter;
  filter["APPLICANT"] = boost::lexical_cast<string>(applicant);
  filter["ACCEPTER"] = boost::lexical_cast<string>(accepter);
  QueryRunner(DB_BUDDY_APPLY, CDbWServer).execute(Statement::buildDelete(
        TABLE_APPLY, filter));
  return true;
}

bool BuddyDatabaseHelper::insertBuddyApplyN(int applicant, int accepter,
    const MyUtil::Str2StrMap& props) {
  MCE_INFO("insertBuddyApplyN applicant=" << applicant << " accepter="
      << accepter);
  const_cast<MyUtil::Str2StrMap&> (props)["APPLICANT"] = boost::lexical_cast<
    string>(applicant);
  const_cast<MyUtil::Str2StrMap&> (props)["ACCEPTER"] = boost::lexical_cast<
    string>(accepter);
  ostringstream tableAccepter;
  tableAccepter << TABLE_APPLY_ACCEPTER << abs(accepter) % TABLE_ACCEPTER_COUNT;
  QueryRunner(DB_BUDDY_ACCEPTER, CDbWServer,tableAccepter.str()).execute(Statement::buildReplace(
        tableAccepter.str(), props));
  ostringstream tableApplicant;
  tableApplicant << TABLE_APPLY_APPLICANT << abs(applicant) % TABLE_APPLICANT_COUNT;
  QueryRunner(DB_BUDDY_APPLICANT, CDbWServer,tableApplicant.str()).execute(Statement::buildReplace(
        tableApplicant.str(), props));
  return true;
}

bool BuddyDatabaseHelper::deleteBuddyApplyN(int applicant, int accepter) {
  MCE_INFO("deleteBuddyApplyN applicant=" << applicant << " accepter="
      << accepter);
  MyUtil::Str2StrMap filter;
  filter["APPLICANT"] = boost::lexical_cast<string>(applicant);
  filter["ACCEPTER"] = boost::lexical_cast<string>(accepter);
  ostringstream tableAccepter;
  tableAccepter << TABLE_APPLY_ACCEPTER << abs(accepter) % TABLE_ACCEPTER_COUNT;
  QueryRunner(DB_BUDDY_ACCEPTER, CDbWServer,tableAccepter.str()).execute(Statement::buildDelete(
        tableAccepter.str(), filter));

  ostringstream tableApplicant;
  tableApplicant << TABLE_APPLY_APPLICANT << abs(applicant) % TABLE_APPLICANT_COUNT;	
  QueryRunner(DB_BUDDY_APPLICANT, CDbWServer,tableApplicant.str()).execute(Statement::buildDelete(
        tableApplicant.str(), filter));

  return true;
}

bool BuddyDatabaseHelper::insertRelationBi(int from, int to) {
  MCE_INFO("insertRelationBi from=" << from << " to=" << to);
  string tableFrom = getRelationTable(from);
  string tableTo = getRelationTable(to);
  MyUtil::Date now(time(NULL));
  Statement sqlFrom;
  sqlFrom << "REPLACE INTO " << tableFrom << " (HOST, GUEST, TIME)"
    << " VALUES ('" << from << "', '" << to << "', '" << now.str()
    << "')";

  Statement sqlTo;
  sqlTo << "REPLACE INTO " << tableTo << " (HOST , GUEST, TIME)"
    << " VALUES ('" << to << "', '" << from << "', '" << now.str()
    << "')";

  QueryRunner(DB_BUDDY_FRIEND, CDbWServer, tableFrom).execute(sqlFrom);
  QueryRunner(DB_BUDDY_FRIEND, CDbWServer, tableTo).execute(sqlTo);

  return true;
}

bool BuddyDatabaseHelper::deleteRelationBi(int from, int to) {
  MCE_INFO("deleteRelationBi from=" << from << " to=" << to);
  string tableFrom = getRelationTable(from);
  string tableTo = getRelationTable(to);
  MyUtil::Date now(time(NULL));
  Statement sqlFrom;
  sqlFrom << "DELETE FROM " << tableFrom << " WHERE (HOST=" << from
    << " AND GUEST=" << to << ")";
  Statement sqlTo;
  sqlTo << "DELETE FROM " << tableTo << " WHERE (HOST=" << to
    << " AND GUEST=" << from << ")";

  QueryRunner(DB_BUDDY_FRIEND, CDbWServer, tableFrom).execute(sqlFrom);
  QueryRunner(DB_BUDDY_FRIEND, CDbWServer, tableTo).execute(sqlTo);

  return true;
}

bool BuddyDatabaseHelper::insertBlock(int from, int to) {
  MCE_INFO("insertBlock from=" << from << " to=" << to);
  Str2StrMap props;
  props["HOST"] = boost::lexical_cast<string>(from);
  props["GUEST"] = boost::lexical_cast<string>(to);
  props["TIME"] = MyUtil::Date(time(NULL)).str();
  QueryRunner(DB_BUDDY_BLOCK, CDbWServer).execute(Statement::buildInsert(
        TABLE_BLOCK, props));
  return true;
}

bool BuddyDatabaseHelper::deleteBlock(int from, int to) {
  MCE_INFO("deleteBlock from=" << from << " to=" << to);
  Str2StrMap filter;
  filter["HOST"] = boost::lexical_cast<string>(from);
  filter["GUEST"] = boost::lexical_cast<string>(to);
  QueryRunner(DB_BUDDY_BLOCK, CDbWServer).execute(Statement::buildDelete(
        TABLE_BLOCK, filter));
  return true;
}

string BuddyDatabaseHelper::getRelationTable(int from) {
  ostringstream table;
  table << TABLE_FRIEND << abs(from) % TABLE_FRIEND_COUNT;
  return table.str();
}

//---------------------------------------------------------------------------

Ice::ObjectPtr EntryListHolderFactory::create(Ice::Int id) {
  MCE_INFO("EntryListHolderFactory::create -> " << id);
  EntrySet building;
  EntryList duplicates;
  {
    Statement sql;
    sql << "SELECT guest FROM relation_block WHERE  host = '" << id << "'";
    BuddyCoreResultHandler handler(id, building, duplicates, "guest",
        DESC_BLOCK);
    QueryRunner(DB_BUDDY_BLOCK, CDbRServer).query(sql, handler);
  }

  {
    Statement sql;
    sql << "SELECT host FROM relation_block WHERE  guest = '" << id << "'";
    BuddyCoreResultHandler handler(id, building, duplicates, "host",
        DESC_BLOCKED);
    QueryRunner(DB_BUDDY_BLOCK, CDbRServer).query(sql, handler);
  }

  {
    ostringstream pattern;
    pattern << "relation_" << id % 100;
    Statement sql;
    sql << "SELECT guest FROM relation_" << id % 100 << " WHERE host = '"
      << id << "' AND disc=0";
    BuddyCoreResultHandler handler(id, building, duplicates, "guest",
        DESC_FRIEND);
    QueryRunner(DB_BUDDY_FRIEND, CDbRServer, pattern.str()).query(
        sql, handler);
  }

  {
    ostringstream pattern;
    pattern << "buddyapply_accepter_" << id % 100;
    Statement sql;
    sql << "SELECT applicant FROM buddyapply_accepter_" << id % 100 <<" WHERE accepter = '" << id
      << "'";
    BuddyCoreResultHandler handler(id, building, duplicates, "applicant",
        DESC_APPLIED);
    QueryRunner(DB_BUDDY_ACCEPTER, CDbRServer, pattern.str()).query(sql, handler);
  }

  {
    ostringstream pattern;
    pattern << "buddyapply_applicant_" << id % 100;
    Statement sql;
    sql << "SELECT accepter FROM buddyapply_applicant_" << id % 100 <<" WHERE applicant = '" << id
      << "'";
    BuddyCoreResultHandler handler(id, building, duplicates, "accepter",
        DESC_APPLY);
    QueryRunner(DB_BUDDY_APPLICANT, CDbRServer, pattern.str()).query(sql, handler);
  }
  /*
     {
     Statement sql;
     sql << "SELECT applicant FROM buddyapply WHERE accepter = '" << id
     << "'";
     BuddyCoreResultHandler handler(id, building, duplicates, "applicant",
     DESC_APPLIED);
     QueryRunner(DB_BUDDY_APPLY, CDbRServer).query(sql, handler);
     }

     {
     Statement sql;
     sql << "SELECT accepter FROM buddyapply WHERE applicant = '" << id
     << "'";
     BuddyCoreResultHandler handler(id, building, duplicates, "accepter",
     DESC_APPLY);
     QueryRunner(DB_BUDDY_APPLY, CDbRServer).query(sql, handler);
     }
   */
  MCE_DEBUG("S.For userId=" << id << " building.size=" << building.size()
      << " duplicate.size=" << duplicates.size());
  deduplicate(id, building, duplicates);
  MCE_DEBUG("E.For userId=" << id << " building.size=" << building.size()
      << " duplicate.size=" << duplicates.size());

  // construct result object.
  EntryList list;
  for (EntrySet::iterator it = building.begin(); it != building.end(); ++it) {
    list.push_back(*it);
  }
  sort(list.begin(), list.end(), less_entry());
  EntryListHolderPtr result = new EntryListHolder(id, list);

  TaskManager::instance().execute(new DupCleanupTask(id, duplicates));
  MCE_INFO("EntryListHolderFactory::create -> " << id << " done");
  return result;
}

void EntryListHolderFactory::deduplicate(int userId, EntrySet& building,
    EntryList& duplicates) {
  for (EntryList::iterator dupIt = duplicates.begin(); dupIt
      != duplicates.end(); //++dupIt
      ) {
    EntrySet::iterator buildingIt = building.find(*dupIt);
    if (buildingIt == building.end()) {
      //             This should not happend.
      MCE_WARN(
          "EntryListHolderFactory::create found a fake duplicate Entry. userId="
          << userId << " " << *dupIt);
      ++dupIt;
      continue;
    }

    if (dupIt->desc == DESC_BLOCKED) {
      if (buildingIt->desc == DESC_BLOCK) {
        building.erase(buildingIt);
        building.insert(Entry(dupIt->to, (DESC_BLOCK | DESC_BLOCKED)));
        dupIt = duplicates.erase(dupIt);
        continue;
      }
    }
    if (dupIt->desc == DESC_BLOCK) {
      if (buildingIt->desc == DESC_BLOCKED) {
        building.erase(buildingIt);
        building.insert(Entry(dupIt->to, (DESC_BLOCK | DESC_BLOCKED)));
        dupIt = duplicates.erase(dupIt);
        continue;
      }
    }
    ++dupIt;
  }
}

void DupCleanupTask::handle() {
  for (EntryList::iterator it = _dup.begin(); it != _dup.end(); ++it) {
    handle(*it);
  }
}

void DupCleanupTask::handle(Entry entry) {
  Statement sql,sqlfrom,sqlto;
  switch (entry.desc) {
    case DESC_APPLIED:
      {	
        /*
           sql << "DELETE FROM buddyapply WHERE accepter=" << _userId
           << " AND applicant=" << entry.to << " LIMIT 1";
           QueryRunner(DB_BUDDY_APPLY, CDbWServer).schedule(sql);
         */

        ostringstream pattern;
        pattern << "buddyapply_accepter_" << _userId % 100;	
        sqlfrom << "DELETE FROM buddyapply_accepter_" << _userId % 100 <<" WHERE accepter=" << _userId
          << " AND applicant=" << entry.to << " LIMIT 1";
        QueryRunner(DB_BUDDY_ACCEPTER, CDbWServer, pattern.str()).schedule(sqlfrom);

        ostringstream patternto;
        patternto << "buddyapply_applicant_" << entry.to % 100;
        sqlto << "DELETE FROM buddyapply_applicant_" << entry.to % 100 <<" WHERE accepter=" << _userId
          << " AND applicant=" << entry.to << " LIMIT 1";
        QueryRunner(DB_BUDDY_APPLICANT, CDbWServer,patternto.str()).schedule(sqlto);

      }
      break;
    case DESC_APPLY:
      {
        /*
           sql << "DELETE FROM buddyapply WHERE accepter=" << entry.to
           << " AND applicant=" << _userId << " LIMIT 1";
           QueryRunner(DB_BUDDY_APPLY, CDbWServer).schedule(sql);	
         */
        ostringstream pattern;
        pattern << "buddyapply_accepter_" << _userId % 100;
        sqlfrom << "DELETE FROM buddyapply_accepter_" << entry.to % 100 << " WHERE accepter=" << entry.to
          << " AND applicant=" << _userId << " LIMIT 1";
        QueryRunner(DB_BUDDY_ACCEPTER, CDbWServer,pattern.str()).schedule(sqlfrom);

        ostringstream patternto;
        patternto << "buddyapply_applicant_" << entry.to % 100;
        sqlto << "DELETE FROM buddyapply_applicant_" << _userId % 100 << " WHERE accepter=" << entry.to
          << " AND applicant=" << _userId << " LIMIT 1";
        QueryRunner(DB_BUDDY_APPLICANT, CDbWServer,patternto.str()).schedule(sqlto);

      }
      return;
    case DESC_FRIEND:
      {
        sql << "DELETE FROM relation_" << _userId % 100 << " WHERE host = '"
          << _userId << "' AND guest='" << entry.to
          << "' AND disc=0 LIMIT 1";
        ostringstream pattern;
        pattern << "relation_" << _userId % 100;
        QueryRunner(DB_BUDDY_FRIEND, CDbWServer, pattern.str()).schedule(sql);
      }
      return;
    default:
      MCE_WARN("Cleanup : " << _userId << " " << entry);
  }
}

void SyncUserCountGuestRequestTask::handle() {
  EntryListHolderPtr holder = ServiceI::instance().findObject<EntryListHolderPtr> (ENTRY_SEGMENT, 
      _userId);
  if (holder) {
    int count = 0;
    Entry key(0, DESC_APPLIED);
    Entry mask(0, DESC_MASK_APPLY);
    EntryList entryResult = holder->get(key, mask);
    count = entryResult.size() > 100 ? 101 : entryResult.size();
    MCE_INFO("syncUserCountGuestRequest id: " << _userId << "applied size: " << count);
    ::xce::usercount::adapter::UserCountAdapter::instance().set(_userId, ::xce::usercount::CGuestRequest, count);
  } else {
    MCE_WARN("SyncUserCountGuestRequestTask::handle" << _userId << " _holder is NULL");
  }
  return;
}

void MissedIdProcessTask::handle() {
  MCE_INFO("MissedIdProcessTask::handle " << _userId << " begin" );
  ServiceI::instance().locateObject<EntryListHolderPtr> (ENTRY_SEGMENT,_userId);
  TaskManager::instance().execute(new SyncUserCountGuestRequestTask(_userId));
  MCE_INFO("MissedIdProcessTask::handle " << _userId << " end" );
}
