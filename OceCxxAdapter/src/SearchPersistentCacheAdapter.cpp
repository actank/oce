/*
 * SearchPersistentCacheAdapter.cpp
 *
 *  Created on: 2009-09-23
 *      Author: scq
 */

#include "SearchPersistentCacheAdapter.h"

using namespace xce::searchcache;
using namespace MyUtil;

void SearchPersistentCacheAdapter::load(int id) {
	_invokeLoad.invoke(id);
}

void SearchPersistentCacheAdapter::load(const MyUtil::IntSeq& ids) {
	_invokeLoad.invoke(ids);
}

SearchPersistentCacheAdapter::SearchPersistentCacheAdapter(){ 
	_invokeLoad.start(128 * 1024).detach();
	_managers.resize(cluster() > 0 ? cluster() : 1);
}

void SearchPersistentCacheAdapter::doLoad(const MyUtil::IntSeq& ids) {
	MCE_INFO("SearchPersistentCacheAdapter::doLoad " << ids.size());
	for(unsigned int i=0;i<cluster();++i){
		vector<SearchPersistentCacheManagerPrx> proxies=locateAll<SearchPersistentCacheManagerPrx>(_managers, "M", i, ONE_WAY);
		for(vector<SearchPersistentCacheManagerPrx>::iterator it=proxies.begin();it!=proxies.end();++it){
			(*it)->load(ids);
		}
	}
}

bool SearchPersistentCacheAdapter::isValid(const Ice::ObjectPrx& proxy){
	return SearchPersistentCacheManagerPrx::uncheckedCast(proxy)->isValid();
}

//***************************************************************************
