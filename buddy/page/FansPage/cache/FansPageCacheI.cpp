/*
 * =====================================================================================
 *
 *       Filename:  FansPageCacheI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/16/2010 01:36:43 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#include "ServiceI.h"
#include "FansPageCacheI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <QueryRunner.h>
#include <time.h>
#include "buddy/page/share/common.h"
#include "FansPageReloaderAdapter.h"

using namespace xce::serverstate;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace ::com::xiaonei::xce;
using namespace MyUtil;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.registerObjectCache(FANS_PAGE_CACHE_DATA, "FP", new FansPageFactory);
  service.getAdapter()->add(&FansPageCacheI::instance(), service.createIdentity("FP", ""));
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FansPageCache.Mod");
  string endpoint = service.getCommunicator()->getProperties()->getProperty("FansPageCache.Endpoint");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FansPageCache.Interval", 5);
  service.registerXceObserver(new ServiceSchemaI);

  // 向Controller注册
  ServerStateSubscriber::instance().initialize("ControllerFansPageCache", &FansPageCacheI::instance(), mod, interval);
}

/*****************************************************************************/


FansPageCacheI::FansPageCacheI() {
  MCE_INFO("[FansPageCacheI::FansPageCacheI]");
}

int FansPageCacheI::updateState(int pageId, int userId, short state, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::updateState] pageId = " << pageId << ", userId = " << userId << ", state = " << state);
  PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, userId);
  if(obj == 0) {
    return 0;
  }
  obj->updateState(pageId, state);
  return 1;
}
  
void FansPageCacheI::updatePageEntry(const PageEntryPtr& page, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::updatePageEntry] id = " << page->id << ", pageId = " << page->pageId << ", userId = " << page->userId << ", state = " << page->state
            << ", oper = " << page->operatorId << ", addTime = " << page->addTime);
  PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, page->userId);
  if(obj == 0) {
    obj = new PageEntrySetI;
    ServiceI::instance().getObjectCache()->addObject(FANS_PAGE_CACHE_DATA, page->userId, obj);  
  }
  obj->updatePageEntry(page);
}

void FansPageCacheI::updatePageEntryBatch(const PageEntryList& pageList, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::updatePageEntryBatch] size = " << pageList.size());
  for(PageEntryList::const_iterator it = pageList.begin(); it != pageList.end(); ++it) {
    updatePageEntry(*it);
  }
}

int FansPageCacheI::insert(const PageEntryPtr& page, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::insert] id = " << page->id << ", pageId = " << page->pageId << ", userId = " << page->userId << ", state = " << page->state
            << ", oper = " << page->operatorId << ", addTime = " << page->addTime);
  PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, page->userId);
  if(obj == 0) {
    obj = new PageEntrySetI;
    ServiceI::instance().getObjectCache()->addObject(FANS_PAGE_CACHE_DATA, page->userId, obj);  
  }
  obj->insertPageEntry(page);
  return 1;
}

int FansPageCacheI::insertBatch(const PageEntryList& pageList, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::insertBatch] size = " << pageList.size());
  int count = 0;
  for(PageEntryList::const_iterator it = pageList.begin(); it != pageList.end(); ++it) {
    count += insert(*it);
  }
  return count;
}

int FansPageCacheI::deleteByUidPid(int pageId, int userId, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::deleteByUidPid] pageId = " << pageId << ", userId = " << userId);
  PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, userId);
  if(obj == 0) {
    return 0;
  }
  obj->deleteByPageId(pageId);
  return 1;
}

int FansPageCacheI::deleteByUid(int userId, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::deleteByUid] userId = " << userId);
  PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, userId);
  if(obj != 0) {
    obj->deleteByPageId(-1);
  }
  return 1;
}

int FansPageCacheI::deleteByPageId(int pageId, const Ice::Current& current) {
  return 0;
}

PageEntryList FansPageCacheI::getPagesByUid(int userId, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::getPagesByUid] userId = " << userId);
  PageEntryList list;
  PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, userId);
  if(obj != 0) {
    list = obj->getByState(-1);
  }
  sort(list.begin(), list.end(), CompPageEntryByStateAddTime());
  return list;
}

PageEntryListMap FansPageCacheI::getPagesByUidBatch(const MyUtil::IntSeq& userIds, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::getPagesByUidBatch] size = " << userIds.size());
  PageEntryListMap result;
  for(MyUtil::IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
    PageEntryList list = getPagesByUid(*it);
    if(list.size() > 0) {
      result.insert(make_pair(*it, list));
    }
  }
  return result;
}

PageEntryList FansPageCacheI::getPagesByUidState(int userId, short state, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::getPagesByUidState] userId = " << userId << ", state = " << state);
  PageEntryList list;
  PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, userId);
  if(obj != 0) {
    list = obj->getByState(state);
  }
  sort(list.begin(), list.end(), CompPageEntryByStateAddTime());
  return list;
}

int FansPageCacheI::getPagesCountByState(int userId, short state, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::getPagesCountByState] userId = " << userId << ", state = " << state);
  int count = 0;
  PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, userId);
  if(obj != 0) {
    count = obj->getCountByState(state);
  }
  return count;
}

PageEntryList FansPageCacheI::getPagesOfUser(int userId, const MyUtil::IntSeq& pageIds, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::getPagesOfUser] userId = " << userId << ", size = " << pageIds.size());
  PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, userId);
  if(obj != 0) {
    return obj->getPagesInIds(pageIds); 
  }
  return PageEntryList();
}

MyUtil::IntSeq FansPageCacheI::getPageIdsOfUser(int userId, const MyUtil::IntSeq& pageIds, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::getPageIdsOfUser] userId = " << userId << ", size = " << pageIds.size());
  PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, userId);
  if(obj != 0) {
    return obj->getPageIdsInIds(pageIds); 
  }
  return MyUtil::IntSeq();
}

MyUtil::Int2IntSeqMap FansPageCacheI::getPageIdsOfUserBatch(const MyUtil::Int2IntSeqMap& pageIdsMap, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::getPageIdsOfUserBatch] size = " << pageIdsMap.size());
  MyUtil::Int2IntSeqMap result;
  for(MyUtil::Int2IntSeqMap::const_iterator it = pageIdsMap.begin(); it != pageIdsMap.end(); ++it) {
    PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, it->first);
    if(obj != 0) {
       MyUtil::IntSeq pageIds = obj->getPageIdsInIds(it->second);
       if(pageIds.size() > 0) {
         result.insert(make_pair(it->first, pageIds));
       }
    }
  }
  return result;
}

int FansPageCacheI::getNormalPageCount(int userId, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::getNormalPageCount] userId = " << userId);
  PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, userId);
  if(obj != 0) {
    return obj->getCountByState(NORMAL_STATE);
  }
  return 0;
}

MyUtil::IntSeq FansPageCacheI::getAllUserPages(int userId, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::getAllUserPages] userId = " << userId);
  PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, userId);
  if(obj != 0) {
    return obj->getPageIdsByState(-1);
  }
  return MyUtil::IntSeq();
}

PageEntryList FansPageCacheI::getLimitPages(int userId, int id, int size, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::getLimitPagesByUid] userId = " << userId << ", id = " << id << ", size = " << size);
  PageEntryList list;
  int maxId = -1;
  int maxPageId = -1;
  BatchPageEntryResultHandlerI handler(maxId, maxPageId, list);
  Statement sql;
  ostringstream str_sql;
  str_sql << "select id, page_id, user_id, state, UNIX_TIMESTAMP(add_time), operator from " << PageHelper::getTableName(userId) << " where user_id = " 
          << userId << " and state = " << NORMAL_STATE << " order by id desc limit " << id << ", " << size;
  sql << str_sql.str();
  MCE_INFO("[FansPageCacheI::getLimitPagesByUid] sql = " << str_sql.str());
  QueryRunner(DATA_SOURCE, CDbRServer).query(sql, handler);
  return list;
}

void FansPageCacheI::addPageEntryToUser(int userId, const PageEntryList& list, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::addPageEntryToUser] userId = " << userId << ", list.size = " << list.size());
  PageEntrySetIPtr obj = ServiceI::instance().findObject<PageEntrySetIPtr>(FANS_PAGE_CACHE_DATA, userId);
  if(obj != 0) {
    obj = new PageEntrySetI;
    ServiceI::instance().getObjectCache()->addObject(FANS_PAGE_CACHE_DATA, userId, obj);  
  }
  obj->insertPageEntryBatch(list);
}

void FansPageCacheI::setData(const MyUtil::ObjectResultPtr& res, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::setData] size = " << res->data.size());
  MCE_INFO("[FansPageCacheI::setData] res.size / ObjectCache size : " << res->data.size() << " / " << ServiceI::instance().getObjectCache()->locateCategory(FANS_PAGE_CACHE_DATA)->size());
  if(res->data.size() == 0) {
    return;
  }
  MyUtil::ObjectResultPtr ptr = new MyUtil::ObjectResult;
  for(std::map<long, Ice::ObjectPtr>::const_iterator it = res->data.begin(); it != res->data.end(); ++it) {
    ptr->data.insert(make_pair(it->first, new PageEntrySetI((PageEntrySetPtr::dynamicCast(it->second))->pageEntries, true)));
  }
  ServiceI::instance().getObjectCache()->addObjects(FANS_PAGE_CACHE_DATA, ptr->data);
}

void FansPageCacheI::addPageEntryToUserBatch(const PageEntryListMap& listMap, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::addPageEntryToUserBatch] map.size = " << listMap.size());
  for(PageEntryListMap::const_iterator it = listMap.begin(); it != listMap.end(); ++it) {
    addPageEntryToUser(it->first, it->second);
  }
}

bool FansPageCacheI::isValid(const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::isValid] " << isValid_);
  IceUtil::RWRecMutex::RLock lock(validMutex_);
  return isValid_;
}

void FansPageCacheI::setValid(bool valid, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::setValid] " << valid);
  IceUtil::RWRecMutex::WLock lock(validMutex_);
  isValid_ = valid;
  if(valid) {
    ServerStateSubscriber::instance().setStatus(1);
  } else{
    ServerStateSubscriber::instance().setStatus(0);
  }
}

Ice::ObjectPtr FansPageFactory::create(int id) {
  FansPageReloaderAdapter::instance().loadFansPageByUid(id);
  return new PageEntrySetI;
}

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
  //TaskManager::instance().config(props, "Fill", TASK_LEVEL_BUDDY_BYID_CACHE_FILL, ThreadPoolConfig(1,100));
}

