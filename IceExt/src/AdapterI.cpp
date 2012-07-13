#include "AdapterI.h"

using namespace MyUtil;
using namespace std;

AdapterI::AdapterI() :
	_cluster(0) {

}

AdapterI::~AdapterI() {

}

void AdapterI::configure(const ChannelPtr& channel) {

	_channel = channel;

	_name = name();
	_endpoints = endpoints();
	_cluster = cluster();

	initialize();
}

Ice::ObjectPtr AdapterI::get(Ice::Int category, Ice::Long id, int timeout) {
	return locate(id, timeout)->locateObject(category, id);
}

ObjectResultPtr AdapterI::get(Ice::Int category, const MyUtil::LongSeq& ids,
		bool parallel, int timeout) {
	if (_cluster <= 0) {
		try {
			return locate(0, timeout)->locateObjects(category, ids);
		} catch (Ice::Exception& e) {
			MCE_WARN("ServiceAdapter::get -> locateObjects err:" << e);
		}
		return new ObjectResult;
	}

	if (parallel) {
		CollectorPtr action = new ParallelCollector(*this, _cluster, category,
				ids, timeout);
		return action->execute();
	} else {
		return SerialCollector(*this, _cluster, category, ids, timeout).execute();
	}
}

void AdapterI::add(Ice::Int category, Ice::Long id, const Ice::ObjectPtr& o) {
	if (_cluster <= 0) {
		locate(0)->addObject(category, id, o);
	} else {
		locate(id)->addObject(category, id, o);
	};
}

void AdapterI::add(Ice::Int category, const ObjectMap& os) {
	if (_cluster <= 0) {
		locate(0)->addObjects(category, os);
	} else {
		map<int, ObjectMap> targets;
		for (ObjectMap::const_iterator it = os.begin(); it != os.end(); ++it) {
			targets[abs(it->first) % _cluster][it->first] = it->second;
		}
		for (map<int, ObjectMap>::iterator it = targets.begin(); it
				!= targets.end(); ++it) {
			locate(it->first)->addObjects(category, it->second);
		}
	}
}

void AdapterI::remove(Ice::Int category, Ice::Long id) {
	if (_cluster <= 0) {
		locate(0)->removeObject(category, id);
	} else {
		locate(id)->removeObject(category, id);
	};
}
void AdapterI::remove(Ice::Int category, const MyUtil::LongSeq& ids) {
	if (_cluster <= 0) {
		locate(0)->removeObjects(category, ids);
	} else {
		map<int, LongSeq> targets;
		for (size_t i = 0; i < ids.size(); ++i) {
			targets[abs(ids.at(i)) % _cluster].push_back(ids.at(i));
		}
		for (map<int, LongSeq>::iterator it = targets.begin(); it
				!= targets.end(); ++it) {
			locate(it->first)->removeObjects(category, it->second);
		}
	}
}

bool AdapterI::has(Ice::Int category, Ice::Long id, int timeout) {
	return locate(0, timeout)->hasObject(category, id);
}

bool AdapterI::has(Ice::Int category, const MyUtil::LongSeq& ids, int timeout) {
	if (_cluster <= 0) {
		return locate(0, timeout)->hasObjects(category, ids);
	} else {
		map<int, LongSeq> targets;
		for (size_t i = 0; i < ids.size(); ++i) {
			targets[abs(ids.at(i)) % _cluster].push_back(ids.at(i));
		}
		for (map<int, LongSeq>::iterator it = targets.begin(); it
				!= targets.end(); ++it) {
			if (!locate(it->first, timeout)->hasObjects(category, it->second)) {
				return false;
			}
		}
		return true;
	}
}

ObjectCachePrx AdapterI::locate(Ice::Int id, int timeout) {
	if (timeout > 0) {
		ObjectCachePrx prx =
				locate<ObjectCachePrx> (_caches, "SC", id, TWO_WAY);
		return ObjectCachePrx::uncheckedCast(prx->ice_timeout(timeout));
	} else {
		return locate<ObjectCachePrx> (_caches, "SC", id, TWO_WAY);
	}
}

//***************************************************************************
void ReplicatedAdapterI::initialize(){
	_validate_thread = new ValidateThread(*this);
	_validate_thread->start(128*1024).detach();
}
void ReplicatedAdapterI::ensure_replicated_proxy_in_cache(
		const Ice::ObjectPrx& replicated) {
	ReplicatedProxyCache::iterator it = _replicated_proxies_cache .find(
			replicated);
	if (it != _replicated_proxies_cache.end())
		return;
	_replicated_proxies_cache[replicated] = make_pair(ProxySeq(),
			_channel->findAllReplicas(replicated));
}

Ice::ObjectPrx ReplicatedAdapterI::select_best_proxy(
		const Ice::ObjectPrx& replicated) {
	IceUtil::Mutex::Lock lock(_replicated_proxies_mutex);
	ensure_replicated_proxy_in_cache(replicated);
	ReplicatedProxyCache::iterator it = _replicated_proxies_cache.find(
			replicated);
	if (it == _replicated_proxies_cache.end())
		return 0;
	int valid_size = it->second.first.size();
	int invalid_size = it->second.second.size();
	MCE_DEBUG("ReplicatedAdapterI::select_best_proxy valid: " << valid_size << " invalid: " << invalid_size);
	if (valid_size > 0) {
		return it->second.first.at(rand() % valid_size);
	} else if (invalid_size > 0) {
		return it->second.second.at(rand() % invalid_size);
	} else {
		return replicated;
	}
}

vector<Ice::ObjectPrx> ReplicatedAdapterI::select_all_proxy(const Ice::ObjectPrx& replicated) {
	vector<Ice::ObjectPrx> results;
	IceUtil::Mutex::Lock lock(_replicated_proxies_mutex);
	ensure_replicated_proxy_in_cache(replicated);
	ReplicatedProxyCache::iterator it=_replicated_proxies_cache.find(replicated);
	if(it==_replicated_proxies_cache.end()){
		MCE_DEBUG("ReplicatedAdapterI::select_all_proxy returns empty");
		return results;
	}
	MCE_DEBUG("ReplicatedAdapterI::select_all_proxy valid: " << it->second.first.size());
	MCE_DEBUG("ReplicatedAdapterI::select_all_proxy invalid: " << it->second.second.size());
	for(vector<Ice::ObjectPrx>::iterator itr=it->second.first.begin();itr!=it->second.first.end();++itr){
		results.push_back(*itr);
	}
	for(vector<Ice::ObjectPrx>::iterator itr=it->second.second.begin();itr!=it->second.second.end();++itr){
		results.push_back(*itr);
	}
	MCE_DEBUG("ReplicatedAdapterI::select_all_proxy returns " << results.size());
	return results;
}

void ReplicatedAdapterI::ValidateThread::run() {
	set<string> noendpointset;
	while (true) {
		sleep(1);
		try {
			ProxySeq keys;
			{
				IceUtil::Mutex::Lock lock(_replicated_adapter._replicated_proxies_mutex);
				for (ReplicatedProxyCache::const_iterator it =
					_replicated_adapter._replicated_proxies_cache.begin(); it
						!= _replicated_adapter._replicated_proxies_cache.end(); ++it) {
					keys.push_back(it->first);
				}
			}
			ReplicatedProxyCache new_cache;
			for (ProxySeq::const_iterator it = keys.begin(); it != keys.end(); ++it) {
				ProxySeqPair validate_result;
				ProxySeq all_replicas = _replicated_adapter._channel->findAllReplicas(*it);
				for (ProxySeq::size_type x = 0; x < all_replicas.size(); ++x) {
					try {
						if (_replicated_adapter.isValid(all_replicas[x]->ice_twoway())) {
							validate_result.first.push_back(all_replicas[x]);
						} else {
							validate_result.second.push_back(all_replicas[x]);
						}
						if (noendpointset.count(all_replicas[x]->ice_toString()) > 0) {
							noendpointset.erase(all_replicas[x]->ice_toString());
						}
					} catch (const Ice::NoEndpointException& e) {
						if (noendpointset.count(all_replicas[x]->ice_toString()) <= 0) {
							MCE_WARN("ReplicatedAdapterI::ValidateThread::run() Ice::Ex while validating " << e << " for "<< all_replicas[x]->ice_toString());
							noendpointset.insert(all_replicas[x]->ice_toString());
						} else {
							MCE_INFO("ReplicatedAdapterI::ValidateThread::run() Ice::Ex while validating " << e << " for "<< all_replicas[x]->ice_toString());
						}
					} catch (Ice::Exception& e) {
						//ensure timeout exception will not break whole thing.
						MCE_WARN("ReplicatedAdapterI::ValidateThread::run() Ice::Ex while validating " << e);
					} catch (...) {
						//ensure timeout exception will not break whole thing.
						MCE_WARN("ReplicatedAdapterI::ValidateThread::run() some exception while validating " << *it);
					}
				}
				new_cache.insert(make_pair(*it, validate_result));
			}

			{
				IceUtil::Mutex::Lock lock(_replicated_adapter._replicated_proxies_mutex);
				for (ReplicatedProxyCache::iterator it =
					_replicated_adapter._replicated_proxies_cache.begin(); it
						!= _replicated_adapter._replicated_proxies_cache.end(); ++it) {
					ReplicatedProxyCache::iterator new_it = new_cache.find(
							it->first);
					if (new_it == new_cache.end())
						continue;
					it->second.first.swap(new_it->second.first);
					it->second.second.swap(new_it->second.second);
					MCE_TRACE("ReplicatedAdapterI::ValidateThread::run " << it->first << " Befor:"<<new_it->second.first.size()<<" After:" << it->second.first.size() );
				}
			}
		} catch (...) {
			//Ensure this thread will not terminate.
			MCE_WARN("ReplicatedAdapterI::ValidateThread::run() some exception in main loop.");
		}
	}
}
//***************************************************************************
Collector::Collector(AdapterI& adapter, size_t cluster, Ice::Int category,
		const MyUtil::LongSeq& ids, int timeout) :
	_adapter(adapter), _cluster(cluster), _category(category), _ids(ids),
			_timeout(timeout) {
	for (size_t i = 0; i < ids.size(); ++i) {
		_targets[abs(ids.at(i)) % _cluster].push_back(ids.at(i));
	}
}

Collector::~Collector() {

}

//***************************************************************************
ParallelCollector::ParallelCollector(AdapterI& adapter, size_t cluster,
		Ice::Int category, const MyUtil::LongSeq& ids, int timeout) :
	Collector(adapter, cluster, category, ids, timeout), _count(0), _done(
			false) {

}

ObjectResultPtr ParallelCollector::execute() {
	for (map<int, LongSeq>::iterator it = _targets.begin(); it
			!= _targets.end(); ++it) {
		TaskManager::instance().execute(new CollectTask(this, it->first, _timeout));
		++_count;
	}
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
	if (_count > 0 && !timedWait(IceUtil::Time::milliSeconds(_timeout))) {
		MCE_WARN("ParallelCollector::finish -> count: " << _count << " missing");
	}
	_done = true;
	MCE_DEBUG("ParallelCollector::finish -> Count: " << _count << " Done: "
			<< _done);
	return new ObjectResult(_result);
}

void ParallelCollector::finish(const ObjectMap& data) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
	if (_done) {
		return;
	}
	_result.insert(data.begin(), data.end());
	if (--_count <= 0) {
		notify();
	}
}

//***************************************************************************
SerialCollector::SerialCollector(AdapterI& adapter, size_t cluster,
		Ice::Int category, const MyUtil::LongSeq& ids, int timeout) :
	Collector(adapter, cluster, category, ids, timeout) {
}

ObjectResultPtr SerialCollector::execute() {
	for (map<int, LongSeq>::iterator it = _targets.begin(); it
			!= _targets.end(); ++it) {
		try {
			ObjectResultPtr r =
					_adapter.locate(it->first, _timeout)->locateObjects(
							_category, it->second);
			_result.insert(r->data.begin(), r->data.end());
		} catch (Ice::Exception& e) {
			MCE_WARN("SerializeCallback::execute -> locateObjects err:" << e);
		}
	}
	return new ObjectResult(_result);
}

//***************************************************************************
ParallelCollector::CollectTask::CollectTask(ParallelCollectorPtr callback,
		int clusterId, int timeout) :
	MyUtil::Task(TASK_LEVEL_PARALLEL_GET), _action(callback),
			_index(clusterId), _timeout(timeout) {
}

void ParallelCollector::CollectTask::handle() {
	ObjectResultPtr r =
			_action->_adapter.locate(_index, _timeout)->locateObjects(
					_action->_category, _action->_targets[_index]);
	_action->finish(r->data);
}

