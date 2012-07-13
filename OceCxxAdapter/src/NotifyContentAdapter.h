#ifndef _NOTIFY_CONTENT_ADAPTER_H_
#define _NOTIFY_CONTENT_ADAPTER_H_

#include "AdapterI.h"
#include "Notify.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "TaskManager.h"

namespace xce {
namespace notify {

class NotifyContentAdapter: public MyUtil::AdapterI,
								 public MyUtil::AdapterISingleton<MyUtil::NotifyChannel, NotifyContentAdapter> {
public:
	NotifyContentAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	NotifyContentManagerPrx getManager(Ice::Long id);
	NotifyContentManagerPrx getManagerOneway(Ice::Long id);

	NotifyContentDict getNotifyDict(const MyUtil::LongSeq& notifyIds);
	void addNotify(const NotifyContentPtr& notify);

protected:
	virtual string name() {
		return "NotifyContent";
	}
	virtual string endpoints() {
		return "@NotifyContent";
	}
	virtual size_t cluster() {
		return 5;
	}

	vector<NotifyContentManagerPrx> _managersOneway;
	vector<NotifyContentManagerPrx> _managers;
};

/////////////////////////////////////////////////////////////////

class ParallelQuery: public IceUtil::Shared {
public:
	NotifyContentDict execute(size_t cluster, const MyUtil::LongSeq& ids, int timeout);

	typedef IceUtil::Handle<ParallelQuery> ParallelQueryPtr;
private:
	friend class CollectTask ;
	void finish(const NotifyContentDict& data) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
		if (_done) {
			return;
		}
		_result.insert(data.begin(), data.end());
		if (--_count <= 0) {
			_mutex.notify();
		}
	}

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	NotifyContentDict _result;
	int _count;
	bool _done;
};
typedef IceUtil::Handle<ParallelQuery> ParallelQueryPtr;

class CollectTask: public MyUtil::Task {
public:
	CollectTask(ParallelQuery::ParallelQueryPtr callback,
				int index, const MyUtil::LongSeq& ids, int timeout) :
		MyUtil::Task(MyUtil::TASK_LEVEL_PARALLEL_GET), _callback(callback),
				_index(index), _ids(ids), _timeout(timeout) {
		;
	}
	virtual void handle() {
		NotifyContentDict dict;
		try{
			dict = NotifyContentAdapter::instance().getManager(_index)->getNotifyDict(_ids);
		} catch (Ice::Exception& e){
			MCE_WARN("CollectTask::handle --> "<<e<<"  "<<__FILE__<<":"<<__LINE__);
		}
		_callback->finish(dict);
	}
private:
	ParallelQuery::ParallelQueryPtr _callback;
	int _index;
	MyUtil::LongSeq _ids;
	int _timeout;
};

}
}

#endif // _NOTIFY_CONTENT_ADAPTER_H_
