#ifndef __FANS_PAGE_ICE__
#define __FANS_PAGE_ICE__

#include <ObjectCache.ice>
#include <Util.ice>
#include <PageCommon.ice>

module xce {
module buddy {

//以下方法对FansPageCache中的数据进行更新，并将对应的结果写回数据库
interface FansPageCache {
  //更新(pageId, userId)对应的PageEntry的状态为state
  int updateState(int pageId, int userId, short state);
  //为FansPageReloader提供的接口，更新一个PageEntry
  void updatePageEntry(PageEntry page);
  //为FansPageReloader提供的接口，更新一批PageEntry
  void updatePageEntryBatch(PageEntryList pageList);
  //插入一个PageEntry
  int insert(PageEntry page);
  //插入一批PageEntry
  int insertBatch(PageEntryList pageList);
  //删除(pageId, userId)对应的PageEntry
  int deleteByUidPid(int pageId, int userId);
  //删除用户userId的所有PageEntry
  int deleteByUid(int userId);
  //删除pageId对应的所有PageEntry
  int deleteByPageId(int pageId);
  //获取用户userId的所有PageEntry, order by state desc, addTime desc
  PageEntryList getPagesByUid(int userId);
  PageEntryListMap getPagesByUidBatch(MyUtil::IntSeq userIds);
  //用户用户userId的所有状态为state的PageEntry，按照addTime降序排列
  PageEntryList getPagesByUidState(int userId, short state);
  //获取用户userId的状态为state的PageEntry的数目
  int getPagesCountByState(int userId, short state);
  //获取pageIds中属于用户userId的所有PageEntry
  PageEntryList getPagesOfUser(int userId, MyUtil::IntSeq pageIds);
  MyUtil::IntSeq getPageIdsOfUser(int userId, MyUtil::IntSeq pageIds);
  MyUtil::Int2IntSeqMap getPageIdsOfUserBatch(MyUtil::Int2IntSeqMap userId2PageIdsMap);
  //必须保证list中每个对象的userId 都等于第一个参数
  void addPageEntryToUser(int userId, PageEntryList list);
  void addPageEntryToUserBatch(PageEntryListMap listMap);
  void setData(MyUtil::ObjectResult res);
  //获取用户userId的所有PageEntry的数目
  int getNormalPageCount(int userId);
  //获取用户userId的所有PageEntry的pageId
  MyUtil::IntSeq getAllUserPages(int userId);
  //user_id={userid} and state={真正的粉丝} order by id desc limit {page} ,{pageSize}对应的所有PageEntry
  PageEntryList getLimitPages(int userId, int id, int size);
  bool isValid();
  void setValid(bool valid);
};

//FansPageReloader中的接口都是从数据库读取相关数据，对FansPageCache中的内容进行更新
interface FansPageReloader {
  void updateState(int pageId, int userId, short state);
  void insert(PageEntry page);
  //插入一批PageEntry
  void insertBatch(PageEntryList pageList);
  void deleteByUidPid(int pageId, int userId);
  //删除用户userId的所有PageEntry
  void deleteByUid(int userId);
  //更新userId关注的所有PageEntry
  void loadFansPageByUid(int userId);
  //更新userId,pageId对应的PageEntry
  void loadFansPageByUidPid(int pageId, int userId);
  //更新userId和state对应的所有PageEntry
  void loadFansPageByUidState(int userId, short state);
  bool isValid();
  void setValid(bool valid);
};

interface FansPagePreloader {
  void preloadAll();
  bool isValid();
  void setValid(bool valid);
};

};
};

#endif
