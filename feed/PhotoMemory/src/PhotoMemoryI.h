/*
 * PhotoMemoryI.h
 *
 *  Created on: 2010-11-17
 *      Author: wu
 */

#ifndef PHOTOMEMORYI_H_
#define PHOTOMEMORYI_H_


#include "Singleton.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include "PhotoMemory.h"

#include <Ice/Ice.h>
#include<string>
#include<vector>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>


using namespace std;
using namespace boost::multi_index;

namespace xce{
namespace feed{

const int DBCluster = 100;
const string PhotoDBInstance = "photo_view_count";
const string PhotoMemDBInstance = "feed_photo_memory";
//const string PhotoMemDBInstance = "feed_exp";//For exp

const int POOL_SIZE = 40000;
const int RESIZE_THRESHOLD = 100;
const int USER_PHOTO_MAXSIZE = 1000;
const unsigned int MAX_USER_COUNT = 250;
const int UPDATE_TIME_INTERNAL = 60*60;

class UserPhoto{
public:
  static const int PHOTO_COUNT = 100;
  UserPhoto(int userid):
    userid_(userid) {
  }

  void Put(const ContentItemSeq& seq) const;

  ContentItemSeq Get(int friendid,int begin, int limit) const;

  ContentItemSeq GetRandomSeq(int friendid,int begin, int limit) const;

  void Remove(Ice::Long photoid) const;

  void RemoveFrinendPhoto(int friendid) const;

  int UserId() const {
    return userid_;
  }

public:
  mutable int userid_;
  mutable ContentItemSeq photos;
  mutable time_t last_access_time_;
};


//typedef UserPhoto* UserPhotoPtr;

typedef boost::multi_index_container<
  UserPhoto,
  indexed_by<
    sequenced<> ,
    hashed_unique<
    member<UserPhoto, int, &UserPhoto::userid_>
    >
  >
> UserPhotoContainer;

typedef UserPhotoContainer::nth_index<1>::type UidIndex;
typedef UserPhotoContainer::nth_index<0>::type SeqIndex;

class UserPhotoPool {
public:
  ContentItemSeq Get(int userid,int friendid,int begin, int limit);
  void Put(int userid, const ContentItemSeq& seq);
  void Remove(int userid, Ice::Long photoid);
  void RemoveFrinendPhoto(int userid, int friendid);



private:
  UserPhotoContainer users_;

  IceUtil::Mutex mutex_;
};


class PhotoMemoryManagerI : public PhotoMemoryManager,
		public MyUtil::Singleton<PhotoMemoryManagerI>{
public:
  virtual void SetRead(int userid,Ice::Long  photoid,const Ice::Current& = Ice::Current());
  virtual PhotoContentSeq GetPhotoContentSeq(int userid,int begin, int end, const Ice::Current& = Ice::Current());
  virtual PhotoContentSeq GetFriendPhotoContentSeq(int userid, int friendid, int begin, int end, const Ice::Current& = Ice::Current());
  virtual void RemoveFriendPhotos(int userid, int friendid, const Ice::Current& =Ice::Current());
  virtual void RemoveDispatchedPhotos(int userid, long photoid, const Ice::Current& =Ice::Current());


protected:
  UserPhotoPool    _pool[POOL_SIZE];
  PhotoContentSeq FetchPhotoContentSeq(int userid, int friendid, int begin, int end);

  bool LoadPhotoContentFromDB(Ice::Long photoid,int userid,PhotoContentPtr content);
  void SavePhotoSeqToDB(int userid, const ContentItemSeq& contentSeq);
  bool GetBasicInfoFromDB(Ice::Long photoid,int userid,PhotoContentPtr content);
  bool GetTagUsersFromDB(Ice::Long photoid,int userid,PhotoContentPtr content);
  bool LoadPhotoSeqFromDB(int userid,ContentItemSeq& seq);

  string GetAlbumFromDB(int albumid, int ownerid);

  ContentItemSeq GetPhotoSeq(int uid);

  string GetPhotoTableName(Ice::Long id) const {
          ostringstream os;
          os << "photo_" << id % DBCluster ;
            return os.str();
  }

  string GetPhotoTagTableName(Ice::Long id) const {
            ostringstream os;
            os << "photo_tag";
              return os.str();
   }

  string GetPhotoSeqTableName(int id) const {
              ostringstream os;
              os << "feed_photo_list_" << id % DBCluster ; //how to hash the table?
                return os.str();
      }

  string GetPhotoAlbumTableName(int id) const {
        ostringstream os;
        os << "album_" << id % DBCluster;
        return os.str();
      }


};



}

}

#endif /* PhotoMemoyManagerI_H_ */
