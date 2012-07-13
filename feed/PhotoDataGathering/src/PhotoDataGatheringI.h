/*
 * PhotoDataGatheringCache.h
 *
 *  Created on: 2010-11-22
 *      Author: wu
 */

#ifndef PhotoDataGatheringI_H_
#define PhotoDataGatheringI_H_

#include <string>
#include <map>
#include <vector>
#include <Ice/Ice.h>
#include "PhotoMemory.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "QueryRunner.h"

#include "PerformanceStat.h"

namespace xce{
namespace feed{
  using namespace std;
  using namespace MyUtil;

  const int DBCluster = 100;
  const string PhotoDBInstance = "photo_view_count";
  //const string FeedDBInstance = "feed_exp";//For exp
  const string PhotoMemDBInstance = "feed_photo_memory";
  //const string PhotoMemDBInstance = "feed_exp";

  const unsigned int MAX_RANKINFO_DICT_SIZE = 100000;
  const unsigned int MAX_PHOTOSEQ_DICT_SIZE = 100000;
  const unsigned int NUMBERS_PER_TRANSFER = 10;
  const unsigned int MAX_PHOTOSEQ_LENGTH = 25;

  const unsigned int FLUSH_THREAD_NUM = 3;

//  const int PhotoSeqTimer_Time = 30*60*1000;//0.5 hour
  const int PhotoContentTimer_Time = 71*60*1000;//1 hour
  const int PhotoRankInfoTimer_Time = 121*60*1000;//2 hour


    const int PhotoSeqTimer_Time = 0.25*60*1000 ;//test
//
//    const int PhotoContentTimer_Time = 3*60*1000;//test
//
//    const int PhotoRankInfoTimer_Time = 4*60*1000;//test



  typedef enum {
  	  CLICKCOUNT,
  	  COMMENTCOUNT,
  	  TAGCOUNT
    }RankInfoType;


  typedef vector<PhotoItem> PhotoList;
  typedef map<int, PhotoList> PhotoListDict;

class PhotoDataGatheringManagerI :  public PhotoDataGatheringManager, public Singleton<PhotoDataGatheringManagerI>{
  public:

  virtual void GatherPhotoList(int ownerid, const ::MyUtil::LongSeq&,
      const ::MyUtil::Int2IntMap&, const ::Ice::Current& = ::Ice::Current());

  virtual void GatherUpLoadPhotos(int ownerid,const ::MyUtil::LongSeq&,
      const ::Ice::Current& = ::Ice::Current());

  virtual void IncCommentCount(Ice::Long PhotoId, const Ice::Current& =
      Ice::Current());

  virtual void IncTagCount(Ice::Long PhotoId, int tagid,
      const Ice::Current& = Ice::Current());

  virtual void SetRead(int uid, Ice::Long PhotoId, const Ice::Current& =
      Ice::Current());

  virtual void Test(const Ice::Current& = Ice::Current());

  void FlushPhotoContent();
  void FlushPhotoSeq();
  void FlushPhotoRankInfo();

  void PartPhotoSeqFlush(map<int, PhotoList> &tmpPhotoListDict);

protected:

  void PhotoListToContentItemSeq(PhotoList &list, ContentItemSeq &seq);

  void SavePhotoSeqToDB(int userid, const ContentItemSeq& contentSeq);

  void IncClickCount(Ice::Long PhotoId);

  void ReSortPhotoListDict(PhotoRankInfoDict& rankDict, PhotoListDict& origin);
  void SortPhotoList(PhotoRankInfoDict& rankDict, PhotoList& list);

  void LoadPhotoContentFromDB(Ice::Long photoid, PhotoContentPtr content);
  bool GetBasicInfoFromDB(Ice::Long photoid, int ownerid,PhotoContentPtr content);
  bool GetTagUsersFromDB(Ice::Long photoid, PhotoContentPtr content);
  string  GetAlbumFromDB(int albumid, int ownerid);


  void DeletePhotoFromDB(int uid, Ice::Long photoId);

  void IncRankInfo(Ice::Long photoid, RankInfoType type);
  void GetRankInfo(PhotoRankInfoDict& rankDict, PhotoListDict& origin);


  int CalculateWeight(PhotoRankInfoDict& rankDict, PhotoItem& item);

  ContentItemSeq GetPhotoSeq(int uid);

  void LoadContentsFromDB(vector<ContentItem>& contentList, PhotoContentSeq &seq);
  bool LoadPhotoContentFromDB(Ice::Long photoid, int userid,
      PhotoContentPtr content);
  bool LoadPhotoSeqFromDB(int userid,ContentItemSeq& seq);

  void ContentItemSeqToPhotoList(ContentItemSeq &seq, PhotoList &list);
  void AddItemToPhotoList(PhotoList &list,PhotoItem item);

  void GetMissedPhotoList(int ownerid,const MyUtil::Int2IntMap& friendRankMap);

  string GetPhotoTableName(int id) const {
    ostringstream os;
    os << "photo_" << id % DBCluster ;
    //os << "photo_1";
    //os << "album_1";
    return os.str();
  }

  string GetPhotoTagTableName(int id) const {
    ostringstream os;
    //os << "photo_tag" << id % DBCluster;
    os << "photo_tag";
    return os.str();
  }

  string GetPhotoAlbumTableName(int id) const {
      ostringstream os;
      os << "album_" << id % DBCluster;
      //os << "photo_tag";
      return os.str();
    }

  string GetPhotoSeqTableName(int id) const {
    ostringstream os;
    os << "feed_photo_list_" << id % DBCluster ; //how to hash the table?
    //os << "photo_list";
    return os.str();
  }


protected:
    PhotoRankInfoDict rankInfoDict;

    //PhotoSeqDict photoSeqDict;
    map<int, PhotoList> photoListDict;

    //PhotoContentDict  photoContentDict;
    //MyUtil::LongSeq photoContentIds;

    vector<ContentItem> photoContentList;

    IceUtil::Mutex rank_mutex_;
    IceUtil::Mutex content_mutex_;
    IceUtil::Mutex photolist_mutex_;

  };


class FlushPhotoSeqTimer: public MyUtil::Timer {
public:
  FlushPhotoSeqTimer() :Timer(PhotoSeqTimer_Time) {}

  virtual void handle()
  {
    MCE_INFO("PhotoDataGathering FlushPhotoSeqTimer line: "<<__LINE__);
    TimeStat ts;
    PhotoDataGatheringManagerI::instance().FlushPhotoSeq();

    MCE_INFO("PhotoDataGathering FlushPhotoSeqTimer use time: "<<ts.getTime()<< " line: "<<__LINE__);
  }
};


class FlushPhotoContentTimer: public MyUtil::Timer {
public:
  FlushPhotoContentTimer() :Timer(PhotoContentTimer_Time) {}

  virtual void handle()
  {
    MCE_INFO("PhotoDataGathering FlushPhotoContentTimer line: "<<__LINE__);
    TimeStat ts;
    PhotoDataGatheringManagerI::instance().FlushPhotoContent();
    MCE_INFO("PhotoDataGathering FlushPhotoContentTimer use time: "<<ts.getTime()<< " line: "<<__LINE__);
  }
};


class FlushPhotoRankInfoTimer: public MyUtil::Timer {
public:
  FlushPhotoRankInfoTimer() :Timer(PhotoRankInfoTimer_Time) {}

  virtual void handle()
  {
    MCE_INFO("PhotoDataGathering FlushPhotoRankInfoTimer line: "<<__LINE__);
    TimeStat ts;
    PhotoDataGatheringManagerI::instance().FlushPhotoRankInfo();
    MCE_INFO("PhotoDataGathering FlushPhotoRankInfoTimer use time: "<<ts.getTime()<< " line: "<<__LINE__);
  }
};

class PartPhotoSeqFlushTimer: public MyUtil::Timer {
public:
  map<int, PhotoList> partPhotoListDict_;

  PartPhotoSeqFlushTimer() :Timer(1) {}

  virtual void handle()
    {
      MCE_INFO("PhotoDataGathering PartPhotoSeqFlushTimer line: "<<__LINE__);
      PhotoDataGatheringManagerI::instance().PartPhotoSeqFlush(partPhotoListDict_);
//      MCE_INFO("PhotoDataGathering FlushPhotoRankInfoTimer use time: "<<ts.getTime()<< " line: "<<__LINE__);
    }

};



};
};

#endif /* PhotoDataGatheringCACHE_H_ */
