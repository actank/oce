/*
 * SearchCacheAdapter.cpp
 *
 *  Created on: 2008-8-22
 *      Author: bbc
 */

#include "SearchCacheAdapter.h"

using namespace xce::searchcache;
using namespace MyUtil;

void SearchCacheAdapter::load(int id) {
	_invokeLoad.invoke(id);
}

void SearchCacheAdapter::load(const MyUtil::IntSeq& ids) {
	_invokeLoad.invoke(ids);
}

void SearchCacheAdapter::update(const int id, const std::string tableName, const std::map<string, string>& fieldPairMap)
{
    _invokeUpdate.invoke(id, tableName, fieldPairMap);
}

SearchCacheAdapter::SearchCacheAdapter(){ 
	_invokeLoad.start(128 * 1024).detach();
	_invokeUpdate.start(128 * 1024).detach();
	_managers.resize(cluster() > 0 ? cluster() : 1);
}

void SearchCacheAdapter::doLoad(const MyUtil::IntSeq& ids) {
	MCE_DEBUG("SearchCacheAdapter::doLoad " << ids.size());
	if (ids.size() <= 0) {
		return;
	}
	for(unsigned int i=0;i<cluster();++i){
		vector<SearchCacheManagerPrx> proxies=locateAll<SearchCacheManagerPrx>(_managers, "M", i, TWO_WAY);
		for(vector<SearchCacheManagerPrx>::iterator it=proxies.begin();it!=proxies.end();++it){
			(*it)->load(ids);
		}
	}
}

void SearchCacheAdapter::doUpdate(const UpdateUnitSeq& updateUnits) {
  MCE_DEBUG("SearchCacheAdapter::doUpdate " << updateUnits.size());
	if (updateUnits.size() <= 0) {
		return;
	}
	for(unsigned int i=0;i<cluster();++i){
		vector<SearchCacheManagerPrx> proxies=locateAll<SearchCacheManagerPrx>(_managers, "M", i, TWO_WAY);
		for(vector<SearchCacheManagerPrx>::iterator it=proxies.begin();it!=proxies.end();++it){
			(*it)->update(updateUnits);
		}
	}
}

bool SearchCacheAdapter::doRemoveMemCacheData(const MyUtil::LongSeq& userIds) {
    MCE_DEBUG("SearchCacheAdapter::doRemoveMemCacheData" << userIds.size());
	if (userIds.size() <= 0) {
		return true;
	}
	for(unsigned int i=0;i<cluster();++i){
		vector<SearchCacheManagerPrx> proxies=locateAll<SearchCacheManagerPrx>(_managers, "M", i, TWO_WAY);
		for(vector<SearchCacheManagerPrx>::iterator it=proxies.begin();it!=proxies.end();++it){
			(*it)->removes(userIds);
		}
	}
    return true;
}

bool SearchCacheAdapter::doUpdateMemCacheData(const Int2ByteSeq& id2Map) {
    MCE_DEBUG("SearchCacheAdapter::doUpdateMemCacheData" << id2Map.size());
    if (id2Map.size() <= 0) {
		return true;
	}

	for(unsigned int i=0;i<cluster();++i){
		vector<SearchCacheManagerPrx> proxies=locateAll<SearchCacheManagerPrx>(_managers, "M", i, TWO_WAY);
		for(vector<SearchCacheManagerPrx>::iterator it=proxies.begin();it!=proxies.end();++it){
      (*it)->puts(id2Map);
		}
	}

    return true;
}

Int2SearchCacheResultMap SearchCacheAdapter::getSearchCacheMap(const MyUtil::IntSeq& userIds) {
    Int2SearchCacheResultMap allResults;
    if(userIds.size() <= 0)
        return allResults;

    for(unsigned int i=0;i<cluster();++i){
		vector<SearchCacheManagerPrx> proxies=locateAll<SearchCacheManagerPrx>(_managers, "M", i, TWO_WAY);
		for(vector<SearchCacheManagerPrx>::iterator it=proxies.begin();it!=proxies.end();++it){
            Int2SearchCacheResultMap subResults = (*it)->getSearchCacheList(userIds);

            for(Int2SearchCacheResultMap::const_iterator iter=subResults.begin(); iter!=subResults.end(); ++iter)
            {
                allResults[iter->first] = iter->second;
            }
		}
	}

    return allResults;
}

Int2ByteSeq SearchCacheAdapter::getSearchCacheMap2(const MyUtil::IntSeq& userIds) {
    Int2ByteSeq allResults;
    if(userIds.size() <= 0)
        return allResults;

    for(unsigned int i=0;i<cluster();++i){
		vector<SearchCacheManagerPrx> proxies=locateAll<SearchCacheManagerPrx>(_managers, "M", i, TWO_WAY);
		for(vector<SearchCacheManagerPrx>::iterator it=proxies.begin();it!=proxies.end();++it){
            Int2ByteSeq subResults = (*it)->getSearchCacheList2(userIds);

            for(Int2ByteSeq::const_iterator iter=subResults.begin(); iter!=subResults.end(); ++iter)
            {
                allResults[iter->first] = iter->second;
            }
		}
	}

    return allResults;
}




bool SearchCacheAdapter::isValid(const Ice::ObjectPrx& proxy){
	return SearchCacheManagerPrx::uncheckedCast(proxy)->isValid();
}

//***************************************************************************

void SearchCacheAdapter::SearchCacheLoadTask::handle() {
	SearchCacheAdapter::instance().doLoad(_ids);	
}

void SearchCacheAdapter::SearchCacheUpdateTask::handle() {
	SearchCacheAdapter::instance().doUpdate(_updateUnits);	
}
