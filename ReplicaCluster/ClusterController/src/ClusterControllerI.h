#ifndef __CLUSTER_CONTROLLER_H__
#define __CLUSTER_CONTROLLER_H__

#include "ReplicaCluster.h"
#include "ReplicaCluster/ClusterState/src/server/ClusterStateListener.h"
#include "ReplicaCluster/ServerState/src/server/ServerStateListener.h"
// #include "AdminConsoleI.h"

namespace xce {
namespace clustercontroller {

class ControllerManagerI : public ControllerManager,
       			   public MyUtil::Singleton<ControllerManagerI>,
			   // virtual public xce::adminconsole::AdminConsoleComponentm
			   virtual public IceUtil::Thread
{

public:
	ControllerManagerI():
		_warnStatus(false),
		_fatalStatus(false) 	{
	}

	virtual ~ControllerManagerI() {
	}

	/* @brief	初始化
	 * @param	intervalFetch	获取服务器信息的时间间隔(秒)
	 * @param	intervalNotify	发送服务器信息的时间间隔(秒)
	 */
	void initialize(int cluster, int intervalFetch=5, int intervalNotify=600);

	/* @brief 	多线程的运行函数
	 */
 	virtual void run();

	/* @brief 	返回各服务状态; slice接口函数
	 */
	xce::clusterstate::ClusterStateInfoPtr GetServerState(const Ice::Current& = Ice::Current());
 
	/* @brief 	通知各client端当前的服务对应关系; slice接口函数
	 */
	void NotifyClusterState(const Ice::Current& = Ice::Current());

	/* @brief	获取各服务状态(ServerState)并决策为服务对应关系(ClusterState); slice接口函数
	 */
	void FetchServerState(const Ice::Current& = Ice::Current());

	/* @brief	管理功能:得到服务对应关系列表
	 */
	string ShowClusterState();

	/* @brief	管理功能:手工设置服务对应列表
	 * @param	pos		所要手工修改的服务位置
	 * @param	endpoints	所要改为的对应服务endpoints
	 * @return	修改是否成功
	 */
	bool ModifyClusterState(size_t pos, const string& endpoints);

	/* @brief	管理功能:当前服务对应状态
	 * @return	当前服务状态: 0 - 默认对应关系; -1 - 使用了备份服务(可正常使用); -2 - 备份服务不足(不可正常使用)
	 */
	int StateValid();

	int Cluster(const Ice::Current& = Ice::Current()) {return _cluster;}

        virtual std::string description();

        virtual std::string help();

        virtual std::string execute(const std::vector<std::string> argv);

private:
	/* @brief	判断服务信息是否改变过
	 * @return	true - 改变过; false - 未改变
	 */
	bool ClusterChanged();

  void check(const clusterstate::ServerStateMap& nState, 
    const clusterstate::ServerStateMap& oState);

	friend class MyUtil::Singleton<ControllerManagerI>;
	// std::vector<std::string> _clusterStateDefault;	/* @brief 默认的服务对应关系 */
	// std::vector<std::string> _clusterBakDefault;		/* @brief 默认的备份服务 */
	xce::clusterstate::ClusterStateInfoPtr _clusterStateOld;	/* @brief 上次更新的服务对应关系 */
	xce::clusterstate::ClusterStateInfoPtr _clusterState;	/* @brief 当前服务对应关系 */

	int _cluster;					/* @breif 对应服务的散的份数 */
	bool _warnStatus;				/* @brief 当前服务是否处于警告状态(使用备份服务) */
	bool _fatalStatus;				/* @brief 当前服务是否处于致命错误状态(备份服务不足) */

	int _intervalFetch;					/* @brief 进行服务对应更新的时间间隔(秒) */
	int _intervalNotify;					/* @brief 进行服务对应发送的时间间隔(秒) */

	time_t _lastFetch;					/* @brief 上次Fetch的时间 */
	time_t _lastNotify;					/* @brief 上次Notify的时间 */
};
	

};
};

#endif
