#ifndef __CLUSTER_STATE_ADAPTER_H__
#define __CLUSTER_STATE_ADAPTER_H__

#include "ReplicaCluster.h"
#include "SubjectObserver.h"
#include "ClusterClient.h"
#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/Channel.h"

namespace xce {
namespace clusterstate {


class ClusterStateSubscriber : public MyUtil::SbjSubscriber,
			public IceUtil::Mutex {
public:
	/* @brief 	构造函数
	 * @param	client		注册来的ClusterState信息的使用者
	 * @param	ctlEndpoints	controller的endpoints
	 * @param	interval	注册时间间隔
	 */
	ClusterStateSubscriber(ClusterClient& client, std::string ctlEndpoints, int interval=120, const MyUtil::ChannelPtr& channel = new MyUtil::OceChannel);

	void init();

	/* @brief 	ICE接口: opera. Observer端可调用的subscriber端操作
	 * @param	sbjStatePtr		从Observer端传递的信息参数
	 * @return	一个空对象的指针(在这个业务中用不到)
	 */	
	virtual ::MyUtil::SbjStatePtr opera(const ::MyUtil::SbjStatePtr&, const ::Ice::Current& = ::Ice::Current()); 

	/* @brief 返回当前ClusterState
	 */
	ServerStateMap& getClusterState();

	/* @brief 返回之前的ClusterState
	 */
	ServerStateMap& getOldClusterState();

	/* @brief 启动时,先去controller获取一次最初的state信息
	 */
	void getInitState();

private:
	/* @brief 注册来的ClusterState信息的使用者 */
	ClusterClient& _client;	

	std::string _ctlEndpoints;
	long _interval;
	
	std::map<std::string, MyUtil::SbjTopicPtr> _mapTopics;

	/* @brief 当前的ClusterState数组 */
	ServerStateMap _clusterState;

	/* @brief 更新前的ClusterState数组 */
	ServerStateMap _oldState;

	MyUtil::ChannelPtr _channel;
	Ice::CommunicatorPtr _ic;
};

/* @brief 触发客户端renew方法的Task */
class RenewTask : public MyUtil::Task {
public:
	RenewTask(ClusterClient& client, const ServerStateMap& clusterState):
		_client(client),
		_clusterState(clusterState) {
	};
	virtual void handle();
private:
	ClusterClient& _client;
	ServerStateMap _clusterState;
};

typedef IceUtil::Handle<ClusterStateSubscriber> ClusterStateSubscriberPtr;


};
};

#endif
