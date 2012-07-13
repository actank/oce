

#include "NotifyContentAdapter.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "TaskManager.h"
#include "Notify.h"

using namespace xce::notify;
using namespace std;
using namespace MyUtil;



NotifyContentManagerPrx NotifyContentAdapter::getManager(Ice::Long id){
	return locate<NotifyContentManagerPrx> (_managers, "M", id, TWO_WAY);
}

NotifyContentManagerPrx NotifyContentAdapter::getManagerOneway(Ice::Long id){
	return locate<NotifyContentManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}

NotifyContentDict NotifyContentAdapter::getNotifyDict(const MyUtil::LongSeq& notifyIds){
	ParallelQueryPtr query = new ParallelQuery();
	MCE_DEBUG("NotifyContentAdapter::getNotifyDict");
	NotifyContentDict contents = query->execute(_cluster,notifyIds,300);
	MCE_DEBUG("NotifyContentAdapter::getNotifyDict  end");
	return contents;
}
void NotifyContentAdapter::addNotify(const NotifyContentPtr& notify){
	getManagerOneway(notify->id)->addNotify(notify);
}

// void NotifyContentAdapter::removeReply(Ice::Long replyId){
// 	getManager(replyId)->removeReply(replyId);
// }
// void NotifyContentAdapter::removeReplys(const MyUtil::LongSeq& replyIds){
// 	map<int, MyUtil::LongSeq> ridMap;
// 	for (size_t i = 0; i < replyIds.size(); ++i) {
// 		ridMap[replyIds.at(i) % cluster()].push_back(replyIds.at(i));
// 	}
// 	for (map<int, MyUtil::LongSeq>::iterator it = ridMap.begin(); it
// 			!= ridMap.end(); ++it) {
// 		getManagerOneway(it->first)->removeReplys(it->second);
// 	}
// }

NotifyContentDict ParallelQuery::execute(size_t cluster, const MyUtil::LongSeq& ids, int timeout) {
	MCE_DEBUG("ParallelQuery::execute");
	_count = 0;
	_done = false;
	vector<MyUtil::LongSeq> targets(cluster);
	for (size_t i = 0; i < ids.size(); ++i) {
		unsigned int id = (unsigned Ice::Long)ids.at(i) % cluster;
		targets[id].push_back(ids.at(i));
	}
	for (size_t i = 0; i < targets.size(); ++i) {
		if (!targets[i].empty()) {
			MyUtil::TaskManager::instance().execute(
				new CollectTask(this, i, targets[i], timeout)
				);
			++_count;
		}
	}
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	//StatFunc stat("ParallelQuery::execute -->timeout");
	if (_count > 0 && !_mutex.timedWait(
			IceUtil::Time::milliSeconds(timeout))) {
		MCE_WARN("ParallelQuery::finish -> count: " << _count << " missing");
	}
	_done = true;
	MCE_DEBUG("ParallelQuery::execute --> _result size="<<_result.size());
	
	return _result;
}
