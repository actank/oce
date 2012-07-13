/*
 * SearchMemCacheAdapter.cpp
 *
 *  Created on: 2008-8-22
 *      Author: bbc
 */

#include "SearchMemCacheAdapter.h"

using namespace xce::searchcache;
using namespace MyUtil;

SearchMemCacheAdapter::SearchMemCacheAdapter(){ 
	_managers.resize(cluster() > 0 ? cluster() : 1);
}

bool SearchMemCacheAdapter::doRemoveMemCacheData(const MyUtil::LongSeq& userIds) {
    //MCE_INFO("SearchMemCacheAdapter::doRemoveMemCacheData" << userIds.size());
	if (userIds.size() <= 0) {
		return true;
	}
	for(unsigned int i=0;i<cluster();++i){
		vector<SearchCacheManagerPrx> proxies=locateAll<SearchCacheManagerPrx>(_managers, "M", i, TWO_WAY);
		for(vector<SearchCacheManagerPrx>::iterator it=proxies.begin();it!=proxies.end();++it){
      try {
        (*it)->removes(userIds);
      } catch (Ice::Exception& e) {
        MCE_WARN("Ice::Exception in SearchMemCacheAdapter::doRemoveMemCacheData. prx=" << *it << ". " << e.what());
      } catch (std::exception& e) {
        MCE_WARN("std::exception in SearchMemCacheAdapter::doRemoveMemCacheData. prx=" << *it  << "." << e.what())
      } catch (...) {
        MCE_WARN("unknown exception in SearchMemCacheAdapter::doRemoveMemCacheData. prx=" << *it << ".");
      }
    }
	}
    return true;
}

bool SearchMemCacheAdapter::doUpdateMemCacheData(const Int2ByteSeq& id2Map) {
    //MCE_INFO("SearchMemCacheAdapter::doUpdateMemCacheData" << id2Map.size());
    if (id2Map.size() <= 0) {
		return true;
	}

	for(unsigned int i=0;i<cluster();++i){
		vector<SearchCacheManagerPrx> proxies=locateAll<SearchCacheManagerPrx>(_managers, "M", i, TWO_WAY);
		for(vector<SearchCacheManagerPrx>::iterator it=proxies.begin();it!=proxies.end();++it){
      try {
        (*it)->puts(id2Map);
      } catch (Ice::Exception& e) {
        MCE_WARN("Ice::Exception in SearchMemCacheAdapter::doUpdateMemCacheData. prx=" << *it << ". " << e.what());
      } catch (std::exception& e) {
        MCE_WARN("std::exception in SearchMemCacheAdapter::doUpdateMemCacheData. prx=" << *it  << "." << e.what())
      } catch (...) {
        MCE_WARN("unknown exception in SearchMemCacheAdapter::doUpdateMemCacheData. prx=" << *it << ".");
      }

		}
	}

    return true;
}

Int2SearchCacheResultMap SearchMemCacheAdapter::getSearchCacheMap(const MyUtil::IntSeq& userIds) {
    Int2SearchCacheResultMap allResults;
    if(userIds.size() <= 0)
        return allResults;

    for(unsigned int i=0;i<cluster();++i){
		vector<SearchCacheManagerPrx> proxies=locateAll<SearchCacheManagerPrx>(_managers, "M", i, TWO_WAY);
		for(vector<SearchCacheManagerPrx>::iterator it=proxies.begin();it!=proxies.end();++it){
      try{

            Int2SearchCacheResultMap subResults = (*it)->getSearchCacheList(userIds);

            for(Int2SearchCacheResultMap::const_iterator iter=subResults.begin(); iter!=subResults.end(); ++iter)
            {
                allResults[iter->first] = iter->second;
            }
      } catch (Ice::Exception& e) {
        MCE_WARN("Ice::Exception in SearchMemCacheAdapter::getSearchCacheMap. prx=" << *it << ". " << e.what());
      } catch (std::exception& e) {
        MCE_WARN("std::exception in SearchMemCacheAdapter::getSearchCacheMap. prx=" << *it << ". " << e.what())
      } catch (...) {
        MCE_WARN("unknown exception in SearchMemCacheAdapter::getSearchCacheMap. prx=" << *it << ".");
      }


		}
	}

    return allResults;
}

Int2ByteSeq SearchMemCacheAdapter::getSearchCacheMap2(const MyUtil::IntSeq& userIds) {
    Int2ByteSeq allResults;
    if(userIds.size() <= 0)
        return allResults;

    for(unsigned int i=0;i<cluster();++i){
		vector<SearchCacheManagerPrx> proxies=locateAll<SearchCacheManagerPrx>(_managers, "M", i, TWO_WAY);
		for(vector<SearchCacheManagerPrx>::iterator it=proxies.begin();it!=proxies.end();++it){
      try {

            Int2ByteSeq subResults = (*it)->getSearchCacheList2(userIds);

            for(Int2ByteSeq::const_iterator iter=subResults.begin(); iter!=subResults.end(); ++iter)
            {
                allResults[iter->first] = iter->second;
            }
      } catch (Ice::Exception& e) {
        MCE_WARN("Ice::Exception in SearchMemCacheAdapter::getSearchCacheMap2. prx=" << *it <<". " << e.what());
      } catch (std::exception& e) {
        MCE_WARN("std::exception in SearchMemCacheAdapter::getSearchCacheMap2. prx=" << *it <<". "<< e.what())
      } catch (...) {
        MCE_WARN("unknown exception in SearchMemCacheAdapter::getSearchCacheMap2. prx=" << *it << ".");
      }
		}
	}

    return allResults;
}

//TODO: locate all 与 locate 一个, 可能会在将来产生问题
bool SearchMemCacheAdapter::IsValid(const int mod) {
  try {
    vector<SearchCacheManagerPrx> proxies=locateAll<SearchCacheManagerPrx>(_managers, "M", mod, TWO_WAY);
    for(vector<SearchCacheManagerPrx>::iterator it=proxies.begin();it!=proxies.end();++it){
      try {
        return (*it)->isValid();
      } catch (Ice::Exception& e) {
        MCE_WARN("Ice::Exception in SearchMemCacheAdapter::IsValid. prx=" << *it << ". " << e.what());
      } catch (std::exception& e) {
        MCE_WARN("std::exception in SearchMemCacheAdapter::IsValid. prx=" << *it  << "." << e.what())
      } catch (...) {
        MCE_WARN("unknown exception in SearchMemCacheAdapter::IsValid. prx=" << *it << ".");
      }
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("Ice::Exception in SearchMemCacheAdapter::IsValid." << e.what());
  } catch (std::exception& e) {
    MCE_WARN("std::exception in SearchMemCacheAdapter::IsValid." << e.what())
  } catch (...) {
    MCE_WARN("unknown exception in SearchMemCacheAdapter::IsValid.");
  }
  return false;
}

bool SearchMemCacheAdapter::SetValid(const int mod, bool status) {
  try {
    vector<SearchCacheManagerPrx> proxies=locateAll<SearchCacheManagerPrx>(_managers, "M", mod, TWO_WAY);
    for(vector<SearchCacheManagerPrx>::iterator it=proxies.begin();it!=proxies.end();++it){
      try {
        (*it)->setValid(status);
      } catch (Ice::Exception& e) {
        MCE_WARN("Ice::Exception in SearchMemCacheAdapter::SetValid. prx=" << *it << ". " << e.what());
      } catch (std::exception& e) {
        MCE_WARN("std::exception in SearchMemCacheAdapter::SetValid. prx=" << *it  << "." << e.what())
      } catch (...) {
        MCE_WARN("unknown exception in SearchMemCacheAdapter::SetValid. prx=" << *it << ".");
      }
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("Ice::Exception in SearchMemCacheAdapter::SetValid." << e.what());
  } catch (std::exception& e) {
    MCE_WARN("std::exception in SearchMemCacheAdapter::SetValid." << e.what())
  } catch (...) {
    MCE_WARN("unknown exception in SearchMemCacheAdapter::SetValid.");
  }

  return true;

}

bool SearchMemCacheAdapter::isValid(const Ice::ObjectPrx& proxy){
	return SearchCacheManagerPrx::uncheckedCast(proxy)->isValid();
}


