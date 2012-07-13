#ifndef XCE_UTIL_CXX_EVICTOR_H_
#define XCE_UTIL_CXX_EVICTOR_H_

#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <list>
#include <map>
#include <vector>
#include <ext/hash_map>
#include "hash_map_string.h"

using namespace std;
using namespace __gnu_cxx;

namespace MyUtil {

#ifndef __LP64__
struct Int64_compare {
	bool operator()(const long long int a, const long long int b) const {
		union {long long int int64; unsigned int int32[2];}v1, v2;
		v1.int64 = a; v2.int64 = b;
		return( (v1.int32[0] == v2.int32[0]) && (v1.int32[1] == v2.int32[1]) );
	}
}; //end Class u64_compare

struct Int64_hash {
	size_t operator()(long long int value) const {
		union {long long int int64; unsigned int int32[2];}i;
		i.int64 = value;
		return i.int32[0] + i.int32[1];
	}
};
#endif

template<class _Key, class _Object, class _HashFn = hash<_Key> ,
		class _EqualKey = equal_to<_Key> > class SizedEvictor: public IceUtil::Shared {
protected:
	typedef list<_Key> EvictorQueue;
	struct Entry {
		_Object obj;
		typename EvictorQueue::iterator pos;
	};
#ifndef __LP64__
	typedef hash_map<_Key, Entry, Int64_hash, Int64_compare> EvictorMap;
#else
	typedef hash_map<_Key, Entry> EvictorMap;
#endif

public:
	SizedEvictor(int size = 1000) :
		_size(size) {
		;
	}
	virtual ~SizedEvictor() {
		;
	}

	void size(size_t size) {
		IceUtil::Mutex::Lock lock(_mutex);
		_size = size;
	}

	size_t size() {
		IceUtil::Mutex::Lock lock(_mutex);
		return _map.size();
	}

	_Object locate(const _Key& id, const _Object& def) {
		IceUtil::Mutex::Lock lock(_mutex);
		//Check if we have a object in the map already
		typename EvictorMap::iterator i = _map.find(id);
		bool newEntry = (i == _map.end());
		if (!newEntry) {
			// Got the entry already, dequeue the entry from
			// its current position.
			Entry& entry = i->second;
			_queue.erase(entry.pos);
			//			// Enqueue the entry at the front, so we got the LRU order.
			hook(entry, i);
			return entry.obj;
		} else {
			return def;
		}
	}

	_Object find(const _Key& id, const _Object& def) {
		IceUtil::Mutex::Lock lock(_mutex);
		//Check if we have a object in the map already
		typename EvictorMap::iterator i = _map.find(id);
		if (i != _map.end()) {
			// Got the entry already, dequeue the entry from
			// its current position.
			return i->second.obj;
		} else {
			return def;
		}
	}

	void add(const _Object& obj, const _Key& id) {
		IceUtil::Mutex::Lock lock(_mutex);
		Entry entry;
		entry.obj = obj;

		while (true) {
			pair<typename EvictorMap::iterator, bool> result = _map.insert(
					make_pair(id, entry));
			if (result.second) {
				hook(result.first->second, result.first);
				evict();
				break;
			}
			typename EvictorMap::iterator pos = _map.find(id);
			if (pos != _map.end()) {
				_queue.erase(pos->second.pos);
				_map.erase(pos);
			}
		}
	}

	_Object remove(const _Key& id) {
		_Object obj;
		IceUtil::Mutex::Lock lock(_mutex);
		typename EvictorMap::iterator pos = _map.find(id);
		if (pos != _map.end()) {
			obj = pos->second.obj;
			_queue.erase(pos->second.pos);
			_map.erase(pos);
		}
		return obj;
	}

	void clear() {
		IceUtil::Mutex::Lock lock(_mutex);
		_map.clear();
		_queue.clear();
	}

	bool has(const _Key& id) {
		IceUtil::Mutex::Lock lock(_mutex);
		return _map.find(id) != _map.end();
	}

protected:
	void evict() {
		// If the evictor queue has grown larger than the limit,
		// look at the excess elements to see whether any of them can be evicted.
		if (_size > 0) {
			while (_map.size() > _size) {
				_Key key = _queue.back();
				_queue.pop_back();
				_map.erase(key);
			}
		}
	}

	void hook(Entry& entry,const typename EvictorMap::iterator& mapit) {
		entry.pos = _queue.insert(_queue.begin(), mapit->first);
	}

protected:
	IceUtil::Mutex _mutex;
	EvictorMap _map;
	EvictorQueue _queue;
	size_t _size;
};

template<class Id, class Object> class TimedEvictor : public IceUtil::Shared {
protected:
	typedef vector<Object> ObjectSeq;
	struct Entry;
	typedef map<Id, Entry> EvictorMap;
	typedef list<typename EvictorMap::iterator> EvictorQueue;

	struct Entry {
		Object obj;
		typename EvictorQueue::iterator pos;
		time_t timestamp;

		void timeit(time_t now) {
			timestamp = now;
		}
		bool expired(int lastTime) {
			return timestamp < lastTime;
		}
	};

public:
	TimedEvictor(time_t duration = -1) :
	_duration(duration) {
		;
	}

	virtual ~TimedEvictor() {
		;
	}

	void duration(time_t duration) {
		_duration = duration;
	}

	Object locate(const Id& id, const Object& def) {
		IceUtil::Mutex::Lock lock(_mutex);
		//Check if we have a object in the map already
		typename EvictorMap::iterator i = _map.find(id);
		bool newEntry = (i == _map.end());
		if (!newEntry) {
			// Got the entry already, dequeue the entry from
			// its current position.
			Entry& entry = i->second;
			_queue.erase(entry.pos);
			hook(entry, i);
			return entry.obj;
		} else {
			return def;
		}
	}

	Object find(const Id& id, const Object& def) {
		IceUtil::Mutex::Lock lock(_mutex);
		//Check if we have a object in the map already
		typename EvictorMap::iterator i = _map.find(id);
		if (i != _map.end()) {
			// Got the entry already, dequeue the entry from
			// its current position.
			return i->second.obj;
		} else {
			return def;
		}
	}

	void add(const Object& obj, const Id& id) {
		IceUtil::Mutex::Lock lock(_mutex);
		Entry entry;
		entry.timestamp = time(NULL);
		entry.obj = obj;
		while (true) {
			pair<typename EvictorMap::iterator, bool> result = _map.insert(make_pair(id, entry));
			if (result.second) {
				result.first->second.pos = _queue.insert(_queue.begin(), result.first);
				evict();
				break;
			}
			typename EvictorMap::iterator pos = _map.find(id);
			if (pos != _map.end()) {
				_queue.erase(pos->second.pos);
				_map.erase(pos);
			}
		}
	}

	Object remove(const Id& id) {
		Object obj;
		IceUtil::Mutex::Lock lock(_mutex);
		typename EvictorMap::iterator pos = _map.find(id);
		if (pos != _map.end()) {
			obj = pos->second.obj;
			_queue.erase(pos->second.pos);
			_map.erase(pos);
		}
		return obj;
	}

	void clear() {
		IceUtil::Mutex::Lock lock(_mutex);
		_map.clear();
		_queue.clear();
	}

	int size() {
		IceUtil::Mutex::Lock lock(_mutex);
		return _map.size();
	}

	bool has(const Id& id) {
		IceUtil::Mutex::Lock lock(_mutex);
		return _map.find(id) != _map.end();
	}

protected:
	void evict() {
		// If the evictor queue has grown larger than the limit,
		// look at the excess elements to see whether any of them can be evicted.
		if (_duration> 0) {
			time_t lastTime = time(NULL) - _duration;
			while (!_queue.empty()) {
				typename EvictorQueue::reverse_iterator p = _queue.rbegin();
				typename EvictorMap::iterator pos = *p;
				if (!pos->second.expired(lastTime)) {
					break;
				}
				_queue.erase(pos->second.pos);
				_map.erase(pos);
			}
		}
	}

	void hook(Entry& entry, const typename EvictorMap::iterator& mapit) {
		entry.timestamp = time(NULL);
		entry.pos = _queue.insert(_queue.begin(), mapit);
	}

protected:
	IceUtil::Mutex _mutex;
	EvictorMap _map;
	EvictorQueue _queue;
	time_t _duration;
};

}

#endif

