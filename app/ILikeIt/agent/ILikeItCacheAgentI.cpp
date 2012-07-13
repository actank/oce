#include "ILikeItCacheAgentI.h"
#include "UtilCxx/src/Allocator.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "OceCxxAdapter/src/ILikeItCacheAdapter.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::platform::storage;
using namespace xce::serverstate;
using namespace xce::clusterstate;
using namespace xce::ilikeit;

// ================ MyUtil::initialize =====================
void MyUtil::initialize() {

	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&ILikeItCacheAgentI::instance(), service.createIdentity("M", ""));

	int cluster = service.getCommunicator()->getProperties()->getPropertyAsInt("ILikeItCache.Cluster");
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("ILikeItCache.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ILikeItCache.Interval", 5);
	ILikeItCacheAgentI::instance().bootstrap(cluster, mod);

	ILikeItCacheAdapter::instance();

	// 加入向Controller注册
	ServerStateSubscriber::instance().initialize("ControllerILikeItCache", 
			&ILikeItCacheAgentI::instance(), "", "M", cluster, mod, interval);
}

// =============== ILikeItCacheAgentI主类 =======================
void ILikeItCacheAgentI::bootstrap(int cluster, int mod) {
	_cluster = cluster;
	_mod = mod;
	MCE_INFO("bootstrap cluster="<<_cluster << " mod="<<_mod);
}

Ice::Int ILikeItCacheAgentI::getLikeCount(const GlobalIdPtr& gid, const Ice::Current&) {

	MCE_DEBUG("[ILikeItCacheAgentI::getLikeCount] getLikeCount : type " << gid->type << " object " << gid->id);
	
	return ILikeItCacheAdapter::instance().getLikeCount(gid->type, gid->id);
}

MyUtil::IntSeq ILikeItCacheAgentI::getLikeCache(const GlobalIdPtr& gid, const Ice::Current&) {

	MCE_DEBUG("[ILikeItCacheAgentI::getLikeCache] get : type " << gid->type << " objectid " << gid->id);
	
	return ILikeItCacheAdapter::instance().getLikeCache(gid->type, gid->id);
}

LikeInfoPtr ILikeItCacheAgentI::getLikeDetail(const GlobalIdPtr& gid, ::Ice::Int uid, ::Ice::Int limit, bool showStrangers, const Ice::Current&) {

	MCE_DEBUG("[ILikeItCacheAgentI::getLikeDetail] get : type " << gid->type << " objectid " << gid->id);

	return ILikeItCacheAdapter::instance().getLikeDetail(gid, uid, limit, showStrangers);

}

LikeInfoSeq ILikeItCacheAgentI::getLikeDetailBatch(const GlobalIdSeq& gids, ::Ice::Int uid, ::Ice::Int limit, ::Ice::Int detailNum, const Ice::Current& ctr)  {

	MCE_DEBUG("[ILikeItCacheAgentI::getLikeDetailBatch] " << gids.size() << " gid(s)");

	return ILikeItCacheAdapter::instance().getLikeDetailBatchG(gids, uid, limit, detailNum);
}


void ILikeItCacheAgentI::load(const GlobalIdSeq& gids, const Ice::Current& crt) {
	
	MCE_WARN("[ILikeItCacheAgentI::reload] Deprecated Function");
}

void ILikeItCacheAgentI::addLikeship(const LikeshipPtr& ship, const Ice::Current& ctr) {

	MCE_WARN("[ILikeItCacheAgentI::addLikeship] Deprecated Function");
}

void ILikeItCacheAgentI::removeLikeship(const LikeshipPtr& ship, const Ice::Current& ctr) {

	MCE_WARN("[ILikeItCacheAgentI::removeLikeship] Deprecated Function");
}

bool ILikeItCacheAgentI::belongsMe(const GlobalIdPtr& gid) {
	return ( gid->id % _cluster == _mod );
}
