#ifndef __ONLINE_ADAPTER_H__
#define __ONLINE_ADAPTER_H__

#include "Online.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace xce {
namespace online {
namespace adapter {
	
using namespace MyUtil;
using namespace xce::online;

const int CLUSTER = 1; /* <@brief  对应服务启动的份数 */

class OnlineAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::OceChannel, OnlineAdapter>,
	virtual public IceUtil::Monitor<IceUtil::Mutex>  {
public:

	OnlineAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);		
	}

	/* @brief	初始化,启动线程
	 */
	void intialize();
	
	/* brief	一个用户的活动信息
	 * param	userIds		当前发送活动信号的用户组
	 */
	void ActiveUsers(int userId);

	/* brief	一组用户的活动信息(Ice接口函数)
	 * param	userIds		当前发送活动信号的用户组
	 */
	void ActiveUsers(const vector<int>& userIds);

	/* brief	得到一组用户的状态(Ice接口函数)
	 * param	userIds		要取信息的用户id集合
	 * return	相应的用户状态
	 */
	map<int, int> getOnlineStat(const vector<int>& userIds);
private:
	/* brief	指定服务标识(AdapterI虚函数重载)
	 * return	服务标识
	 */
	virtual string name() {
		return "M";
	}

	/* brief	指定endpoints在以用户id的散列编号前的部分(AdapterI虚函数重载)
	 * return
	 */
	virtual string endpoints() {
		return "@Online";
	}

	/* brief	指定对应服务启动的数量(AdapterI虚函数重载)
	 * return	对应服务启动的数量
	 */
	virtual size_t cluster() {
		return CLUSTER;
	}

	OnlineManagerPrx getOnlineManager(int id);
	OnlineManagerPrx getOnlineManagerOneway(int id);

	vector<OnlineManagerPrx> _managers;
	vector<OnlineManagerPrx> _managersOneway;
	
private:
	vector<int> _activeids;

	void retrive(vector<int>& toInvokeCache);

	class TrackDispatcher : virtual public IceUtil::Thread {
	public:
		TrackDispatcher(OnlineAdapter& adapter):
			_lastInvokeTime(time(NULL)), _adapter(adapter) {
		};
		virtual void run() {
			MCE_INFO("[OnlineDispatcher]: _dispatchThread started");
			while ( true ) {
				try {
					invoke();
					_adapter.retrive(_toInvokeCache);
				} catch (...) {
					MCE_WARN("[OnlineDispatcher]: _dispatchThread Exception");
				}
			}
		}
	private:
		vector<int> _toInvokeCache;
		time_t _lastInvokeTime;
		OnlineAdapter& _adapter;
		
		void invoke() {
			if ( shouldInvoke()) {
				invokeAndClearCache();
			}
		};
		
		bool shouldInvoke() {
			return (_toInvokeCache.size() > 1000) || (time(NULL) - _lastInvokeTime > 1);
		};

		void invokeAndClearCache() {
			vector<int> toInvokeCache_swap;
			_toInvokeCache.swap(toInvokeCache_swap);
			_lastInvokeTime = time(NULL);
			_adapter.ActiveUsers(toInvokeCache_swap);		
		};
	};

	typedef IceUtil::Handle<TrackDispatcher> TrackDispatcherPtr;
	TrackDispatcherPtr _dispatchThread;
};

};
};
};

#endif

