/*
 * ILikeItCacheOldAdapter.h
 *
 *  Created on: 2009-07-06
 *      Author: zhanghan
 */

#ifndef __ILIKEIT_CACHE_OLD_ADAPTER_H__
#define __ILIKEIT_CACHE_OLD_ADAPTER_H__

#include <IceUtil/RWRecMutex.h>
#include "ILikeIt.h"
#include "IceLogger.h"
#include "IceExt/src/TopicI.h"
#include "app/ILikeIt/share/ILikeItUtil.h"
#include "UtilCxx/src/TaskManager.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace ilikeit {

const int TASK_LEVEL_LOADSHIPS_OLD = 1;
const int TASK_LEVEL_ADDSHIPS_OLD = 2;
const int TASK_LEVEL_REMOVESHIPS_OLD = 3;

class ILikeItCacheOldAdapter : 
	public MyUtil::Singleton<ILikeItCacheOldAdapter> {
public:
	int getLikeCount(int type, long id);

	MyUtil::IntSeq getLikeCache(int type, long id);

	LikeInfoPtr getLikeDetail(const std::string& gidStr, int uid, int limit, bool showStrangers=true);
	
	LikeInfoPtr getLikeDetail(const GlobalIdPtr& gid, int uid, int limit, bool showStrangers=true);
	
	LikeInfoSeq getLikeDetailBatch(const std::vector<std::string>& gidStrs, int uid, int limit, int detailNum);

	void load(const xce::ilikeit::GlobalIdSeq& gids);

	void loadSync(const xce::ilikeit::GlobalIdSeq& gids);	// 加入一个同步load的方法 -- 090728 by zhanghan

	void addLikeship(const LikeshipPtr& ship);
	
	void removeLikeship(const LikeshipPtr& ship);

protected:
	friend class MyUtil::Singleton<ILikeItCacheOldAdapter>;
	ILikeItCacheOldAdapter();

private:
	xce::clusterstate::ClientInterface<ILikeItCachePrx> _clientCS;   /* @brief ClusterState的接口 */
	int _cluster;
};

}
}

#endif
