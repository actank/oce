#ifndef __SERVER_STATE_ICE__
#define __SERVER_STATE_ICE__

#include <SubjectObserver.ice>

module xce {
module serverstate {

class ServerStateInfo extends MyUtil::SbjState {
	string servername;	/* @brief 服务名 */	
	string identity;	/* @brief 服务标识 */
	Object* prx;		/* @brief 服务的代理 */
	int cluster;		/* @brief 服务总的散的份数 */
	int mod;		/* @brief 服务在Cluster所属的份数 */
	int status;		/* @brief 服务的状态 */
	bool newset;		/* @brief 服务是否新重启 */
};

dictionary<string, Object*> ServerPrxMap;
dictionary<string, int> ServerPrxStatusMap;

class ServerStateInfoMutil extends ServerStateInfo
{ 
	ServerPrxMap prxMap;          /* @brief 同个进程中存在多个服务时，
							      该进程中的所有标识和服务代理的map*/
	
	ServerPrxStatusMap statusMap; /* @brief 同个进程中存在多个服务时，
							      该进程中的所有标识和服务状态的map*/
};

sequence<ServerStateInfo> ServerStateSeq;

};


module clusterstate {

dictionary<int, xce::serverstate::ServerStateSeq> ServerStateMap;

class ClusterStateInfo extends MyUtil::SbjState {
	ServerStateMap states;	/* @brief 有效的服务器排列信息组 */
};

};

module clustercontroller {

class ControllerManager {
	
	xce::clusterstate::ClusterStateInfo GetServerState();	/* @brief 返回服务信息 */
	
	void NotifyClusterState();		/* @brief 向客户端发送服务信息 */

	void FetchServerState();		/* @brief 从各服务端获取服务信息 */
	
};

};


};

#endif
