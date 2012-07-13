#ifndef __CLUSTER_CLIENT_H__
#define __CLUSTER_CLIENT_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

namespace xce{
namespace clusterstate{

using namespace std;
using namespace Ice;
using namespace xce::serverstate;
	
class ClusterClient : virtual public Ice::Object {
public:
	/* @brief 	接口函数, ClusterState更新时相应执行的操作
	 * @param	clusterState	新的服务器组合信息
	 */
	virtual void renew(const ServerStateMap& clusterState)=0;	

};

typedef IceUtil::Handle<ClusterClient> ClusterClientPtr;

};
};

#endif
