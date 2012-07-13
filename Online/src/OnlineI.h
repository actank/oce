#ifndef __ONLINE_EVICTOR_H__
#define __ONLINE_EVICTOR_H__

#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include "ServiceI.h"
#include "Evictor.h"
#include "Online.h"
#include "TaskManager.h"
#include "OnlineEventMediatorAdapter.h"

namespace xce {
namespace online {

using namespace std;
using namespace MyUtil;

/* ------------------------------------------------------
 * @class OnlineEvictor
 * @brief 处理用户的上下线记录
 ----------------------------------------------------- */
class OnlineEvictor: public TimedEvictor<int, int>  {
public :
	OnlineEvictor(time_t duration = 20*60) :
		TimedEvictor<int, int>::TimedEvictor(duration) {
	}

	/* @brief 	清除下线用户并返回其列表
	 * @param	vecUserIds	过期(判定为下线)用户列表
	 */
	void evict(vector<int>& vecUserIds);

	/* @brief	添加一个在线用户,若其本来已在线则更新位置和时间戳
	 * @param	obj	要添加的用户信息obj
	 * @param	id	要添加用户的id
	 * @return	是否新增用户
	 */	
	bool add(int obj, int id);
};

/* ----------------------------------------------------------
 * @class EvictTask
 * @brief 定时判断用户是否下线的线程
 --------------------------------------------------------- */
class EvictTask : public MyUtil::Task, public IceUtil::Mutex {
public:
	EvictTask(OnlineEvictor& evictor, time_t freq):
		_evictor(evictor),
		_freq(freq) {
	};
	virtual void handle();
private:
	OnlineEvictor& _evictor;
	time_t _freq;
};

/* ----------------------------------------------------------
 * @class OnlineManagerI
 * @brief 接口主类:用户上下线信息服务
 --------------------------------------------------------- */
class OnlineManagerI : virtual public OnlineManager, 
		       virtual public MyUtil::Singleton<OnlineManagerI>	 {
public :	
	/* @brief	初始化
	 * @param	duration	判断时间(秒)
	 */
	void initialize(time_t duration);

	/* @brief 	接口实现类:用户活动
	 * @param	userIds		本次有活动的用户id集合
	 */
    	virtual void UserActivity(const ::MyUtil::IntSeq& userIds, const ::Ice::Current& = ::Ice::Current());

	/* @brief	接口实现类:查询当前用户信息
	 * @param	userIds		查询状态的用户集合
	 * @return	用户状态map
	 */
    	virtual ::MyUtil::Int2IntMap UserOnlineStat(const ::MyUtil::IntSeq& userIds, const ::Ice::Current& = ::Ice::Current());

	/* @brief	向外通告一组用户上线 (暂无实现,留出接口)
	 * @param	userIds		上线的一组用户的id
	 */
	void UsersOnline(const vector<int>& userIds);

	/* @brief	向外通告一组用户下线 (暂无实现,留出接口)
	 * @param	userIds		上线的一组用户的id
	 */
	void UsersOffline(const vector<int>& userIds);

	/* @brief	返回类成员_evictor
	 * @return	_evictor
	 */
	OnlineEvictor& getEvictor();

private:
	friend class MyUtil::Singleton<OnlineManager>;
	OnlineEvictor _evictor;
};

	
};
};

#endif
