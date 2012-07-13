/*
 * UserCacheClient.h
 *
 *  Created on: 2008-12-29
 *      Author: bbc
 */

#ifndef _ONLINEEVENTMEDIATORADAPTER_H_
#define _ONLINEEVENTMEDIATORADAPTER_H_

#include "OnlineEventMediator.h"
#include "AdapterI.h"
#include "TaskManager.h"

namespace xce {
namespace mediator{
namespace onlineevent {

class BatchEventInvoker : virtual public IceUtil::Thread {
public:
	void invoke(const MyUtil::Int2IntMap& data);
protected:
	virtual void run();
private:
	MyUtil::Int2IntMap _cache;
	IceUtil::Monitor<IceUtil::Mutex> _mutex;
};

class OnlineEventMediatorAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton<MyUtil::OceChannel, OnlineEventMediatorAdapter> {
public:
	OnlineEventMediatorAdapter();
	void sendOnlineEvent(int userId, int online_type);
	void sendOnlineEvent(const MyUtil::Int2IntMap& batch_online);
	void directSendOnlineEvent(const MyUtil::Int2IntMap& batch_online);
private:
	BatchEventInvoker _invoker;

	virtual string name() {
		return "OnlineEventMediator";
	}
	virtual string endpoints() {
		return "@OnlineEventMediator";
	}
	virtual size_t cluster() {
		return 0;
	}
};

}
}
}
#endif /* _ONLINEEVENTMEDIATORADAPTER_H_ */
