#ifndef _FEEDCONTENTMANAGER_H_
#define _FEEDCONTENTMANAGER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "RFeed.h"
#include "TaskManager.h"
#include "TaskManager.h"
#include "FeedMemcProxy/client/feed_memc_client.h"

namespace xce {
namespace feed {
class CheckTaskSizeTimer : public Timer, public Singleton<CheckTaskSizeTimer>{
public:
	CheckTaskSizeTimer():Timer(10*1000){
		from_memc_ = true;
	}
	void handle();
	bool FromMemc(){
		return from_memc_;
	}
private:
	bool	from_memc_;
};

class FeedContentAdapter: public MyUtil::AdapterI,
		public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel,
				FeedContentAdapter> {
public:
	FeedContentAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	  TaskManager::instance().scheduleRepeated(&CheckTaskSizeTimer::instance());
  }

	FeedContentDict getFeedDict(const MyUtil::LongSeq& feedIds,int uid=0);
	//Feed2WeightMap getWeight(const MyUtil::LongSeq& feedIds);

	//FeedContentDict getFeedDict2(const UpdateTimeSeq& seq);
	//Feed2WeightMap getWeight2(const UpdateTimeSeq& seq);
  FeedDataPtr getFeed(Ice::Long source, int stype, int actor,Ice::Long & feedid,int uid = 0);
  FeedDataPtr getFeed(Ice::Long feedid,int uid=0);
  FeedDataPtr getFeedFromMemcLoader(Ice::Long feedid);

	void replaceFeedReply(const MyUtil::LongSeq& feedIds,
			const FeedReplyPtr& reply);
	int addFeedContent(const FeedDataPtr& feedData);
	int addFeedContentWithReply(const FeedDataPtr& feedData,
			const FeedReplyPtr& reply);
	void removeFeed(long feedId);
	void removeFeeds(const MyUtil::LongSeq& feedIds);

	void recoveryContent(long feedId);

	void ReplaceXML(const Ice::LongSeq & feedIds ,const string & xml );
	//	void removeFeedReply(long feedId, long replyId);

	FeedContentManagerPrx getManager(Ice::Long id);
	FeedContentManagerPrx getManagerOneway(Ice::Long id);
protected:
	virtual string name() {
		return "FeedContent";
	}
	virtual string endpoints() {
		return "@FeedContent";
	}
	virtual size_t cluster() {
		return 10;
	}

	FeedContentManagerPrx getTempManager();

	vector<FeedContentManagerPrx> _managersOneway;
	vector<FeedContentManagerPrx> _managers;
};
template<class T, class SEQ> class CollectTask;
template<class T, class SEQ>
class ParallelQuery: public IceUtil::Shared {
public:
	T execute(size_t cluster, int category, const MyUtil::LongSeq& ids,
			int timeout) {
		_count = 0;
		_done = false;
		vector<MyUtil::LongSeq> targets(cluster);
		for (size_t i = 0; i < ids.size(); ++i) {
			unsigned int id = (unsigned Ice::Long)ids.at(i) % cluster;
			targets[id].push_back(ids.at(i));
		}
		for (size_t i = 0; i < targets.size(); ++i) {
			if (!targets[i].empty()) {
				MyUtil::TaskManager::instance().execute(new CollectTask<T,
						MyUtil::LongSeq> (this, i, targets[i], timeout));
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
		return _result;
	}

	/*
	T execute(size_t cluster, int category, const UpdateTimeSeq& ids,
			int timeout) {
		_count = 0;
		_done = false;
		vector<UpdateTimeSeq> targets(cluster);
		for (size_t i = 0; i < ids.size(); ++i) {
			unsigned int id = (unsigned Ice::Long)ids.at(i).feed % cluster;
			targets[id].push_back(ids.at(i));
		}
		for (size_t i = 0; i < targets.size(); ++i) {
			if (!targets[i].empty()) {
				MyUtil::TaskManager::instance().execute(new CollectTask<T,
						UpdateTimeSeq> (this, i, targets[i], timeout));
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
		return _result;
	}*/

	typedef IceUtil::Handle<ParallelQuery<T, SEQ> > Ptr;
private:
	friend class CollectTask<T, SEQ> ;
	void finish(const T& data) {
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
	T _result;
	int _count;
	bool _done;
};

template<class T, class SEQ>
class CollectTask: public MyUtil::Task {
public:
	CollectTask(typename ParallelQuery<T, SEQ>::Ptr callback, int index,
			const SEQ& ids, int timeout) :
		MyUtil::Task(MyUtil::TASK_LEVEL_PARALLEL_GET), _callback(callback),
				_index(index), _ids(ids), _timeout(timeout) {
		;
	}
	virtual void handle() {
		T dict;
		SEQ seq;
		_handle(dict,seq);
		_callback->finish(dict);
	}
private:

	void _handle(FeedContentDict& t, MyUtil::LongSeq& ids) {
	  if(_ids.empty()){
      return;
    }
    //if(CheckTaskSizeTimer::instance().FromMemc()){
    if(false){
      try{
        timeval begin,end;
        gettimeofday(&begin, NULL);
        t = FeedMemcClient::instance().GetFeedDict(_ids);
        gettimeofday(&end, NULL);
        float cost = 1000000*(end.tv_sec-begin.tv_sec)+end.tv_usec-begin.tv_usec;
        cost/=1000;
        MCE_INFO("CollectTask::handle --> FeedMemcClient.getFeedDict index:" << _index << " ids:" << _ids.size() << " res:" << t.size() << " cost:" << cost);
      }catch(...){
			  MCE_WARN("CollectTask::handle --> FeedMemcClient.getFeedDict index:" << _index);
      }
    }else{	
      try {
			  t = FeedContentAdapter::instance().getManager(_index)->getFeedDict(_ids);
		  } catch (Ice::Exception& e) {
			  MCE_WARN("CollectTask::handle --> FeedContentAdapter.getFeedDict index:"<<_index<<" err:"<<e);
		  }
    }
	}

	/*void _handle(Feed2WeightMap& t, MyUtil::LongSeq& ids) {
		try {
			t = FeedContentAdapter::instance().getManager(_index)->getWeight(
					_ids);
		} catch (Ice::Exception& e) {
			MCE_WARN("CollectTask::handle --> FeedContentAdapter.getWeight index:"<<_index<<" err:"<<e);
		}
	}

	void _handle(FeedContentDict& t,UpdateTimeSeq& seq) {
		try {
			t = FeedContentAdapter::instance().getManager(_index)->getFeedDict2(
					_ids);
		} catch (Ice::Exception& e) {
			MCE_WARN("CollectTask::handle --> FeedContentAdapter.getFeedDict2 index:"<<_index<<" err:"<<e);
		}
	}

	void _handle(Feed2WeightMap& t, UpdateTimeSeq& seq) {
		try {
			t = FeedContentAdapter::instance().getManager(_index)->getWeight2(
					_ids);
		} catch (Ice::Exception& e) {
			MCE_WARN("CollectTask::handle --> FeedContentAdapter.getWeight2 index:"<<_index<<" err:"<<e);
		}
	}*/

	typename ParallelQuery<T, SEQ>::Ptr _callback;
	int _index;
	SEQ _ids;
	int _timeout;
};


}
;
}
;

#endif /* _RFEEDCONTENTMANAGER_H_ */
