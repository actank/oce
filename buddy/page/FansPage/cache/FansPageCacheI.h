/*
 * =====================================================================================
 *
 *       Filename:  FansPageCache.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/15/2010 06:13:33 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#ifndef __FANSPAGE_CACHEI_H__
#define __FANSPAGE_CACHEI_H__

#include <map>
#include <Singleton.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"
#include "FansPage.h"

namespace xce {
namespace buddy {

const int NORMAL_STATE = 0;
const int FANS_PAGE_CACHE_DATA = 0;

class FansPageCacheI : virtual public FansPageCache, virtual public MyUtil::Singleton<FansPageCacheI> {
public:
  FansPageCacheI();
  virtual int updateState(int pageId, int userId, short state, const Ice::Current& current = Ice::Current());
  //为FansPageReloader提供的接口，更新一个PageEntry
  virtual void updatePageEntry(const PageEntryPtr& page, const Ice::Current& current = Ice::Current());
  //为FansPageReloader提供的接口，更新一批PageEntry
  virtual void updatePageEntryBatch(const PageEntryList& pageList, const Ice::Current& current = Ice::Current());
  virtual int insert(const PageEntryPtr& page, const Ice::Current& current = Ice::Current());
  virtual int insertBatch(const PageEntryList& pageList, const Ice::Current& current = Ice::Current());
  virtual int deleteByUidPid(int pageId, int userId, const Ice::Current& current = Ice::Current());
  virtual int deleteByUid(int userId, const Ice::Current& current = Ice::Current());
  virtual int deleteByPageId(int pageId, const Ice::Current& current = Ice::Current());
  virtual PageEntryList getPagesByUid(int userId, const Ice::Current& current = Ice::Current());
  virtual PageEntryListMap getPagesByUidBatch(const MyUtil::IntSeq& userIds, const Ice::Current& current = Ice::Current());
  virtual PageEntryList getPagesByUidState(int userId, short state, const Ice::Current& current = Ice::Current());
  virtual int getPagesCountByState(int userId, short state, const Ice::Current& current = Ice::Current());
  virtual PageEntryList getPagesOfUser(int userId, const MyUtil::IntSeq& pageIds, const Ice::Current& current = Ice::Current());
  virtual MyUtil::IntSeq getPageIdsOfUser(int userId, const MyUtil::IntSeq& pageIds, const Ice::Current& current = Ice::Current());
  virtual MyUtil::Int2IntSeqMap getPageIdsOfUserBatch(const MyUtil::Int2IntSeqMap& pageIdsMap, const Ice::Current& current = Ice::Current());
  virtual int getNormalPageCount(int userId, const Ice::Current& current = Ice::Current());
  virtual MyUtil::IntSeq getAllUserPages(int userId, const Ice::Current& current = Ice::Current());
  virtual PageEntryList getLimitPages(int userId, int id, int size, const Ice::Current& current = Ice::Current());
  virtual void addPageEntryToUser(int userId, const PageEntryList& list, const Ice::Current& current = Ice::Current());
  virtual void setData(const MyUtil::ObjectResultPtr& res, const Ice::Current& current = Ice::Current());
  virtual void addPageEntryToUserBatch(const PageEntryListMap& list, const Ice::Current& current = Ice::Current());
  virtual bool isValid(const Ice::Current& current = Ice::Current());
  virtual void setValid(bool valid, const Ice::Current& current = Ice::Current());
private:
  bool isEqual(const PageEntryPtr& ptr1, const PageEntryPtr& ptr2);
  void assignPageEntry(const PageEntryPtr& ptr1, const PageEntryPtr& ptr2);
  bool isValid_;
  IceUtil::RWRecMutex validMutex_;
};

class FansPageFactory : virtual public MyUtil::ServantFactory {
public:
  virtual Ice::ObjectPtr create(int id);
};

class ServiceSchemaI: public MyUtil::Descriptor {
public:
  virtual void configure(const Ice::PropertiesPtr& props);
};

}
}
#endif
