#ifndef __USER_COUNT_ADAPTER_H__
#define __USER_COUNT_ADAPTER_H__

#include "UserCount.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace xce {
namespace usercount {
namespace adapter {
using namespace MyUtil;
using namespace xce::usercount;

const int CLUSTER = 10; /* <@brief  对应服务启动的份数 */

const int CADNOTIFY = 36;

class UserCountAdapter : public ReplicatedClusterAdapterI<UserCountManagerPrx>,
	public Singleton<UserCountAdapter> {
public:

	UserCountAdapter() : ReplicatedClusterAdapterI<UserCountManagerPrx>("ControllerUserCount",120,300){
	}

	/* brief	用户指定类型计数增加指定值(调用Ice接口)
	 * param	id	用户id
	 * param	type	指定类型
	 * param	count	计数值增加量
	 */
	void inc(int id, int type, int count=1);

	/* brief	用户指定类型计数减少指定值(调用Ice接口)
	 * param	id	用户id
	 * param	type	指定类型
	 * param	count	计数值减少量
	 */
	void dec(int id, int type, int count=1);

	/* brief	设定用户指定类型计数为指定值(调用Ice接口)
	 * param	id	用户id
	 * param	type	指定类型
	 * param	count	计数值设置量
	 */
	void set(int id, int type, int count);

	/* brief	得到用户指定类型的计数值(调用Ice接口)
	 * param	id	用户id
	 * param	type	指定类型
	 * return	计数值
	 */
	int get(int id, int type);

	/* brief	得到用户所有类型的计数信息
	 * param	id	用户id
	 * return	用户所有计数信息组成的类实例的指针
	 */
	UserCounterPtr getAll(int id);

	/* brief	Reload用户所有类型的计数信息
	 * param	id	用户id
	 */
	void reload(int id);

private:

	UserCountManagerPrx getUserCountManager(int id);
	UserCountManagerPrx getUserCountManagerOneway(int id);

};

}
;
}
;
}

#endif

