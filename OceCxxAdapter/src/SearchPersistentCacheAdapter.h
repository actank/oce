/*
 * SearchPersistentCacheAdpter.h
 *
 *  Created on: 2009-09-23
 *      Author: scq 
 */

#ifndef SEARCHPERSISTENTCACHEADAPTER_H_
#define SEARCHPERSISTENTCACHEADAPTER_H_

#include "TopicI.h"
#include "AdapterI.h"
#include "TaskManager.h"
#include <IceUtil/RWRecMutex.h>
#include <SearchCache.h>

namespace xce {
namespace searchcache {

class SearchPersistentCacheAdapter: public MyUtil::ReplicatedAdapterI,
		public MyUtil::AdapterISingleton<MyUtil::SearchChannel, SearchPersistentCacheAdapter> {
public:

	SearchPersistentCacheAdapter();

	void load(int id);
	void load(const MyUtil::IntSeq& ids);

	Ice::Int getFriendCount(int userid);
	MyUtil::IntSeq getFriendList(int userid);

protected:
	bool isValid(const Ice::ObjectPrx& proxy);
private:
	class InvokeLoad: virtual public IceUtil::Thread {
		public:
			void invoke(int userId) {
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				_cache.push_back(userId);
				if (!_cache.empty()) {
					_mutex.notify();
				}
			}
			void invoke(const MyUtil::IntSeq& userIds) {
				if (userIds.empty()) {
					return;
				}
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				_cache.insert(_cache.end(), userIds.begin(), userIds.end());
				if (!_cache.empty()) {
					_mutex.notify();
				}
			}
		protected:
			virtual void run() {
				while (true) {
					try {
						MyUtil::IntSeq batch;
						{
							IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
							if (_cache.empty()) {
								_mutex.wait();
								continue;
							}
							batch.swap(_cache);
						}
						SearchPersistentCacheAdapter::instance().doLoad(batch);
						sleep(1);
					} catch (...) {
						MCE_WARN("InvokeReload main loop exception");
					}
				}
			}
		private:
			MyUtil::IntSeq _cache;
			IceUtil::Monitor<IceUtil::Mutex> _mutex;
		};

	InvokeLoad _invokeLoad;

	vector<SearchPersistentCacheManagerPrx> _managers;

	//friend class InvokeLoad;
	void doLoad(const MyUtil::IntSeq& ids);

	virtual string name() {
		return "SearchPersistentCache";
	}
	virtual string endpoints() {
		return "@SearchPersistentCache";
	}
	virtual size_t cluster() {
		return 4;
	}
};

}
}

#endif /* SEARCHCAHCEADAPTER_H_ */
