/*
 * =====================================================================================
 *
 *       Filename:  FriendsRecentPhotosCommon.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/02/2011 09:52:25 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#include "FriendsRecentPhotosCommon.h"
#include "util/cpp/IntegerSeqSerialize.h"
#include <sstream>

using namespace xce::friendsphotos;

bool PhotoWithOwnerId::cmpPhotoWithOwnerIdPtr(const PhotoWithOwnerIdPtr& ptr1, const PhotoWithOwnerIdPtr& ptr2) {
  return ptr1->photoId > ptr2->photoId;
}

UserPhotosSeq PhotoWithOwnerId::formUserPhotosSeq(const vector<PhotoWithOwnerIdPtr>& photoWithOwnerIdPtrs) {
  UserPhotosSeq result;
  for (vector<PhotoWithOwnerIdPtr>::const_iterator it = photoWithOwnerIdPtrs.begin(); it != photoWithOwnerIdPtrs.end(); ++it) {
    if (!result.empty() && (*(result.end() - 1))->userId == (*it)->ownerId) {
      (*(result.end() - 1))->photos.push_back(new Photo((*it)->uploadTime, (*it)->photoId));
    } else {
      UserPhotosPtr ptr = new UserPhotos;
      ptr->userId = (*it)->ownerId;
      ptr->photos.push_back(new Photo((*it)->uploadTime, (*it)->photoId));
      result.push_back(ptr);
    }
  }
  return result;
}

vector<PhotoWithOwnerIdPtr> PhotoWithOwnerId::getBetweenBeginEnd(vector<PhotoWithOwnerIdPtr>& photoWithOwnerIdPtrs, int begin, int end) {
  nth_element(photoWithOwnerIdPtrs.begin(), photoWithOwnerIdPtrs.begin() + begin, photoWithOwnerIdPtrs.end(), PhotoWithOwnerId::cmpPhotoWithOwnerIdPtr);
  nth_element(photoWithOwnerIdPtrs.begin() + begin, photoWithOwnerIdPtrs.begin() + end, photoWithOwnerIdPtrs.end(), PhotoWithOwnerId::cmpPhotoWithOwnerIdPtr);
  vector<PhotoWithOwnerIdPtr> result(photoWithOwnerIdPtrs.begin() + begin, photoWithOwnerIdPtrs.begin() + end);
  sort(result.begin(), result.end(), PhotoWithOwnerId::cmpPhotoWithOwnerIdPtr);
  return result;
}

PhotoInfo::PhotoInfo(int pUploadTime, long pPhotoId) : uploadTime(pUploadTime), photoId(pPhotoId) {
}

int PhotoInfo::getUploadTime() const {
  return this->uploadTime;
}

long PhotoInfo::getPhotoId() const {
  return this->photoId;
}

UserPhotosInfo::UserPhotosInfo(int pUserId) {
  this->userId = pUserId;
}

vector<UserPhotosInfoPtr> UserPhotosInfo::generateEmptyUserPhotosInfo(const vector<int>& userIds) {
  vector<UserPhotosInfoPtr> result;
  for (vector<int>::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
    result.push_back(new UserPhotosInfo(*it));
  }
  return result;
}

UserPhotosInfo::UserPhotosInfo(const UserPhotosPtr& userPhotosPtr) {
  this->userId = userPhotosPtr->userId;
  for (vector<PhotoPtr>::const_iterator it = userPhotosPtr->photos.begin(); it != userPhotosPtr->photos.end(); ++it) {
    this->photos.push_back(new PhotoInfo((*it)->uploadTime, (*it)->photoId));
  }
}

UserPhotosPtr UserPhotosInfo::parseTo() {
  UserPhotosPtr ptr = new UserPhotos;
  IceUtil::Mutex::Lock lock(*this);
  ptr->userId = this->userId;
  for (vector<PhotoInfoPtr>::const_iterator it = this->photos.begin(); it != this->photos.end(); ++it) {
    ptr->photos.push_back(new Photo((*it)->getUploadTime(), (*it)->getPhotoId()));
  }
  return ptr;
}

void UserPhotosInfo::addPhotoInfo(const PhotoInfoPtr& photoInfoPtr) {
  IceUtil::Mutex::Lock lock(*this);
  bool inserted = false;
  for (vector<PhotoInfoPtr>::iterator it = this->photos.begin(); it != this->photos.end(); ++it) {
    if (photoInfoPtr->getPhotoId() == (*it)->getPhotoId()) {
      return;
    }
    if (photoInfoPtr->getUploadTime() >= (*it)->getUploadTime()) {
      this->photos.insert(it, photoInfoPtr);
      inserted = true;
      break;
    }
  }
  if (!inserted && this->photos.size() < MAX_PHOTOS_SIZE) {
    this->photos.push_back(photoInfoPtr);
  }
  if (this->photos.size() > MAX_PHOTOS_SIZE) {
    this->photos.resize(MAX_PHOTOS_SIZE, photoInfoPtr);
  }
}

void UserPhotosInfo::addPhotoInfo(const vector<PhotoInfoPtr>& photoInfoPtrs) {
  for (vector<PhotoInfoPtr>::const_iterator it = photoInfoPtrs.begin(); it != photoInfoPtrs.end(); ++it) {
    addPhotoInfo(*it);
  }
}

void UserPhotosInfo::setPhotoInfo(const vector<PhotoInfoPtr>& photoInfoPtrs) {
  IceUtil::Mutex::Lock lock(*this);
  this->photos = photoInfoPtrs;
}

//photos是按照uploadTime排序的
void UserPhotosInfo::deletePhotoInfoById(long photoId) {
  IceUtil::Mutex::Lock lock(*this);
  for (vector<PhotoInfoPtr>::iterator it = this->photos.begin(); it != this->photos.end(); ++it) {
    if ((*it)->getPhotoId() == photoId) {
      this->photos.erase(it);
      break;
    }
  }
}

void UserPhotosInfo::deletePhotoInfoById(const vector<long>& photoIds) {
  for (vector<long>::const_iterator it = photoIds.begin(); it != photoIds.end(); ++it) {
    deletePhotoInfoById(*it);
  }
}

vector<PhotoInfoPtr> UserPhotosInfo::getPhotoInfos() const {
  IceUtil::Mutex::Lock lock(*this);
  return this->photos;
}

vector<long> UserPhotosInfo::getPhotoIds() const {
  IceUtil::Mutex::Lock lock(*this);
  vector<long> photoIds;
  for (vector<PhotoInfoPtr>::const_iterator it = this->photos.begin(); it != this->photos.end(); ++it) {
    photoIds.push_back((*it)->getPhotoId());
  }
  return photoIds;
}

vector<int> UserPhotosInfo::getUploadTimes() const {
  IceUtil::Mutex::Lock lock(*this);
  vector<int> uploadTimes;
  for (vector<PhotoInfoPtr>::const_iterator it = this->photos.begin(); it != this->photos.end(); ++it) {
    uploadTimes.push_back((*it)->getUploadTime());
  }
  return uploadTimes;
}

int UserPhotosInfo::getUserId() const {
  IceUtil::Mutex::Lock lock(*this);
  return this->userId;
}

void UserPhotosInfo::setUserId(int pUserId) {
  IceUtil::Mutex::Lock lock(*this);
  this->userId = pUserId;
}

void UserPhotosInfo::clearPhotoInfos() {
  IceUtil::Mutex::Lock lock(*this);
  this->photos.clear();
}

string UserPhotosInfo::toString() const {
  ostringstream result;
  result << "UserId = " << this->userId << " photos[";
  for (vector<PhotoInfoPtr>::const_iterator it = this->photos.begin(); it != this->photos.end(); ++it) {
    if (it != this->photos.begin()) {
      result << ", ";
    }
    result << "(" << (*it)->getUploadTime() << ", " << (*it)->getPhotoId() << ")";
  }
  result << "]";
  return result.str();
}

vector<PhotoWithOwnerIdPtr> UserPhotosInfo::toPhotoWithOwnerIdPtrs() {
  IceUtil::Mutex::Lock lock(*this);
  vector<PhotoWithOwnerIdPtr> result;
  for (vector<PhotoInfoPtr>::const_iterator it = this->photos.begin(); it != this->photos.end(); ++it) {
    result.push_back(new PhotoWithOwnerId(this->userId, (*it)->getUploadTime(), (*it)->getPhotoId()));
  }
  return result;
}

UserFriends::UserFriends(int pUserId, const vector<int>& pFriends) {
  this->userId = pUserId;
  this->friends = pFriends;
}

void UserFriends::setUserId(int pUserId) {
  IceUtil::Mutex::Lock lock(*this);
  this->userId = pUserId;
}

//内部使用swap, pFriends完了以后数据就变了
void UserFriends::setFriends(vector<int>& pFriends) {
  IceUtil::Mutex::Lock lock(*this);
  this->friends.swap(pFriends);
}

int UserFriends::getUserId() const {
  IceUtil::Mutex::Lock lock(*this);
  return this->userId;
}

vector<int> UserFriends::getFriends() const {
  IceUtil::Mutex::Lock lock(*this);
  return this->friends;
}

MyUtil::ObjectResultPtr UserPhotosInfo::parseFromUserPhotosResultPtr(const MyUtil::ObjectResultPtr& ptr) {
  MyUtil::ObjectResultPtr resultPtr = new MyUtil::ObjectResult;
  for (map<long, Ice::ObjectPtr>::const_iterator it = ptr->data.begin(); it != ptr->data.end(); ++it) {
    resultPtr->data.insert(make_pair(it->first, new UserPhotosInfo(UserPhotosPtr::dynamicCast(it->second)))); 
  }
  return resultPtr;
}

MyUtil::ObjectResultPtr UserPhotosInfo::parseFromUserPhotosSeq(const UserPhotosSeq& userPhotos) {
  MyUtil::ObjectResultPtr resultPtr = new MyUtil::ObjectResult;
  for (vector<UserPhotosPtr>::const_iterator it = userPhotos.begin(); it != userPhotos.end(); ++it) {
    resultPtr->data.insert(make_pair((*it)->userId, new UserPhotosInfo(*it)));
  }
  return resultPtr;
}

MyUtil::ObjectResultPtr UserPhotosInfo::changeToUserPhotosInfoMap(const vector<UserPhotosInfoPtr>& ptrs) {
  MyUtil::ObjectResultPtr resultPtr = new MyUtil::ObjectResult;
  for (vector<UserPhotosInfoPtr>::const_iterator it = ptrs.begin(); it != ptrs.end(); ++it) {
    resultPtr->data.insert(make_pair((*it)->getUserId(), (*it)->parseTo()));
  }
  return resultPtr;
}

vector<int> UserPhotosInfo::getUserIdsFromUserPhotosSeq(const UserPhotosSeq& userPhotosSeq) {
  vector<int> userIds;
  for (vector<UserPhotosPtr>::const_iterator it = userPhotosSeq.begin(); it != userPhotosSeq.end(); ++it) {
    userIds.push_back((*it)->userId);
  }
  return userIds;
}

PhotoResultPtr xce::friendsphotos::getPhotoResultFromUsers(ServiceI& service, const vector<int>& friends, int begin, int limit) {
  PhotoResultPtr ptr = new PhotoResult;
  vector<PhotoWithOwnerIdPtr> photoWithOwnerIdPtrs;
  for (MyUtil::IntSeq::const_iterator it = friends.begin(); it != friends.end(); ++it) {
    UserPhotosInfoPtr ptr = service.locateObject<UserPhotosInfoPtr>(USER_PHOTOS_INFO, *it);
    if (ptr != 0 || ptr != EMPTY_USER_PHOTOS_INFO_PTR) {
      vector<PhotoWithOwnerIdPtr> tempPtrs = ptr->toPhotoWithOwnerIdPtrs();
      photoWithOwnerIdPtrs.insert(photoWithOwnerIdPtrs.end(), tempPtrs.begin(), tempPtrs.end());
    }
  }
  ptr->total = photoWithOwnerIdPtrs.size();
  if (begin >= ptr->total) {
    return ptr;
  }
  int end = min(ptr->total, begin + limit);
  //从一个数组里获取从第n大的元素到第m大的元素
  vector<PhotoWithOwnerIdPtr> sortedPtrs = PhotoWithOwnerId::getBetweenBeginEnd(photoWithOwnerIdPtrs, begin, end);
  ptr->userPhotos = PhotoWithOwnerId::formUserPhotosSeq(sortedPtrs);
  return ptr;
}

string xce::friendsphotos::serialize(const vector<int>& uploadTimes, const vector<long>& photoIds) {
  vector<int> uploadTimesNew(uploadTimes);
  vector<long> photoIdsNew(photoIds);
  uploadTimesNew.resize(MAX_PHOTOS_SIZE, -1);
  photoIdsNew.resize(MAX_PHOTOS_SIZE, -1);
  return IntegerSeqSerialize<int>::serialize(uploadTimesNew) + IntegerSeqSerialize<long>::serialize(photoIdsNew);
}

template<typename T>
vector<T> xce::friendsphotos::findElementsInANotB(const vector<T>& setA, const vector<T>& setB) {
  vector<T> result;
  vector<T> sortedB(setB);
  sort(sortedB.begin(), sortedB.end());
  for (int i = 0, size = setA.size(); i < size; ++i) {
    if (!binary_search(sortedB.begin(), sortedB.end(), setA[i])) {
      result.push_back(setA[i]);
    }
  }
  return result;
}
vector<int> UserPhotosInfo::getUserIdsFromUserPhotosInfoSeq(const vector<UserPhotosInfoPtr>& ptrs) {
  vector<int> result;
  for (vector<UserPhotosInfoPtr>::const_iterator it = ptrs.begin(); it != ptrs.end(); ++it) {
    result.push_back((*it)->getUserId());
  }
  return result;
}

UserPhotosInfoResultHandlerI::UserPhotosInfoResultHandlerI(vector<UserPhotosInfoPtr>& pUserPhotosInfoPtrs) : userPhotosInfoPtrs(pUserPhotosInfoPtrs) {
}

bool UserPhotosInfoResultHandlerI::handle(mysqlpp::Row& row) const {
  int userId = (int) row[COLUMN_USERID.c_str()];
  mysqlpp::Row::value_type photosRaw = row[COLUMN_PHOTOS.c_str()];
  string photos(photosRaw.data(), photosRaw.length());
  vector<int> uploadTimes;
  vector<long> photoIds;
  if (photos.length() == MAX_PHOTOS_SIZE * (sizeof(long) + sizeof(int))) {
    uploadTimes = MyUtil::IntegerSeqSerialize<int>::unserialize(photos.substr(0, photos.length() / 3));
    photoIds = MyUtil::IntegerSeqSerialize<long>::unserialize(photos.substr(photos.length() / 3, photos.length() * 2 / 3));
  }
  vector<PhotoInfoPtr> photoInfoPtrs;
  for (size_t i = 0; i < uploadTimes.size(); ++i) {
    if (uploadTimes[i] == -1) {
      break;
    }
    photoInfoPtrs.push_back(new PhotoInfo(uploadTimes[i], photoIds[i]));
  }
  UserPhotosInfoPtr ptr = new UserPhotosInfo(userId);
  ptr->setPhotoInfo(photoInfoPtrs);
  userPhotosInfoPtrs.push_back(ptr);
  return true;
}

template<typename T>
BatchIdResultHandler<T>::BatchIdResultHandler(const string& idColumnName, vector<T>& ids) : _idColumnName(idColumnName), _ids(ids) {
}

template<typename T>
bool BatchIdResultHandler<T>::handle(mysqlpp::Row& row) const {
  T id = (T)row[_idColumnName.c_str()];
  _ids.push_back(id);
  return true;
}

