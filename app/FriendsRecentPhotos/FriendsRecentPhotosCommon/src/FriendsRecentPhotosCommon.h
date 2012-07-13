/*
 * =====================================================================================
 *
 *       Filename:  FriendsRecentPhotosCommon.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/02/2011 09:30:02 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#ifndef __FRIENDS_RECENT_PHOTOS_COMMON_H__
#define __FRIENDS_RECENT_PHOTOS_COMMON_H__

#include "FriendsRecentPhotos.h"

#include "QueryRunner.h"
#include "Singleton.h"
#include <vector>
#include <algorithm>
#include "ServiceI.h"

namespace xce {
namespace friendsphotos {

  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  using namespace std;

  const string DB_SOURCE = "friendsphotos";
  const int USER_PHOTOS_INFO = 1;
  const int USER_FRIENDS = 2;
  
  //设定的最大好友数，如果一个用户的好友数多于这个数，则认为该用户为节点用户
  const int MAX_FRIENDS_NUM = 5000;
  const size_t MAX_PHOTOS_SIZE = 18;
  const string TABLE_NAME = "user_recent_photos";
  const string COLUMN_USERID = "user_id";
  const string COLUMN_PHOTOS = "photos";
  const int TABLE_MOD = 100;

  const int PRODUCE_BATCH_SIZE = 500;
  const int CONSUME_BATCH_SIZE = 500;
  const int CONSUME_BATCH_THREAD_SIZE = 5;
  const int WRITE_BATCH_SIZE = 1000;
  const int PRX_DATAQUEUE_MAX_SIZE = 50000;

  class PhotoWithOwnerId; 
  typedef IceUtil::Handle<PhotoWithOwnerId> PhotoWithOwnerIdPtr;

  //在对结果排序的时候用到的一个临时的数据结构
  class PhotoWithOwnerId : virtual public Ice::Object {
    public:
      int ownerId;
      int uploadTime;
      long photoId;
      PhotoWithOwnerId(int pOwnerId, int pUploadTime, long pPhotoId) : ownerId(pOwnerId), uploadTime(pUploadTime), photoId(pPhotoId) {}
      static bool cmpPhotoWithOwnerIdPtr(const PhotoWithOwnerIdPtr& ptr1, const PhotoWithOwnerIdPtr& ptr2);
      static UserPhotosSeq formUserPhotosSeq(const vector<PhotoWithOwnerIdPtr>& photoWithOwnerIdPtrs);
      //将photoWithOwnerIdPtrs中的元素按照uploadTime从大到小排序后，取其中的第begin到end - 1个元素(inclusive)
      //实现的过程中不需要对photoWithOwnerIdPtrs整个进行排序
      static vector<PhotoWithOwnerIdPtr> getBetweenBeginEnd(vector<PhotoWithOwnerIdPtr>& photoWithOwnerIdPtrs, int begin, int end);
  };

  class PhotoInfo : virtual public Ice::Object {
    public:
      PhotoInfo(int pUploadTime, long pPhotoId);
      int getUploadTime() const;
      long getPhotoId() const;
    private:
      int uploadTime;
      long photoId;
  };

  typedef IceUtil::Handle<PhotoInfo> PhotoInfoPtr; 

  class UserPhotosInfo;
  
  typedef IceUtil::Handle<UserPhotosInfo> UserPhotosInfoPtr;
  
  class UserPhotosInfo : virtual public Ice::Object, virtual public IceUtil::Mutex {
    public:
      static vector<UserPhotosInfoPtr> generateEmptyUserPhotosInfo(const vector<int>& userIds);
      static vector<int> getUserIdsFromUserPhotosInfoSeq(const vector<UserPhotosInfoPtr>& ptrs);
      static MyUtil::ObjectResultPtr parseFromUserPhotosResultPtr(const MyUtil::ObjectResultPtr& ptr);

      static MyUtil::ObjectResultPtr parseFromUserPhotosSeq(const UserPhotosSeq& userPhotos);

      static MyUtil::ObjectResultPtr changeToUserPhotosInfoMap(const vector<UserPhotosInfoPtr>& ptrs);

      static vector<int> getUserIdsFromUserPhotosSeq(const UserPhotosSeq& userPhotosSeq);

      UserPhotosInfo(const UserPhotosPtr& userPhotosPtr);
      UserPhotosInfo(int userId);
      UserPhotosPtr parseTo();
      void addPhotoInfo(const PhotoInfoPtr& photoInfoPtr);
      void addPhotoInfo(const vector<PhotoInfoPtr>& photoInfoPtrs);
      void setPhotoInfo(const vector<PhotoInfoPtr>& photoInfoPtrs);
      void deletePhotoInfoById(long photoId);
      void deletePhotoInfoById(const vector<long>& photoIds);
      vector<PhotoInfoPtr> getPhotoInfos() const;
      vector<long> getPhotoIds() const;
      vector<int> getUploadTimes() const;
      int getUserId() const;
      void setUserId(int pUserId);
      void clearPhotoInfos();
      vector<PhotoWithOwnerIdPtr> toPhotoWithOwnerIdPtrs();
      string toString() const;
    private:
      int userId;
      vector<PhotoInfoPtr> photos;
  };


  //所有空对象都指到这一个上
  const UserPhotosInfoPtr EMPTY_USER_PHOTOS_INFO_PTR = new UserPhotosInfo(-1);
  const UserPhotosPtr EMPTY_USER_PHOTOS_PTR = new UserPhotos;

  //存放节点用户的好友，因为节点用户的好友太多了，不能从Buddy服务中现取，所以必须存储起来
  class UserFriends : virtual public Ice::Object, virtual public IceUtil::Mutex {
    public:
      UserFriends(int pUserId, const vector<int>& pFriends);
      void setUserId(int pUserId);
      //内部使用swap, pFriends完了以后数据就变了
      void setFriends(vector<int>& pFriends);
      int getUserId() const;
      vector<int> getFriends() const;
    private:
      int userId;
      vector<int> friends;
  };

  typedef IceUtil::Handle<UserFriends> UserFriendsPtr;

  PhotoResultPtr getPhotoResultFromUsers(ServiceI& service, const vector<int>& friends, int begin, int limit);

  string serialize(const vector<int>& uploadTimes, const vector<long>& photoIds);

  //查找所有在setA中出现，但是没有在setB中出现的元素,如果A,B是两个集合的话，用数学语言描述就是求A - B
  template<typename T>
  vector<T> findElementsInANotB(const vector<T>& setA, const vector<T>& setB);


  class UserPhotosInfoResultHandlerI : public com::xiaonei::xce::ResultHandler {
    public:
      UserPhotosInfoResultHandlerI(vector<UserPhotosInfoPtr>& userPhotosInfoPtrs);
      virtual bool handle(mysqlpp::Row&) const;
    private:
      vector<UserPhotosInfoPtr>& userPhotosInfoPtrs;
  };
  
  template<typename T>
  class BatchIdResultHandler : public com::xiaonei::xce::ResultHandler {
    public:
      BatchIdResultHandler(const string& idColumnName, vector<T>& ids);
      virtual bool handle(mysqlpp::Row&) const;
    private:
      string _idColumnName;
      vector<T>& _ids;
  };
}
}

#endif
