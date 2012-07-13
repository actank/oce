// **********************************************************************
//
// Copyright (c) 2005-2006 DuDu, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef __OBJECT_CACHE_I_H__
#define __OBJECT_CACHE_I_H__

#include <map>
#include <sstream>
#include "Evictor.h"
#include "IceLogger.h"
#include "TaskManager.h"
#include "ObjectCache.h"
#include <stdlib.h>
using namespace std;

namespace MyUtil {

class ServantFactory;
typedef IceUtil::Handle<ServantFactory> ServantFactoryPtr;
// **********************************************************************
class ObjectBaseMap : public IceUtil::Shared{
public:
	virtual void size(Ice::Int size)=0;
        virtual int size()=0;

        virtual Ice::ObjectPtr find(Ice::Long id)=0;
        virtual ObjectResultPtr find(const MyUtil::LongSeq& ids)=0;

	virtual Ice::ObjectPtr locate(Ice::Long id)=0;
        virtual ObjectResultPtr locate(const MyUtil::LongSeq& ids)=0;

	virtual void add(Ice::Long id, const Ice::ObjectPtr& servant)=0;
        virtual void add(const ObjectMap& servants)=0;

        virtual void remove(Ice::Long id)=0;
	virtual void remove(const MyUtil::LongSeq& ids)=0;

        virtual void preload(Ice::Long id)=0;
        virtual void preload(const MyUtil::LongSeq& ids)=0;

        virtual void reload(Ice::Long id)=0;
        virtual void reload(const MyUtil::LongSeq& ids)=0;

        virtual Ice::ObjectPtr create(Ice::Long id)=0;
        virtual ObjectResultPtr create(const MyUtil::LongSeq& ids)=0;

	virtual	bool has(Ice::Long id)=0;

protected:
	class PreloadTask : public MyUtil::Task {
		public:
			PreloadTask(Ice::Long id, ObjectBaseMap& baseMap) :
				MyUtil::Task(TASK_LEVEL_PRELOAD), _baseMap(baseMap) {
					_ids.push_back(id);
				}
			PreloadTask(const MyUtil::LongSeq ids, ObjectBaseMap& baseMap) :
				MyUtil::Task(TASK_LEVEL_PRELOAD), _ids(ids), _baseMap(baseMap) {
				}
			virtual void handle() {
				_baseMap.locate(_ids);
			}
		private:
			MyUtil::LongSeq _ids;
			ObjectBaseMap& _baseMap;
	};
	friend class ::MyUtil::ObjectBaseMap::PreloadTask;

	class ReloadTask : public MyUtil::Task {
		public:
			ReloadTask(Ice::Long id, ObjectBaseMap& baseMap) :
				MyUtil::Task(TASK_LEVEL_RELOAD), _baseMap(baseMap) {
					_ids.push_back(id);
				}
			ReloadTask(const MyUtil::LongSeq ids, ObjectBaseMap& baseMap) :
				MyUtil::Task(TASK_LEVEL_RELOAD), _ids(ids), _baseMap(baseMap) {
				}
			virtual void handle() {
				_baseMap.create(_ids);
			}
		private:
			MyUtil::LongSeq _ids;
			ObjectBaseMap& _baseMap;
	};
	friend class ::MyUtil::ObjectBaseMap::ReloadTask;

};
typedef IceUtil::Handle<ObjectBaseMap> ObjectBaseMapPtr;

// **************************************************************************
class ObjectEvictor :public ObjectBaseMap {
protected:

	string _alias;
	Ice::Int _category;
	typedef SizedEvictor<Ice::Long, Ice::ObjectPtr> Evictor;
	typedef IceUtil::Handle<Evictor> EvictorPtr;
	struct Chunk : public IceUtil::Shared {
		EvictorPtr evictor;
		ServantFactoryPtr factory;
	};
	typedef IceUtil::Handle<Chunk> ChunkPtr;
	vector<ChunkPtr> _chunks;
	size_t _chunkSize;
public:
	ObjectEvictor(const string& alias, Ice::Int category, int size,
			const ServantFactoryPtr& factory);
	virtual ~ObjectEvictor();

	virtual	void size(Ice::Int size);
	virtual int size();

	virtual Ice::ObjectPtr find(Ice::Long id);
	virtual	ObjectResultPtr find(const MyUtil::LongSeq& ids);

	virtual	Ice::ObjectPtr locate(Ice::Long id);
	virtual	ObjectResultPtr locate(const MyUtil::LongSeq& ids);

	virtual	void add(Ice::Long id, const Ice::ObjectPtr& servant);
	virtual	void add(const ObjectMap& servants);

	virtual	void remove(Ice::Long id);
	virtual	void remove(const MyUtil::LongSeq& ids);

	virtual	void preload(Ice::Long id);
	virtual	void preload(const MyUtil::LongSeq& ids);

	virtual	void reload(Ice::Long id);
	virtual	void reload(const MyUtil::LongSeq& ids);

	virtual	Ice::ObjectPtr create(Ice::Long id);
	virtual	ObjectResultPtr create(const MyUtil::LongSeq& ids);

	virtual	bool has(Ice::Long id);

};
typedef IceUtil::Handle<ObjectEvictor> ObjectEvictorPtr;

// **************************************************************************
class ObjectCacheI : public ObjectCache {
public:
	ObjectCacheI(const Ice::ObjectAdapterPtr& adapter) {
		if (adapter) {
			adapter->add(this, adapter->getCommunicator()->stringToIdentity("SC"));
		}
	}
	virtual ~ObjectCacheI() {
	}

	virtual Ice::ObjectPtr findObject(Ice::Int category, Ice::Long id,
			const Ice::Current& = Ice::Current());
	virtual ObjectResultPtr findObjects(Ice::Int category,
			const MyUtil::LongSeq& ids, const Ice::Current& = Ice::Current());

	virtual Ice::ObjectPtr locateObject(Ice::Int category, Ice::Long id,
			const Ice::Current& = Ice::Current());
	virtual ObjectResultPtr locateObjects(Ice::Int category,
			const MyUtil::LongSeq& ids, const Ice::Current& = Ice::Current());

	virtual void addObject(Ice::Int category, Ice::Long id,
			const Ice::ObjectPtr&, const Ice::Current& = Ice::Current());
	virtual void addObjects(Ice::Int category, const ObjectMap& os,
			const Ice::Current& = Ice::Current());

	virtual void removeObject(Ice::Int category, Ice::Long id,
			const Ice::Current& = Ice::Current());
	virtual void removeObjects(Ice::Int category, const MyUtil::LongSeq& ids,
			const Ice::Current& = Ice::Current());

	virtual bool hasObject(Ice::Int category, Ice::Long id,
			const Ice::Current& = Ice::Current());
	virtual bool hasObjects(Ice::Int category, const MyUtil::LongSeq& ids,
			const Ice::Current& = Ice::Current());

	virtual void preloadObject(Ice::Int category, Ice::Long id,
			const Ice::Current& = Ice::Current());
	virtual void preloadObjects(Ice::Int category, const MyUtil::LongSeq& ids,
			const Ice::Current& = Ice::Current());

	virtual void reloadObject(Ice::Int category, Ice::Long id,
			const Ice::Current& = Ice::Current());
	virtual void reloadObjects(Ice::Int category, const MyUtil::LongSeq& ids,
			const Ice::Current& = Ice::Current());

	void registerCategory(Ice::Int category, const string& alias,
			const ServantFactoryPtr& factory, const Ice::PropertiesPtr& props,bool useEvictor);

	void deregisterCategory(Ice::Int category);

	ObjectBaseMapPtr locateCategory(Ice::Int category);

	void configure(const Ice::PropertiesPtr& props);

private:
	typedef vector<ObjectBaseMapPtr> ObjectCacheSeq;
	ObjectCacheSeq _caches;
	map<string, int> _keys;
};
typedef IceUtil::Handle<ObjectCacheI> ObjectCacheIPtr;

// **************************************************************************
class ServantFactory : public IceUtil::Shared {
public:
	virtual Ice::ObjectPtr create(ObjectBaseMap* evictor, Ice::Int category,
			Ice::Long id) {
		return create(id);
	}
	virtual ObjectResultPtr create(ObjectBaseMap* evictor, Ice::Int category,
			const MyUtil::LongSeq& ids) {
		return create(ids);
	}

	virtual Ice::ObjectPtr create(Ice::Long id) {
		return create((Ice::Int)id);
	}
	virtual Ice::ObjectPtr create(Ice::Int id) {
		return 0;
	}
	virtual ObjectResultPtr create(const MyUtil::LongSeq& ids) {
		ObjectResultPtr res = new ObjectResult();
		for (size_t i = 0; i < ids.size(); ++i) {
			try {
				Ice::ObjectPtr obj = create(ids.at(i));
				if (obj) {
					res->data[ids.at(i)] = obj;
				}
			} catch (...) {
				//
			}
		}
		return res;
	}
	virtual ObjectResultPtr create(const MyUtil::IntSeq& ids) {
		ObjectResultPtr res = new ObjectResult();
		for (size_t i = 0; i < ids.size(); ++i) {
			try {
				Ice::ObjectPtr obj = create(ids.at(i));
				if (obj) {
					res->data[ids.at(i)] = obj;
				}
			} catch (...) {
				//
			}
		}
		return res;
	}
	virtual ServantFactoryPtr clone() {
		return this;
	}
};

// **************************************************************************
class SyncServantFactory : public ServantFactory,
	public IceUtil::Monitor<IceUtil::Mutex> {
private:
	ServantFactoryPtr _factory;
	struct Request : IceUtil::Shared {
		int stat;
		int count;
		Ice::Long id;
		Ice::ObjectPtr object;
		Ice::Exception e;
		IceUtil::Monitor<IceUtil::Mutex> mutex;
	};
	typedef IceUtil::Handle<Request> RequestPtr;
	typedef map<Ice::Int, RequestPtr> RequestMap;
	RequestMap _requests;

	enum {
		REQUEST_HANDLING,
		REQUEST_FINISHED
	};

public:
	SyncServantFactory(const ServantFactoryPtr& factory) :
		_factory(factory) {
	}
	virtual ~SyncServantFactory() {
	}

	virtual Ice::ObjectPtr create(Ice::Long id);

	virtual ServantFactoryPtr clone();

	void finish(Ice::Long id);
	void handle(const RequestPtr& request);

	class CreateTask : public Task {
public:
		CreateTask(const RequestPtr& request, SyncServantFactory* factory) :
			Task(TASK_LEVEL_CREATE), _request(request), _factory(factory) {
		}
		virtual void handle() {
			_factory->handle(_request);
		}
private:
		RequestPtr _request;
		SyncServantFactory* _factory;
	};
};

class Retention : public IceUtil::Shared {
protected:
#ifndef __LP64__
	typedef hash_map<Ice::Long, Ice::ObjectPtr, Int64_hash, Int64_compare> RetentionMap;
#else
	typedef hash_map<Ice::Long, Ice::ObjectPtr> RetentionMap;
#endif

public:
	Retention() {
		;
	}
	virtual ~Retention() {
		;
	}

	size_t size() {
		IceUtil::RWRecMutex::RLock lock(_mutex);
		return _map.size();
	}

	Ice::ObjectPtr locate(const Ice::Long& id, const Ice::ObjectPtr& def) {
		IceUtil::RWRecMutex::RLock lock(_mutex);
		RetentionMap::iterator i = _map.find(id);
		bool newEntry = (i == _map.end());
		if (!newEntry) {
			Ice::ObjectPtr& object = i->second;
			return object;
		} else {
			return def;
		}
	}
	
	void locate(const MyUtil::LongSeq& ids, MyUtil::LongSeq& missingIds, MyUtil::ObjectResultPtr& result)
	{
		IceUtil::RWRecMutex::RLock lock(_mutex);
	
		for(MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it)
		{
			RetentionMap::iterator findIt = _map.find(*it);
			if(findIt != _map.end())
			{
				result->data[*it] = findIt->second;
			}else
			{
				missingIds.push_back(*it);
			}
		}
	}

	Ice::ObjectPtr find(const Ice::Long& id, const Ice::ObjectPtr& def) {
		IceUtil::RWRecMutex::RLock lock(_mutex);
		RetentionMap::iterator i = _map.find(id);
		if (i != _map.end()) {
			return i->second;
		} else {
			return def;
		}
	}

	void add(const Ice::ObjectPtr& obj, const Ice::Long& id) {
		IceUtil::RWRecMutex::WLock lock(_mutex);
		Ice::ObjectPtr object = obj;

		while (true) {
			pair<RetentionMap::iterator, bool> result = _map.insert(
					make_pair(id, object));
			if (result.second) {
				break;
			}
			RetentionMap::iterator pos = _map.find(id);
			if (pos != _map.end()) {
				_map.erase(pos);
			}
		}
	}

	void add(const ObjectMap& servants)
	{
		IceUtil::RWRecMutex::WLock lock(_mutex);

		for(ObjectMap::const_iterator it = servants.begin(); it != servants.end(); ++it)
		{
			Ice::Long id = it->first;
			
			Ice::ObjectPtr object = it->second;

			while (true) {
				pair<RetentionMap::iterator, bool> result = _map.insert(
						make_pair(id, object));
				if (result.second) {
					break;
				}
				RetentionMap::iterator pos = _map.find(id);
				if (pos != _map.end()) {
					_map.erase(pos);
				}
			}
		}

	}
	
	Ice::ObjectPtr remove(const Ice::Long& id) {
		Ice::ObjectPtr obj;
		IceUtil::RWRecMutex::WLock lock(_mutex);
		RetentionMap::iterator pos = _map.find(id);
		if (pos != _map.end()) {
			obj = pos->second;
			_map.erase(pos);
		}
		return obj;
	}

	void clear() {
		IceUtil::RWRecMutex::WLock lock(_mutex);
		_map.clear();
	}

	bool has(const Ice::Long& id) {
		IceUtil::RWRecMutex::RLock lock(_mutex);
		return _map.find(id) != _map.end();
	}

protected:
	IceUtil::RWRecMutex _mutex;
	RetentionMap _map;
};

typedef IceUtil::Handle<Retention> RetentionPtr;

class ObjectRetention : public ObjectBaseMap {
protected:
	string _alias;
	Ice::Int _category;

	struct Chunk : public IceUtil::Shared {
		RetentionPtr retention;
		ServantFactoryPtr factory;
	};
	typedef IceUtil::Handle<Chunk> ChunkPtr;
	vector<ChunkPtr> _chunks;
	size_t _chunkSize;
public:
	ObjectRetention(const string& alias, Ice::Int category, int size,
			const ServantFactoryPtr& factory);
	virtual ~ObjectRetention() {}

	virtual int size();
	virtual void size(Ice::Int);
	virtual	Ice::ObjectPtr find(Ice::Long id);
	virtual	ObjectResultPtr find(const MyUtil::LongSeq& ids);

	virtual	Ice::ObjectPtr locate(Ice::Long id);
	virtual	ObjectResultPtr locate(const MyUtil::LongSeq& ids);

	virtual	void add(Ice::Long id, const Ice::ObjectPtr& servant);
	virtual	void add(const ObjectMap& servants);

	virtual	void remove(Ice::Long id);
	virtual	void remove(const MyUtil::LongSeq& ids);

	virtual	void preload(Ice::Long id);
	virtual	void preload(const MyUtil::LongSeq& ids);

	virtual	void reload(Ice::Long id);
	virtual	void reload(const MyUtil::LongSeq& ids);

	virtual	Ice::ObjectPtr create(Ice::Long id);
	virtual	ObjectResultPtr create(const MyUtil::LongSeq& ids);

	virtual	bool has(Ice::Long id);
};

typedef IceUtil::Handle<ObjectRetention> ObjectRetentionPtr;

// **************************************************************************
}
;

#endif

