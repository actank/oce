// **********************************************************************
//
// Copyright (c) 2005-2006 DuDu, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include "ObjectCacheI.h"

using namespace std;
using namespace MyUtil;

//-------------------------------------------------------------------------
ObjectEvictor::ObjectEvictor(const string& alias, Ice::Int category, int size,
		const ServantFactoryPtr& factory) :
	_alias(alias), _category(category) {
//	if (size <= 10) {
//		_chunkSize = 1;
//	} else if (size <= 1000) {
//		_chunkSize = 7;
//	} else if (size <= 10000) {
//		_chunkSize = 1021;
//	} else {
//		_chunkSize = 10111;
//	}
	if (size <= 1000) {
		_chunkSize = 1;
	} else {
		_chunkSize = 11;
	}
	_chunks.reserve(_chunkSize);
	for (size_t i = 0; i < _chunkSize; ++i) {
		ChunkPtr chunk = new Chunk;
		chunk->evictor = new Evictor;
		chunk->factory = factory ? factory->clone() : factory;
		chunk->evictor->size(size / _chunkSize + 1);
		_chunks.push_back(chunk);
	}
}

ObjectEvictor::~ObjectEvictor() {

}

void ObjectEvictor::size(Ice::Int size) {
	for (size_t i = 0; i < _chunkSize; ++i) {
		_chunks.at(i)->evictor->size(size / _chunkSize + 1);
	}
}
int ObjectEvictor::size() {
	int result = 0;
	for (size_t i = 0; i < _chunkSize; ++i) {
		result += _chunks.at(i)->evictor->size();
	}
	return result;
}

Ice::ObjectPtr ObjectEvictor::find(Ice::Long id) {
	ChunkPtr segment = _chunks[id % _chunkSize];
	return segment->evictor->find(id, 0);
}

ObjectResultPtr ObjectEvictor::find(const MyUtil::LongSeq& ids) {
	ObjectResultPtr result = new ObjectResult;
	for (uint i = 0; i < ids.size(); ++i) {
		Ice::Long id = ids[i];
		Ice::ObjectPtr obj = _chunks.at(id % _chunkSize)->evictor->find(id, 0);
		if (obj) {
			result->data[id] = obj;
		}
	}
	return result;
}

Ice::ObjectPtr ObjectEvictor::locate(Ice::Long id) {
	ChunkPtr chunk = _chunks.at(id % _chunkSize);
	Ice::ObjectPtr obj = chunk->evictor->locate(id, 0);
	if (obj) {
		return obj;
	} else {
		Ice::ObjectPtr obj = create(id);
		if (obj) {
			return obj;
		}
	}
	throw NoSuchObjectException("ObjectEvictor::locate");
}

ObjectResultPtr ObjectEvictor::locate(const MyUtil::LongSeq& ids) {
	ObjectResultPtr result = new ObjectResult;
	MyUtil::LongSeq missingIds;
	for (uint i = 0; i < ids.size(); ++i) {
		Ice::Long id = ids[i];
		Ice::ObjectPtr obj =
				_chunks.at(id % _chunkSize)->evictor->locate(id, 0);
		if (obj) {
			result->data[id] = obj;
		} else {
			missingIds.push_back(id);
		}
	}
	if (!missingIds.empty()) {
		ObjectResultPtr other = create(missingIds);
		if (other) {
			for (ObjectMap::iterator it = other->data.begin(); it
					!= other->data.end(); ++it) {
				result->data[it->first] = it->second;
			}
		}
	}
	return result;
}

void ObjectEvictor::add(Ice::Long id, const Ice::ObjectPtr& servant) {
	_chunks.at(id % _chunkSize)->evictor->add(servant, id);
}

void ObjectEvictor::add(const ObjectMap& servants) {
	for (ObjectMap::const_iterator it = servants.begin(); it != servants.end(); ++it) {
		_chunks.at(it->first % _chunkSize)->evictor->add(it->second, it->first);
	}
}

void ObjectEvictor::remove(Ice::Long id) {
	_chunks.at(id % _chunkSize)->evictor->remove(id);
}

void ObjectEvictor::remove(const MyUtil::LongSeq& ids) {
	for (uint i = 0; i < ids.size(); ++i) {
		_chunks.at(ids[i] % _chunkSize)->evictor->remove(ids[i]);
	}
}

void ObjectEvictor::preload(Ice::Long id) {
	TaskManager::instance().execute(new PreloadTask(id, *this));
}

void ObjectEvictor::preload(const MyUtil::LongSeq& ids) {
	TaskManager::instance().execute(new PreloadTask(ids, *this));
}

void ObjectEvictor::reload(Ice::Long id) {
	TaskManager::instance().execute(new ReloadTask(id, *this));
}

void ObjectEvictor::reload(const MyUtil::LongSeq& ids) {
	TaskManager::instance().execute(new ReloadTask(ids, *this));
}

Ice::ObjectPtr ObjectEvictor::create(Ice::Long id) {
	ChunkPtr chunk = _chunks.at(id % _chunkSize);
	if (chunk->factory) {
		try {
			Ice::ObjectPtr obj = chunk->factory->create(this, _category, id);
			if (obj) {
				chunk->evictor->add(obj, id);
				MCE_INFO("ObjectEvictor::create -> " << _alias << " -> " << id
						<< " -> size: " << size());
				return obj;
			}
		} catch (Ice::Exception& e) {
			MCE_WARN("ObjectEvictor::create -> " << id << "/" << _alias << ", "
					<< e);
		} catch (std::exception& e) {
			MCE_WARN("ObjectEvictor::create -> " << id << "/" << _alias << ", "
					<< e.what());
		} catch (...) {
			MCE_WARN("ObjectEvictor::create -> " << id << "/" << _alias
					<< ", unkown");
		}
	}
	return 0;
}

ObjectResultPtr ObjectEvictor::create(const MyUtil::LongSeq& ids) {
	ServantFactoryPtr factory = _chunks[ids.front() % _chunkSize]->factory;
	if (factory) {
		ObjectResultPtr result = factory->create(this, _category, ids);
		if (result) {
			for (ObjectMap::iterator it = result->data.begin(); it
					!= result->data.end(); ++it) {
				_chunks.at(it->first % _chunkSize)->evictor->add(it->second,
						it->first);
			}
			return result;
		}
	}
	return 0;
}

bool ObjectEvictor::has(Ice::Long id) {
	return _chunks.at(id % _chunkSize)->evictor->has(id);
}

//---------------------------------------------------------------------------
Ice::ObjectPtr ObjectCacheI::findObject(Ice::Int category, Ice::Long id,
		const Ice::Current& current) {
	ObjectBaseMapPtr cache = locateCategory(category);
	if (cache) {
		return cache->find(id);
	}
	return 0;
}

ObjectResultPtr ObjectCacheI::findObjects(Ice::Int category,
		const MyUtil::LongSeq& ids, const Ice::Current& current) {
	ObjectBaseMapPtr cache = locateCategory(category);
	if (cache) {
		return cache->find(ids);
	}
	return 0;
}

Ice::ObjectPtr ObjectCacheI::locateObject(Ice::Int category, Ice::Long id,
		const Ice::Current& current) {
	ObjectBaseMapPtr cache = locateCategory(category);
	if (cache) {
		return cache->locate(id);
	}
	throw NoSuchObjectException("ObjectCacheI::locateObject");
}

ObjectResultPtr ObjectCacheI::locateObjects(Ice::Int category,
		const MyUtil::LongSeq& ids, const Ice::Current& current) {
	ObjectBaseMapPtr cache = locateCategory(category);
	if (cache) {
		return cache->locate(ids);
	}
	throw NoSuchObjectException("ObjectCacheI::locateObjects");
}

void ObjectCacheI::addObject(Ice::Int category, Ice::Long id,
		const Ice::ObjectPtr& o, const Ice::Current&) {
	ObjectBaseMapPtr cache = locateCategory(category);
	if (cache) {
		cache->add(id, o);
	}
}

void ObjectCacheI::addObjects(Ice::Int category, const ObjectMap& os,
		const Ice::Current& current) {
	if (!os.empty()) {
		ObjectBaseMapPtr cache = locateCategory(category);
		if (cache) {
			cache->add(os);
		}
	}
}

void ObjectCacheI::removeObject(Ice::Int category, Ice::Long id,
		const Ice::Current& current) {
	ObjectBaseMapPtr cache = locateCategory(category);
	if (cache) {
		cache->remove(id);
	}
}
void ObjectCacheI::removeObjects(Ice::Int category, const MyUtil::LongSeq& ids,
		const Ice::Current& current) {
	ObjectBaseMapPtr cache = locateCategory(category);
	if (cache) {
		cache->remove(ids);
	}
}

bool ObjectCacheI::hasObject(Ice::Int category, Ice::Long id,
		const Ice::Current& current) {
	ObjectBaseMapPtr cache = locateCategory(category);
	if (cache) {
		return cache->has(id);
	}
	return false;
}

bool ObjectCacheI::hasObjects(Ice::Int category, const MyUtil::LongSeq& ids,
		const Ice::Current& current) {
	ObjectBaseMapPtr cache = locateCategory(category);
	if (cache) {
		for (MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
			if (!cache->has(*it)) {
				return false;
			}
		}
		return true;
	}
	return false;
}

void ObjectCacheI::preloadObject(Ice::Int category, Ice::Long id,
		const Ice::Current&) {
	ObjectBaseMapPtr cache = locateCategory(category);
	if (cache) {
		cache->preload(id);
	}
}

void ObjectCacheI::preloadObjects(Ice::Int category,
		const MyUtil::LongSeq& ids, const Ice::Current& current) {
	if (ids.empty()) {
		return;
	}
	ObjectBaseMapPtr cache = locateCategory(category);
	if (cache) {
		cache->preload(ids);
	}
}

void ObjectCacheI::reloadObject(Ice::Int category, Ice::Long id,
		const Ice::Current&) {
	ObjectBaseMapPtr cache = locateCategory(category);
	if (cache) {
		cache->reload(id);
	}
}

void ObjectCacheI::reloadObjects(Ice::Int category, const MyUtil::LongSeq& ids,
		const Ice::Current& current) {
	if (ids.empty()) {
		return;
	}
	ObjectBaseMapPtr cache = locateCategory(category);
	if (cache) {
		cache->reload(ids);
	}
}

void ObjectCacheI::registerCategory(Ice::Int category, const string& alias,
		const ServantFactoryPtr& factory, const Ice::PropertiesPtr& props, bool useEvictor) {
	if (_caches.size() <= (size_t) category) {
		_caches.resize(category + 1);
	}
	if (_caches.at(category)) {
		MCE_WARN("ObjectCache: " << alias << " existed, abort");
		throw ;
	}
	ostringstream key;
	key << "Service.ObjectCache." << alias << ".CacheSize";
	int size = props->getPropertyAsIntWithDefault(key.str(), 1000000);
	if(useEvictor) {
		MCE_WARN("ObjectCache -> " << key.str() << " -> " << size);
		_caches.at(category) = new ObjectEvictor(alias, category, size, factory);
	} else {
		_caches.at(category) = new ObjectRetention(alias, category, size, factory);
	}
	_keys[key.str()] = category;
}

void ObjectCacheI::deregisterCategory(Ice::Int category) {
	if (_caches.size() > (size_t) category) {
		_caches.at(category) = 0;
	}
}

ObjectBaseMapPtr ObjectCacheI::locateCategory(Ice::Int category) {
	if (_caches.size() > (size_t) category) {
		return _caches.at(category);
	}
	MCE_WARN("Locate ObjectCache: " << category << ", NULL");
	return 0;
}

void ObjectCacheI::configure(const Ice::PropertiesPtr& props) {
	for (map<string, int>::iterator it = _keys.begin(); it != _keys.end(); ++it) {
		int size = props->getPropertyAsIntWithDefault(it->first, 0);
		if (size != 0) {
			_caches.at(it->second)->size(size);
			MCE_WARN("ObjectCacheI::configure -> " << it->first << " -> "
					<< size);
		}
	}
}

//---------------------------------------------------------------------------
Ice::ObjectPtr SyncServantFactory::create(Ice::Long id) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
	RequestMap::iterator pos = _requests.find(id);
	if (pos != _requests.end()) {
		pos->second->count++;
	} else {
		RequestPtr request = new Request;
		request->id = id;
		request->stat = REQUEST_HANDLING;
		request->count = 1;
		_requests[id] = request;
		TaskManager::instance().execute(new CreateTask(request, this));
	}
	while (true) {
		RequestMap::iterator pos = _requests.find(id);
		if (pos == _requests.end()) {
			return 0;
		}
		RequestPtr request = pos->second;
		if (request->stat == REQUEST_FINISHED) {
			request->count--;
			if (request->count <= 0) {
				_requests.erase(request->id);
			}
			return request->object;
		}
		try {
			wait();
		} catch (...) {
			// do nothing
		}
	};
}

ServantFactoryPtr SyncServantFactory::clone() {
	return new SyncServantFactory(_factory);
}

void SyncServantFactory::finish(Ice::Long id) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
	RequestMap::iterator pos = _requests.find(id);
	if (pos != _requests.end()) {
		pos->second->stat = REQUEST_FINISHED;
	} else {
		MCE_WARN("SyncServantFactory::finish -> no such id in requests, " << id);
		RequestPtr request = new Request;
		request->stat = REQUEST_FINISHED;
		_requests[id] = request;
	}

	notifyAll();
}

void SyncServantFactory::handle(const RequestPtr& request) {
	try {
		request->object = _factory->create(request->id);
	} catch (Ice::Exception& e) {
		// do nothing
		MCE_WARN("SyncServantFactory::handle -> " << request->id << e);
	} catch (std::exception& e) {
		MCE_WARN("SyncServantFactory::handle -> " << request->id << e.what());
	} catch (...) {
		MCE_WARN("SyncServantFactory::handle -> " << request->id);
	}
	finish(request->id);
}

//----------------------------

ObjectRetention::ObjectRetention(const string& alias, Ice::Int category, int size,
		const ServantFactoryPtr& factory) :
	_alias(alias), _category(category) {
	if (size <= 1000) {
		_chunkSize = 1;
	} else {
		_chunkSize = 11;
	}
	_chunks.reserve(_chunkSize);
	for (size_t i = 0; i < _chunkSize; ++i) {
		ChunkPtr chunk = new Chunk;
		chunk->retention = new Retention;
		chunk->factory = factory ? factory->clone() : factory;
		_chunks.push_back(chunk);
	}

	srand( unsigned (time( NULL) ));
}

int ObjectRetention::size() {
	int result = 0;
	for (size_t i = 0; i < _chunkSize; ++i) {
		result += _chunks.at(i)->retention->size();
	}
	return result;
}
void ObjectRetention::size(Ice::Int size){
	return;
}
Ice::ObjectPtr ObjectRetention::find(Ice::Long id) {
	ChunkPtr segment = _chunks[id % _chunkSize];
	return segment->retention->find(id, 0);
}

ObjectResultPtr ObjectRetention::find(const MyUtil::LongSeq& ids) {
	ObjectResultPtr result = new ObjectResult;
	for (uint i = 0; i < ids.size(); ++i) {
		Ice::Long id = ids[i];
		Ice::ObjectPtr obj = _chunks.at(id % _chunkSize)->retention->find(id, 0);
		if (obj) {
			result->data[id] = obj;
		}
	}
	return result;
}

Ice::ObjectPtr ObjectRetention::locate(Ice::Long id) {
	ChunkPtr chunk = _chunks.at(id % _chunkSize);
	Ice::ObjectPtr obj = chunk->retention->locate(id, 0);
	if (obj) {
		return obj;
	} else {
		Ice::ObjectPtr obj = create(id);
		if (obj) {
			return obj;
		}
	}
	throw NoSuchObjectException("ObjectRetention::locate");
}

ObjectResultPtr ObjectRetention::locate(const MyUtil::LongSeq& ids) {
	ObjectResultPtr result = new ObjectResult;
	MyUtil::LongSeq missingIds;
	
	MyUtil::LongSeq idsChunk[_chunkSize];
	
	for (uint i = 0; i < ids.size(); ++i)
	{
		Ice::Long id = ids[i];
		idsChunk[id % _chunkSize].push_back(id);
	} 

	int iRandom = rand() % (int)_chunkSize;

	for(int i =iRandom; i < (int)_chunkSize + iRandom; ++i )
	{
		int j = i % _chunkSize;

		if(idsChunk[j].size() > 0)
		{
			_chunks.at(j)->retention->locate(idsChunk[j], missingIds, result);
		}
	}
	
	if (!missingIds.empty()) {
		ObjectResultPtr other = create(missingIds);
		if (other) {
			for (ObjectMap::iterator it = other->data.begin(); it
					!= other->data.end(); ++it) {
				result->data[it->first] = it->second;
			}
		}
	}
	return result;
}

void ObjectRetention::add(Ice::Long id, const Ice::ObjectPtr& servant) {
	_chunks.at(id % _chunkSize)->retention->add(servant, id);
}

void ObjectRetention::add(const ObjectMap& servants) {
	
	ObjectMap dataChunk[_chunkSize];

	for (ObjectMap::const_iterator it = servants.begin(); it != servants.end(); ++it)
	{
		dataChunk[it->first % _chunkSize].insert(make_pair(it->first, it->second));
	}

	int iRandom = rand() % (int)_chunkSize;

	for(int i =iRandom; i < (int)_chunkSize + iRandom; ++i )
	{
		int j = i % _chunkSize;
		
		if(dataChunk[j].size()>0)
		{
			_chunks.at(j % _chunkSize)->retention->add(dataChunk[j]);
		}
	}

}

void ObjectRetention::remove(Ice::Long id) {
	_chunks.at(id % _chunkSize)->retention->remove(id);
}

void ObjectRetention::remove(const MyUtil::LongSeq& ids) {
	for (uint i = 0; i < ids.size(); ++i) {
		_chunks.at(ids[i] % _chunkSize)->retention->remove(ids[i]);
	}
}

void ObjectRetention::preload(Ice::Long id) {
	TaskManager::instance().execute(new PreloadTask(id, *this));
}

void ObjectRetention::preload(const MyUtil::LongSeq& ids) {
	TaskManager::instance().execute(new PreloadTask(ids, *this));
}

void ObjectRetention::reload(Ice::Long id) {
	TaskManager::instance().execute(new ReloadTask(id, *this));
}

void ObjectRetention::reload(const MyUtil::LongSeq& ids) {
	TaskManager::instance().execute(new ReloadTask(ids, *this));
}

Ice::ObjectPtr ObjectRetention::create(Ice::Long id) {
	ChunkPtr chunk = _chunks.at(id % _chunkSize);
	if (chunk->factory) {
		try {
			Ice::ObjectPtr obj = chunk->factory->create(this, _category, id);
			if (obj) {
				chunk->retention->add(obj, id);
				MCE_INFO("ObjectRetention::create -> " << _alias << " -> " << id
						<< " -> size: " << size());
				return obj;
			}
		} catch (Ice::Exception& e) {
			MCE_WARN("ObjectRetention::create -> " << id << "/" << _alias << ", "
					<< e);
		} catch (std::exception& e) {
			MCE_WARN("ObjectRetention::create -> " << id << "/" << _alias << ", "
					<< e.what());
		} catch (...) {
			MCE_WARN("ObjectRetention::create -> " << id << "/" << _alias
					<< ", unkown");
		}
	}
	return 0;
}

ObjectResultPtr ObjectRetention::create(const MyUtil::LongSeq& ids) {
	ServantFactoryPtr factory = _chunks[ids.front() % _chunkSize]->factory;
	if (factory) {
		ObjectResultPtr result = factory->create(this, _category, ids);
		if (result) {
			add(result->data);
			return result;
		}
	}
	return 0;
}

bool ObjectRetention::has(Ice::Long id) {
	return _chunks.at(id % _chunkSize)->retention->has(id);
}
