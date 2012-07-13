/*
 * =====================================================================================
 *
 *       Filename:  FansPageCacheAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/16/2010 08:24:52 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#ifndef __FANSPAGE_CACHE_ADAPTER_H__
#define __FANSPAGE_CACHE_ADAPTER_H__

#include <IceUtil/RWRecMutex.h>
#include <FansPage.h>
#include <AdapterI.h>
#include <TaskManager.h>

namespace xce {
namespace buddy {
namespace adapter {

class FansPageCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<FansPageCachePrx>, public MyUtil::Singleton<FansPageCacheAdapter> {
public:
  FansPageCacheAdapter();
  int updateState(int pageId, int userId, short state);
  void updatePageEntry(const PageEntryPtr page);
  void updatePageEntryBatch(const PageEntryList pageList);
  int insert(const PageEntryPtr page);
  int insertBatch(const PageEntryList pageList);
  int deleteByUidPid(int pageId, int userId);
  int deleteByUid(int userId);
  PageEntryList getPagesByUid(int userId);
  PageEntryListMap getPagesByUidBatch(const MyUtil::IntSeq& userIds);
  PageEntryList getPagesByUidState(int userId, short state);
  int getPagesCountByState(int userId, short state);
  PageEntryList getPagesOfUser(int userId, const MyUtil::IntSeq& pageIds);
  MyUtil::IntSeq getPageIdsOfUser(int userId, const MyUtil::IntSeq& pageIds);
  int getNormalPageCount(int userId);
  MyUtil::IntSeq getAllUserPages(int userId);
  PageEntryList getLimitPages(int userId, int id, int size);
  void addPageEntryToUser(int userId, const PageEntryList& pageList);
  void addPageEntryToUserBatch(const PageEntryListMap& listMap);
  void setData(const MyUtil::ObjectResult& res);
  bool isValid();
  void setValid(bool valid);
private:
  vector<PageEntryList> distribute(const PageEntryList list, int size);
};

}
}
}
#endif
