#include "OnlineEventMediatorAdapter.h"

using xce::mediator::onlineevent::OnlineEventMediatorAdapter;
using xce::mediator::onlineevent::BatchEventInvoker;
using MyUtil::Int2IntMap;
using MyUtil::BATCH_ONE_WAY;

OnlineEventMediatorAdapter::OnlineEventMediatorAdapter() {
	_invoker.start(1024 * 1024).detach();
}

void OnlineEventMediatorAdapter::sendOnlineEvent(int userId, int online_type) {
	MCE_DEBUG("OnlineEventMediatorAdapter::sendOnlineEvent() + userId = " << userId << " onlineType : " << online_type);
	locate<OnlineEventMediatorManagerPrx> ("M", BATCH_ONE_WAY)->sendOnlineEventSingle(
			userId, online_type);
}
void OnlineEventMediatorAdapter::sendOnlineEvent(const Int2IntMap& batch_online) {

	_invoker.invoke(batch_online);
}

void OnlineEventMediatorAdapter::directSendOnlineEvent(const Int2IntMap& batch_online) {
	MCE_DEBUG("OnlineEventMediatorAdapter::sendOnlineEvent(const Int2IntMap& batch_online) size=" << batch_online.size());
	for (Int2IntMap::const_iterator it = batch_online.begin(); it != batch_online.end(); ++it) {
			MCE_DEBUG("OnlineEventMediatorAdapter::sendOnlineEvent(const Int2IntMap& batch_online) userID "<< it->first<< " onlineType : " << it->second);
		}

	locate<OnlineEventMediatorManagerPrx> ("M", BATCH_ONE_WAY)->sendOnlineEventBatch(
			batch_online);
}

void BatchEventInvoker::invoke(const MyUtil::Int2IntMap& data) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	bool needsNotify = _cache.empty();
	_cache.insert(data.begin(), data.end());
	if (needsNotify) {
		_mutex.notify();
	}
}

void BatchEventInvoker::run() {
	while (true) {
		try {
			MyUtil::Int2IntMap batch;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				if (_cache.empty()) {
					_mutex.wait();
					continue;
				}
				batch.swap(_cache);
			}
			OnlineEventMediatorAdapter::instance().directSendOnlineEvent(batch);
			sleep(1);
		} catch (...) {
			/** ensure the main loop will not exit **/
			// MCE_WARN("InvokeSetOnline main loop exception");
		}
	}

}
