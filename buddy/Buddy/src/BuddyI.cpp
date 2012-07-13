#include "BuddyI.h"
#include "ServiceI.h"
#include "BuddyAdapter.h"
#include "BuddyLogicAdapter.h"
#include <boost/lexical_cast.hpp>
#include "ConnectionQuery.h"
#include "QueryRunner.h"
#include "Date.h"
#include "ActiveTrackAdapter.h"
#include "UserCountAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace MyUtil;
using namespace xce::buddy;
using namespace com::xiaonei::xce;
using namespace ::xce::log;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(new BuddyManagerI, service.createIdentity(
			"M", ""));
	service.registerObjectCache(BUDDY_RELATION, "BR", new SyncServantFactory(
			new BuddyRelationHolderFactory));
	
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Buddy.Mod");
        string endpoint = service.getCommunicator()->getProperties()->getProperty("Buddy.Endpoint");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Buddy.Interval", 5);
        BuddyManagerI::instance().bootstrap(endpoint, mod);

        // 向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerBuddy",
                        &BuddyManagerI::instance(), mod, interval);
	BuddyManagerPrx localprx = BuddyManagerPrx::uncheckedCast(ServerStateSubscriber::instance().getServerPrx());
	BuddyAdapter::instance().setLocalPrx(localprx);	
	
	TaskManager::instance().config(TASK_LEVEL_CACHEUSERCOUNT, MyUtil::ThreadPoolConfig(1, 10));
	ActiveTrack::instance().initialize();
}

void BuddyManagerI::bootstrap(const string& endpoint, int mod) {
        _endpoint = endpoint;
        _mod = mod;
        MCE_INFO( "[BuddyManagerI::bootstrap] endpoint=" << _endpoint << " mod=" << _mod );
}


OpState BuddyManagerI::itemEmptyCheck(const BuddyState& state, const Relationship& relation, OpState& result,
             const ApplyInfo* applyInfo, const Ice::Current& current)
{
    result.first = NoPath;
    result.second = NoPath;
    switch(state)
    {
        case ADDAPPLY:
               {
                   result.second = Apply;
                   BuddyDatabaseHelper::instance().insertBuddyApply(relation.from,
                                relation.to, applyInfo->props);
                   BuddyAdapter::instance().cacheAddApply(relation, current);
                   MCE_INFO("addApply success.  accepter=" << relation.to
                                << " applicant=" << relation.from);    
               } break;
        case ACCEPTAPPLY:
        case DENYAPPLY:
               {
                   MCE_WARN(translateBuddyState(state)<< "found nothing to do. applicant=" << relation.from
                                << " accepter=" << relation.to);
                   BuddyAdapter::instance().reload(relation.to);
               } break;
        case ADDBLOCK:
               {
                   result.second = Block;
                   BuddyDatabaseHelper::instance().insertBlock(relation.from, relation.to);
                   BuddyAdapter::instance().cacheAddBlock(relation, Cache_NORMALBLOCK,current);
               } break;
        case REMOVEFRIEND:
        case REMOVEBLOCK:
               {
                   MCE_WARN(translateBuddyState(state)<< " found nothing to do. from=" << relation.from
                                << " to=" << relation.to);
                   BuddyAdapter::instance().reload(relation.from);
               } break; 
        default:
            MCE_WARN("unsupported type in itemEmptyCheck ");   
    }
    return result;
}

OpState BuddyManagerI::itemOverflowCheck(const BuddyState& state,
       const Relationship& relation, OpState& result, const Ice::Current& current)
{
    result.first = NoPath;
    result.second = NoPath;
    switch(state)
    {
        case ACCEPTAPPLY:
        case DENYAPPLY:
               {
                  MCE_WARN(translateBuddyState(state)<<" found more than 1 item . This value should always be 1. I will reload. from = "
                                << relation.from << " to = "<< relation.to << " for Cache");
                  BuddyAdapter::instance().reload(relation.to); 
               } break;
        case ADDAPPLY:
        case REMOVEFRIEND:
        case ADDBLOCK:
        case REMOVEBLOCK:
               {
                  MCE_WARN(translateBuddyState(state)<<" found more than 1 item. This value should always be 1. I will reload. from = "
                                << relation.from << " to = " << relation.to << " for Cache");
                  BuddyAdapter::instance().reload(relation.from);
               } break;

        default:
            MCE_WARN("unsupported type in itemOverflowCheck ");
    }
    return result;
}


OpState BuddyManagerI::processWithState(const BuddyState& state, const Relationship& relation,
                const ApplyInfo* applyInfo, const Ice::Current& current)
{
    MCE_DEBUG( translateBuddyState(state)<< " to=" << relation.to << " from="
                        << relation.from);
    OpState result;

    BuddyRelationHolderPtr buddyrealtion = locateBuddyRelationHolder(relation.from);
    //EntryList entry = getEntryList(relation,getDescFromState(state));

    specialHandle(state,relation,current);
	
    int i = buddyrealtion->getCount(relation.from,result.first);
    if ( 0 == i) { // empty() check must be the first checking.
        return itemEmptyCheck(state,relation,result,applyInfo,current);
    }

    if ( i > 1) {
        return itemOverflowCheck(state,relation,result);
    }

    result.second = result.first;
    return processHandleWithState(state, relation, result,applyInfo, current);
}

OpState BuddyManagerI::processHandleWithState(const BuddyState& state, const Relationship& relation, OpState& result,
                const ApplyInfo* applyInfo, const Ice::Current& current)
{
    switch(state)
    {
        case ADDAPPLY:
            return addApplyHandle( relation, *applyInfo,result, current);
        case ACCEPTAPPLY:
            return acceptApplyHandle(relation,result,current);
        case DENYAPPLY:
            return denyApplyHandle(relation,result,current);
        case REMOVEFRIEND:
            return removeFriendHandle(relation,result,current); 
        case ADDBLOCK:
            return addBlockHandle(relation,result, current);
        case REMOVEBLOCK:
            return removeBlockHandle(relation,result,current);
        default:
            MCE_WARN("unsupportd state in processWithState");
    }
    return result;
}

void BuddyManagerI::specialHandle(const BuddyState& state, const Relationship& relation, const Ice::Current& current)
{
    switch(state)
    {
        case ADDAPPLY:
            {
                // 记录LOG
                ostringstream object;
                object << relation.to;
                string ref = "UNKNOWN";
                map<string, string>::const_iterator itRef = current.ctx.find("REF");
                if (itRef != current.ctx.end()) {
                    ref = (*itRef).second;
                }
                ActiveTrack::instance().action(relation.from, ref, "BUDDYAPPLY",
                        object.str());
            } break;
        case ACCEPTAPPLY:
        case DENYAPPLY:
            {
                /* We need this line below locateEntryListHolder */
                BuddyDatabaseHelper::instance().deleteBuddyApply(relation.from,
                        relation.to);
            } break;

        default:
           //do nothing
           break;
    }
}
void BuddyManagerI::addApplyBatch(const ApplyshipMap& applies,
		const ApplyInfoMap& infos, const Ice::Current& current) {
	MCE_DEBUG("addApplyBatch: size = " << applies.size());
	for (ApplyshipMap::const_iterator applyIt = applies.begin(); applyIt
			!= applies.end(); ++applyIt) {
		Relationship apply = applyIt->second;
		ApplyInfo info;
		ApplyInfoMap::const_iterator infoIt = infos.find(applyIt->first);
		if (infoIt == infos.end()) {
			MCE_WARN("can not find matched ApplyInfo for Relationship: "
					<< applyIt->first);
			continue;
		} else {
			info = infoIt->second;
		}
		MCE_DEBUG("addApplyBatch: accept " << applyIt->first);
		addApply(apply, info, current);
	}
}

void BuddyManagerI::addApply(const Relationship& apply, const ApplyInfo& info,
		const Ice::Current& current) {
	TaskManager::instance().execute(new AddApplyTask(this, apply, info, current));
}

void AddApplyTask::handle() {
	_bm->processWithState(ADDAPPLY,_apply, &_info, _current);
}

OpState BuddyManagerI::addApplyWithState(const Relationship& apply, const ApplyInfo& info,
                const Ice::Current& current) {
    return processWithState(ADDAPPLY,apply,&info, current);
}

OpState BuddyManagerI::addApplyHandle( const Relationship& apply,const ApplyInfo& applyInfo,
                                       OpState& result, const Ice::Current& current)
{
    switch (result.first) {
        case Applied:
                MCE_DEBUG("Found Applied, confirm it.");
                result.second = Friend;
                BuddyDatabaseHelper::instance().deleteBuddyApply(apply.to,
                                apply.from);
                BuddyDatabaseHelper::instance().insertRelationBi(apply.from,
                                apply.to);
                Relationship confirm;
                confirm.from = apply.to;
                confirm.to = apply.from;
                BuddyAdapter::instance().cacheAcceptApply(confirm, current);
                return result;
        case Block:
                MCE_DEBUG("AddApply found Block. remove it and add apply");
                result.second = Apply;
                BuddyDatabaseHelper::instance().deleteBlock(apply.from,
                                apply.to);
                BuddyDatabaseHelper::instance().insertBuddyApply(apply.from,
                                apply.to, applyInfo.props);
                break;
        case BiBlock:
                MCE_DEBUG("AddApply found BiBlock. remove block and exit.");
                result.second = Blocked;
                BuddyDatabaseHelper::instance().deleteBlock(apply.from,
                                apply.to);
                Relationship removeBiBlock;
                removeBiBlock.from = apply.from;
                removeBiBlock.to = apply.to;
                BuddyAdapter::instance().cacheRemoveBlock(removeBiBlock, Cache_BIBLOCK,
                                current);
                return result;
        case Apply:
                // no needs to apply two times, return.
                MCE_DEBUG("Already apply");
                return result;
        case Blocked:
                MCE_DEBUG("Apply cannot overwrite Blocked");
                return result;
        case Friend:
                MCE_DEBUG("Already Friend");
                return result;
        default:
                MCE_WARN("AddApply says: 'this should no be seen'. result.first = "
                                << result.first);
        }

        // Cache
        BuddyAdapter::instance().cacheAddApply(apply, current);

        return result;
}

OpStateMap BuddyManagerI::addApplyWithStateBatch(
		const ApplyshipMap& applies, const ApplyInfoMap& infos,
		const Ice::Current& current) {
	MCE_DEBUG("addApplyWithStateBatch: size = " << applies.size());
	OpStateMap opstates;
	for (ApplyshipMap::const_iterator applyIt = applies.begin(); applyIt
			!= applies.end(); ++applyIt) {
		Relationship apply = applyIt->second;
		ApplyInfo info;
		ApplyInfoMap::const_iterator infoIt = infos.find(applyIt->first);
		if (infoIt == infos.end()) {
			MCE_WARN("can not find matched ApplyInfo for Relationship: "
					<< applyIt->first);
			continue;
		} else {
			info = infoIt->second;
		}
		MCE_DEBUG("addApplyBatch: accept " << applyIt->first);
		opstates[applyIt->first] = addApplyWithState(apply, info, current);
	}
	return opstates;
}

void BuddyManagerI::acceptAllApplied(::Ice::Int from,
		const Ice::Current& current) {
	MyUtil::IntSeq ids = getAppliedSeq(from, 0, -1, current);
	for (MyUtil::IntSeq::iterator it = ids.begin(); it != ids.end(); ++it) {
		Relationship apply;
		apply.from = *it;
		apply.to = from;
		MCE_DEBUG("acceptAllApplied: accept " << *it);
		acceptApply(apply, current);
	}
}
void BuddyManagerI::acceptApply(const Relationship& apply,
		const Ice::Current& current) {
	TaskManager::instance().execute(new AcceptApplyTask(this, apply, current));
}
void AcceptApplyTask::handle() {
	_bm->processWithState(ACCEPTAPPLY,_apply);
}

OpState BuddyManagerI::acceptApplyWithState(const Relationship& apply,
                const Ice::Current& current) {
    return processWithState(ACCEPTAPPLY,apply);
}

OpState BuddyManagerI::acceptApplyHandle(const Relationship& apply, OpState& result,
                const Ice::Current& current)
{
    switch (result.first) {
        case Applied:
                /*
                 * Done at the very beginning.
                 BuddyDatabaseHelper::instance().deleteBuddyApply(apply.from,
                 apply.accepter);
                 */
                BuddyDatabaseHelper::instance().insertRelationBi(apply.from,
                                apply.to);
                result.second = Friend;
                BuddyAdapter::instance().cacheAcceptApply(apply, current);
                MCE_INFO("acceptApply success.  accepter=" << apply.to
                                << " applicant=" << apply.from);
                return result;
        case Apply:
        case Block:
        case BiBlock:
        case Blocked:
        case Friend:
                MCE_WARN("acceptApply not applied, just return. accepter="
                                << apply.to << " applicant=" << apply.from);
                return result;
        default:
                MCE_WARN("acceptApply says: 'this should no be seen'. accepter="
                                << apply.to << " applicant=" << apply.from
                                << "result.first = " << result.first);
        }
        return result;
}

void BuddyManagerI::denyAllApplied(::Ice::Int from,
		const Ice::Current& current) {
	MyUtil::IntSeq ids = getAppliedSeq(from, 0, -1, current);
	for (MyUtil::IntSeq::iterator it = ids.begin(); it != ids.end(); ++it) {
		Relationship apply;
		apply.from = *it;
		apply.to = from;
		MCE_DEBUG("denyAllApplied: deny " << *it);
		denyApply(apply, current);
	}
}
void BuddyManagerI::denyApply(const Relationship& apply,
		const Ice::Current& current) {
	TaskManager::instance().execute(new DenyApplyTask(this, apply, current));
}
void DenyApplyTask::handle() {
	_bm->processWithState(DENYAPPLY,_apply);
}

OpState BuddyManagerI::denyApplyWithState(const Relationship& apply,
                const Ice::Current& current) {
    return processWithState(DENYAPPLY,apply);
}

OpState BuddyManagerI::denyApplyHandle(const Relationship& apply, OpState& result, 
                const Ice::Current& current)
{
    switch (result.first) {
        case Applied:
                /*
                 * Done at the very beginning.
                 BuddyDatabaseHelper::instance().deleteBuddyApply(apply.applicant,
                 apply.accepter);
                 */
                result.second = NoPath;
                BuddyAdapter::instance().cacheDenyApply(apply, current);
                MCE_INFO("denyApply success.accepter  =" << apply.to
                                << " applicant =" << apply.from);
                return result;
        case Apply:
        case Block:
        case Blocked:
        case BiBlock:
        case Friend:
        default:
                MCE_WARN("denyApplied says: 'this should no be seen'. result.first = "
                                << result.first);
        }
        return result;
}
::Ice::Int BuddyManagerI::getAppliedCount(::Ice::Int from,
		const Ice::Current& current) {
	return getAppliedSeq(from, 0, -1, current).size();
}

::MyUtil::IntSeq BuddyManagerI::getAppliedSeq(::Ice::Int from,
		::Ice::Int begin, ::Ice::Int limit, const Ice::Current& current) {
	MyUtil::IntSeq result = locateBuddyRelationHolder(from)->getBuddyList(Applied); 
	MCE_DEBUG("BuddyManagerI::getAppliedSeq -> " << from << " -> "
			<< result.size());
	return result;
}

::Ice::Int BuddyManagerI::getApplyCount(::Ice::Int from,
		const Ice::Current& current) {
	return getApplySeq(from, 0, -1, current).size();
}

::MyUtil::IntSeq BuddyManagerI::getApplySeq(::Ice::Int from,
		::Ice::Int begin, ::Ice::Int limit, const Ice::Current& current) {
	MyUtil::IntSeq result = locateBuddyRelationHolder(from)->getBuddyList(Apply);
	MCE_DEBUG("BuddyManagerI::getApplySeq -> " << from << " -> "
			<< result.size());
	return result;
}
::MyUtil::Int2IntSeqMap BuddyManagerI::getApplySeqBatch(
		const ::MyUtil::IntSeq& userIds, const Ice::Current& current) {
	::MyUtil::Int2IntSeqMap result;
	for (IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
		IntSeq sublist = getApplySeq(*it, 0, -1, current);
		result.insert(make_pair(*it, sublist));
	}
	return result;
}

void BuddyManagerI::removeFriend(const Relationship& buddy,
		const Ice::Current& current) {
	TaskManager::instance().execute(new RemoveFriendTask(this, buddy, current));
}
void RemoveFriendTask::handle() {
	_bm->processWithState(REMOVEFRIEND,_buddy);
}

OpState BuddyManagerI::removeFriendWithState(const Relationship& buddy,
                const Ice::Current& current) {
    return processWithState(REMOVEFRIEND,buddy );
}

OpState BuddyManagerI::removeFriendHandle(const Relationship& buddy, OpState& result,
                const Ice::Current& current)
{
    switch (result.first) {
        case Friend:
                result.second = NoPath;
                BuddyDatabaseHelper::instance().deleteRelationBi(buddy.from, buddy.to);
                BuddyAdapter::instance().cacheRemoveFriend(buddy,current);
                MCE_INFO("removeFriend success.  from=" << buddy.from << " to="
                                << buddy.to);
                return result;
        case Apply:
        case Applied:
        case Blocked:
        case Block:
        case BiBlock:
                break;
        default:
                MCE_WARN("removeFriend says: 'this should no be seen'. result.first = "
                                << result.first);
        }
        return result;
}

::Ice::Int BuddyManagerI::getFriendCount(::Ice::Int userId,
		const Ice::Current& current) {
	return getFriendSeq(userId, 0, -1, current).size();
}

::MyUtil::IntSeq BuddyManagerI::getFriendSeq(::Ice::Int userId,
		::Ice::Int begin, ::Ice::Int limit, const Ice::Current& current) {
	MyUtil::IntSeq result = locateBuddyRelationHolder(userId)->getBuddyList(Friend); 
	MCE_DEBUG("BuddyManagerI::getFriendSeq -> " << userId << " -> "
			<< result.size());
	return result;
}
::MyUtil::Int2IntSeqMap BuddyManagerI::getFriendSeqBatch(
		const ::MyUtil::IntSeq& userIds, const Ice::Current& current) {
	::MyUtil::Int2IntSeqMap result;
	for (IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
		IntSeq sublist = getFriendSeq(*it, 0, -1, current);
		result.insert(make_pair(*it, sublist));
	}
	return result;
}

void BuddyManagerI::addBlock(const Relationship& block,
		const Ice::Current& current) {
	TaskManager::instance().execute(new AddBlockTask(this, block, current));
}
void AddBlockTask::handle() {
	_bm->processWithState(ADDBLOCK,_block );
}

OpState BuddyManagerI::addBlockWithState(const Relationship& block,
                const Ice::Current& current) {
    return processWithState(ADDBLOCK,block);
}

OpState BuddyManagerI::addBlockHandle( const Relationship& block, OpState& result,
                const Ice::Current& current)
{
    bool biBlock = Cache_NORMALBLOCK;
        switch (result.first) {
        case Apply:
                BuddyDatabaseHelper::instance().deleteBuddyApply(block.from, block.to);
                break;
        case Applied:
                BuddyDatabaseHelper::instance().deleteBuddyApply(block.to, block.from);
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
                biBlock = Cache_BIBLOCK;
                break;
        case Friend:
                BuddyDatabaseHelper::instance().deleteRelationBi(block.from, block.to);
                break;
        default:
                MCE_WARN("AddBlock says: 'this should no be seen'. result.first = "
                                << result.first);
        }

        // Cache
        BuddyDatabaseHelper::instance().insertBlock(block.from, block.to);
        BuddyAdapter::instance().cacheAddBlock(block, biBlock, current);
        MCE_INFO("addBlock success.  from=" << block.from << " to=" << block.to);

        return result;
}

void BuddyManagerI::removeBlock(const Relationship& block,
		const Ice::Current& current) {
	TaskManager::instance().execute(new RemoveBlockTask(this, block, current));
}
void RemoveBlockTask::handle() {
	_bm->processWithState(REMOVEBLOCK,_block);
}

OpState BuddyManagerI::removeBlockWithState(const Relationship& block,
                const Ice::Current& current) {
    return processWithState(REMOVEBLOCK,block);
}

OpState BuddyManagerI::removeBlockHandle( const Relationship& block, OpState& result,
                const Ice::Current& current)
{
    bool biBlock = Cache_NORMALBLOCK;
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
                biBlock = Cache_BIBLOCK;
                break;
        default:
                MCE_WARN("AddBlock says: 'this should no be seen'. result.first = "
                                << result.first);
        }
        BuddyDatabaseHelper::instance().deleteBlock(block.from, block.to);
        BuddyAdapter::instance().cacheRemoveBlock(block, biBlock, current);
        MCE_INFO("removeBlock success.  from=" << block.from << " to=" << block.to);
        return result;
}

::Ice::Int BuddyManagerI::getBlockCount(::Ice::Int from,
		const Ice::Current& current) {
	return getBlockSeq(from, 0, -1, current).size();
}

::MyUtil::IntSeq BuddyManagerI::getBlockSeq(::Ice::Int from,
		::Ice::Int begin, ::Ice::Int limit, const Ice::Current& current) {
	MyUtil::IntSeq result = locateBuddyRelationHolder(from)->getBuddyList(Block); 
	MCE_DEBUG("BuddyManagerI::getBlockSeq -> " << from << " -> "
			<< result.size());
	return result;
}

::MyUtil::Int2IntSeqMap BuddyManagerI::getBlockSeqBatch(
		const ::MyUtil::IntSeq& userIds, const Ice::Current& current) {
	::MyUtil::Int2IntSeqMap result;
	for (IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
		IntSeq sublist = getBlockSeq(*it, 0, -1, current);
		result.insert(make_pair(*it, sublist));
	}
	return result;
}

void BuddyManagerI::reload(::Ice::Int userId, const Ice::Current& current) {
	BuddyAdapter::instance().reload(userId);
}

BuddyDesc BuddyManagerI::translate(uint32_t desc) {
	return BuddyDescHelper::translateDesc(desc);
}

BuddyRelationHolderPtr BuddyManagerI::locateBuddyRelationHolder(uint32_t fromId) {
	return ServiceI::instance().locateObject<BuddyRelationHolderPtr> (
			BUDDY_RELATION, fromId);

}
/* --------------------------------------------------------------------------------------------- */

//void BuddyManagerI::reload(::Ice::Int id, const Ice::Current& current) {
//	removeEntryListHolder(id);
//}

//void BuddyManagerI::addFriend(const Relationship& ship,
//		const Ice::Current& current) {
	//TODO back door has not been completed yet...haha
//}

void BuddyManagerI::cacheAddApply(const Relationship& apply,
		const Ice::Current& current) {
	BuddyRelationHolderPtr holder = NULL;
	if (isLocalCache(current)) {
		holder=findBuddyRelationHolder(apply.from);
		if(holder){
			holder->addApply(apply.to);
			cacheUserCountGuestRequest(apply.to);
		}
		MCE_DEBUG("**CacheAddApply Local**" << " Applyship < from:" <<apply.from<< " to :" << apply.to <<" >");
	} else {
		holder=findBuddyRelationHolder(apply.to);
                if(holder){
                        holder->addApplied(apply.from);
			cacheUserCountGuestRequest(apply.to);
                }
		MCE_DEBUG("**CacheAddApply Remote**" <<" Applyship < from:" <<apply.from<< " to :" << apply.to <<" >" );
	}
}

void BuddyManagerI::cacheAcceptApply(const Relationship& apply,
		const Ice::Current& current) {
	BuddyRelationHolderPtr holder = NULL;
	if (isLocalCache(current)) {
		holder=findBuddyRelationHolder(apply.to);
		if(holder){
			holder->removeApplied(apply.from);
                        holder->addFriend(apply.from);
                        cacheUserCountGuestRequest(apply.to);
                }
                MCE_DEBUG("**CacheAcceptApply Local**" << " Applyship < from:" <<apply.from<< " to :" << apply.to <<" >");
	} else {
		holder=findBuddyRelationHolder(apply.from);
                if(holder){
                        holder->removeApply(apply.to);
                        holder->addFriend(apply.to);
                        cacheUserCountGuestRequest(apply.to);
                }
                MCE_DEBUG("**CacheAcceptApply Remote**" << " Applyship < from:" <<apply.from<< " to :" << apply.to <<" >");
	}
}

void BuddyManagerI::cacheDenyApply(const Relationship& apply,
		const Ice::Current& current) {
	BuddyRelationHolderPtr holder = NULL;
	if (isLocalCache(current)) {
		holder=findBuddyRelationHolder(apply.to);
                if(holder){
                        holder->removeApplied(apply.from);
                        cacheUserCountGuestRequest(apply.to);
                }
                MCE_DEBUG("**CacheDenyApply Local**" << " Applyship < from:" <<apply.from<< " to :" << apply.to <<" >");
	} else {
		holder=findBuddyRelationHolder(apply.from);
                if(holder){
                        holder->removeApply(apply.to);
                        cacheUserCountGuestRequest(apply.to);
                }
                MCE_DEBUG("**CacheDenyApply Remote**" << " Applyship < from:" <<apply.from<< " to :" << apply.to <<" >");
	}
}

void BuddyManagerI::cacheRemoveFriend(const Relationship& ship,
		const Ice::Current& current) {
	BuddyRelationHolderPtr holder = NULL;
	if (isLocalCache(current)) {
		holder=findBuddyRelationHolder(ship.from);
		if(holder){
                        holder->removeFriend(ship.to);
                        cacheUserCountGuestRequest(ship.to);
                }
                MCE_DEBUG("**CacheRemoveFriend Local**" << " Relation < from:" <<ship.from<< " to :" << ship.to <<" >");
	} else {
		holder=findBuddyRelationHolder(ship.to);
                if(holder){
                        holder->removeFriend(ship.from);
                        cacheUserCountGuestRequest(ship.to);
                }
                MCE_DEBUG("**CacheRemoveFriend Local**" << " Relation < from:" <<ship.from<< " to :" << ship.to <<" >");
	}
}

void BuddyManagerI::cacheAddBlock(const Relationship& block,
		const Ice::Current& current) {
	BuddyRelationHolderPtr holder = NULL;
	if (isLocalCache(current)) {
		holder=findBuddyRelationHolder(block.from);
                if(holder){
			holder->removeApply(block.to);
			holder->removeApplied(block.to);
                        holder->removeFriend(block.to);
			if (isBiblock(current)) {
                        	holder->removeBlocked(block.to);
				holder->addBiBlock(block.to);
                	} else {
                        	holder->addBlock(block.to);
                	}
			cacheUserCountGuestRequest(block.to);
                }
		MCE_DEBUG("**CacheAddBlock Local**" << " Block < from:" <<block.from<< " to :" << block.to <<" >");
	} else {
		holder=findBuddyRelationHolder(block.to);
                if(holder){
                        holder->removeApply(block.from);
                        holder->removeApplied(block.from);
                        holder->removeFriend(block.from);
                        if (isBiblock(current)) {
                                holder->removeBlocked(block.from);
                                holder->addBiBlock(block.from);
                        } else {
                                holder->addBlock(block.from);
                        }
			cacheUserCountGuestRequest(block.to);
                }
                MCE_DEBUG("**CacheAddBlock Remote**" << " Block < from:" <<block.from<< " to :" << block.to <<" >");
	}
}

void BuddyManagerI::cacheRemoveBlock(const Relationship& block,
		const Ice::Current& current) {
	BuddyRelationHolderPtr holder = NULL;
	if (isLocalCache(current)) {
		holder = findBuddyRelationHolder(block.from);
		if (holder) {
			if(isBiblock(current)){
				holder->removeBiBlock(block.to);
				holder->addBlocked(block.to);
			}
			else{
				holder->removeBlock(block.to);
			}
		}
		MCE_DEBUG("**CacheRemoveBlock Local**" << " Block < from:" <<block.from<< " to :" << block.to <<" >");
	} else {
		holder = findBuddyRelationHolder(block.to);
                if (holder) {
                        if(isBiblock(current)){
                                holder->removeBiBlock(block.from);
                                holder->addBlocked(block.from);
                        }
                        else{
                                holder->removeBlock(block.from);
                        }
                }
		MCE_DEBUG("**CacheRemoveBlock Remote**" << " Block < from:" <<block.from<< " to :" << block.to <<" >");
	}
}

bool BuddyManagerI::isLocalCache(const Ice::Current& current) {
	return !current.ctx.count(Cache_REMOTE_KEY);
}

bool BuddyManagerI::isBiblock(const Ice::Current& current) {
	return current.ctx.count(Cache_BIBLOCK_KEY);
}

BuddyRelationHolderPtr BuddyManagerI::findBuddyRelationHolder(uint32_t fromId) {
	return ServiceI::instance().findObject<BuddyRelationHolderPtr> (BUDDY_RELATION,
			fromId);
}

void BuddyManagerI::removeBuddyRelationHolder(uint32_t fromId) {
	return ServiceI::instance().removeObject(BUDDY_RELATION, fromId);
}

void BuddyManagerI::cacheUserCountGuestRequest(uint32_t userId) {
	BuddyRelationHolderPtr holder = findBuddyRelationHolder(userId);
	if (holder) {
		uint32_t count = 0;
		MyUtil::IntSeq applied = holder->getBuddyList(Applied);
		count = applied.size();
		if(count > 100){
			count = 100;
		}
		TaskManager::instance().execute(new CacheUserCountGuestRequestTask(userId,
				count));
	}
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

Ice::ObjectPtr BuddyRelationHolderFactory::create(Ice::Int id) {
	MCE_INFO("BuddyRelationHolderFactory::create -> " << id);
	set<int> building[8];
	BuddyRelation duplicates;
	{
		Statement sql;
		sql << "SELECT guest FROM relation_block WHERE  host = '" << id << "'";
		BuddyResultHandler handler(id, building[Block], duplicates.block, "guest");
		QueryRunner(DB_BUDDY_BLOCK, CDbRServer).query(sql, handler);
	}

	{
		Statement sql;
		sql << "SELECT host FROM relation_block WHERE  guest = '" << id << "'";
		BuddyResultHandler handler(id, building[Blocked], duplicates.blocked, "host");
		QueryRunner(DB_BUDDY_BLOCK, CDbRServer).query(sql, handler);
	}

	{
		ostringstream pattern;
		pattern << "relation_" << id % 100;
		Statement sql;
		sql << "SELECT guest FROM relation_" << id % 100 << " WHERE host = '"
				<< id << "' AND disc=0";
		BuddyResultHandler handler(id, building[Friend], duplicates.friends, "guest");
		QueryRunner(DB_BUDDY_FRIEND, CDbRServer, pattern.str()).query(sql, handler);
	}

	{
		Statement sql;
		sql << "SELECT applicant FROM buddyapply WHERE accepter = '" << id
				<< "'";
		BuddyResultHandler handler(id, building[Applied], duplicates.applied, "applicant");
		QueryRunner(DB_BUDDY_APPLY, CDbRServer).query(sql, handler);
	}

	{
		Statement sql;
		sql << "SELECT accepter FROM buddyapply WHERE applicant = '" << id
				<< "'";
		BuddyResultHandler handler(id, building[Apply], duplicates.apply, "accepter");
		QueryRunner(DB_BUDDY_APPLY, CDbRServer).query(sql, handler);
	}
	{
		//build biblock list
		MyUtil::IntSeq tmp(16);
		MyUtil::IntSeq::iterator tmpit;		
		tmpit=set_intersection(building[Block].begin(), building[Block].end(), building[Blocked].begin(), building[Blocked].end(), tmp.begin());
		for(MyUtil::IntSeq::iterator it = tmp.begin(); it != tmpit; it++){
			building[BiBlock].insert(*it);
			building[Block].erase(*it);
			building[Blocked].erase(*it);
		}

	}
		
	// construct result object.
	BuddyRelation buddyrelation;
	if( !building[Block].empty()){
		for (set<int>::iterator it = building[Block].begin(); it != building[Block].end(); ++it) {
			buddyrelation.block.push_back(*it);
		}
	}
	if( !building[Blocked].empty()){
                for (set<int>::iterator it = building[Blocked].begin(); it != building[Blocked].end(); ++it) {
                        buddyrelation.blocked.push_back(*it);
                }
        }
	if( !building[BiBlock].empty()){
                for (set<int>::iterator it = building[BiBlock].begin(); it != building[BiBlock].end(); ++it) {
                        buddyrelation.biblock.push_back(*it);
                }
        }
	if( !building[Friend].empty()){
                for (set<int>::iterator it = building[Friend].begin(); it != building[Friend].end(); ++it) {
                        buddyrelation.friends.push_back(*it);
                }
        }
	if( !building[Apply].empty()){
                for (set<int>::iterator it = building[Apply].begin(); it != building[Apply].end(); ++it) {
                        buddyrelation.apply.push_back(*it);
                }
        }
	if( !building[Applied].empty()){
                for (set<int>::iterator it = building[Applied].begin(); it != building[Applied].end(); ++it) {
                        buddyrelation.applied.push_back(*it);
                }
        }
	BuddyRelationHolderPtr result = new BuddyRelationHolder(id, buddyrelation);

	TaskManager::instance().execute(new DupCleanupTask(id, duplicates));
	MCE_INFO("EntryListHolderFactory::create -> " << id << " done");
	return result;
}

void DupCleanupTask::handle() {
	if( !_dup.block.empty()){
		for (MyUtil::IntSeq::const_iterator it = _dup.block.begin(); it != _dup.block.end(); ++it) {
			handle(Block,*it);
		}
	}
	if(!_dup.blocked.empty()){
		for (MyUtil::IntSeq::const_iterator it = _dup.blocked.begin(); it != _dup.blocked.end(); ++it) {
                	handle(Blocked,*it);
        	}
	}
	if(!_dup.friends.empty()){
		for (MyUtil::IntSeq::const_iterator it = _dup.friends.begin(); it != _dup.friends.end(); ++it) {
                	handle(Friend,*it);
        	}
	}
	if(!_dup.apply.empty()){
		for (MyUtil::IntSeq::const_iterator it = _dup.apply.begin(); it != _dup.block.end(); ++it) {
                	handle(Apply,*it);
        	}
	}
	if(!_dup.applied.empty()){
		for (MyUtil::IntSeq::const_iterator it = _dup.applied.begin(); it != _dup.applied.end(); ++it) {
                	handle(Applied,*it);
        	}
	}
}

void DupCleanupTask::handle(const BuddyDesc desc, const int to) {
	Statement sql;
	switch (desc) {
	case Block:
		sql << "DELETE FROM relation_block WHERE host=" << _userId
                                << " AND guest=" << to << " LIMIT 1";
                QueryRunner(DB_BUDDY_BLOCK, CDbWServer).schedule(sql);
                break;
	case Blocked:
		sql << "DELETE FROM relation_block WHERE host=" << to
                                << " AND guest=" << _userId << " LIMIT 1";
                QueryRunner(DB_BUDDY_BLOCK, CDbWServer).schedule(sql);
                break;

	case Applied:
		sql << "DELETE FROM buddyapply WHERE accepter=" << _userId
				<< " AND applicant=" << to << " LIMIT 1";
		QueryRunner(DB_BUDDY_APPLY, CDbWServer).schedule(sql);
		break;
	case Apply:
		sql << "DELETE FROM buddyapply WHERE accepter=" << to
				<< " AND applicant=" << _userId << " LIMIT 1";
		QueryRunner(DB_BUDDY_APPLY, CDbWServer).schedule(sql);
		return;
	case Friend:
		sql << "DELETE FROM relation_" << _userId % 100 << " WHERE host = '"
				<< _userId << "' AND guest='" << to
				<< "' AND disc=0 LIMIT 1";
		{
			ostringstream pattern;
			pattern << "relation_" << _userId % 100;
			QueryRunner(DB_BUDDY_FRIEND, CDbWServer, pattern.str()).schedule(sql);
		}
		return;
	default:
		MCE_WARN("Cleanup : " << _userId << " " << to);
	}
}

void CacheUserCountGuestRequestTask::handle() {
	::xce::usercount::adapter::UserCountAdapter::instance().set(_userId, ::xce::usercount::CGuestRequest,
			_count);
}
