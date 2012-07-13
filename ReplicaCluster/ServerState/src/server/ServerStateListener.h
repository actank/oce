#ifndef __SERVER_STATE_LISTENER_H__
#define __SERVER_STATE_LISTENER_H__

#include "ReplicaCluster.h"
#include "ReplicaCluster/SubjectObserver/src/server/SubjectObserverI.h"
#include "UtilCxx/src/Singleton.h"


namespace xce {
namespace serverstate {

class ServerStateListener : public MyUtil::Singleton<ServerStateListener> {
			 
public:
	ServerStateListener():
	 	_name("ServerState") 	{
	}

        /* @brief  得到服务器状态列表
	 */
	ServerStateSeq fetch();

	ServerStateSeq getSeq();

private:
	 ServerStateSeq _ssss;	 
	 string _name;
	 string _endpoints;
};

};
};






#endif
