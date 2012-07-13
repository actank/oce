#include "BuddyCoreLogicI.h"
#include <boost/lexical_cast.hpp>
#include "ServiceI.h"
#include "FriendDistAdapter.h"
#include "FriendFinderReplicaAdapter.h"
#include "TalkRightAdapter.h"
#include "FeedDispatcherAdapter.h"
#include "BuddyGroupAdapter.h"
#include "OfferFriendsAdapter.h"
#include "FriendFinderNewComerWorkerAdapter.h"
#include "FriendFinderByMSNReplicaAdapter.h"
#include "RecommendFriendAdapter.h"
#include "UserCountAdapter.h"

#include "OnlineCenterAdapter.h"
#include "PresenceMsgHandlerAdapter.h"
#include "IqMsgHandlerAdapter.h"

#include "BuddyByIdReplicaAdapter.h"
#include "BuddyByNameReplicaAdapter.h"
#include "BuddyByOnlineTimeReplicaAdapter.h"
#include "BuddyByAddTimeReplicaAdapter.h"
#include "FriendCountCacheReplicaAdapter.h"
#include "BuddyRedoMonitor.h"
#include "AntispamRelationAdapter.h"
#include "BuddyByApplicantAdapter.h"
#include "BuddyRelationReplicaAdapter.h"
#include "CardRelationCacheAdapter.h"
#include "AbstractCheckerAdapterFactory.h"
#include "FriendFinderWorkerAdapter.h"
#include "SocialGraphLogicAdapter.h"
#include "BuddyFriendsAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "BuddyXoaLogicAdapter.h"
#include "SocialGraphRcdMessageServiceAdapter.h"

using namespace MyUtil;
using namespace mop::hi::svc::adapter;
using namespace mop::hi::svc::model;
using namespace com::xiaonei::service::buddy;
using namespace talk::online::adapter;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::service;
using namespace com::xiaonei::talk::adapter;
using namespace xce::feed;
using namespace xce::buddyapplycache;
using namespace talk::adapter;
using namespace xce::recommendfriend::adapter;
using namespace xce::usercount::adapter;
using namespace com::xiaonei::xce;
using namespace xce::buddy::adapter;
using namespace xce::friendfinder::adapter;
using namespace xce::offerfriends::adapter;
using namespace xce::socialgraph::adapter;
using namespace xce::redomonitor;
using namespace xce::antispamrelation::adapter;
using namespace xce::cardcache;
using namespace ::xce::serverstate;
using namespace xce::check;
using namespace xce::socialgraph;
using namespace xce::buddy;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  BuddyCoreLogicI* manager = new BuddyCoreLogicI;
  service.getAdapter()->add(manager, service.createIdentity(BUDDY_CORE_LOGIC, ""));

  TaskManager::instance().config(TASK_LEVEL_NORMAL, ThreadPoolConfig(1, 1));

  //    --------------------------------HA--------------------------------------------------
  int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyCoreLogic.Mod", 0);
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyCoreLogic.Interval",5);
  ServerStateSubscriber::instance().initialize("ControllerBuddyCoreLogic",manager,mod,interval);

  //IM
  TaskManager::instance().config(TASK_LEVEL_PRESENCESUBSCRIBE, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_PRESENCESUBSCRIBED, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_PRESENCEUNSUBSCRIBED, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REMOVEBUDDYNOTIFY, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_ADDBUDDYONLINESTAT, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REMOVEBUDDYONLINESTAT, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_RELOADBLOCKUSER, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_NOTIFYBUDDYAPPLYCHANGE, ThreadPoolConfig(1, 1) );

  //XFeed
  TaskManager::instance().config(TASK_LEVEL_DELIVERSUPPLY, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_DELINSQL, ThreadPoolConfig(1, 1) );

  //Apply & Apply_Count
  //TaskManager::instance().config(TASK_LEVEL_REBUILDBUDDYAPPLYCACHE, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REBUILDBUDDYAPPLYCACHEN, ThreadPoolConfig(1, 1) );
  //TaskManager::instance().config(TASK_LEVEL_REMOVEBUDDYAPPLYCACHE, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REMOVEBUDDYAPPLYCACHEN, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_INCUSERCOUNTGUESTREQUEST, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_DECUSERCOUNTGUESTREQUEST, ThreadPoolConfig(1, 1) );

  //people you may know
  TaskManager::instance().config(TASK_LEVEL_APPLYOFFER, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_ACCEPTOFFER, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REMOVEMSNFINDER, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REMOVEFRIENDFINDER, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REMOVECOMMOMFRIEND, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REBUILDFRIENDFINDER, ThreadPoolConfig(1, 1) );

  //Recommend
  TaskManager::instance().config(TASK_LEVEL_REMOVERECOMMENDFRIEND, ThreadPoolConfig(1, 1) );

  //FriendDist
  TaskManager::instance().config(TASK_LEVEL_REBUILDFRIENDDIST, ThreadPoolConfig(1, 1) );

  //BuddyGroup
  TaskManager::instance().config(TASK_LEVEL_REMOVEBUDDYGROUP, ThreadPoolConfig(1, 1) );

  //New Buddy
  TaskManager::instance().config(TASK_LEVEL_ADDBUDDYBYIDLOADER , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REMOVEBUDDYBYIDCACHE , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REMOVEBUDDYBYNAMECACHE , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REMOVEBUDDYBYONLINETIMECACHE , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REMOVEBUDDYBYADDTIMECACHE , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_DECFRIENDCOUNTCACHE, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_RELOADBUDDYBYIDLOADER , ThreadPoolConfig(1, 1) );

  TaskManager::instance().config(TASK_LEVEL_NOTIFYANTISPAMADDAPPLY, ThreadPoolConfig(1, 1) );

  //BuddyByApplicantCache
  TaskManager::instance().config(TASK_LEVEL_ADDBUDDYBYAPPLICANTCACHE, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REMOVEBUDDYBYAPPLICANTCACHE, ThreadPoolConfig(1, 1) );

  //BuddyRelationCache
  TaskManager::instance().config(TASK_LEVEL_SETBUDDYRELATIONCACHE_APPLY, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_SETBUDDYRELATIONCACHE_FROM_TO, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_SETBUDDYRELATIONCACHE_BLOCK, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_SETBUDDYRELATIONCACHE_ACCEPTAPPLY, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_SETBUDDYRELATIONCACHE_DENYAPPLY, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_SETBUDDYRELATIONCACHE_REMOVEBLOCK, ThreadPoolConfig(1, 1) );

  TaskManager::instance().config(TASK_LEVEL_REMOVECARDRELATIONCACHE_FRIEND, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_CHANGEBUDDYNOTIFYFEED, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_CHECKADDBUDDY, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_CHECKREMOVEBUDDY, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_CHECKADDAPPLY, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_CHECKREMOVEAPPLY, ThreadPoolConfig(1, 1) );

  TaskManager::instance().config(TASK_LEVEL_SOCALGRAPHLOGICPEPORT, ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_BUDDYAPPLYNOTIFYFEED, ThreadPoolConfig(1, 1) );	


  TaskManager::instance().config(TASK_LEVEL_ADDBUDDYFRIENDCACHE , ThreadPoolConfig(1, 1) );	
  TaskManager::instance().config(TASK_LEVEL_REMOVEBUDDYFRIENDCACHE , ThreadPoolConfig(1, 1) );	

  TaskManager::instance().config(TASK_LEVEL_ADDBUDDYAPPLYNOTIFYRELOADER , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REMOVEBUDDYAPPLYNOTIFYRELOADER , ThreadPoolConfig(1, 1) );

  TaskManager::instance().config(TASK_LEVEL_BECOMEFRIENDNOTIFYBYIDRELOADER , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_REMOVEFRIENDNOTIFYBYIDRELOADER , ThreadPoolConfig(1, 1) );

  TaskManager::instance().config(TASK_LEVEL_XOAADDAPPLYTASK , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_XOAACCEPTAPPLYTASK , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_XOADENYAPPLYTASK , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_XOAADDFRIENDTASK , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_XOAREMOVEFRIENDTASK , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_XOAADDBLOCKTASK , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_XOAREMOVEBLOCKTASK , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_XOARELOADTASK , ThreadPoolConfig(1, 1) );

  TaskManager::instance().config(TASK_LEVEL_REMOVERCDCACHETASK , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_BECOMEFRIENDSTASK , ThreadPoolConfig(1, 1) );
  TaskManager::instance().config(TASK_LEVEL_BLOCKTASK , ThreadPoolConfig(1, 1) );

}

void BuddyCoreLogicI::addApply(const Applyship& apply,
    const Ice::Current& current) {
  MCE_INFO("addApply -> " << apply.applicant << " apply " << apply.accepter);

  //TaskManager::instance().execute(new PresenceSubscribeTask(const_cast<Ice::Context&>(current.ctx)["id"], apply.applicant, apply.accepter, const_cast<Ice::Context&>(current.ctx)["status"]));
  TaskManager::instance().execute(new PresenceSubscribeWithPostscriptTask(const_cast<Ice::Context&>(current.ctx)["id"], apply.applicant, apply.accepter, const_cast<Ice::Context&>(current.ctx)["status"], const_cast<Ice::Context&>(current.ctx)["WHY"]));
  //TaskManager::instance().execute(new RebuildBuddyApplyCacheTask(apply.accepter));
  TaskManager::instance().execute(new SetBuddyRelationCacheTask(apply, ::xce::buddy::Apply));
  TaskManager::instance().execute(new RebuildBuddyApplyCacheNTask(apply.accepter, current.ctx));
  TaskManager::instance().execute(new AddBuddyApplyNotifyReloaderTask(apply.accepter, current.ctx));
  TaskManager::instance().execute(new RemoveFriendFinderTask(apply.applicant, apply.accepter));
  //TaskManager::instance().execute(new RemoveCommonFriendTask(apply.applicant, apply.accepter));
  TaskManager::instance().execute(new ApplyOfferTask(apply.applicant, apply.accepter));
  TaskManager::instance().execute(new ApplyFriendFinderNewComerTask(apply.applicant, apply.accepter));
  TaskManager::instance().execute(new RemoveMSNFinderTask(apply.applicant, apply.accepter));
  TaskManager::instance().execute(new RemoveRecommendFriendTask(apply.applicant, apply.accepter));
  TaskManager::instance().execute(new NotifyBuddyApplyChangeTask(apply.accepter));
  TaskManager::instance().execute(new NotifyAntispamAddApplyTask(apply.applicant,apply.accepter,current.ctx));
  TaskManager::instance().execute(new AddBuddyByApplicantCacheTask(apply, current.ctx));
  //TaskManager::instance().execute(new CheckAddApplyTask(apply));
  TaskManager::instance().execute(new BuddyApplyNotifyFeedTask(apply.applicant, apply.accepter));
  TaskManager::instance().execute(new XoaAddApplyTask(apply.applicant, apply.accepter, current.ctx));
  TaskManager::instance().execute(new RemoveRcdCacheTask(apply.applicant, apply.accepter, current.ctx));
  MCE_DEBUG("addApply -> " << apply.applicant << " apply " << apply.accepter << " done");
}

void BuddyCoreLogicI::acceptApply(const Applyship& apply,
    const Ice::Current& current) {
  MCE_INFO("acceptApply -> " << apply.accepter << " accept " << apply.applicant);

  //TaskManager::instance().execute(new RemoveBuddyApplyCacheTask(apply.accepter, apply.applicant));
  TaskManager::instance().execute(new SetBuddyRelationCacheTask(apply, ::xce::buddy::Friend, TASK_LEVEL_SETBUDDYRELATIONCACHE_ACCEPTAPPLY));
  TaskManager::instance().execute(new RemoveBuddyApplyCacheNTask(apply.accepter, apply.applicant));
  TaskManager::instance().execute(new RemoveBuddyApplyNotifyReloaderTask(apply.accepter, apply.applicant));
  //remember to modify addFriend to keep up with acceptApply
  TaskManager::instance().execute(new PresenceSubscribedTask(const_cast<Ice::Context&>(current.ctx)["id"],apply.applicant, apply.accepter));
  TaskManager::instance().execute(new RemoveRecommendFriendTask(apply.applicant, apply.accepter));

  TaskManager::instance().execute(new RebuildFriendDistTask(apply.accepter));
  TaskManager::instance().execute(new RebuildFriendDistTask(apply.applicant));


  {
    string value = "";
    string doStr = "add";
    stringstream sthStr;
    sthStr<<apply.applicant;
    OperaDataPtr key = new OperaData(apply.accepter, doStr, sthStr.str());	
    TaskManager::instance().execute(new CheckAddBuddyTask( key, value));
  }
  TaskManager::instance().execute(new AddBuddyByIdLoaderTask(apply.accepter,apply.applicant));

  TaskManager::instance().execute(new AddBuddyFriendCacheTask(apply.accepter,apply.applicant));

  TaskManager::instance().execute(new AddBuddyOnlineStatTask(apply.accepter, apply.applicant));
  TaskManager::instance().execute(new RemoveFriendFinderTask(apply.accepter, apply.applicant));
  TaskManager::instance().execute(new RemoveFriendFinderTask(apply.applicant, apply.accepter));
  //TaskManager::instance().execute(new RemoveCommonFriendTask(apply.accepter, apply.applicant));
  //TaskManager::instance().execute(new RemoveCommonFriendTask(apply.applicant, apply.accepter));
  TaskManager::instance().execute(new AcceptOfferTask(apply.accepter, apply.applicant));
  TaskManager::instance().execute(new AcceptFriendFinderNewComerTask(apply.accepter, apply.applicant));
  TaskManager::instance().execute(new RemoveMSNFinderTask(apply.accepter, apply.applicant));
  TaskManager::instance().execute(new RemoveMSNFinderTask(apply.applicant, apply.accepter));
  TaskManager::instance().execute(new DeliverSupplyTask(apply.accepter, apply.applicant, current.ctx));

  TaskManager::instance().execute(new NotifyBuddyApplyChangeTask(apply.accepter));

  TaskManager::instance().execute(new RemoveBuddyByApplicantCacheTask(apply.applicant, apply.accepter));

  TaskManager::instance().execute(new BecomeFriendsTask(apply.applicant, apply.accepter, current.ctx));

  //TaskManager::instance().execute(new CheckRemoveApplyTask(apply));

  //  TaskManager::instance().execute(new SocalGraphLogicReportTask(apply));
  TaskManager::instance().execute(new SocalGraphLogicReportTask(apply.applicant,apply.accepter,com::xiaonei::service::buddy::Apply));
  TaskManager::instance().execute(new BecomeFriendNotifyByIdReloaderTask(apply.applicant, apply.accepter));
  TaskManager::instance().execute(new BecomeFriendNotifyByIdReloaderTask(apply.accepter,apply.applicant));
  TaskManager::instance().execute(new ChangeBuddyNotifyFeedTask(apply.applicant, apply.accepter));
  TaskManager::instance().execute(new XoaAcceptApplyTask(apply.applicant, apply.accepter, current.ctx));
  MCE_DEBUG("acceptApply -> " << apply.accepter << " accept " << apply.applicant << " done");
}

void BuddyCoreLogicI::addFriend(const Ice::Int from, const Ice::Int to,
    const Ice::Current& current) {
  MCE_INFO("addFriend ->  from: " << from << " to: " << to);

  //TaskManager::instance().execute(new RemoveBuddyApplyCacheTask(apply.accepter, apply.applicant));
  //TaskManager::instance().execute(new RemoveBuddyApplyCacheNTask(apply.accepter, apply.applicant));
  TaskManager::instance().execute(new SetBuddyRelationCacheTask(from,to, ::xce::buddy::Friend));
  TaskManager::instance().execute(new PresenceSubscribedTask(const_cast<Ice::Context&>(current.ctx)["id"],from, to));
  TaskManager::instance().execute(new RemoveRecommendFriendTask(from, to));

  TaskManager::instance().execute(new RebuildFriendDistTask(to));
  TaskManager::instance().execute(new RebuildFriendDistTask(from));

  TaskManager::instance().execute(new AddBuddyByIdLoaderTask(to,from));
  TaskManager::instance().execute(new AddBuddyFriendCacheTask(to,from));

  TaskManager::instance().execute(new AddBuddyOnlineStatTask(to, from));
  TaskManager::instance().execute(new RemoveFriendFinderTask(to, from));
  TaskManager::instance().execute(new RemoveFriendFinderTask(from, to));
  //TaskManager::instance().execute(new RemoveCommonFriendTask(to,from));
  //TaskManager::instance().execute(new RemoveCommonFriendTask(from, to));
  TaskManager::instance().execute(new ApplyOfferTask(to, from));
  TaskManager::instance().execute(new ApplyOfferTask(from, to));
  TaskManager::instance().execute(new ApplyFriendFinderNewComerTask(to, from));
  TaskManager::instance().execute(new ApplyFriendFinderNewComerTask(from, to));
  TaskManager::instance().execute(new RemoveMSNFinderTask(to, from));
  TaskManager::instance().execute(new RemoveMSNFinderTask(from, to));
  TaskManager::instance().execute(new DeliverSupplyTask(to, from, current.ctx));

  TaskManager::instance().execute(new RemoveBuddyByApplicantCacheTask(from, to));
  TaskManager::instance().execute(new BecomeFriendNotifyByIdReloaderTask(from, to));
  TaskManager::instance().execute(new BecomeFriendNotifyByIdReloaderTask(to,from));
  TaskManager::instance().execute(new ChangeBuddyNotifyFeedTask(from, to));
  TaskManager::instance().execute(new XoaAddFriendTask(from, to, current.ctx));

  TaskManager::instance().execute(new BecomeFriendsTask(from, to, current.ctx));
  MCE_DEBUG("addFriend -> from: " << from << " to: " << to << " done");
}

void BuddyCoreLogicI::denyApply(const Applyship& apply,
    const Ice::Current& current) {
  MCE_INFO("denyApply -> " << apply.accepter << " deny " << apply.applicant);
  TaskManager::instance().execute(new SetBuddyRelationCacheTask(apply, ::xce::buddy::NoPath,TASK_LEVEL_SETBUDDYRELATIONCACHE_DENYAPPLY));
  //TaskManager::instance().execute(new RemoveBuddyApplyCacheTask(apply.accepter, apply.applicant));
  TaskManager::instance().execute(new RemoveBuddyApplyCacheNTask(apply.accepter, apply.applicant));
  TaskManager::instance().execute(new RemoveBuddyApplyNotifyReloaderTask(apply.accepter, apply.applicant));
  TaskManager::instance().execute(new PresenceUnsubscribedTask(const_cast<Ice::Context&>(current.ctx)["id"], apply.applicant, apply.accepter));
  TaskManager::instance().execute(new RemoveFriendFinderTask(apply.accepter, apply.applicant));
  //TaskManager::instance().execute(new RemoveCommonFriendTask(apply.accepter, apply.applicant));
  TaskManager::instance().execute(new RemoveMSNFinderTask(apply.accepter, apply.applicant));
  TaskManager::instance().execute(new NotifyBuddyApplyChangeTask(apply.accepter));
  TaskManager::instance().execute(new RemoveBuddyByApplicantCacheTask(apply.applicant, apply.accepter));
  TaskManager::instance().execute(new XoaDenyApplyTask(apply.applicant, apply.accepter, current.ctx));
  //TaskManager::instance().execute(new CheckRemoveApplyTask(apply));
  MCE_DEBUG("denyApply -> " << apply.accepter << " deny " << apply.applicant << " done");
}

void BuddyCoreLogicI::removeFriend(const Friendship& buddy,
    const Ice::Current& current) {
  MCE_INFO("removeFriend -> " << buddy.from << " remove " << buddy.to);

  TaskManager::instance().execute(new SetBuddyRelationCacheTask(buddy.from,buddy.to, ::xce::buddy::NoPath));
  TaskManager::instance().execute(new DelInSqlTask("feedwb", buddy.from, buddy.to));

  TaskManager::instance().execute(new RemoveBuddyOnlineStatTask(buddy.from, buddy.to));
  TaskManager::instance().execute(new RemoveBuddyGroupTask(buddy.from, buddy.to));
  TaskManager::instance().execute(new RemoveBuddyNotifyTask(buddy.from, buddy.to));

  //TaskManager::instance().execute(new DecUserFriendCountTask(buddy.from));
  //TaskManager::instance().execute(new DecUserFriendCountTask(buddy.to));

  TaskManager::instance().execute(new RebuildFriendDistTask(buddy.from));
  TaskManager::instance().execute(new RebuildFriendDistTask(buddy.to));

  {
    string value = "";
    string doStr = "remove";
    ostringstream fromSth,toSth;
    fromSth<<buddy.to;
    toSth<<buddy.from;
    OperaDataPtr fromkey = new OperaData(buddy.from,doStr,fromSth.str());
    OperaDataPtr tokey = new OperaData(buddy.to,doStr,toSth.str());
    TaskManager::instance().execute(new CheckRemoveBuddyTask( fromkey, value));
    TaskManager::instance().execute(new CheckRemoveBuddyTask( tokey, value));
  }
  TaskManager::instance().execute(new RemoveBuddyByIdCacheTask(buddy.to,buddy.from));
  TaskManager::instance().execute(new RemoveBuddyFriendCacheTask(buddy.to,buddy.from));
  TaskManager::instance().execute(new RemoveBuddyByNameCacheTask(buddy.to,buddy.from));
  TaskManager::instance().execute(new RemoveBuddyByOnlineTimeCacheTask(buddy.to,buddy.from));
  TaskManager::instance().execute(new RemoveBuddyByAddTimeCacheTask(buddy.to,buddy.from));
  TaskManager::instance().execute(new DecFriendCountCacheTask(buddy.to));
  TaskManager::instance().execute(new RemoveBuddyByIdCacheTask(buddy.from,buddy.to));
  TaskManager::instance().execute(new RemoveBuddyFriendCacheTask(buddy.from,buddy.to));
  TaskManager::instance().execute(new RemoveBuddyByNameCacheTask(buddy.from,buddy.to));
  TaskManager::instance().execute(new RemoveBuddyByOnlineTimeCacheTask(buddy.from,buddy.to));
  TaskManager::instance().execute(new RemoveBuddyByAddTimeCacheTask(buddy.from,buddy.to));
  TaskManager::instance().execute(new DecFriendCountCacheTask(buddy.from));

  TaskManager::instance().execute(new RemoveCardRelationCacheTask(buddy.from, buddy.to));	
  TaskManager::instance().execute(new ChangeBuddyNotifyFeedTask(buddy.from, buddy.to));
  TaskManager::instance().execute(new RemoveFriendNotifyByIdReloaderTask(buddy.from, buddy.to));
  TaskManager::instance().execute(new RemoveFriendNotifyByIdReloaderTask(buddy.to,buddy.from));
  TaskManager::instance().execute(new XoaRemoveFriendTask(buddy.from,buddy.to, current.ctx));
  MCE_DEBUG("removeFriend -> " << buddy.from << " remove " << buddy.to << " done");
}

void BuddyCoreLogicI::addBlock(const Blockship& block,
    const Ice::Current& current) {
  MCE_INFO("addBlock -> " << block.from << " block " << block.to);

  //TaskManager::instance().execute(new RemoveBuddyApplyCacheTask(block.from, block.to));
  //TaskManager::instance().execute(new RemoveBuddyApplyCacheTask(block.to, block.from));

  MyUtil::Str2StrMap::const_iterator it;
  it = current.ctx.find("RELATION");
  if( it != current.ctx.end()){
    TaskManager::instance().execute(new SetBuddyRelationCacheTask(block,
          ::xce::buddy::BuddyDesc(boost::lexical_cast<int>(it->second))));
  }else{
    MCE_WARN( "addBlock notify RelationCache error: " << block.from << " -> " << block.to );
  }

  TaskManager::instance().execute(new RemoveBuddyApplyCacheNTask(block.from, block.to));
  TaskManager::instance().execute(new RemoveBuddyApplyCacheNTask(block.to, block.from));
  TaskManager::instance().execute(new RemoveBuddyApplyNotifyReloaderTask(block.from, block.to));
  TaskManager::instance().execute(new RemoveBuddyApplyNotifyReloaderTask(block.to, block.from));

  TaskManager::instance().execute(new RemoveBuddyOnlineStatTask(block.from, block.to));
  TaskManager::instance().execute(new ReloadBlockUserTask(block.from));

  TaskManager::instance().execute(new RemoveFriendFinderTask(block.from, block.to));
  TaskManager::instance().execute(new RemoveFriendFinderTask(block.to, block.from));
  //TaskManager::instance().execute(new RemoveCommonFriendTask(block.from, block.to));
  //TaskManager::instance().execute(new RemoveCommonFriendTask(block.to, block.from));
  TaskManager::instance().execute(new RebuildFriendFinderTask(block.from));
  TaskManager::instance().execute(new RebuildFriendFinderTask(block.to));

  TaskManager::instance().execute(new RebuildFriendDistTask(block.from));
  TaskManager::instance().execute(new RebuildFriendDistTask(block.to));

  TaskManager::instance().execute(new ReloadBuddyByIdLoaderTask(block.from));
  TaskManager::instance().execute(new ReloadBuddyByIdLoaderTask(block.to));

  TaskManager::instance().execute(new RemoveBuddyByApplicantCacheTask(block.from, block.to));
  TaskManager::instance().execute(new RemoveBuddyByApplicantCacheTask(block.to, block.from));

  TaskManager::instance().execute(new ChangeBuddyNotifyFeedTask(block.from, block.to));
  TaskManager::instance().execute(new SocalGraphLogicReportTask(block.from,block.to,com::xiaonei::service::buddy::Block));

  TaskManager::instance().execute(new RemoveFriendNotifyByIdReloaderTask(block.from, block.to));
  TaskManager::instance().execute(new RemoveFriendNotifyByIdReloaderTask(block.to,block.from));

  TaskManager::instance().execute(new XoaAddBlockTask(block.from, block.to, current.ctx));

  TaskManager::instance().execute(new BlockTask(block.from, block.to, current.ctx));
  MCE_DEBUG("addBlock -> " << block.from << " block " << block.to << " done");
}

void BuddyCoreLogicI::removeBlock(const Blockship& block,
    const Ice::Current& current) {
  MCE_INFO("removeBlock -> " << block.from << " remove block " << block.to);

  TaskManager::instance().execute(new ReloadBlockUserTask(block.from));

  MyUtil::Str2StrMap::const_iterator it;
  it = current.ctx.find("RELATION");
  if( it != current.ctx.end()){
    TaskManager::instance().execute(new SetBuddyRelationCacheTask(block, 
          ::xce::buddy::BuddyDesc(boost::lexical_cast<int>(it->second)), TASK_LEVEL_SETBUDDYRELATIONCACHE_REMOVEBLOCK));
  }else{
    MCE_WARN( "removeBlock notify RelationCache error: " << block.from << " -> " << block.to );
  }

  TaskManager::instance().execute(new XoaRemoveBlockTask(block.from, block.to, current.ctx));
  MCE_INFO("removeBlock -> " << block.from << " remove block " << block.to << " done");
}

void BuddyCoreLogicI::reload(const Ice::Int userId, const Ice::Current& current) {

  MCE_INFO("reload -> " << userId );

  TaskManager::instance().execute(new ReloadBuddyByIdLoaderTask(userId));
  TaskManager::instance().execute(new XoaReloadTask(userId,current.ctx));

  MCE_INFO("reload -> " << userId << " done");
}

// **************************************************************************

void PresenceSubscribeWithPostscriptTask::handle() {
  try {
    PresenceMsgHandlerAdapter::instance().presenceSubscribeWithPostscript(id_, from_, to_, status_, postscript_);
  } catch (...) {
    // do nothing...
  }
}

void PresenceSubscribeTask::handle() {
  PresenceMsgHandlerAdapter::instance().presenceSubscribe(_id, _from, _to, _status);
}

void PresenceSubscribedTask::handle() {
  PresenceMsgHandlerAdapter::instance().presenceSubscribed(_id, _from, _to);
}

void PresenceUnsubscribedTask::handle() {
  PresenceMsgHandlerAdapter::instance().presenceUnsubscribed(_id, _from, _to);
}
/*
   void RebuildBuddyApplyCacheTask::handle() {
   BuddyApplyCacheAdapter::instance().rebuildBuddyApplyCache(_id);
   }

   void RemoveBuddyApplyCacheTask::handle() {
   BuddyApplyCacheAdapter::instance().removeBuddyApplyCache(_id, _applicant);
   }
 */
void RebuildBuddyApplyCacheNTask::handle() {
  //        BuddyApplyCacheAdapterN::instance().rebuildBuddyApplyCache(_id,_ctx);
  std::stringstream ss;
  std::string desc;
  ss << "BuddyApplyCache " << _id;
  desc = ss.str();

  RebuildBuddyApplyCacheRedoMonitor<Ice::Context> rbac(_ctx,_id,false,desc);
  rbac.monitorHandle(); 
}

void AddBuddyApplyNotifyReloaderTask::handle(){
  std::stringstream ss;
  std::string desc;
  ss << "BuddyApplyNotifyReloader " << _id;
  desc = ss.str();

  AddBuddyApplyNotifyReloaderRedoMonitor<Ice::Context> abanr(_ctx,_id,false,desc);
  abanr.monitorHandle();
}

void BecomeFriendNotifyByIdReloaderTask::handle(){
  std::stringstream ss;
  std::string desc;
  ss << "BecomeFriendNotifyByIdReloader " << _host << " " << _guest;
  desc = ss.str();
  BecomeFriendNotifyByIdReloaderRedoMonitor<Ice::Context> bfnb(_host,_guest,false,desc);
  bfnb.monitorHandle(); 
}

void RemoveFriendNotifyByIdReloaderTask::handle(){
  std::stringstream ss;
  std::string desc;
  ss << "RemoveFriendNotifyByIdReloader " << _host << " " << _guest;
  desc = ss.str();
  RemoveFriendNotifyByIdReloaderRedoMonitor<Ice::Context> rfnb(_host,_guest,false,desc);
  rfnb.monitorHandle();
}

void RemoveBuddyApplyCacheNTask::handle() {
  //BuddyApplyCacheAdapterN::instance().removeBuddyApplyCache(_id, _applicant);
  std::stringstream ss;
  std::string desc;
  ss << "BuddyApplyCache " << _id;
  desc = ss.str();

  RemoveBuddyApplyCacheRedoMonitor<Ice::Context> instance(_id,_applicant,false,desc);
  instance.monitorHandle();
}

void RemoveBuddyApplyNotifyReloaderTask::handle(){
  std::stringstream ss;
  std::string desc;
  ss << "BuddyApplyNotifyReloader " << _id;
  desc = ss.str();

  RemoveBuddyApplyNotifyReloaderRedoMonitor<Ice::Context> instance(_id,_applicant,false,desc);
  instance.monitorHandle();
}

void AddBuddyByApplicantCacheTask::handle() {
  BuddyByApplicantLoaderAdapter::instance().addAccepter(_id, _accepter);
}

void RemoveBuddyByApplicantCacheTask::handle() {
  BuddyByApplicantLoaderAdapter::instance().removeAccepter(_id, _accepterId);
}

void RemoveCommonFriendTask::handle() {
  FriendFinderWorkerAdapter::instance().RemoveCommonFriend(_userId, _friendId);
}


void RemoveFriendFinderTask::handle() {
  //FriendFinderGuideReplicaNAdapter::instance().removeCommonFriend(_userId, _friendId);
}

void RebuildFriendFinderTask::handle() {
  //FriendFinderGuideReplicaNAdapter::instance().rebuildCommonFriend(_userId);
}

void ApplyOfferTask::handle() {
  OfferFriendsLoaderAdapter::instance().apply(_applicant,_acceptor);
}

void AcceptOfferTask::handle() {
  OfferFriendsLoaderAdapter::instance().accept(_acceptor,_applicant);
}

void ApplyFriendFinderNewComerTask::handle() {
  try{
    FriendFinderNewComerWorkerAdapter::instance().apply(_applicant,_acceptor);
  }catch( Ice::Exception e ){
    MCE_WARN("[FriendFinderNewComerWorkerAdapter::apply()] applicant=" << _applicant
        << " acceptor=" << _acceptor
        << " found Ice::Exception : " << e.what() );
  } catch( ... ){
    MCE_WARN("[FriendFinderNewComerWorkerAdapter::apply()] applicant=" << _applicant
        << " acceptor=" << _acceptor
        << " found unknown exception" );
  }
}

void AcceptFriendFinderNewComerTask::handle() {
  try{
    FriendFinderNewComerWorkerAdapter::instance().accept(_acceptor,_applicant);
  }catch( Ice::Exception e ){
    MCE_WARN("[FriendFinderNewComerWorkerAdapter::accept()] applicant=" << _applicant
        << " acceptor=" << _acceptor
        << " found Ice::Exception : " << e.what() );
  } catch( ... ){
    MCE_WARN("[FriendFinderNewComerWorkerAdapter::accept()] applicant=" << _applicant
        << " acceptor=" << _acceptor
        << " found unknown exception" );
  }
}

void RemoveMSNFinderTask::handle() {
  FriendFinderByMSNAdapter::instance().remove(_userId, _friendId);
}

void AddBuddyOnlineStatTask::handle() {
  OnlineCenterAdapter::instance().addBuddy(_host, _guest);
}

void RemoveBuddyOnlineStatTask::handle() {
  OnlineCenterAdapter::instance().removeBuddy(_host, _guest);
}

void DeliverSupplyTask::handle() {
  FeedDispatcherAdapter::instance().addBuddyNotify(_supplier, _accepter);
}

void RemoveBuddyGroupTask::handle() {
  BuddyGroupAdapter::instance().removeBuddy(_from, _to);
}

void RemoveBuddyNotifyTask::handle() {
  IqMsgHandlerAdapter::instance().removeBuddyNotify(_from, _to);
}

void DelInSqlTask::handle() {
  /*
     {	
     ConnectionHolder conn(_source, CDbWServer," ");
     mysqlpp::Query query = conn.query();
     query << "DELETE FROM relation_feed_high"
     << " WHERE SOURCE=" << _from << " AND TARGET=" << _to
     << " OR SOURCE=" << _to << " AND TARGET=" << _from;
     query.store();
     }
     {
     ConnectionHolder conn(_source, CDbWServer," ");
     mysqlpp::Query query = conn.query();
     query << "DELETE FROM relation_feed_low"
     << " WHERE SOURCE=" << _from << " AND TARGET=" << _to
     << " OR SOURCE=" << _to << " AND TARGET=" << _from;
     query.store();
     }
   */
  Statement sqlFeedHigh;
  sqlFeedHigh << "DELETE FROM relation_feed_high"
    << " WHERE SOURCE=" << _from << " AND TARGET=" << _to
    << " OR SOURCE=" << _to << " AND TARGET=" << _from;
  QueryRunner(_source, CDbWServer).schedule(sqlFeedHigh);
  Statement sqlFeedLow;
  sqlFeedLow << "DELETE FROM relation_feed_low"
    << " WHERE SOURCE=" << _from << " AND TARGET=" << _to
    << " OR SOURCE=" << _to << " AND TARGET=" << _from;
  QueryRunner(_source, CDbWServer).schedule(sqlFeedLow);
}

void ReloadBlockUserTask::handle() {
  TalkRightAdapter::instance().reloadBlockUser(_id);
}

void RemoveRecommendFriendTask::handle() {
  RecommendFriendAdapter::instance().removeRecommendFriend(_ownerId,_otherId);
}

void RebuildFriendDistTask::handle() {
  //FriendDistAdapter::instance().rebuildFriendDist(_id);
}

void AddBuddyByIdLoaderTask::handle() {
  //BuddyByIdLoaderAdapter::instance().add(_userid,_friendid);
  std::stringstream ss;
  std::string desc;
  ss << "BuddyByIdLoader " << _userid <<" " << _friendid;
  desc = ss.str();

  AddBuddyByIdLoaderRedoMonitor<Ice::Context> instance(_userid,_friendid,false,desc);
  instance.monitorHandle();
}

void RemoveBuddyByIdCacheTask::handle() {
  BuddyByIdCacheAdapter::instance().remove(_userid,_friendid);
}

void AddBuddyFriendCacheTask::handle() {
  BuddyFriendsLoaderAdapter::instance().add(_userid,_friendid);
}
void RemoveBuddyFriendCacheTask::handle() {
  BuddyFriendsCacheAdapter::instance().remove(_userid,_friendid);
}
void RemoveBuddyByNameCacheTask::handle() {
  BuddyByNameCacheAdapter::instance().remove(_userid,_friendid);
}

void RemoveBuddyByOnlineTimeCacheTask::handle() {
  BuddyByOnlineTimeCacheAdapter::instance().remove(_userid,_friendid);
}

void RemoveBuddyByAddTimeCacheTask::handle() {
  BuddyByAddTimeCacheAdapter::instance().remove(_userid,_friendid);
}

void DecFriendCountCacheTask::handle() {
  FriendCountCacheAdapter::instance().dec(_userid);
}

void ReloadBuddyByIdLoaderTask::handle() {
  BuddyByIdLoaderAdapter::instance().reload(_id);
}

void NotifyBuddyApplyChangeTask::handle(){
  IqMsgHandlerAdapter::instance().NotifyBuddyApplyChange(_id);
}

void NotifyAntispamAddApplyTask::handle(){
  Str2StrMap map;
  ostringstream applicant;
  applicant << _applicant;
  ostringstream accepter;
  accepter << _accepter;
  map["func"] = "addfriends._notify";
  map["addfriends._notify.applicant"] = applicant.str();
  map["addfriends._notify.accepter"] = accepter.str();
  map["addfriends.Operation"] = "AddApply";
  map["addfriends._notify.time"] = _ctx["TIME"];
  map["addfriends._notify.why"] = _ctx["WHY"];
  map["addfriends._notify.statfrom"] = _ctx["STAT_FROM"];
  map["addfriends._notify.groupname"] = _ctx["GROUP_NAME"];
  map["addfriends._notify.ref"] = _ctx["REF"];
  AntispamRelationAdapter::instance().notify(map);
}

void SetBuddyRelationCacheTask::handle() {
  BuddyRelationLoaderAdapter::instance().setRelation(_relation, _desc);
}

void RemoveCardRelationCacheTask::handle() {
  CardRelationCacheAdapter::instance().remove(_host,_guest);
}

void ChangeBuddyNotifyFeedTask::handle() {
  FeedDispatcherAdapter::instance().changeBuddyNotify(_host,_guest);
}

void BuddyApplyNotifyFeedTask::handle() {
  FeedDispatcherAdapter::instance().buddyApplyNotify(_host,_guest);
}

void CheckAddBuddyTask::handle() {
  AbstractCheckerAdapterFactory::instance().getBuddyChecker("BuddyCoreLogicCheckAddBuddy","@CheckerBuddy", 
      "CheckerAddBuddy","BuddyCoreLogic","BuddyCoreLogic")->asycAdd(_key,_value);
}

void CheckRemoveBuddyTask::handle() {
  AbstractCheckerAdapterFactory::instance().getBuddyChecker("BuddyCoreLogicCheckRemoveBuddy","@CheckerBuddy",
      "CheckerRemoveBuddy","BuddyCoreLogic","BuddyCoreLogic")->asycAdd(_key,_value);
}

void CheckAddApplyTask::handle() {
  MCE_INFO(" CheckAddApplyTask::handle -> " << _apply.applicant << " apply " << _apply.accepter);
  string doStr = "applied";
  stringstream sthStr;
  sthStr << _apply.applicant;

  OperaDataPtr key = new OperaData(_apply.accepter, doStr, sthStr.str());
  ostringstream value;
  value << sthStr.str();
  string business =  ServiceI::instance().getBusiness();
  string name = ServiceI::instance().getName();
  string replica = name;
  AbstractCheckerAdapterFactory::instance().getOceChecker( replica+"CheckerAddApply", "@CheckerApply",
      "CheckerAddApply", business, replica)->asycAdd(key,value.str());
}

void CheckRemoveApplyTask::handle() {
  MCE_INFO(" CheckRemoveApplyTask::handle -> " << _apply.applicant << " apply " << _apply.accepter);
  string doStr = "removeApplied";
  stringstream sthStr;
  sthStr << _apply.applicant;

  OperaDataPtr key = new OperaData(_apply.accepter, doStr, sthStr.str());
  ostringstream value;
  value << sthStr.str();
  string business =  ServiceI::instance().getBusiness();
  string name = ServiceI::instance().getName();
  string replica = name;

  AbstractCheckerAdapterFactory::instance().getOceChecker( replica+"CheckerRemoveApply", "@CheckerApply",
      "CheckerRemoveApply", business, name)->asycAdd(key,value.str());
}

void SocalGraphLogicReportTask::handle(){
  MCE_INFO(" SocalGraphLogicReportTask::handle -> " << _host << " do " << _guest << _desc);
  ::xce::socialgraph::Message message;
  ::xce::socialgraph::Info info;
  info.hostId = _host ;
  info.type = "FFUCR";
  if(com::xiaonei::service::buddy::Apply == _desc){
    info.operation = ::xce::socialgraph::APPLY;
  }else if(com::xiaonei::service::buddy::Block == _desc){
    info.operation = ::xce::socialgraph::DELETE;
  }
  info.friendIds.push_back(_guest);
  message.content.insert(std::make_pair(info.type, info));
  SocialGraphLogicAdapter::instance().Report( info.hostId , message);
}

//------------------------BuddyRcdLogic-------------------------

void RemoveRcdCacheTask::handle() {
	RcdMessageServiceAdapter::instance().RemoveRcdCache(_host, _guest);
}

void BecomeFriendsTask::handle() {
	RcdMessageServiceAdapter::instance().BecomeFriends(_host, _guest);
}

void BlockTask::handle() {
	RcdMessageServiceAdapter::instance().Block(_host, _guest);
}

//------------------------BuddyXoaLogic-------------------------
void XoaAddApplyTask::handle(){
	BuddyXoaLogicAdapter::instance().addApply(_applicant, _apply, _ctx);
}

void XoaAcceptApplyTask::handle(){
	BuddyXoaLogicAdapter::instance().acceptApply(_applicant, _apply, _ctx);
}

void XoaDenyApplyTask::handle(){
	BuddyXoaLogicAdapter::instance().denyApply(_applicant, _apply, _ctx);
}

void XoaAddFriendTask::handle(){
	BuddyXoaLogicAdapter::instance().addFriend(_from, _to, _ctx);
}

void XoaRemoveFriendTask::handle(){
	BuddyXoaLogicAdapter::instance().removeFriend(_from, _to, _ctx);
}

void XoaAddBlockTask::handle(){
	BuddyXoaLogicAdapter::instance().addBlock(_block, _blocked, _ctx);
}

void XoaRemoveBlockTask::handle(){
	BuddyXoaLogicAdapter::instance().removeBlock(_block, _blocked, _ctx);
}

void XoaReloadTask::handle(){
  BuddyXoaLogicAdapter::instance().reload(_userId,_ctx);
}
