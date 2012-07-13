/*
 * DistSearchLogicAdapter.h
 *
 *  Created on: 2010-12-13
 *      Author: lijie
 */

#ifndef DISTSEARCHLOGICADAPTER_H_
#define DISTSEARCHLOGICADAPTER_H_

#include "TopicI.h"
#include "AdapterI.h"
#include "TaskManager.h"
#include <IceUtil/RWRecMutex.h>
#include <SearchCache.h>

namespace xce {
namespace searchcache {

class DistSearchLogicAdapter: public MyUtil::Singleton<DistSearchLogicAdapter>, public MyUtil::ReplicatedClusterAdapterI<SearchCacheManagerPrx> {

public:

	DistSearchLogicAdapter();

	void load(int id);
	void load(const MyUtil::IntSeq& ids);
  void update(const int id, const std::string tableName, const std::map<string, string>& fieldPairMap);

  void setValid(int service_mod, int index, bool newState);
  void isValidTest(int service_mod, std::vector<SearchCacheManagerPrx>& proxies);
protected:
	bool isValid(const Ice::ObjectPrx& proxy);
private:
	class InvokeLoad: virtual public IceUtil::Thread {
		public:
			void invoke(int userId) {
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				_cache.push_back(userId);
				if (_cache.size() >= 200) {
					_mutex.notify();
				}
			}
			void invoke(const MyUtil::IntSeq& userIds) {
				if (userIds.empty()) {
					return;
				}
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				_cache.insert(_cache.end(), userIds.begin(), userIds.end());
				if (_cache.size() >= 200) {
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
							if (_cache.size() < 200) {
								_mutex.timedWait(IceUtil::Time::seconds(1));
							}
							batch.swap(_cache);
						}
						MyUtil::TaskManager::instance().execute(new SearchCacheLoadTask(batch));
					} catch (...) {
						MCE_WARN("InvokeReload main loop exception");
					}
				}
			}
		private:
			MyUtil::IntSeq _cache;
			IceUtil::Monitor<IceUtil::Mutex> _mutex;
		};



	class InvokeUpdate: virtual public IceUtil::Thread {
		public:
			void invoke(const int id, const std::string tableName, const std::map<string, string>& fieldPairMap) {
                UpdateUnit updateUnit;
                updateUnit.id = id;
                updateUnit.table= tableName;
                if(0 < fieldPairMap.size())
                {
                    std::map<string, string>::const_iterator iter;
                    for(iter=fieldPairMap.begin(); iter!=fieldPairMap.end(); iter++)
                    {
                        updateUnit.fieldPairMap[iter->first] = iter->second;
                    }

                }
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				_cache.push_back(updateUnit);
				if (_cache.size() >= 200) {
					_mutex.notify();
				}
			}

		protected:
			virtual void run() {
				while (true) {
					try {
						UpdateUnitSeq batch;
						{
							IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
							if (_cache.size() < 200) {
								_mutex.timedWait(IceUtil::Time::seconds(1));
							}
							batch.swap(_cache);
						}
						MyUtil::TaskManager::instance().execute(new SearchCacheUpdateTask(batch));
					} catch (...) {
						MCE_WARN("InvokeReupdate main loop exception");
					}
				}
			}
		private:
			UpdateUnitSeq _cache;
			IceUtil::Monitor<IceUtil::Mutex> _mutex;
		};

	InvokeLoad _invokeLoad;
	InvokeUpdate _invokeUpdate;

	vector<SearchCacheManagerPrx> _managers;

	class SearchCacheLoadTask: public MyUtil::Task {
	public:
		SearchCacheLoadTask(MyUtil::IntSeq& ids):Task(10) {_ids.swap(ids);}
		virtual ~SearchCacheLoadTask() {}
		virtual void handle();
	private:
		MyUtil::IntSeq _ids;
	};

    class SearchCacheUpdateTask: public MyUtil::Task {
	public:
		SearchCacheUpdateTask(UpdateUnitSeq& updateUnits):Task(10) {_updateUnits.swap(updateUnits);}
		virtual ~SearchCacheUpdateTask() {}
		virtual void handle();
	private:
		UpdateUnitSeq _updateUnits;
	};
	void doLoad(const MyUtil::IntSeq& ids);
public:
	void doUpdate(const UpdateUnitSeq& updateUnits);

	virtual size_t cluster() {
	  return 4;
	}
};

}
}

#endif /* SEARCHCAHCEADAPTER_H_ */
