#include "SyncAdapter.h"
#include "ServiceI.h"
#include "BuddyCoreLogicAdapter.h"

using namespace Ice;
using namespace com::xiaonei::service::buddy;
using namespace MyUtil;

void SyncAdapter::reload(::Ice::Int id ) {
  _localSyncManager->reload(id);
  BuddyCoreLogicAdapter::instance().reload(id);
}

void SyncAdapter::syncAddFriend(const Ice::Int& from, const Ice::Int& to, const Ice::Current& current) {
  _localSyncManager->syncAddFriend(from, to);

  Ice::Context ctx(current.ctx);
  buildContext(ctx,ADDFRIEND);
  getRemoteSyncManager(to)->syncAddFriend(from, to, ctx);
  BuddyCoreLogicAdapter::instance().addFriend(from, to, ctx);
}

void SyncAdapter::syncAddApply(const Applyship& apply,
    const Ice::Current& current) {
  _localSyncManager->syncAddApply(apply);

  Ice::Context ctx(current.ctx);
  buildContext(ctx, ADDAPPLY);
  getRemoteSyncManager(apply.accepter)->syncAddApply(apply, ctx);
  BuddyCoreLogicAdapter::instance().addApply(apply, ctx);
}

void SyncAdapter::syncAcceptApply(const Applyship& apply,
    const Ice::Current& current) {
  _localSyncManager->syncAcceptApply(apply);

  Ice::Context ctx(current.ctx);
  buildContext(ctx, ACCEPTAPPLY);
  getRemoteSyncManager(apply.applicant)->syncAcceptApply(apply, ctx);
  BuddyCoreLogicAdapter::instance().acceptApply(apply, ctx);
}

void SyncAdapter::syncDenyApply(const Applyship& apply,
    const Ice::Current& current) {
  _localSyncManager->syncDenyApply(apply);

  Ice::Context ctx(current.ctx);
  buildContext(ctx, DENYAPPLY);
  getRemoteSyncManager(apply.applicant)->syncDenyApply(apply, ctx);
  BuddyCoreLogicAdapter::instance().denyApply(apply, ctx);
}

void SyncAdapter::syncRemoveFriend(const Friendship& buddy,
    const Ice::Current& current) {
  _localSyncManager->syncRemoveFriend(buddy);

  Ice::Context ctx(current.ctx);
  buildContext(ctx, REMOVEFRIEND);
  getRemoteSyncManager(buddy.to)->syncRemoveFriend(buddy, ctx);
  BuddyCoreLogicAdapter::instance().removeFriend(buddy, ctx);
}

void SyncAdapter::syncAddBlock(const Blockship& block, bool biblock,
    const Ice::Current& current1) {
  Ice::Current current(current1);
  buildContext(current.ctx, ADDBLOCK, SYNC_LOCAL, biblock);
  _localSyncManager->syncAddBlock(block, current);

  Ice::Context ctx(current1.ctx);
  buildContext(ctx, ADDBLOCK, SYNC_REMOTE, biblock);
  getRemoteSyncManager(block.to)->syncAddBlock(block, ctx);
  BuddyCoreLogicAdapter::instance().addBlock(block, current.ctx);
}

void SyncAdapter::syncRemoveBlock(const Blockship& block, bool biblock,
    const Ice::Current& current1) {
  Ice::Current current(current1);
  buildContext(current.ctx, REMOVEBLOCK,SYNC_LOCAL, biblock);
  _localSyncManager->syncRemoveBlock(block, current);

  Ice::Context ctx(current1.ctx);
  buildContext(ctx, REMOVEBLOCK,SYNC_REMOTE, biblock);
  getRemoteSyncManager(block.to)->syncRemoveBlock(block, ctx);
  BuddyCoreLogicAdapter::instance().removeBlock(block, current.ctx);
}

BuddyCoreSyncManagerPrx SyncAdapter::getRemoteSyncManager(int id) {
  if(0 == getProxy((long)id)){
    return _syncManagers.at(abs(id)%10);
  }else{
    return getProxy((long)id);
  }
}

/*
   BuddyCoreSyncManagerPrx& SyncAdapter::getRemoteSyncManagerOneway(int id)
   {
   if (_syncManagersOneway.size() < _cluster)
   {
   _syncManagersOneway.resize(_cluster);
   }
   size_t pos = abs(id) % _cluster;
   if (!_syncManagersOneway[pos])
   {
   _syncManagersOneway[pos] = createProxy<BuddyCoreSyncManagerPrx>("S",
   pos, false);
   }
   return _syncManagersOneway[pos];
   }
 */
void SyncAdapter::initialize() {
  MCE_DEBUG("SyncAdapter::initialize begin");
  ServiceI& service = ServiceI::instance();
  ObjectPtr obj = service.getAdapter()->find(service.createIdentity("S", ""));
  _localSyncManager = BuddyCoreSyncManagerPtr::dynamicCast(obj);

  MyUtil::ChannelPtr _channel = new OceChannel();
  for(int i=0; i<10; i++){
    ostringstream endpoint;
    endpoint << "@BuddyCore" << i;
    _syncManagers.push_back( _channel->uncheckedCast<BuddyCoreSyncManagerPrx>(_channel->locate("S", endpoint.str(),TWO_WAY, 300)));
  }
  MCE_DEBUG("SyncAdapter::initialize end");
}

void SyncAdapter::buildContext(Ice::Context& ctx, Operation oper, bool remote, bool biblock) {
  if (remote) {
    ctx.insert(make_pair(SYNC_REMOTE_KEY, SYNC_REMOTE_VALUE));
  }
  if (biblock) {
    ctx.insert(make_pair(SYNC_BIBLOCK_KEY, SYNC_BIBLOCK_VALUE));
  }
  std::stringstream ss;
  switch(oper){
    case ADDAPPLY:{
                    ss<<Apply;
                  }break;
    case ACCEPTAPPLY:{
                       ss<<Friend;
                     }break;
    case DENYAPPLY:{
                     ss<<NoPath;
                   }break;
    case REMOVEFRIEND:{
                        ss<<NoPath;
                      }break;
    case ADDBLOCK:{
                    if(biblock){
                      ss<<BiBlock;
                    }else{
                      ss<<Block;
                    }

                  }break;
    case REMOVEBLOCK:{
                       if(biblock){
                         ss<<Blocked;
                       }else{
                         ss<<NoPath;
                       }
                     }break;
    case ADDFRIEND:{
                     ss<<Friend;
                   }break;
    default:
                   ss<<NoPath;
                   break;
  }
  ctx.insert(make_pair(RELATION,ss.str()));
}

