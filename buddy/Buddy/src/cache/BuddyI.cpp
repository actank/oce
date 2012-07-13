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
#include "../../../BuddyEntry/src/BuddyCreate.h"
#include "../../../../OceCxxAdapter/src/BuddyRelationReplicaAdapter.h"

using namespace MyUtil;
using namespace xce::buddy;
//using namespace xce::buddy::adapter;
using namespace com::xiaonei::xce;
using namespace ::xce::log;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&BuddyManagerI::instance(), service.createIdentity(
			"M", ""));
	service.registerObjectCache(BUDDY_RELATION, "BR", new SyncServantFactory(
			new BuddyRelationHoldersICacheFactory));
	
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Buddy.Mod");
        string endpoint = service.getCommunicator()->getProperties()->getProperty("Buddy.Endpoint");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Buddy.Interval", 5);

        // 向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerBuddy",
                        &BuddyManagerI::instance(), mod, interval);
	BuddyManagerPrx localprx = BuddyManagerPrx::uncheckedCast(ServerStateSubscriber::instance().getProxySelf());
	BuddyAdapter::instance().setLocalPrx(localprx);	
	
	TaskManager::instance().config(TASK_LEVEL_CACHEUSERCOUNT, MyUtil::ThreadPoolConfig(1, 10));
	ActiveTrack::instance().initialize();
}

string BuddyManagerI::translateBuddyState(const BuddyState& state) const {
	
	switch(state)
	{
                case NULLSTATE:
                    return "Null State";
                case ADDAPPLY:
                    return "Add Apply";
                case ACCEPTAPPLY:
                    return "Accept Apply";
                case DENYAPPLY: 
                    return "Deny Apply";
                case REMOVEFRIEND:
                    return "Remove Friend";
                case ADDBLOCK:
                    return "Add Block";
                case REMOVEBLOCK:
                    return "Remove Block";
                default:
                    return "Null State";
	}
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
                   	BuddyAdapter::instance().cacheAddApply(relation, *applyInfo, current.ctx);
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
                   	BuddyAdapter::instance().cacheAddBlock(relation, Cache_NORMALBLOCK,current.ctx);
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


OpState BuddyManagerI::processHandler(const BuddyState& state, const Relationship& relation,
                const ApplyInfo* applyInfo, const Ice::Current& current)
{
	MCE_DEBUG( "[BuddyManagerI::processHandler] "<<translateBuddyState(state)<< " to=" << relation.to << " from="
                        << relation.from);
	OpState result;
	result.first = NoPath;
	result.second = NoPath;
    
	//作自身判断
	if(relation.from == relation.to){
        	//允许对已存在自身关系作操作，不允许新加
		if( ADDAPPLY == state || ADDBLOCK == state){
			result.second = Self;
			return result;
		}	
   	}
    
    	int id = 0;    
    	switch(state) {
        	case ADDAPPLY:
                case REMOVEFRIEND:
                case ADDBLOCK:
                case REMOVEBLOCK:
                        id = relation.from;
                        break;
                case ACCEPTAPPLY:
                case DENYAPPLY:
                        id = relation.to;
                        break;
                default:
                        break;
    	}

    	specialHandle(state,relation,current);
	
    	int i = locateBuddyRelation(id)->getCount(relation.from,result.first);
    	if ( 0 == i) { // empty() check must be the first checking.
        	return itemEmptyCheck(state,relation,result,applyInfo,current);
    	}

    	if ( i > 1) {
        	return itemOverflowCheck(state,relation,result);
    	}

    	result.second = result.first;
    	return processHandleWithState(state, relation, result, applyInfo, current);
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
            		MCE_WARN("unsupportd state in processHandler");
    	}
    	return result;
}

void BuddyManagerI::specialHandle(const BuddyState& state, const Relationship& relation, 
		const Ice::Current& current){
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

void BuddyManagerI::addApplyBatch(const ApplyshipMap& applies, const ApplyInfoMap& infos, 
		const Ice::Current& current) {
	MCE_DEBUG("addApplyBatch: size = " << applies.size());
	for (ApplyshipMap::const_iterator applyIt = applies.begin(); 
			applyIt != applies.end(); ++applyIt) {
		
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
	TaskManager::instance().execute(new AddApplyTask( apply, info, current));
}

void AddApplyTask::handle() {
	BuddyManagerI::instance().processHandler(ADDAPPLY,_apply, &_info, _current);
}

OpState BuddyManagerI::addApplyWithState(const Relationship& apply, const ApplyInfo& info,
                const Ice::Current& current) {
    return processHandler(ADDAPPLY,apply,&info, current);
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
                	BuddyAdapter::instance().cacheAcceptApply(confirm, current.ctx);
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
                                current.ctx);
                	return result;
        	case Apply:
                	// no needs to apply two times, return.
                	MCE_DEBUG("Already apply");
                	//return result;
			// do it again, table buddyapply is divided into buddyapply_applicant and buddyapply_accepter
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
        BuddyAdapter::instance().cacheAddApply(apply,applyInfo,current.ctx);
        return result;
}

OpStateMap BuddyManagerI::addApplyWithStateBatch(
		const ApplyshipMap& applies, const ApplyInfoMap& infos,
		const Ice::Current& current) {
	MCE_DEBUG("addApplyWithStateBatch: size = " << applies.size());
	OpStateMap opstates;
	for (ApplyshipMap::const_iterator applyIt = applies.begin(); 
			applyIt!= applies.end(); ++applyIt) {
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
	TaskManager::instance().execute(new AcceptApplyTask( apply, current));
}
void AcceptApplyTask::handle() {
	BuddyManagerI::instance().processHandler(ACCEPTAPPLY,_apply);
}

OpState BuddyManagerI::acceptApplyWithState(const Relationship& apply,
                const Ice::Current& current) {
    	return processHandler(ACCEPTAPPLY,apply);
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
              		BuddyAdapter::instance().cacheAcceptApply(apply, current.ctx);
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
	TaskManager::instance().execute(new DenyApplyTask( apply, current));
}
void DenyApplyTask::handle() {
	BuddyManagerI::instance().processHandler(DENYAPPLY,_apply);
}

OpState BuddyManagerI::denyApplyWithState(const Relationship& apply,
                const Ice::Current& current) {
    	return processHandler(DENYAPPLY,apply);
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
                	BuddyAdapter::instance().cacheDenyApply(apply, current.ctx);
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
	MyUtil::IntSeq result;
	result = locateBuddyRelation(from)->getBuddyList(Applied); 
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
        MyUtil::IntSeq result;
	result = locateBuddyRelation(from)->getBuddyList(Apply);
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
	TaskManager::instance().execute(new RemoveFriendTask( buddy, current));
}
void RemoveFriendTask::handle() {
	BuddyManagerI::instance().processHandler(REMOVEFRIEND,_buddy);
}

OpState BuddyManagerI::removeFriendWithState(const Relationship& buddy,
                const Ice::Current& current) {
    	return processHandler(REMOVEFRIEND,buddy );
}

OpState BuddyManagerI::removeFriendHandle(const Relationship& buddy, OpState& result,
                const Ice::Current& current)
{
    	switch (result.first) {
        	case Friend:
                	result.second = NoPath;
                	BuddyDatabaseHelper::instance().deleteRelationBi(buddy.from, buddy.to);
                	BuddyAdapter::instance().cacheRemoveFriend(buddy,current.ctx);
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
	MyUtil::IntSeq result;
	result = locateBuddyRelation(userId)->getBuddyList(Friend); 
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
	TaskManager::instance().execute(new AddBlockTask(block, current));
}
void AddBlockTask::handle() {
	BuddyManagerI::instance().processHandler(ADDBLOCK,_block );
}

OpState BuddyManagerI::addBlockWithState(const Relationship& block,
                const Ice::Current& current) {
    	return processHandler(ADDBLOCK,block);
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
        BuddyAdapter::instance().cacheAddBlock(block, biBlock, current.ctx);
        MCE_INFO("addBlock success.  from=" << block.from << " to=" << block.to);

        return result;
}

void BuddyManagerI::removeBlock(const Relationship& block,
		const Ice::Current& current) {
	TaskManager::instance().execute(new RemoveBlockTask( block, current));
}
void RemoveBlockTask::handle() {
	BuddyManagerI::instance().processHandler(REMOVEBLOCK,_block);
}

OpState BuddyManagerI::removeBlockWithState(const Relationship& block,
                const Ice::Current& current) {
    	return processHandler(REMOVEBLOCK,block);
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
        BuddyAdapter::instance().cacheRemoveBlock(block, biBlock, current.ctx);
        MCE_INFO("removeBlock success.  from=" << block.from << " to=" << block.to);
        return result;
}

::Ice::Int BuddyManagerI::getBlockCount(::Ice::Int from,
		const Ice::Current& current) {
	return getBlockSeq(from, 0, -1, current).size();
}

::MyUtil::IntSeq BuddyManagerI::getBlockSeq(::Ice::Int from,
		::Ice::Int begin, ::Ice::Int limit, const Ice::Current& current) {
	MyUtil::IntSeq result;
	result = locateBuddyRelation(from)->getBuddyList(Block); 
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

/*void BuddyManagerI::reload(::Ice::Int userId, const Ice::Current& current) {
	BuddyAdapter::instance().reload(userId);
}*/

BuddyDesc BuddyManagerI::translate(uint32_t desc) const {
	return BuddyDescHelper::translateDesc(desc);
}

BuddyRelationHoldersIPtr BuddyManagerI::locateBuddyRelation(uint32_t fromId) {
	return ServiceI::instance().locateObject<BuddyRelationHoldersIPtr> (
			BUDDY_RELATION, fromId);

}
/* --------------------------------------------------------------------------------------------- */

//void BuddyManagerI::reload(::Ice::Int id, const Ice::Current& current) {
//	removeEntryListHolder(id);
//}
/*
void BuddyManagerI::addFriend(const Relationship& relation,
		const Ice::Current& current) {
	//TODO back door has not been completed yet...haha
 	TaskManager::instance().execute(new AddFriendTask( relation,  current));
}

void AddFriendTask::handle() {
        BuddyManagerI::instance().processHandler(ADDFRIEND, _relation, _current);
}

OpState BuddyManagerI::addFriendWithState(const Relationship& relation, 
                const Ice::Current& current) {
    	return processHandler(ADDFRIEND,relation, current);
}

OpState BuddyManagerI::addFriendHandle(const Relationship& relation, OpState& result,
                const Ice::Current& current)
{
        switch (result.first) {
                case Applied:*/
                        /* 
                        * Done at the very beginning.
                        BuddyDatabaseHelper::instance().deleteBuddyApply(apply.from,
                        apply.accepter);
                        */
                        /*BuddyDatabaseHelper::instance().insertRelationBi(apply.from,
                                apply.to);
                        result.second = Friend;
                        BuddyAdapter::instance().cacheAcceptApply(apply, current.ctx);
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
*/	
BuddyRelationHoldersPtr BuddyManagerI::cacheAddApply(const Relationship& apply, const StateRecorder& checksr,
		const Ice::Current& current) {
	BuddyRelationHoldersPtr brhp = NULL;
	if (isLocalCache(current)) {
		BuddyRelationHoldersIPtr holderI=findBuddyRelation(apply.from);
		if(holderI){
			holderI->addApply(apply.to);
			cacheUserCountGuestRequest(apply.to);
			assignStateRecorder(holderI,apply,ADDAPPLY,true,checksr,current);
			brhp = holderI->getBuddyRelationHolders();
		}else{
			MCE_INFO("**CacheAddApply Local** CHECKERROR , reason: "<<apply.from <<" not in object cache");
			brhp = new BuddyRelationHolders;
			brhp->hsr.lastid = apply.from;
			brhp->hsr.laststate = CHECKERROR;
		}
		MCE_DEBUG("**CacheAddApply Local**" << " Applyship < from:" <<apply.from<< " to :" << apply.to <<" >");
	} else {
		BuddyRelationHoldersIPtr holderI=findBuddyRelation(apply.to);
                if(holderI){
                        holderI->addApplied(apply.from);
			cacheUserCountGuestRequest(apply.to);
			assignStateRecorder(holderI,apply,ADDAPPLY,false,checksr,current);
                	brhp = holderI->getBuddyRelationHolders();
		}else{
			MCE_INFO("**CacheAddApply Remote** CHECKERROR, reason: "<< apply.to <<" not in object cache");
			brhp = new BuddyRelationHolders;
			brhp->hsr.lastid = apply.to;
                        brhp->hsr.laststate = CHECKERROR;
		}
		MCE_DEBUG("**CacheAddApply Remote**" <<" Applyship < from:" <<apply.from<< " to :" << apply.to <<" >" );
	}
	return brhp; 
}

BuddyRelationHoldersPtr BuddyManagerI::cacheAcceptApply(const Relationship& apply, const StateRecorder& checksr,
		const Ice::Current& current) {
	BuddyRelationHoldersPtr brhp = NULL;
	if (isLocalCache(current)) {
		BuddyRelationHoldersIPtr holderI=findBuddyRelation(apply.to);
		if(holderI){
			holderI->removeApplied(apply.from);
                        holderI->addFriend(apply.from);
                        cacheUserCountGuestRequest(apply.to);
			assignStateRecorder(holderI,apply,ACCEPTAPPLY,true,checksr,current);
                	brhp = holderI->getBuddyRelationHolders();
		}else{
			MCE_INFO("**CacheAcceptApply Local** CHECKERROR");
			brhp = new BuddyRelationHolders;
                        brhp->hsr.lastid = apply.to;
                        brhp->hsr.laststate = CHECKERROR;
		}
                MCE_DEBUG("**CacheAcceptApply Local**" << " Applyship < from:" <<apply.from<< " to :" << apply.to <<" >");
	} else {
                BuddyRelationHoldersIPtr holderI=findBuddyRelation(apply.from);	
		if(holderI){
			holderI->removeApply(apply.to);
                        holderI->addFriend(apply.to);
                        cacheUserCountGuestRequest(apply.to);
			assignStateRecorder(holderI,apply,ACCEPTAPPLY,false,checksr,current);
        		brhp = holderI->getBuddyRelationHolders();      
		}else{
			MCE_INFO("**CacheAcceptApply Remote** CHECKERROR");
			brhp = new BuddyRelationHolders;
			brhp->hsr.lastid = apply.from;
                        brhp->hsr.laststate = CHECKERROR;
		}
                MCE_DEBUG("**CacheAcceptApply Remote**" << " Applyship < from:" <<apply.from<< " to :" << apply.to <<" >");
	}
	return brhp;
}

BuddyRelationHoldersPtr BuddyManagerI::cacheDenyApply(const Relationship& apply, const StateRecorder& checksr,
		const Ice::Current& current) {
	BuddyRelationHoldersPtr brhp = NULL;
	if (isLocalCache(current)) {
                BuddyRelationHoldersIPtr holderI=findBuddyRelation(apply.to);
		if(holderI){
                        holderI->removeApplied(apply.from);
                        cacheUserCountGuestRequest(apply.to);
			assignStateRecorder(holderI,apply,DENYAPPLY,true,checksr,current);
			brhp = holderI->getBuddyRelationHolders();
		}else{
			brhp = new BuddyRelationHolders;
			brhp->hsr.lastid = apply.to;
                        brhp->hsr.laststate = CHECKERROR;
		}
                MCE_DEBUG("**CacheDenyApply Local**" << " Applyship < from:" <<apply.from<< " to :" << apply.to <<" >");
	} else {
                BuddyRelationHoldersIPtr holderI=findBuddyRelation(apply.from);	
		if(holderI){
                        holderI->removeApply(apply.to);
                        cacheUserCountGuestRequest(apply.to);
			assignStateRecorder(holderI,apply,DENYAPPLY,false,checksr,current);
			brhp = holderI->getBuddyRelationHolders();
		}else{
			brhp = new BuddyRelationHolders;
			brhp->hsr.lastid = apply.from;
                        brhp->hsr.laststate = CHECKERROR;
		}
                MCE_DEBUG("**CacheDenyApply Remote**" << " Applyship < from:" <<apply.from<< " to :" << apply.to <<" >");
	}
	return brhp;
}

BuddyRelationHoldersPtr BuddyManagerI::cacheRemoveFriend(const Relationship& ship, const StateRecorder& checksr,
		const Ice::Current& current) {
	BuddyRelationHoldersPtr brhp = NULL;
	if (isLocalCache(current)) {
		BuddyRelationHoldersIPtr holderI=findBuddyRelation(ship.from);
		if(holderI){
                        holderI->removeFriend(ship.to);
                        cacheUserCountGuestRequest(ship.to);
			assignStateRecorder(holderI,ship,REMOVEFRIEND,true,checksr,current);
			brhp = holderI->getBuddyRelationHolders();
		}else{
			brhp = new BuddyRelationHolders;
			brhp->hsr.lastid = ship.from;
                        brhp->hsr.laststate = CHECKERROR;
		}
                MCE_DEBUG("**CacheRemoveFriend Local**" << " Relation < from:" <<ship.from<< " to :" << ship.to <<" >");
	} else {
                BuddyRelationHoldersIPtr holderI=findBuddyRelation(ship.to);
		if(holderI){
                        holderI->removeFriend(ship.from);
                        cacheUserCountGuestRequest(ship.to);
			assignStateRecorder(holderI,ship,REMOVEFRIEND,false,checksr,current);
			brhp = holderI->getBuddyRelationHolders();
		}else{
			brhp = new BuddyRelationHolders;
			brhp->hsr.lastid = ship.to;
                        brhp->hsr.laststate = CHECKERROR;
		}
                MCE_DEBUG("**CacheRemoveFriend Local**" << " Relation < from:" <<ship.from<< " to :" << ship.to <<" >");
	}
	return brhp;
}

BuddyRelationHoldersPtr BuddyManagerI::cacheAddBlock(const Relationship& block, const StateRecorder& checksr,
		const Ice::Current& current) { 
	BuddyRelationHoldersPtr brhp = NULL;
	if (isLocalCache(current)) {
                BuddyRelationHoldersIPtr holderI=findBuddyRelation(block.from);
		if(holderI){
			holderI->removeApply(block.to);
			holderI->removeApplied(block.to);
                        holderI->removeFriend(block.to);
			if (isBiblock(current)) {
                        	holderI->removeBlocked(block.to);
				holderI->addBiBlock(block.to);
                	} else {
                        	holderI->addBlock(block.to);
                	}
			cacheUserCountGuestRequest(block.to);
			assignStateRecorder(holderI,block,ADDBLOCK,true,checksr,current);
			brhp = holderI->getBuddyRelationHolders();
		}else{
			brhp = new BuddyRelationHolders;
			brhp->hsr.lastid = block.from;
                        brhp->hsr.laststate = CHECKERROR;
		}
		MCE_DEBUG("**CacheAddBlock Local**" << " Block < from:" <<block.from<< " to :" << block.to <<" >");
	} else {
                BuddyRelationHoldersIPtr holderI=findBuddyRelation(block.to);
		if(holderI){
                        holderI->removeApply(block.from);
                        holderI->removeApplied(block.from);
                        holderI->removeFriend(block.from);
                        if (isBiblock(current)) {
                                holderI->removeBlocked(block.from);
                                holderI->addBiBlock(block.from);
                        } else {
                                holderI->addBlock(block.from);
                        }
			cacheUserCountGuestRequest(block.to);
			assignStateRecorder(holderI,block,ADDBLOCK,false,checksr,current);
			brhp = holderI->getBuddyRelationHolders();
		}else{
			brhp = new BuddyRelationHolders;
			brhp->hsr.lastid = block.to;
                        brhp->hsr.laststate = CHECKERROR;
		}
                MCE_DEBUG("**CacheAddBlock Remote**" << " Block < from:" <<block.from<< " to :" << block.to <<" >");
	}
	return brhp;
}

BuddyRelationHoldersPtr BuddyManagerI::cacheRemoveBlock(const Relationship& block, const StateRecorder& checksr,
		const Ice::Current& current) {
	BuddyRelationHoldersPtr brhp = NULL;
	if (isLocalCache(current)) {
		BuddyRelationHoldersIPtr holderI=findBuddyRelation(block.from);
		if (holderI) {
			if(isBiblock(current)){
				holderI->removeBiBlock(block.to);
				holderI->addBlocked(block.to);
			}
			else{
				holderI->removeBlock(block.to);
			}
			assignStateRecorder(holderI,block,REMOVEBLOCK,true,checksr,current);
			brhp = holderI->getBuddyRelationHolders();
		}else{
			brhp = new BuddyRelationHolders;
			brhp->hsr.lastid = block.from;
                        brhp->hsr.laststate = CHECKERROR;
		}
		MCE_DEBUG("**CacheRemoveBlock Local**" << " Block < from:" <<block.from<< " to :" << block.to <<" >");
	} else {
                BuddyRelationHoldersIPtr holderI=findBuddyRelation(block.to);
		if (holderI) {
                        if(isBiblock(current)){
                                holderI->removeBiBlock(block.from);
                                holderI->addBlocked(block.from);
                        }
                        else{
                                holderI->removeBlock(block.from);
                        }
			assignStateRecorder(holderI,block,REMOVEBLOCK,false,checksr,current);
			brhp = holderI->getBuddyRelationHolders();
		}else{
			brhp = new BuddyRelationHolders;
			brhp->hsr.lastid = block.to;
                        brhp->hsr.laststate = CHECKERROR;
		}
		MCE_DEBUG("**CacheRemoveBlock Remote**" << " Block < from:" <<block.from<< " to :" << block.to <<" >");
	}
	return brhp;
}

bool BuddyManagerI::isLocalCache(const Ice::Current& current) {
	return !current.ctx.count(Cache_REMOTE_KEY);
}

bool BuddyManagerI::isBiblock(const Ice::Current& current) {
	return current.ctx.count(Cache_BIBLOCK_KEY);
}

bool BuddyManagerI::isMainCache(const Ice::Current& current) {
        return !current.ctx.count(Cache_MAIN_KEY);
}

BuddyRelationHoldersIPtr BuddyManagerI::findBuddyRelation(uint32_t fromId) {
	return ServiceI::instance().findObject<BuddyRelationHoldersIPtr> (BUDDY_RELATION,
			fromId);
}

void BuddyManagerI::removeBuddyRelation( const Ice::Int fromId, const Ice::Current& current) {
	ServiceI::instance().removeObject(BUDDY_RELATION, fromId);
}

void BuddyManagerI::load( Ice::Int fromId, const Ice::ObjectPtr& objptr, 
		const Ice::Current& current) {
        MCE_INFO( "[BuddyManagerI::load] userId=" << fromId );
	ServiceI::instance().addObject(objptr, BUDDY_RELATION,
                        fromId);
	//ServiceI::instance().locateObject(objptr, BUDDY_RELATION,
          //              fromId);
	
}

void BuddyManagerI::cacheUserCountGuestRequest(uint32_t userId) {
	uint32_t count = 0;
	MyUtil::IntSeq applied = locateBuddyRelation(userId)->getBuddyList(Applied);
	count = applied.size();
	if(count > 100){
		count = 100;
	}
	TaskManager::instance().execute(new CacheUserCountGuestRequestTask(userId,
			count));
}

void BuddyManagerI::assignStateRecorder( BuddyRelationHoldersIPtr& holderI,const Relationship& ship, 
			const BuddyState state, const bool local, /*StateRecorder& oldsr, */
	                const StateRecorder& checksr,const Ice::Current& current){
	StateRecorder newsr;
	switch(state)
	{
         	case ADDAPPLY:
		case REMOVEFRIEND:
		case ADDBLOCK:
		case REMOVEBLOCK:
                   	newsr.lastid = local ? ship.to : ship.from;
                    	break;
           	case ACCEPTAPPLY:
		case DENYAPPLY:
                    	newsr.lastid = local ? ship.from : ship.to;
                       	break;
              	default:
                   	break;
      	}

	if(isMainCache(current)){
        	//oldsr= holderI.getStateRecorder();
        	newsr.laststate = state;
        	holderI->setStateRecorder(newsr);
		const_cast<StateRecorder&>(checksr) = holderI->getStateRecorder();
        }
        else{
               	if( holderI->checkStateRecorder(checksr)){
                	//oldsr= holderI.getStateRecorder();
                        newsr.laststate = state;
                        holderI->setStateRecorder(newsr);
                }
                else{
			newsr.laststate = CHECKERROR;
			holderI->setStateRecorder(newsr);
			//oldsr.lastid = newsr.lastid;
                        //oldsr.laststate = CHECKERROR;
                }
        }
}

bool BuddyManagerI::isValid(const Ice::Current&) {
        MCE_INFO( "[BuddyManagerI::isValid] " << _isValid );
        IceUtil::RWRecMutex::RLock lock(_validMutex);
        return _isValid;
}

void BuddyManagerI::setValid(bool valid,const Ice::Current&) {
        MCE_INFO( "[BuddyManagerI::setValid] " << valid );
        IceUtil::RWRecMutex::WLock lock(_validMutex);
        _isValid = valid;
        if( valid ){
                ServerStateSubscriber::instance().setStatus(1);
        } else{
                ServerStateSubscriber::instance().setStatus(0);
        }
}

void BuddyManagerI::setData(const ::MyUtil::ObjectResultPtr& data, const Ice::Current& current) {
        MCE_INFO( "[BuddyManagerI::setData] size=" << data->data.size());
        TaskManager::instance().execute(new FillTask(data));
}

BuddyRelationPtr BuddyManagerI::getBuddyRelation(const ::Ice::Int userId, const Ice::Current& current){
	return locateBuddyRelation(userId)->getBuddyRelationHolders()->hbr;
}

Int2BuddyRelationMap BuddyManagerI::getBuddyRelationBatch(const ::MyUtil::IntSeq& userIds, const Ice::Current& current){
	Int2BuddyRelationMap result;
        for (IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
                BuddyRelationPtr brp = getBuddyRelation(*it, current);
                result.insert(make_pair(*it, brp));
        }
        return result;
}

::BuddyDesc BuddyManagerI::getRelation(const Relationship& relation,
                const Ice::Current& current) {
        if (relation.from == relation.to) {
                return ::Self;
        }
	return xce::buddy::adapter::BuddyRelationCacheAdapter::instance().getRelation(relation);
	//return NoPath;
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
	ostringstream tableAccepter;
        tableAccepter << TABLE_APPLY_ACCEPTER << abs(accepter) % TABLE_ACCEPTER_COUNT;
	ostringstream tableApplicant;
        tableApplicant << TABLE_APPLY_APPLICANT << abs(applicant) % TABLE_APPLICANT_COUNT;
	
	QueryRunner(DB_BUDDY_ACCEPTER, CDbWServer,tableAccepter.str()).execute(Statement::buildReplace(
                	        tableAccepter.str(), props));
	QueryRunner(DB_BUDDY_APPLICANT, CDbWServer,tableApplicant.str()).execute(Statement::buildReplace(
				tableApplicant.str(), props));
	return true;
}

bool BuddyDatabaseHelper::deleteBuddyApply(int applicant, int accepter) {
	MCE_INFO("deleteBuddyApply applicant=" << applicant << " accepter="
			<< accepter);
	MyUtil::Str2StrMap filter;
	filter["APPLICANT"] = boost::lexical_cast<string>(applicant);
	filter["ACCEPTER"] = boost::lexical_cast<string>(accepter);
	
	ostringstream tableAccepter;
        tableAccepter << TABLE_APPLY_ACCEPTER << abs(accepter) % TABLE_ACCEPTER_COUNT;
	ostringstream tableApplicant;
        tableApplicant << TABLE_APPLY_APPLICANT << abs(applicant) % TABLE_APPLICANT_COUNT;
		
	QueryRunner(DB_BUDDY_ACCEPTER, CDbWServer,tableAccepter.str()).execute(Statement::buildDelete(
				tableAccepter.str(), filter));
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

/*****************************************************************/
void CacheUserCountGuestRequestTask::handle() {
	::xce::usercount::adapter::UserCountAdapter::instance().set(_userId, ::xce::usercount::CGuestRequest,
			_count);
}

void FillTask::handle() {
        ServiceI::instance().getObjectCache()->addObjects(BUDDY_RELATION, _buddyData->data);
	MCE_INFO("FillTask done. Task size / ObjectCache size : " << _buddyData->data.size() << 
		" / " << ServiceI::instance().getObjectCache()->locateCategory(BUDDY_RELATION)->size());
}

