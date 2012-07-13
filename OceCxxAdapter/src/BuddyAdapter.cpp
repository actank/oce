#include "BuddyAdapter.h"
#include "BuddyLogicAdapter.h"
#include "../../buddy/BuddyEntry/src/BuddyCreate.h"

namespace xce {
namespace buddy {

BuddyManagerPrx BuddyAdapter::getManager(int timeout) {
        if(timeout>0) {
		return BuddyManagerPrx::uncheckedCast(_localprx->ice_twoway()->ice_timeout(timeout));
	}
	return BuddyManagerPrx::uncheckedCast(_localprx->ice_twoway());
}

BuddyManagerPrx BuddyAdapter::getManagerOneway() {
        return BuddyManagerPrx::uncheckedCast(_localprx->ice_oneway());
}

void BuddyAdapter::addApply(const Relationship& apply,
		const ApplyInfo& info) {
	BuddyManagerPrx prx = getManagerOneway();
	if(prx){
		prx->addApply(apply, info);
	}
}

OpState BuddyAdapter::addApplyWithState(const Relationship& apply,
		const ApplyInfo& info) {
	BuddyManagerPrx prx = getManager();
	if(prx){
		return prx->addApplyWithState(apply, info);
	}
	OpState op;
	op.first = NoPath;
        op.second = NoPath;
	return op;
}

void BuddyAdapter::acceptApply(const Relationship& apply) {
	BuddyManagerPrx prx = getManagerOneway();
	if(prx){
		prx->acceptApply(apply);
	}
}

OpState BuddyAdapter::acceptApplyWithState(const Relationship& apply) {
	BuddyManagerPrx prx = getManager();
        if(prx){
		return prx->acceptApplyWithState(apply);
	}
	OpState op;
	op.first = NoPath;
        op.second = NoPath;
        return op;
}

void BuddyAdapter::denyApply(const Relationship& apply) {
	BuddyManagerPrx prx = getManagerOneway();
        if(prx){
		prx->denyApply(apply);
	}
}

OpState BuddyAdapter::denyApplyWithState(const Relationship& apply) {
	BuddyManagerPrx prx = getManager();
        if(prx){
		return prx->denyApplyWithState(apply);
	}
	OpState op;
	op.first = NoPath;
	op.second = NoPath;
        return op;
}

void BuddyAdapter::denyAllApplied(::Ice::Int userId) {
	BuddyManagerPrx prx = getManagerOneway();
        if(prx){
		prx->denyAllApplied(userId);
	}
}
::Ice::Int BuddyAdapter::getAppliedCount(::Ice::Int userId) {
	BuddyManagerPrx prx = getManager();
	if(prx){
		return prx->getAppliedCount(userId);
	}
	return 0;
}
::MyUtil::IntSeq BuddyAdapter::getAppliedSeq(::Ice::Int userId,
		::Ice::Int begin, ::Ice::Int limit) {
	BuddyManagerPrx prx = getManager();
        if(prx){
		return prx->getAppliedSeq(userId, begin, limit);
	}
	::MyUtil::IntSeq seq;
	return seq;
}
::Ice::Int BuddyAdapter::getApplyCount(::Ice::Int userId) {
	BuddyManagerPrx prx = getManager();
        if(prx){
		return prx->getApplyCount(userId);
	}
	return 0;
}

::MyUtil::IntSeq BuddyAdapter::getApplySeq(::Ice::Int userId,
		::Ice::Int begin, ::Ice::Int limit) {
	BuddyManagerPrx prx = getManager();
        if(prx){
		return prx->getApplySeq(userId, begin, limit);
	}
	::MyUtil::IntSeq seq;
	return seq;
}

void BuddyAdapter::removeFriend(const Relationship& buddy) {
	BuddyManagerPrx prx = getManagerOneway();
	if(prx){
		prx->removeFriend(buddy);
	}
}
OpState BuddyAdapter::removeFriendWithState(const Relationship& buddy) {
	BuddyManagerPrx prx = getManager();
        if(prx){
		return prx->removeFriendWithState(buddy);
	}
	OpState op;
	op.first = NoPath;
        op.second = NoPath;
	return op;
}

::Ice::Int BuddyAdapter::getFriendCount(::Ice::Int userId) {
	BuddyManagerPrx prx = getManager();
        if(prx){
		return prx->getFriendCount(userId);
	}
	return 0;
}

::MyUtil::IntSeq BuddyAdapter::getFriendSeq(::Ice::Int userId,
		::Ice::Int begin, ::Ice::Int limit, int timeout) {
	//std::cout << "getFriendSeq " << userId << std::endl;
	//try {
	BuddyManagerPrx prx = getManager(timeout);
        if(prx){
		return prx->getFriendSeq(userId, begin, limit);
	}
	::MyUtil::IntSeq seq;
	return seq;	
	/*} catch(std::exception& e) {
		std::cout << "BuddyAdapter::getFriendSeq [std::exception] userId="<< userId << " begin=" << begin << " limit=" << limit << e.what() << std::endl;
		return ::MyUtil::IntSeq();
	} catch(...) {
		std::cout << "BuddyAdapter::getFriendSeq [...] userId="<< userId << " begin=" << begin << " limit=" << limit << std::endl;
		return ::MyUtil::IntSeq();
	}*/
	//std::cout << "getFriendSeq done " << userId << std::endl;
}

void BuddyAdapter::addBlock(const Relationship& block) {
	BuddyManagerPrx prx = getManagerOneway();
	if(prx){
		prx->addBlock(block);
	}
}

OpState BuddyAdapter::addBlockWithState(const Relationship& block) {
	BuddyManagerPrx prx = getManager();
	if(prx){
		return prx->addBlockWithState(block);
	}
	OpState op;
	op.first = NoPath;
        op.second = NoPath;
	return op;
}

void BuddyAdapter::removeBlock(const Relationship& block) {
	BuddyManagerPrx prx = getManagerOneway();
        if(prx){
		prx->removeBlock(block);
	}
}

OpState BuddyAdapter::removeBlockWithState(const Relationship& block) {
	BuddyManagerPrx prx = getManager();
        if(prx){
		return prx->removeBlockWithState(block);
	}
	OpState op;
	op.first = NoPath;
        op.second = NoPath;
        return op;
}

void BuddyAdapter::changeBlockList(int from,
		const MyUtil::IntSeq& blockIds) {
	MyUtil::IntSeq oldBlockIds = getBlockSeq(from, 0, -1);
	for (size_t i = 0; i < oldBlockIds.size(); ++i) {
		Relationship b;
		b.from = from;
		b.to = oldBlockIds.at(i);
		removeBlock(b);
	}
	for (size_t i = 0; i < blockIds.size(); ++i) {
		Relationship b;
		b.from = from;
		b.to = blockIds.at(i);
		addBlock(b);
	}
}

::Ice::Int BuddyAdapter::getBlockCount(::Ice::Int userId) {
	BuddyManagerPrx prx = getManager();
        if(prx){
		return prx->getBlockCount(userId);
	}
	return 0;
}

::MyUtil::IntSeq BuddyAdapter::getBlockSeq(::Ice::Int userId,
		::Ice::Int begin, ::Ice::Int limit, int timeout) {
	BuddyManagerPrx prx = getManager(timeout);
        if(prx){
		return prx->getBlockSeq(userId, begin, limit);
	}
	::MyUtil::IntSeq seq;
	return seq;
}

BuddyDesc BuddyAdapter::getRelation(const Relationship& relation,
		int timeout) {
	//return getManager(timeout)->getRelation(relation);
	//do in BuddyRelationCache
	return NoPath;
}

void BuddyAdapter::reload(Ice::Int userId){
	remove(userId);
	BuddyLoaderAdapter::instance().load(userId);
}

void BuddyAdapter::load(Ice::Int userId, Ice::ObjectPtr objptr) {
	MCE_DEBUG( "[BuddyAdapter::load] id= " << userId );
	TaskManager::instance().execute(new LoadTask(userId, objptr));
}

void LoadTask::handle() {
        vector<BuddyManagerPrx> seq = BuddyAdapter::instance().getProxySeq(_id % BuddyAdapter::instance().getCluster());
        if( seq.empty()){
                MCE_WARN( "[LoadTask::handle] get 0 proxy" );
                return;
        }
        MCE_DEBUG( "[LoadTask::handle] " << seq.size() << " cache will be loaded" );
        for(vector<BuddyManagerPrx>::const_iterator it = seq.begin(); it!= seq.end(); it++){
                MCE_DEBUG( "[LoadTask::handle] proxy = " << (*it));
             	try{
			(*it)->load((uint32_t)_id,_objptr);
        	}
		catch (Ice::Exception& e){
                        MCE_WARN( "[LoadTask::handle] "<< (*it) << " load failed");
			MCE_WARN( " Ice::Exception: " << e.what() << " at " << __FILE__ << ":" << __LINE__ );
                }
		catch(...){
			MCE_WARN( "[LoadTask::handle] "<< (*it) << " load failed, unknown reason" );
		}
	}
}

void BuddyAdapter::remove(Ice::Int userId) {
	vector<BuddyManagerPrx> seq = getProxySeq(userId % getCluster());
        if( seq.empty()){
		MCE_WARN( "[BuddyAdapter::remove] get 0 proxy" );
                return;
	}
	for(vector<BuddyManagerPrx>::const_iterator it = seq.begin(); it!= seq.end(); it++){
		(*it)->removeBuddyRelation(userId);
	}
}

void BuddyAdapter::updateRelation(const Relationship& relation, const BuddyState& oper, BuddyRelationPtr localbrp, 
		BuddyRelationPtr remotebrp, const bool biblock, const Ice::Context& currentctx) {
        BuddyRelationHoldersPtr localbrhp = new BuddyRelationHolders;
	StateRecorder sr;
	sr.lastid = 0;
	sr.laststate = NULLSTATE;
	Ice::Context ctxlocal(currentctx);
	if(biblock){
		buildContext(ctxlocal, Cache_MAIN,Cache_LOCAL, Cache_BIBLOCK);	
	}else{
		buildContext(ctxlocal, Cache_MAIN,Cache_LOCAL,Cache_NORMALBLOCK);
	}
	const StateRecorder checksr = sr;
	int id = relation.from;
	try {
		switch(oper){
                	case ADDAPPLY:{
                		localbrhp = _localprx->cacheAddApply(relation,checksr,ctxlocal);
				} break;
        		case ACCEPTAPPLY:{
				localbrhp = _localprx->cacheAcceptApply(relation,checksr,ctxlocal);
				id = relation.to;
                        	} break;
                	case DENYAPPLY: {
				localbrhp = _localprx->cacheDenyApply(relation,checksr,ctxlocal);
				id = relation.to;
                		} break;
                	case REMOVEFRIEND: {
				localbrhp = _localprx->cacheRemoveFriend(relation,checksr,ctxlocal);
                       		} break;
                	case ADDBLOCK: {
                        	localbrhp = _localprx->cacheAddBlock(relation, checksr,ctxlocal);
                        	} break;
                	case REMOVEBLOCK: {
                        	localbrhp = _localprx->cacheRemoveBlock(relation, checksr, ctxlocal);
                        	} break;
			default:
				break;
        	}
	}
	catch (Ice::Exception& e){
        	MCE_WARN( "[BuddyAdapter::updateRelation] "<< e << " at " << __FILE__ << ":" << __LINE__);
		return;
        }
	catch (...){
                MCE_WARN(" [BuddyAdapter::updateRelation] Unkown exception at " << __FILE__ << ":" << __LINE__);
        	return;
	}

	if(localbrhp->hsr.laststate == CHECKERROR){
		reload(id);
	}else{
		localbrp = localbrhp->hbr;	
        	TaskManager::instance().execute(new LocalUpdateRelationTask( _localprx,relation ,oper, biblock,sr,currentctx));
        }

	Ice::Context ctxremote(currentctx);
        if(biblock){
                buildContext(ctxremote,Cache_MAIN,Cache_REMOTE,Cache_BIBLOCK);
        }else{
                buildContext(ctxremote,Cache_MAIN,Cache_REMOTE,Cache_NORMALBLOCK); 
        }
	BuddyRelationHoldersPtr remotebrhp = new BuddyRelationHolders;
	BuddyManagerPrx remoteprx;
	id = relation.to;
	try{
		switch(oper){
       			case ADDAPPLY:{
				remoteprx = getProxy(id);
                		if(remoteprx){
					remotebrhp = remoteprx->cacheAddApply(relation,sr,ctxremote);
                    			}
				} break;
                	case ACCEPTAPPLY:{
				id = relation.from;
				remoteprx = getProxy(id);
                        	if(remoteprx){
					remotebrhp = remoteprx->cacheAcceptApply(relation,sr,ctxremote);
                        		}
				} break;
               		case DENYAPPLY: {
				id = relation.from;
				remoteprx = getProxy(id);
				if(remoteprx){
                			remotebrhp = remoteprx->cacheDenyApply(relation,sr,ctxremote);
                        		}
				} break;
                	case REMOVEFRIEND: {
				remoteprx = getProxy(id);
                        	if(remoteprx){
					remotebrhp = remoteprx->cacheRemoveFriend(relation,sr,ctxremote);
                       			}
				} break;
                	case ADDBLOCK: {
				remoteprx = getProxy(id);
                        	if(remoteprx){
					remotebrhp = remoteprx->cacheAddBlock(relation, sr,ctxremote);
                        	}
				} break;
                	case REMOVEBLOCK: {
				remoteprx = getProxy(id);
				if(remoteprx){
                        		remotebrhp = remoteprx->cacheRemoveBlock(relation, sr,ctxremote);
                        		}
				} break;
			default: {
				MCE_WARN("BuddyAdapter::updateRelation remote oper not defined and oper = " << oper);
				return;
				}
        	}
	}
	catch (Ice::Exception& e){
                MCE_WARN( "[BuddyAdapter::updateRelation] "<< e << " at " << __FILE__ << ":" << __LINE__);
                return;
        }
        catch (...){
                MCE_WARN(" [BuddyAdapter::updateRelation] Unkown exception at " << __FILE__ << ":" << __LINE__);
                return;
        }

	if(remotebrhp->hsr.laststate == CHECKERROR){
                reload(id);
        }else{
		remotebrp = remotebrhp->hbr;
		TaskManager::instance().execute(new RemoteUpdateRelationTask( remoteprx,relation ,oper, biblock, sr, currentctx));
	}
}

void  LocalUpdateRelationTask::handle(){
	BuddyRelationHoldersPtr localbrhp = new BuddyRelationHolders;
	int id = 0;
	switch(_oper)
        {
                case ADDAPPLY:
                case REMOVEFRIEND:
                case ADDBLOCK:
                case REMOVEBLOCK:
                        id = _relation.from;
                        break;
                case ACCEPTAPPLY:
                case DENYAPPLY:
                        id = _relation.to;
                        break;
                default:
			MCE_WARN("LocalUpdateRelationTask::handle state not defined and oper = " << _oper);
                        break;
        }

        vector<BuddyManagerPrx> seq = BuddyAdapter::instance().getProxySeq(id % BuddyAdapter::instance().getCluster());
        if( seq.empty())
                return;
	StateRecorder sr;
	sr.lastid = 0;
        sr.laststate = NULLSTATE;
	localbrhp->hsr.lastid = 0;
	localbrhp->hsr.laststate = NULLSTATE;	
        for(vector<BuddyManagerPrx>::const_iterator it = seq.begin(); it!= seq.end(); it++){
		if( _prx->ice_getIdentity()==(*it)->ice_getIdentity())
			continue;
		Ice::Context ctxother(_context);
		if(_biblock){
			BuddyAdapter::instance().buildContext(ctxother, Cache_OTHER,Cache_LOCAL, Cache_BIBLOCK);
		}else{
			BuddyAdapter::instance().buildContext(ctxother, Cache_OTHER,Cache_LOCAL, Cache_NORMALBLOCK);
		}
		try {
                	switch(_oper){
                        	case ADDAPPLY:{
                                        localbrhp = (*it)->cacheAddApply(_relation,_sr, ctxother);
                                } break;
                        	case ACCEPTAPPLY:{
                                        localbrhp = (*it)->cacheAcceptApply(_relation,_sr, ctxother);
                                } break;
                        	case DENYAPPLY: {
                                        localbrhp = (*it)->cacheDenyApply(_relation,_sr, ctxother);
                                } break;
                        	case REMOVEFRIEND: {
                                        localbrhp = (*it)->cacheRemoveFriend(_relation,_sr,ctxother);
                                } break;
                        	case ADDBLOCK: {
                                        localbrhp = (*it)->cacheAddBlock(_relation,_sr, ctxother);
                               	} break;
                        	case REMOVEBLOCK: {
                                        localbrhp = (*it)->cacheRemoveBlock(_relation,_sr, ctxother);
                               	} break;
				default:
					MCE_WARN("LocalUpdateRelationTask::handle state not defined and oper = " << _oper);
				break;
                	}
		}
		catch (Ice::Exception& e){
                	MCE_WARN( "[LocalUpdateRelationTask::handle()] "<< e << " at " << __FILE__ << ":" << __LINE__);
                	return;
        	}
        	catch (...){
        	        MCE_WARN(" [LocalUpdateRelationTask::handle()] Unkown exception at " << __FILE__ << ":" << __LINE__);
                	return;
        	}	
		if(localbrhp->hsr.laststate == CHECKERROR){
			BuddyAdapter::instance().reload(id);
			break;
		}
        }	
}

void  RemoteUpdateRelationTask::handle(){
	BuddyRelationHoldersPtr remotebrhp = new BuddyRelationHolders;
	int id = 0;
        switch(_oper)
        {
                case ADDAPPLY:
                case REMOVEFRIEND:
                case ADDBLOCK:
                case REMOVEBLOCK:
                        id = _relation.to;
                        break;
                case ACCEPTAPPLY:
                case DENYAPPLY:
                        id = _relation.from;
                        break;
                default:
                        MCE_WARN("RemoteUpdateRelationTask::handle state not defined and oper = " <<_oper);
                        break;
        }

	vector<BuddyManagerPrx> seq = BuddyAdapter::instance().getProxySeq(id % BuddyAdapter::instance().getCluster());
        if( seq.empty())
                return;
        StateRecorder sr;
	sr.lastid = 0;
	sr.laststate = NULLSTATE;
	remotebrhp->hsr.lastid = 0;
	remotebrhp->hsr.laststate = NULLSTATE;	
	for(vector<BuddyManagerPrx>::const_iterator it = seq.begin(); it!= seq.end(); it++){
                if( _prx->ice_getIdentity() ==  (*it)->ice_getIdentity())
                        continue;
		Ice::Context ctx(_context);
		if(_biblock){
                        BuddyAdapter::instance().buildContext(ctx, Cache_OTHER,Cache_REMOTE, Cache_BIBLOCK);
                }else{
                        BuddyAdapter::instance().buildContext(ctx, Cache_OTHER,Cache_REMOTE, Cache_NORMALBLOCK);
                }
		try {
			switch(_oper){
                        	case ADDAPPLY:{
                                        remotebrhp = (*it)->cacheAddApply(_relation,_sr,ctx);
                                } break;
                        	case ACCEPTAPPLY:{
                                        remotebrhp = (*it)->cacheAcceptApply(_relation,_sr,ctx);
                                } break;
                        	case DENYAPPLY: {
                                        remotebrhp = (*it)->cacheDenyApply(_relation,_sr,ctx);
                                } break;
                        	case REMOVEFRIEND: {
                                        remotebrhp = (*it)->cacheRemoveFriend(_relation,_sr,ctx);
                                } break;
                        	case ADDBLOCK: {
                                        remotebrhp = (*it)->cacheAddBlock(_relation, _sr,ctx);
                                } break;
                        	case REMOVEBLOCK: {
                                        remotebrhp = (*it)->cacheRemoveBlock(_relation, _sr ,ctx);
                                } break;
				default:
					MCE_WARN("RemoteUpdateRelationTask::handle state not defined and oper = " << _oper);
				break;
                	}
		}
		catch (Ice::Exception& e){
                        MCE_WARN( "[RemoteUpdateRelationTask::handle()] "<< e << " at " << __FILE__ << ":" << __LINE__);
                        return;
                }
                catch (...){
                        MCE_WARN(" [RemoteUpdateRelationTask::handle()] Unkown exception at " << __FILE__ << ":" << __LINE__);
                        return;
                }
		if(remotebrhp->hsr.laststate == CHECKERROR){
			BuddyAdapter::instance().reload(id);
                        break;
                }
	}
}

void BuddyAdapter::cacheAddApply(const Relationship& apply,
                const Ice::Context& ctx) {
        BuddyRelationPtr localbrp = new BuddyRelation;
	BuddyRelationPtr remotebrp = new BuddyRelation;
	updateRelation(apply,ADDAPPLY,localbrp,remotebrp);
        BuddyLogicAdapter::instance().addApply(apply, localbrp,remotebrp,ctx);
}

void BuddyAdapter::cacheAcceptApply(const Relationship& apply,
                const Ice::Context& ctx) {
	BuddyRelationPtr localbrp = new BuddyRelation;
        BuddyRelationPtr remotebrp = new BuddyRelation;
        updateRelation(apply,ACCEPTAPPLY,localbrp,remotebrp);
        BuddyLogicAdapter::instance().acceptApply(apply,localbrp,remotebrp, ctx);
}

void BuddyAdapter::cacheDenyApply(const Relationship& apply,
                const Ice::Context& ctx) {
	BuddyRelationPtr localbrp = new BuddyRelation;
        BuddyRelationPtr remotebrp = new BuddyRelation;
        updateRelation(apply,DENYAPPLY,localbrp,remotebrp);
        BuddyLogicAdapter::instance().denyApply(apply,localbrp,remotebrp, ctx);
}

void BuddyAdapter::cacheRemoveFriend(const Relationship& buddy,
                const Ice::Context& ctx) {
	BuddyRelationPtr localbrp = new BuddyRelation;
        BuddyRelationPtr remotebrp = new BuddyRelation;
        updateRelation(buddy,REMOVEFRIEND,localbrp,remotebrp);
        BuddyLogicAdapter::instance().removeFriend(buddy, localbrp,remotebrp,ctx);
}

void BuddyAdapter::cacheAddBlock(const Relationship& block, bool biblock,
                const Ice::Context& ctx) {
	BuddyRelationPtr localbrp = new BuddyRelation;
        BuddyRelationPtr remotebrp = new BuddyRelation;
        updateRelation(block,ADDBLOCK,localbrp,remotebrp,biblock,ctx);
        BuddyLogicAdapter::instance().addBlock(block,localbrp,remotebrp, ctx);
}

void BuddyAdapter::cacheRemoveBlock(const Relationship& block, bool biblock,
                const Ice::Context& ctx) {
	BuddyRelationPtr localbrp = new BuddyRelation;
        BuddyRelationPtr remotebrp = new BuddyRelation;
        updateRelation(block,REMOVEBLOCK,localbrp,remotebrp,biblock,ctx);
        BuddyLogicAdapter::instance().removeBlock(block, localbrp,remotebrp,ctx);
}

void BuddyAdapter::buildContext(Ice::Context& ctx, bool maincache,bool remote, bool biblock) {
        if (remote) {
                ctx.insert(make_pair(Cache_REMOTE_KEY, Cache_REMOTE_VALUE));
        }
        if (biblock) {
                ctx.insert(make_pair(Cache_BIBLOCK_KEY, Cache_BIBLOCK_VALUE));
        }
	if(maincache){
		ctx.insert(make_pair(Cache_MAIN_KEY, Cache_MAIN_VALUE));
	}
}

::MyUtil::Int2IntSeqMap BuddyAdapter::getFriendSeqBatch(::MyUtil::IntSeq userIds, int timeout ){
	BuddyManagerPrx prx = getManager(timeout);
        if(prx){
                return prx->getFriendSeqBatch(userIds);
        }
        ::MyUtil::Int2IntSeqMap map;
        return map;
}

BuddyRelationPtr BuddyAdapter::getBuddyRelation(::Ice::Int userId, int timeout ){
	BuddyManagerPrx prx = getManager(timeout);
        if(prx){
                return prx->getBuddyRelation(userId);
        }
        return NULL;
}        
//为BuddyRelationCache预加载使用的批量接口
Int2BuddyRelationMap BuddyAdapter::getBuddyRelationBatch(::MyUtil::IntSeq userIds, int timeout ){
	BuddyManagerPrx prx = getManager(timeout);
        if(prx){
                return prx->getBuddyRelationBatch(userIds);
        }
        Int2BuddyRelationMap map;
        return map;

}
// ========= BuddyLoaderAdapter =================================
void BuddyLoaderAdapter::load( Ice::Int id ){
        MCE_DEBUG( "[BuddyLoaderAdapter::load] id=" << id );
        getProxyOneway(id)->load( id );
}

}
}

