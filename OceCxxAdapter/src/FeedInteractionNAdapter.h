#ifndef RESTMEDIATORADAPTER_H_
#define RESTMEDIATORADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedInteraction.h"

namespace xce {
namespace feed {

using namespace MyUtil;

enum InteractionType{INTERACTION_CLICK=1, INTERACTION_REPLY=2, INTERACTION_VIEW=3};

class FeedInteractionNAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedChannel, FeedInteractionNAdapter> {
public:
  FeedInteractionNAdapter() {
	  uptime_ = -1;
    reset();
	}
  void AddFeedInteraction(long feed, int actor, int range, int time); 
  void Inc(InteractionType type, Ice::Long feed, int count=1);
  void IncBatch(InteractionType type, MyUtil::Long2IntMap feedCounts);

  FeedInteractionCountPtr GetFeedInteractionCount(Ice::Long feed, bool load); 
  FeedInteractionCountSeq GetFeedInteractionCountSeq(MyUtil::LongSeq feeds, bool load);
  FeedInteractionCountSeq GetFeedInteractionCountSeq2(MyUtil::LongSeq feeds, bool load);
  void IncHitCount(Ice::Long feedid, string content);

  
  FeedInteractionCountSeq GetFeedInteractionCountSeqImpl(MyUtil::LongSeq feeds, bool load);
  FeedInteractionCountSeq GetFeedInteractionCountSeqImpl2(MyUtil::LongSeq feeds, bool load);

  FeedInteractionPrx getManagerOneway(Ice::Long id);
	FeedInteractionPrx getManager(Ice::Long id);

protected:
  void reset(){
    _managers.clear();
    _managersOneway.clear();
    _managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }
	virtual string name() {
		return "FeedInteractionN";
	}
	virtual string endpoints() {
		return "@FeedInteractionN";
	}
	virtual size_t cluster() {
		return 10;
	}
	
  vector<FeedInteractionPrx> _managersOneway;
  vector<FeedInteractionPrx> _managers;
  //map<int, vector<FeedInteractionPrx> > _managers;
  int uptime_;
};

template<class T, class SEQ> class CollectTaskInteraction;
template<class T, class SEQ>
class ParallelQueryInteraction : public IceUtil::Shared {
public:
	T execute(size_t cluster, int category, const MyUtil::LongSeq& ids, int timeout, bool load){
	  MCE_INFO("ParallelQueryInteraction::execute -> size:" << ids.size() << " tasksize:" << MyUtil::TaskManager::instance().size(MyUtil::TASK_LEVEL_PARALLEL_GET));
		_count = 0;
		_done = false;
		vector<MyUtil::LongSeq> targets(cluster);
		for (size_t i = 0; i < ids.size(); ++i) {
			unsigned int id = (unsigned Ice::Long)ids.at(i) % cluster;
			targets[id].push_back(ids.at(i));
		}
		for (size_t i = 0; i < targets.size(); ++i) {
			if (!targets[i].empty()) {
	      //MCE_INFO("ParallelQueryInteraction::execute -> new task index:" << i);
				MyUtil::TaskManager::instance().execute(new CollectTaskInteraction<T,
						MyUtil::LongSeq> (this, i, targets[i], timeout, load));
				++_count;
			}
		}
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
		if (_count > 0 && !_mutex.timedWait(
				IceUtil::Time::milliSeconds(timeout))) {
			MCE_WARN("ParallelQueryInteraction::finish -> count: " << _count << " missing");
		}
		_done = true;
		return _result;
	}
	typedef IceUtil::Handle<ParallelQueryInteraction<T, SEQ> > Ptr;
private:
	friend class CollectTaskInteraction<T, SEQ> ;
	void finish(const T& data) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
		if (_done) {
			return;
		}
		_result.insert(_result.end(), data.begin(), data.end());
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
class CollectTaskInteraction: public MyUtil::Task {
public:
	CollectTaskInteraction(typename ParallelQueryInteraction<T, SEQ>::Ptr callback, int index,
			const SEQ& ids, int timeout, bool load) :
		MyUtil::Task(MyUtil::TASK_LEVEL_PARALLEL_GET), _callback(callback),
				_index(index), _ids(ids), _timeout(timeout), _load(load) {
		;
	}
	virtual void handle() {
		T dict;
		SEQ seq;
		_handle(dict,seq);
		_callback->finish(dict);
	}
private:
	void _handle(FeedInteractionCountSeq& t, MyUtil::LongSeq& ids) {
    timeval begin, end;
    gettimeofday(&begin, NULL);
		bool timeout = false;
    try {
			//FeedInteractionPrx prx = FeedInteractionNAdapter::instance().getManager(_index);
			t = FeedInteractionNAdapter::instance().GetFeedInteractionCountSeqImpl2(_ids, _load);
      //if(_ids.empty()){
      //  return;
      //}
      //t = prx->GetFeedInteractionCountSeq(_ids, _load);
		} catch (Ice::Exception& e) {
			//MCE_WARN("CollectTaskInteraction::handle --> FeedInteractionNAdapter.GetFeedInteractionCountSeq index:"<<_index<<" err:"<<e);
      timeout = true;
      gettimeofday(&end, NULL);
      float timeuse = 1000000*(end.tv_sec-begin.tv_sec)+end.tv_usec-begin.tv_usec;
      timeuse/=1000;
		  MCE_WARN("CollectTaskInteraction::handle --> GetFeedInteractionCountSeq end index:" 
      << _index << " size:" << _ids.size() << " res:" << t.size() << " time:" << timeuse << " load:" << _load);
		}
	  //MCE_INFO("CollectTaskInteraction::handle --> GetFeedInteractionCountSeq index:"<<_index<<" size:"<<_ids.size()<<"res:"<<t.size());
	}
  typename ParallelQueryInteraction<T, SEQ>::Ptr _callback;
	int _index;
	SEQ _ids;
	int _timeout;
  bool _load;
};

}
}

#endif /*RESTMEDIATORADAPTER_H_*/
