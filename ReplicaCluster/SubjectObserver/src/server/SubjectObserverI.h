#ifndef __SUBJECT_OBSERVER_I_H__
#define __SUBJECT_OBSERVER_I_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "SubjectObserver.h"
#include "ServiceI.h"
#include <boost/date_time.hpp>

namespace MyUtil
{
using namespace std;

const string SUBJECT_OBSERVER = "SO";

const int TASK_LEVEL_NOTIFY = 1;

class SubjectObserverI : public SubjectObserver,
	public Singleton<SubjectObserverI>,
	public IceUtil::Mutex {
public:
	SubjectObserverI(){}
	virtual ~SubjectObserverI(){}

	virtual SbjTopicPtr getSbjTopic(const string& name, const Ice::Current& = Ice::Current());
	virtual SbjTopicPtr query(const string& name, const Ice::Current& = Ice::Current());

	/* @brief 	注册一个subscriber
	 * @param	name		topic名
	 * @param	subscriber	待注册的subscriber的proxy
	 */
	virtual void subscribe(const string& name, const SbjSubscriberPrx& subscriber, const Ice::Current& = Ice::Current());

	/* @brief	注销一个subscriber
	 * @param	name		topic名
	 * @param	subscriber	待注销的subscriber的proxy
	 */
	virtual void unsubscribe(const string& name, const SbjSubscriberPrx& subscriber, const Ice::Current& = Ice::Current());

	/* @brief	对所有subscriber进行"通知"类型的操作(不关心返回值)
	 * @param	name		topic名
	 * @param	state		进行通知的参数
	 */
	virtual void notifyall(const string& name, const SbjStatePtr& state, const Ice::Current& = Ice::Current());

	/* @brief	对所有subscriber进行获取返回值的操作
	 * @param	name		topic名
	 * @param	state		进行通知的参数
	 * @retrun	从各个subscriber返回值的列表(无顺序,如果需要区分,需要在具体业务的返回值中加标识)
	 */
	virtual SbjStateSeq fetch(const string& name, const SbjStatePtr& state, const Ice::Current& = Ice::Current());
private:
	void update(const SbjTopicPtr& sbjt, const SbjStatePtr& state, const Ice::Context& ctx = Ice::Context());
	void get(SbjStateSeq& receives, const SbjTopicPtr& sbjt, const SbjStatePtr& state, const Ice::Context& ctx = Ice::Context());
	void doUnsubscribe(const string& name, const SbjSubscriberPrx& subscriber);
};

class NotifyTask : public MyUtil::Task {
public:
	NotifyTask(SubjectObserver& observer, const SbjTopicPtr& topic,
			const SbjSubscriberSeq& subscribers, const SbjStatePtr& state, const Ice::Context& ctx) :
		MyUtil::Task(TASK_LEVEL_NOTIFY),
		_observer(observer), 
		_topic(topic), 
		_subscribers(subscribers),
		_state(state),
		_ctx(ctx) {
	}
	virtual void handle();
private:
	SubjectObserver& _observer;
	SbjTopicPtr _topic;
	SbjSubscriberSeq _subscribers;
	SbjStatePtr _state;
	Ice::Context _ctx;
};

};

#endif
