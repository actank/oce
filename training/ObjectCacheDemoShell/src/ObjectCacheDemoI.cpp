#include "ObjectCacheDemoI.h"
#include <sstream>
#include <iostream>

using namespace xce::demos;
using namespace MyUtil;

//ObjectCacheDemo
//-------------------------------------------------------------
//ObjectCacheDemoI::ObjectCacheDemoI(const Ice::ObjectAdapterPtr& adapter){
//	ObjectCacheDemoI(adapter, 1000000);
//}

ObjectCacheDemoI::ObjectCacheDemoI(const Ice::ObjectAdapterPtr& adapter, int capacity){
	if (adapter) {
		MCE_INIT("./log", "DEBUG")
		//初始化ObjCache
		_cache = new MyUtil::ObjectCacheI(adapter);
		//注册一个category
		Ice::PropertiesPtr properties = Ice::createProperties();
		//设置object数量上限
		stringstream ss;
		ss << capacity;
		properties->setProperty("Service.ObjectCache.UE.CacheSize", ss.str());
		_cache->registerCategory(UE, "UE", new DomoCacheFactoryI , properties, true);
	}
}

void ObjectCacheDemoI::load(const Ice::Current& current) {
//	addObject(1,1000000); //添加方式1：范围添加方式
	LongSeq ids;
	for(long i = 1; i <= 1000000; i++){
		ids.push_back(i);
	}
//	addObject(ids); //添加方式2：同步添加
	TaskManager::instance().execute(new LoadTask(ids, *this)); //添加方式3：异步添加
}

void ObjectCacheDemoI::preload(const Ice::Current& current) {
	LongSeq ids;
	for(long i = 1; i <= 1000000; i++){
		ids.push_back(i);
	}
	_cache->preloadObjects(UE, ids, current);
}

void ObjectCacheDemoI::reload(const Ice::Current& current) {
	LongSeq ids;
	for(long i = 1; i <= 1000000; i++){
		ids.push_back(i);
	}
	_cache->reloadObjects(UE, ids, current);
}

void ObjectCacheDemoI::addObject(long id){
	_cache->addObject(UE, id, new UserEmail(id));
}

void ObjectCacheDemoI::addObject(LongSeq& ids){
	for(LongSeq::iterator i = ids.begin(); i!= ids.end(); i++){
		_cache->addObject(UE, *i, new UserEmail(*i));
	}
}

void ObjectCacheDemoI::addObject(long id, long range){
	for(long i = id; i < id + range; i++){
		_cache->addObject(UE, i, new UserEmail(i));
	}
}

int ObjectCacheDemoI::size(const Ice::Current&) {
	ObjectBaseMapPtr evictor = _cache->locateCategory(0);
	if(evictor)return evictor->size();
	else return 0;
}

string ObjectCacheDemoI::getEmail(long id, const Ice::Current&) {
	UserEmailPtr ue = UserEmailPtr::dynamicCast(_cache->locateObject(0, (long)id));
	return ue->mail();
}

void ObjectCacheDemoI::setEmail(long id, const string& email, const Ice::Current&) {
	UserEmailPtr ue = UserEmailPtr::dynamicCast(_cache->locateObject(0, (long)id));
	ue->mail(email);
}

string ObjectCacheDemoI::touchEmail(long id, const Ice::Current&) {
	UserEmailPtr ue = UserEmailPtr::dynamicCast(_cache->findObject(0, (long)id));
	if(ue){
		return ue->mail();
	}else{
		return string("NULL");
	}
}

MyUtil::ObjectCacheIPtr& ObjectCacheDemoI::getCache(){
	return _cache;
}

//FactoryI //模拟加载数据
//-------------------------------------------------------------
Ice::ObjectPtr DomoCacheFactoryI::create(Ice::Long id) {
	UserEmailPtr obj = new UserEmail(id);
	return obj;
}

MyUtil::ObjectResultPtr DomoCacheFactoryI::create(const MyUtil::LongSeq& ids) {
	ObjectResultPtr res = new ObjectResult;
	if (ids.empty()) {
		return res;
	}
	for (size_t i = 0; i < ids.size(); ++i) {
		try {
			Ice::ObjectPtr obj = create(ids.at(i));
			if (obj) {
				res->data[ids.at(i)] = obj;
			}
		} catch (...) {
			cerr << "Unknown error! " << endl;
		}
	}
	return res;
}
