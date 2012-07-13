/*
 * QuestCacheAdapter.h
 *
 *  Created on: 2009-12-24
 *      Author: zhanghan
 */

#ifndef __QUEST_CACHE_H__
#define __QUEST_CACHE_H__

#include "Quest.h"
#include "Channel.h"
#include "AdapterI.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"
#include "IceLogger.h"

namespace xce {
namespace quest {

const int DEFAULT_CLUSTER = 10;

class QuestCacheAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton<MyUtil::QuestChannel, QuestCacheAdapter> {
public:
	QuestCacheAdapter();

	bool newFlow(int userid);

	TipPtr showTip(int userid);

	void click(int tipid, int userid);

	void renewTips();
	
	void renewFlow();

	void responseUrl(int userid, const string& url);	

private:
	virtual string name() {
		return "M";
	}

	virtual string endpoints() {
		return "@QuestCache";
	}

	virtual size_t cluster() {
		return DEFAULT_CLUSTER;
	}

	QuestCachePrx getQuestCacheOneway(int id);
	vector<QuestCachePrx> managersOneway_;

	QuestCachePrx getQuestCacheDatagram(int id);
	vector<QuestCachePrx> managersDatagram_;

	xce::clusterstate::ClientInterface<QuestCachePrx> clientCS_;
	std::vector<QuestCachePrx> prxs;
};

};
};

#endif
