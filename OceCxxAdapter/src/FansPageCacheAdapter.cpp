/*
 * =====================================================================================
 *
 *       Filename:  FansPageCacheAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/16/2010 08:26:13 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#include "FansPageCacheAdapter.h"
#include "FansPage.h"
 
using namespace xce::buddy::adapter;
using namespace xce::buddy;

FansPageCacheAdapter::FansPageCacheAdapter() : MyUtil::ReplicatedClusterAdapterI <FansPageCachePrx>("ControllerFansPageCache", 120, 300){
}

int FansPageCacheAdapter::updateState(int pageId, int userId, short state) {
  vector<FansPageCachePrx> prxs = getAllProxySeq(userId);
  for(vector<FansPageCachePrx>::const_iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx) {
    try {
      (*itPrx)->updateState(pageId, userId, state);
    } catch (const Ice::Exception& e) {
      MCE_WARN("[FansPageCacheAdapter::updateState] Ice::Exception " << e);
    }
  }
  return 0;
}

void FansPageCacheAdapter::updatePageEntry(const PageEntryPtr page) {
  vector<FansPageCachePrx> prxs = getAllProxySeq(page->userId);
  for(vector<FansPageCachePrx>::const_iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx) {
    try {
      (*itPrx)->updatePageEntry(page);
    } catch (const Ice::Exception& e) {
      MCE_WARN("[FansPageCacheAdapter::updatePageEntry] Ice::Exception " << e);
    }
  }
}

void FansPageCacheAdapter::updatePageEntryBatch(const PageEntryList pageList) {
  MCE_INFO("[FansPageCacheAdapter::updatePageEntryBatch] size = " << pageList.size());
  int size = getCluster();
  vector<PageEntryList> disList = distribute(pageList, size);
  for(vector<PageEntryList>::const_iterator it = disList.begin(); it != disList.end(); ++it) {
    if((*it).size() == 0) {
      continue;
    }
    vector<FansPageCachePrx> prxs = getAllProxySeq((*it)[0]->userId);
    for(vector<FansPageCachePrx>::const_iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx) {
      try {
        (*itPrx)->updatePageEntryBatch(*it);
      } catch (const Ice::Exception& e) {
        MCE_WARN("[FansPageCacheAdapter::updatePageEntryList] Ice::Exception " << e);
      }
    }
  }
}

int FansPageCacheAdapter::insert(const PageEntryPtr page) {
  vector<FansPageCachePrx> prxs = getAllProxySeq(page->userId);
  for(vector<FansPageCachePrx>::const_iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx) {
    try {
      (*itPrx)->insert(page);
    } catch (const Ice::Exception& e) {
      MCE_WARN("[FansPageCacheAdapter::insert] Ice::Exception " << e);
    }
  }
  return 0;
}

int FansPageCacheAdapter::insertBatch(const PageEntryList pageList) {
  int size = getCluster();
  vector<PageEntryList> disList = distribute(pageList, size);
  for(vector<PageEntryList>::const_iterator it = disList.begin(); it != disList.end(); ++it) {
    if((*it).size() == 0) {
      continue;
    }
    vector<FansPageCachePrx> prxs = getAllProxySeq((*it)[0]->userId);
    for(vector<FansPageCachePrx>::const_iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx) {
      try {
        (*itPrx)->insertBatch(*it);
      } catch (const Ice::Exception& e) {
        MCE_WARN("[FansPageCacheAdapter::insertBatch] Ice::Exception " << e);
      }
    }
  }
  return 0;
}

int FansPageCacheAdapter::deleteByUidPid(int pageId, int userId) {
  vector<FansPageCachePrx> prxs = getAllProxySeq(userId);
  for(vector<FansPageCachePrx>::const_iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx) {
    try {
      (*itPrx)->deleteByUidPid(pageId, userId);
    } catch (const Ice::Exception& e) {
      MCE_WARN("[FansPageCacheAdapter::deleteByUidPid] Ice::Exception " << e);
    }
  }
  return 0;
}

int FansPageCacheAdapter::deleteByUid(int userId) {
  vector<FansPageCachePrx> prxs = getAllProxySeq(userId);
  for(vector<FansPageCachePrx>::const_iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx) {
    try {
      (*itPrx)->deleteByUid(userId);
    } catch (const Ice::Exception& e) {
      MCE_WARN("[FansPageCacheAdapter::deleteByUid] Ice::Exception " << e);
    }
  }
  return 0;
}

void FansPageCacheAdapter::addPageEntryToUser(int userId, const PageEntryList& pageList) {
  vector<FansPageCachePrx> prxs = getAllProxySeq(userId);
  for(vector<FansPageCachePrx>::const_iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx) {
    try {
      (*itPrx)->addPageEntryToUser(userId, pageList);
    } catch (const Ice::Exception& e) {
      MCE_WARN("[FansPageCacheAdapter::addPageEntryToUser] Ice::Exception " << e);
    }
  }
}

void FansPageCacheAdapter::addPageEntryToUserBatch(const PageEntryListMap& listMap) {
  for(PageEntryListMap::const_iterator it = listMap.begin(); it != listMap.end(); ++it) {
    addPageEntryToUser(it->first, it->second);
  }
}

PageEntryList FansPageCacheAdapter::getPagesByUid(int userId) {
  try {
    return getProxy(0)->getPagesByUid(userId);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageCacheAdapter::getPagesByUid] Ice::Exception " << e);
    return PageEntryList();
  }
}

PageEntryListMap FansPageCacheAdapter::getPagesByUidBatch(const MyUtil::IntSeq& userIds) {
  try {
    return getProxy(0)->getPagesByUidBatch(userIds);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageCacheAdapter::getPagesByUidBatch] Ice::Exception " << e);
    return PageEntryListMap();
  }
}

PageEntryList FansPageCacheAdapter::getPagesByUidState(int userId, short state) {
  try {
    return getProxy(0)->getPagesByUidState(userId, state);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageCacheAdapter::getPagesByUidState] Ice::Exception " << e);
    return PageEntryList();
  }
}

int FansPageCacheAdapter::getPagesCountByState(int userId, short state) {
  try {
    return getProxy(0)->getPagesCountByState(userId, state);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageCacheAdapter::getPagesCountByState] Ice::Exception " << e);
    return 0;
  }
}

PageEntryList FansPageCacheAdapter::getPagesOfUser(int userId, const MyUtil::IntSeq& pageIds) {
  try {
    return getProxy(0)->getPagesOfUser(userId, pageIds);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageCacheAdapter::getPagesOfUser] Ice::Exception " << e);
    return PageEntryList();
  }
}

MyUtil::IntSeq FansPageCacheAdapter::getPageIdsOfUser(int userId, const MyUtil::IntSeq& pageIds) {
  try {
    return getProxy(0)->getPageIdsOfUser(userId, pageIds);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageCacheAdapter::getPageIdsOfUser] Ice::Exception " << e);
    return MyUtil::IntSeq();
  }
}

int FansPageCacheAdapter::getNormalPageCount(int userId) {
  try {
    return getProxy(0)->getNormalPageCount(userId);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageCacheAdapter::getPageCountOfUser] Ice::Exception " << e);
    return 0;
  }
}

MyUtil::IntSeq FansPageCacheAdapter::getAllUserPages(int userId) {
  try {
    return getProxy(0)->getAllUserPages(userId);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageCacheAdapter::getUserPages] Ice::Exception " << e);
    return MyUtil::IntSeq();
  }
}

PageEntryList FansPageCacheAdapter::getLimitPages(int userId, int id, int size) {
  try {
    return getProxy(0)->getLimitPages(userId, id, size);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageCacheAdapter::getLimitPagesByUid] Ice::Exception " << e);
    return PageEntryList();
  }
}

bool FansPageCacheAdapter::isValid() {
  try {
    return getProxy(0)->isValid();
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageCacheAdapter::isValid] Ice::Exception " << e);
    return false;
  }
}

void FansPageCacheAdapter::setValid(bool valid) {
  try {
    getProxy(0)->setValid(valid);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageCacheAdapter::setValid] Ice::Exception " << e);
  }
}

vector<PageEntryList> FansPageCacheAdapter::distribute(const PageEntryList list, int size) {
  vector<PageEntryList> result(size);
  for(PageEntryList::const_iterator it = list.begin(); it != list.end(); ++it) {
    result[(*it)->userId % size].push_back(*it);
  }
  return result;
}
