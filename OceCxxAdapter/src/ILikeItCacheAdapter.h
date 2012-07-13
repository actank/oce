/*
 * ILikeItCacheAdapter.h
 *
 *  Created on: 2010-01-28
 *      Author: zhanghan
 */

#ifndef __ILIKEIT_CACHE_ADAPTER_H__
#define __ILIKEIT_CACHE_ADAPTER_H__

#include <IceUtil/RWRecMutex.h>
#include "ILikeIt.h"
#include "IceLogger.h"
#include "IceExt/src/TopicI.h"
#include "app/ILikeIt/share/ILikeItUtil.h"
#include "UtilCxx/src/TaskManager.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace ilikeit {

const int TASK_LEVEL_LOADSHIPS = 1;
const int TASK_LEVEL_ADDSHIPS = 2;
const int TASK_LEVEL_REMOVESHIPS = 3;

const int CACHE_CLUSTER = 1;

class ILikeItCacheAdapter : 
	public MyUtil::Singleton<ILikeItCacheAdapter> {
public:
	int getLikeCount(int type, long id);

	MyUtil::IntSeq getLikeCache(int type, long id);

	LikeInfoPtr getLikeDetail(const std::string& gidStr, int uid, int limit, bool showStrangers=true);
	LikeInfo2Ptr getLikeDetail2(const std::string& gidStr, int uid, int limit, bool showStrangers=true);
	
	LikeInfoPtr getLikeDetail(const GlobalIdPtr& gid, int uid, int limit, bool showStrangers=true);
	LikeInfo2Ptr getLikeDetail2(const GlobalIdPtr& gid, int uid, int limit, bool showStrangers=true);

	LikeInfoSeq getLikeDetailBatchG(const std::vector<GlobalIdPtr>& gids, int uid, int limit, int detailNum);
	LikeInfo2Seq getLikeDetailBatchG2(const std::vector<GlobalIdPtr>& gids, int uid, int limit, int detailNum);
	
	LikeInfoSeq getLikeDetailBatch(const std::vector<std::string>& gidStrs, int uid, int limit, int detailNum);
	LikeInfo2Seq getLikeDetailBatch2(const std::vector<std::string>& gidStrs, int uid, int limit, int detailNum);

	void setData(const ::MyUtil::ObjectResultPtr& objects, int mod, ILikeItCachePrx& localPrx);

	void load(const xce::ilikeit::GlobalIdSeq& gids);

	void loadSync(const xce::ilikeit::GlobalIdSeq& gids);	// 加入一个同步load的方法 -- 090728 by zhanghan

	void addLikeship(const LikeshipPtr& ship);
	
	void removeLikeship(const LikeshipPtr& ship);

	bool isValid(long id);

protected:
	friend class MyUtil::Singleton<ILikeItCacheAdapter>;
	ILikeItCacheAdapter();

private:
	xce::clusterstate::ClientInterface<ILikeItCachePrx> _clientCS;   /* @brief ClusterState的接口 */
	int _cluster;
};

}
}

#endif
