/*
 * ILikeItLogicAdapter.h
 *
 *  Created on: 2009-07-06
 *      Author: zhanghan
 */

#ifndef __ILIKEIT_LOGIC_H__
#define __ILIKEIT_LOGIC_H__

#include <IceUtil/RWRecMutex.h>
#include "ILikeIt.h"
#include "IceExt/src/TopicI.h"
#include "IceLogger.h"
#include "UtilCxx/src/TaskManager.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace ilikeit {

const int TASK_LEVEL_LOAD = 1;

class ILikeItLogicAdapter : 
	public MyUtil::Singleton<ILikeItLogicAdapter> {
public:
	void addLike(const std::string& gidStr, int uid);

	void addLikeBatch(const std::vector<pair<std::string, int> >& shipStrSeq);

	void removeLike(const std::string& gidStr, int uid);

	void removeLikeBatch(const std::vector<pair<std::string, int> >& shipStrSeq);

	LikeInfoPtr addLikeAndGet(const std::string& gidStr, int uid);
	LikeInfo2Ptr addLikeAndGet2(const std::string& gidStr, int uid);
	
	LikeInfoPtr removeLikeAndGet(const std::string& gidStr, int uid);
	LikeInfo2Ptr removeLikeAndGet2(const std::string& gidStr, int uid);

protected:
	friend class MyUtil::Singleton<ILikeItLogicAdapter>;
	ILikeItLogicAdapter();

private:
	xce::clusterstate::ClientInterface<ILikeItManagerPrx> _clientCS;   /* @brief ClusterState的接口 */
};


}
}

#endif
