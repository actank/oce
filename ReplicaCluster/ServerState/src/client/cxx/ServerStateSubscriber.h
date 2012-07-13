#ifndef __SERVER_STATE_CLIENT__
#define __SERVER_STATE_CLIENT__

#include "SubjectObserver.h"
#include "ReplicaCluster.h"
#include "IceExt/src/Channel.h"
#include "UtilCxx/src/Singleton.h"

namespace xce {
namespace serverstate {

class ServerStateBackDoorI : virtual public ServerStateBackDoor {
public:
  ServerStateBackDoorI(const Ice::ObjectAdapterPtr& adapter) {
    if (adapter) {
      adapter->add(this, adapter->getCommunicator()->stringToIdentity("SSBD"));
    }
  }
  virtual MyUtil::Str2StrMap control(const Str2StrSeqMap& data, const Ice::Current& c);
};

typedef IceUtil::Handle<ServerStateBackDoorI> ServerStateBackDoorIPtr;

class ServerStateSubscriber : public MyUtil::SbjSubscriber,
			public MyUtil::Singleton<ServerStateSubscriber>,
			public IceUtil::Mutex {

public:
	ServerStateSubscriber():
       		_hasTopic(false),
		_inited(false) 	{		
	};

	/* @brief 	初始化
	 * @param	ctrEndpoints	controller的端点
	 * @param	pServer		服务本身的指针
	 * @param	mod		服务在cluster中所属的编号
	 * @param	interval	注册时间间隔(秒)
	 */
	void initialize(const string& ctrEndpoints, const Ice::ObjectPtr pServer, int mod=0, int interval=300, const MyUtil::ChannelPtr& channel=new MyUtil::OceChannel, const string& threadPoolSize = "10", const string& threadPoolSizeMax = "3000", const string& stackSize = "131072", const string& protocol="tcp");

	//为多个服务在同一个进程中所作的初始化	

	void initialize(const string& ctrEndpoints, const map<string,Ice::ObjectPtr>& pServerList, int mod=0, int interval=300, const MyUtil::ChannelPtr& channel = new MyUtil::OceChannel, const string& threadPoolSize = "10", const string& threadPoolSizeMax = "3000", const string& stackSize = "131072");
	
  void initialize(const string& ctrEndpoints, const map<string,Ice::ObjectPtr>& pServerList, const Ice::ObjectPtr pServer, int mod=0, int interval=300, const MyUtil::ChannelPtr& channel = new MyUtil::OceChannel, const string& threadPoolSize = "10", const string& threadPoolSizeMax = "3000", const string& stackSize = "131072", const string& protocal="tcp");

	/* @brief	初始化 兼容旧接口
	 */
	void initialize(const string& ctrEndpoints, const Ice::ObjectPtr pServer, const string& endpoints, const string& identity, int cluster, int mod=0, int interval=300, const MyUtil::ChannelPtr& channel= new MyUtil::OceChannel, const string& threadPoolSize = "10", const string& threadPoolSizeMax = "3000", const string& stackSize = "131072");

	/* @brief 	ICE接口: opera. Observer端可调用的, 从subscriber端获取服务名和服务状态
	 * @param	sbjStatePtr		从Observer端传递的信息参数
	 * @return	
	 */	
	virtual ::MyUtil::SbjStatePtr opera(const ::MyUtil::SbjStatePtr&, const ::Ice::Current& = ::Ice::Current()); 
	void setStatus(int statue);
	void setStatus(int statue, string identifier);
  int getStatus();
  int getStatus(const string& identifier);

	/* @brief	获取自身的proxy */
	Ice::ObjectPrx getProxySelf();

private:
	/*@brief  	将形如(tcp:udp -p 9000)的协议和形如(10.22.206.157的ip)
	  	  	转换为形如(tcp -h 10.22.206.157:udp -h 10.22.206.157 -p 9000)的endponits
	 *@param  	protocal	协议字符串;格式与icegrid配置文件一致,为tcp或udp -p XXX或tcp:udp -p XXXX;
	 				多个协议之间用:分隔
	 *@param	localIP		本机IP，格式为xx.xx.xx.xx
	 *@return	组合结果,格式为tcp -h xx.xx.xx.xx或tcp -h xx.xx.xx.xx:
	 */
	static string protocol2Endpoints(const string& protocal, const string& localIP);

	bool bIsMutilServicesInOneProcess;
	string _ctrEndpoints;
	// int _cluster;
	int _mod;
	long _interval;
	MyUtil::SbjTopicPtr _topic;
	Ice::ObjectPrx _prx;
	ServerPrxMap   _prxMap;   //为多个服务在同一个进程中进行存储
	ServerPrxStatusMap   _statusMap;   //为多个服务在同一个进程中进行存储
	MyUtil::ChannelPtr _channel;

	int _status;

	bool _newSet;	/* @brief 是否刚刚重启 */
	bool _hasTopic;
	bool _inited;
  ServerStateBackDoorIPtr _door;

  void init_ServerStateBackDoor();
};

typedef IceUtil::Handle<ServerStateSubscriber> ServerStateSubscriberPtr;


};
};

#endif
