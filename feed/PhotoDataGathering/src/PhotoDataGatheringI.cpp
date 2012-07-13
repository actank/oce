/*
 * PhotoDataGathering.cpp
 *
 *  Created on: 2010-11-26
 *      Author: wu
 */


#include "PhotoMemory.h"
#include "PhotoDataGatheringI.h"
#include "../../FeedMemcProxy/client/photo_memc_client.h"
#include "ServiceI.h"
#include <stdlib.h>
#include <string.h>
#include "TaskManager.h"
#include <set>
#include "PerformanceStat.h"
#include "Allocator.h"

using namespace com::xiaonei::platform::storage;
using namespace xce::feed;
using namespace com::xiaonei::xce;
using namespace std;


void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&PhotoDataGatheringManagerI::instance(), service.createIdentity("M", ""));
  TaskManager::instance().scheduleRepeated(new FlushPhotoSeqTimer());
  TaskManager::instance().scheduleRepeated(new FlushPhotoContentTimer());
}



namespace xce {

namespace feed {


void PhotoDataGatheringManagerI::GatherUpLoadPhotos(int ownerid,const MyUtil::LongSeq& photoIds,const Ice::Current&)
{
  IceUtil::Mutex::Lock lock(content_mutex_);
  MCE_INFO("PhotoDataGatheringManagerI::GatherUpLoadPhotos ---> ownerid: "<<ownerid<<" photoContentList size "<<photoContentList.size()<<" line: "<<__LINE__);
  for (unsigned int i = 0; i < photoIds.size(); i++) {
    ContentItem item;
    item.photoid = photoIds[i];
    item.ownerid = ownerid;
    vector<ContentItem>::iterator it = photoContentList.begin();
    photoContentList.insert(it,item);//因为是新照片，所以加在头部
  }
}

void PhotoDataGatheringManagerI::ContentItemSeqToPhotoList(ContentItemSeq &seq, PhotoList &list)
{
  for (unsigned int i = 0; i < seq.size(); i++) {
    PhotoItem item;
    item.photoid = seq[i].photoid;
    item.friendfrank = 0;
    item.ownerid = seq[i].ownerid;
    list.push_back(item);
  }
}

void PhotoDataGatheringManagerI::AddItemToPhotoList(PhotoList &list,PhotoItem item)
{
  for(unsigned int i=0;i<list.size();i++)
  {
      if(list[i].photoid == item.photoid)
      {
        return;
      }
  }
  PhotoList::iterator it = list.begin();
  list.insert(it,item);
}


void PhotoDataGatheringManagerI::GetMissedPhotoList(int ownerid,const MyUtil::Int2IntMap& friendRankMap)
{
  vector<int> missedUidSeq;
  map<int, PhotoList> missedPhotoMap;

  {
    IceUtil::Mutex::Lock lock(photolist_mutex_);
    MyUtil::Int2IntMap::const_iterator it = friendRankMap.begin();
    for (; it != friendRankMap.end(); it++) {
      int uid = it->first;

      if(uid==ownerid)
      {
        continue;// Don't send photos to owner itself
      }

      PhotoListDict::iterator pit = photoListDict.find(uid);
      if (pit == photoListDict.end()) {
        missedUidSeq.push_back(uid);
      }
    }
  }

  for (unsigned int i = 0; i < missedUidSeq.size(); i++) {
    PhotoList list;
    ContentItemSeq seq;
    int uid = missedUidSeq[i];
    seq = GetPhotoSeq(uid);
    if (!seq.empty()) {
      ContentItemSeqToPhotoList(seq, list);
    }
    missedPhotoMap.insert(make_pair(uid, list));
  }

  {
    IceUtil::Mutex::Lock lock(photolist_mutex_);

    map<int, PhotoList>::iterator it = missedPhotoMap.begin();
    for (; it != missedPhotoMap.end(); it++) {
      photoListDict.insert(make_pair(it->first, it->second));
    }

  }
}

void PhotoDataGatheringManagerI::GatherPhotoList(int ownerid,const MyUtil::LongSeq& photoIds,const MyUtil::Int2IntMap& friendRankMap,const Ice::Current&)
{
  MCE_INFO("Enter PhotoDataGatheringManagerI::GatherPhotoList --->   "<<" ownerid: "<<ownerid<<"friends number: "<<friendRankMap.size()<<" line: "<<__LINE__);

  GetMissedPhotoList(ownerid, friendRankMap);

  IceUtil::Mutex::Lock lock(photolist_mutex_);

  for (unsigned int i = 0; i < photoIds.size(); i++) {
    long photoid = photoIds[i];
    MyUtil::Int2IntMap::const_iterator it = friendRankMap.begin();

    for (; it != friendRankMap.end(); it++) {
      int uid = it->first;

      if (uid == ownerid) {
        continue;// Don't send photos to owner itself
      }

      PhotoItem item;
      item.photoid = photoid;

      item.friendfrank = it->second;
      item.ownerid = ownerid;
      PhotoListDict::iterator pit = photoListDict.find(uid);

      if (pit == photoListDict.end()) {
        MCE_INFO("PhotoDataGatheringManagerI::GatherPhotoList ---> can not find uid in dict.   uid: "<<uid<<" line: "<<__LINE__);
        continue;

      } else {
        AddItemToPhotoList(pit->second, item);
      }

    }

  }
  MCE_INFO("Exit PhotoDataGatheringManagerI::GatherPhotoList --->   "<<" ownerid: "<<ownerid<<" line: "<<__LINE__);
}

void PhotoDataGatheringManagerI::IncRankInfo(Ice::Long photoId,RankInfoType type)
{
  IceUtil::Mutex::Lock lock(rank_mutex_);
  map<Ice::Long, PhotoRankInfoPtr>::iterator it = rankInfoDict.find(photoId);
  if (it == rankInfoDict.end()) {
    //Get from Memcache
    PhotoRankInfoPtr photoRank = PhotoMemcClient::instance().GetPhotoRankInfo(
        photoId);
    if (!photoRank) {
      photoRank = new PhotoRankInfo();
      photoRank->photoid = photoId;
      photoRank->clickcount = 0;
      photoRank->commentcount = 0;
      photoRank->tagcount = 0;

      if (!photoRank) {
        return;
      }
    }
    photoRank->photoid = photoId;//??? need to add this? for a bug
    rankInfoDict.insert(make_pair(photoId, photoRank));//最开始圆括号写成尖括号了,编译错很诡异，脑残
    it = rankInfoDict.find(photoId);
  }

  if (type == CLICKCOUNT) {
    it->second->clickcount++;
  } else if (type == COMMENTCOUNT) {
    it->second->commentcount++;
  } else if (type == TAGCOUNT) {
    it->second->tagcount++;
  }
}

void PhotoDataGatheringManagerI::IncClickCount( Ice::Long photoId)
{
  MCE_INFO("Enter PhotoDataGatheringManagerI::IncClickCount ---> photoId "<<photoId<<" line: "<<__LINE__);
//	IncRankInfo(photoId,CLICKCOUNT);
}

void PhotoDataGatheringManagerI::IncCommentCount(Ice::Long photoId,const Ice::Current&)
{
  MCE_INFO("Enter PhotoDataGatheringManagerI::IncCommentCount ---> photoId "<<photoId<<" line: "<<__LINE__);
//	IncRankInfo(photoId,COMMENTCOUNT);
}

void PhotoDataGatheringManagerI::IncTagCount(Ice::Long photoId, int tagid, const Ice::Current&) {
  MCE_INFO("Enter PhotoDataGatheringManagerI::IncTagCount ---> photoId "<<photoId<<" line: "<<__LINE__);
//	IncRankInfo(photoId,TAGCOUNT);

	PhotoContentPtr content = PhotoMemcClient::instance().GetPhotoContent(photoId);
	if(content)
	{
	  content->tagusers.push_back(tagid);
	  PhotoMemcClient::instance().SetPhotoContent(content);
	}
}

void PhotoDataGatheringManagerI::PhotoListToContentItemSeq(PhotoList &list, ContentItemSeq &seq)
{
   for(int i =0;i<list.size();i++)
   {
     ContentItem item;
     item.ownerid = list[i].ownerid;
     item.photoid = list[i].photoid;
     seq.push_back(item);
   }
}

void PhotoDataGatheringManagerI::SetRead(int uid,Ice::Long photoId,const Ice::Current&)
{
  {
    IceUtil::Mutex::Lock lock(photolist_mutex_);
    PhotoListDict::iterator pit = photoListDict.find(uid);

    if (pit != photoListDict.end()) {
      for (unsigned int i = 0; i < pit->second.size(); i++) {
        if (pit->second.at(i).photoid == photoId) {
          pit->second.erase(pit->second.begin() + i);
//          IncClickCount(photoId);
          ContentItemSeq seq;
          PhotoListToContentItemSeq(pit->second,seq);
          SavePhotoSeqToDB(uid,seq);
          pit->second.clear();
          PhotoMemcClient::instance().SetPhotoSeq(uid,seq);
          MCE_INFO("PhotoDataGatheringManagerI::SetRead --->deleted  uid: "<<uid<<" photoId "<<photoId<<" line: "<<__LINE__);
          return;
        }
      }
    }
  }
  //Load PhotoList
  ContentItemSeq seq = GetPhotoSeq(uid);
  PhotoList list;
  bool find_flag = false;
  for (unsigned int i = 0; i < seq.size(); i++) {
    PhotoItem item;
    if (seq[i].photoid == photoId) {
      MCE_INFO("PhotoDataGatheringManagerI::SetRead --->deleted  uid: "<<uid<<" photoId "<<photoId<<" line: "<<__LINE__);
      find_flag = true;
      seq.erase(seq.begin()+i);
      continue;
    }

  }

  if(find_flag)
  {
//    IncClickCount(photoId);
    {
        IceUtil::Mutex::Lock lock(photolist_mutex_);
//        photoListDict.insert(make_pair(uid, list));
        SavePhotoSeqToDB(uid,seq);
        PhotoMemcClient::instance().SetPhotoSeq(uid,seq);
    }
//    MCE_INFO("PhotoDataGatheringManagerI::SetRead ---> found in db or memc! deleted  uid  : "<<uid<<" photoId "<<photoId<<" line: "<<__LINE__);
  }

}

ContentItemSeq PhotoDataGatheringManagerI::GetPhotoSeq(int uid)
{
  ContentItemSeq seq;
  seq = PhotoMemcClient::instance().GetPhotoSeq(uid);
  if(seq.empty())
  {
    LoadPhotoSeqFromDB(uid,seq);
  }
  return seq;

}



int PhotoDataGatheringManagerI::CalculateWeight(PhotoRankInfoDict& rankDict,PhotoItem& item)
{

   long photoid = item.photoid;
   PhotoRankInfoDict::iterator it = rankDict.find(photoid);
   int res = 0;
   if(it!=rankDict.end())
   {
      if(it->second)
      {
         res = it->second->clickcount + it->second->commentcount*20 + it->second->tagcount*50;
      }

   }

   res +=  item.friendfrank;
   return res;
}

void PhotoDataGatheringManagerI::GetRankInfo(PhotoRankInfoDict& rankDict, PhotoListDict& origin)
{
  //MCE_INFO("Enter PhotoDataGatheringManagerI::CompleteRankInfo --->  line: "<<__LINE__);
  map<int, PhotoList>::iterator it = origin.begin();
  MyUtil::LongSeq missedPhotoSeq;

  set<long> photoSet;

  for (; it != origin.end(); it++) {
    for (unsigned int i = 0; i < it->second.size(); i++) {
      long id = it->second.at(i).photoid;
      photoSet.insert(id);
      }
    }

  MyUtil::LongSeq seq;
  PhotoRankInfoDict dict;

  for(set<long>::iterator it = photoSet.begin(); it != photoSet.end(); it++) {
    PhotoRankInfoPtr rank = PhotoMemcClient::instance().GetPhotoRankInfo(*it);
    if(rank)
    {
      rankDict.insert(make_pair(*it, rank));
    }

  }

}


void PhotoDataGatheringManagerI::SortPhotoList(PhotoRankInfoDict& rankDict, PhotoList& list)
{
  //MCE_INFO("Enter PhotoDataGatheringManagerI::SortPhotoList --->  line: "<<__LINE__);
  unsigned int max;
  PhotoItem temp;

  for (unsigned int i = 0; i < list.size()&&i <20; i++) {
    max = i;
    for (unsigned int j = i + 1; j < list.size()&&j<20; j++)
    {
      int w_j = CalculateWeight(rankDict,list[j]);
      int w_max = CalculateWeight(rankDict,list[max]);

      if (w_j > w_max)
        max = j;//对i之后的数进行扫描将最大的数赋予max
    }

    if (max != i) {
      temp = list[i];
      list[i] = list[max];
      list[max] = temp;
    }//判断max与i是否相等，若＝则说明原假设正确反之交换数值

  }
}

void PhotoDataGatheringManagerI::ReSortPhotoListDict(PhotoRankInfoDict& rankDict, PhotoListDict& origin)
{
  TimeStat ts;
  map<int, PhotoList>::iterator it = origin.begin();
  for (; it != origin.end(); it++) {
    SortPhotoList(rankDict, it->second);
  }
  MCE_INFO("PhotoDataGathering ReSortPhotoListDict use time: "<<ts.getTime()<< " line: "<<__LINE__);
}


void PhotoDataGatheringManagerI::FlushPhotoRankInfo()
{
  PhotoRankInfoDict tempRankDict;
  {
    IceUtil::Mutex::Lock lock(rank_mutex_);
    int size = rankInfoDict.size();
    MCE_INFO("PhotoDataGatheringManagerI::FlushPhotoRank ---> rankInfoDict size: "<<size<<" line: "<<__LINE__);

    tempRankDict.swap(rankInfoDict);

      rankInfoDict.clear();
      PhotoRankInfoDict emptyDict;
      rankInfoDict.swap(emptyDict);
  }

  unsigned int times = 0;
  PhotoRankInfoSeq seq;

  for (PhotoRankInfoDict::iterator it = tempRankDict.begin(); it
      != tempRankDict.end(); it++) {

    if (times < NUMBERS_PER_TRANSFER) {

      seq.push_back(it->second);
      times++;
    } else {
      PhotoMemcClient::instance().SetPhotoRankInfoSeq(seq);
      seq.clear();
      times = 0;
    }

  }//end for

  if(times!=0)
  {
      PhotoMemcClient::instance().SetPhotoRankInfoSeq(seq);
  }
  MCE_INFO("Exit PhotoDataGatheringManagerI::FlushPhotoRank --->  line: "<<__LINE__);
}

void PhotoDataGatheringManagerI::PartPhotoSeqFlush(map<int, PhotoList> &tmpPhotoListDict)
{

  PhotoRankInfoDict rankDict;
  {
    unsigned int times = 0;
    UserPhotoSeqList tmpSeqList;
    for (map<int, PhotoList>::iterator it = tmpPhotoListDict.begin(); it
        != tmpPhotoListDict.end(); it++) {
      ContentItemSeq seq;
      unsigned int size = it->second.size();
      unsigned int end = size;
      if (size > MAX_PHOTOSEQ_LENGTH) {
        end = MAX_PHOTOSEQ_LENGTH;
      }
      for (unsigned int i = 0; i < end; i++) {
        ContentItem item;
        item.photoid = it->second.at(i).photoid;
        item.ownerid = it->second.at(i).ownerid;
        seq.push_back(item);
      }
      //MCE_INFO("PhotoDataGatheringManagerI::FlushPhotoSeq seq size: "<<seq.size()<<"  line: "<<__LINE__);
      SavePhotoSeqToDB(it->first, seq);
      UserPhotoSeqPtr userPhotoSeq = new UserPhotoSeq();
      userPhotoSeq->userid = it->first;
      userPhotoSeq->itemList.swap(seq);

      if (times < NUMBERS_PER_TRANSFER) {
        tmpSeqList.push_back(userPhotoSeq);
        times++;
      } else {
        tmpSeqList.push_back(userPhotoSeq);
        PhotoMemcClient::instance().SetPhotoSeqList(tmpSeqList);
        tmpSeqList.clear();
        times = 0;
      }

    }//end for
    if (times != 0) {
      PhotoMemcClient::instance().SetPhotoSeqList(tmpSeqList);
    }
  }

}



void PhotoDataGatheringManagerI::FlushPhotoSeq()
{
  PartPhotoSeqFlushTimer *timerArray[FLUSH_THREAD_NUM];
  for (int i = 0; i < FLUSH_THREAD_NUM; i++) {
    timerArray[i] = new PartPhotoSeqFlushTimer();
  }

  {
    IceUtil::Mutex::Lock lock(photolist_mutex_);

    int size = photoListDict.size();
    MCE_INFO("PhotoDataGatheringManagerI::FlushPhotoSeq ---> photoListDict_size: "<<size<<" line: "<<__LINE__);

    int step_len = photoListDict.size() / FLUSH_THREAD_NUM;
    int times = 0;
    int count = 0;

    for (map<int, PhotoList>::iterator it = photoListDict.begin(); it
        != photoListDict.end(); it++) {
      timerArray[times]->partPhotoListDict_.insert(make_pair(it->first,
          it->second));
      count++;
      if (count == step_len&&times<FLUSH_THREAD_NUM-1) {
        count = 0;
        times += 1;
      }
    }

    photoListDict.clear();
    map<int, PhotoList> emptyList;
    photoListDict.swap(emptyList);

  }
  for (int i = 0; i < FLUSH_THREAD_NUM; i++) {
    TaskManager::instance().schedule(timerArray[i]);
  }

  MCE_INFO("Exit PhotoDataGatheringManagerI::FlushPhotoSeq --->  line: "<<__LINE__);
}

void PhotoDataGatheringManagerI::FlushPhotoContent()
{
//  MCE_INFO("Enter PhotoDataGatheringManagerI::FlushPhotoContent --->  line: "<<__LINE__);
  vector<ContentItem> tmpPhotoContentList;
  PhotoContentSeq seq;

  {
    IceUtil::Mutex::Lock lock(content_mutex_);
    MCE_INFO("PhotoDataGatheringManagerI::FlushPhotoContent ---> photoContentList_size: "<<photoContentList.size()<<" line: "<<__LINE__);
    tmpPhotoContentList.swap(photoContentList);
    //photoContentList.clear();
    vector<ContentItem> emptyList;
    photoContentList.clear();
    photoContentList.swap(emptyList);
  }

  LoadContentsFromDB(tmpPhotoContentList, seq);

  unsigned int times = 0;
  PhotoContentSeq tmpSeq;

  for(unsigned int i=0;i<seq.size();i++){

    if (times < NUMBERS_PER_TRANSFER) {
      tmpSeq.push_back(seq[i]);
      times++;
    } else {
      tmpSeq.push_back(seq[i]);
      PhotoMemcClient::instance().SetPhotoContentSeq(tmpSeq);
      tmpSeq.clear();
      times = 0;
    }

  }

  if(times != 0)
  {
    PhotoMemcClient::instance().SetPhotoContentSeq(tmpSeq);
  }
  MCE_INFO("Exit PhotoDataGatheringManagerI::FlushPhotoContent --->  line: "<<__LINE__);
}

void PhotoDataGatheringManagerI::LoadContentsFromDB(vector<ContentItem>& contentList,PhotoContentSeq &seq)
{
  for(unsigned int i=0;i<contentList.size();i++)
  {
    PhotoContentPtr content  = new PhotoContent();
    if(LoadPhotoContentFromDB(contentList[i].photoid,contentList[i].ownerid,content))
    {
      seq.push_back(content);
    }
  }

}

bool PhotoDataGatheringManagerI::GetTagUsersFromDB(Ice::Long photoid, PhotoContentPtr content)
{
  if(!content)
  {
    return false;
  }
    string table_name = GetPhotoTagTableName(photoid);
     Statement sql;

        sql << "select targetId from "<< table_name<< " where photo=" <<photoid;


     mysqlpp::StoreQueryResult res = QueryRunner("album_info", CDbRServer,
         table_name).store(sql);


     if (!res) {
       MCE_INFO("PhotoDataGatheringManagerI::GetTagUsersFromDB DB select FAILED !!line: "<<__LINE__);
       MCE_INFO("FAILED_Number:1 Photoid:"<<photoid<<" "<<"File"<<__FILE__);
       return false;
     }

     if (res.num_rows() <= 0) {
       MCE_INFO("PhotoDataGatheringManagerI::GetTagUsersFromDB result is EMPTY!! line: "<<__LINE__);
       return true;
     }

     for (unsigned int i = 0; i < res.num_rows(); i++) {

         mysqlpp::Row row = res.at(i);
         content->tagusers.push_back(row["targetId"]);
     }

       return true;

}


bool PhotoDataGatheringManagerI::GetBasicInfoFromDB(Ice::Long photoid,int ownerid,PhotoContentPtr content)
 {
  if (!content) {
    return false;
  }
  string table_name = GetPhotoTableName(ownerid);
  Statement sql;

  sql << "select id, album, title, owner, head_url from " << table_name << " where id="
        << photoid;

  mysqlpp::StoreQueryResult res =
      QueryRunner(PhotoDBInstance, CDbRServer, table_name).store(sql);

  if (!res) {
    MCE_INFO("PhotoDataGatheringManagerI::GetBasicInfoFromDB DB select FAILED !!line: "<<__LINE__);
    MCE_INFO("FAILED_Number:2 Photoid:"<<photoid<<" "<<"File"<<__FILE__);
    return false;
  }

  if (res.num_rows() <= 0) {
    MCE_INFO("PhotoDataGatheringManagerI::GetBasicInfoFromDB result is EMPTY!! line: "<<__LINE__);
    return true;
  }


  mysqlpp::Row row = res.at(0);
  content->photoid = row["id"];
  content->ownerid = row["owner"];


  content->title = row["title"].c_str();
  string basic_url = row["head_Url"].c_str();
  content->url = Allocator::instance().getPhotoUrl(basic_url);

   int albumid = row["album"];

  string albumname = GetAlbumFromDB(albumid, ownerid);
  content->albumname = albumname.c_str();
  return true;
 }


string  PhotoDataGatheringManagerI::GetAlbumFromDB(int albumid, int ownerid)
{

    string table_name = GetPhotoAlbumTableName(ownerid);
    Statement sql;

    sql << "select name from " << table_name << " where id="
        << albumid;

    mysqlpp::StoreQueryResult res =
        QueryRunner("album", CDbRServer, table_name).store(sql);

    if (!res) {
      MCE_INFO("PhotoDataGatheringManagerI::GetAlbumFromDB DB select FAILED !!line: "<<__LINE__);
      MCE_INFO("FAILED_Number:3 albumid: "<<albumid<<" "<<"ownerid: "<<ownerid<<" line: "<<__LINE__);
      return string();
    }

    if (res.num_rows() <= 0) {
      MCE_INFO("PhotoDataGatheringManagerI::GetAlbumFromDB result is EMPTY!! line: "<<__LINE__);
      return string();
    }


    mysqlpp::Row row = res.at(0);
    return row["name"].c_str();

}


bool PhotoDataGatheringManagerI::LoadPhotoContentFromDB(Ice::Long photoid,int uid,PhotoContentPtr content)
{
  bool res = GetBasicInfoFromDB(photoid,uid, content);
  if(!res)
  {
    return false;
  }
  GetTagUsersFromDB(photoid,content);
  return true;

}

static string LongToStr(long id)
{
  char buf[32];
  memset(buf, 0, 32);
  sprintf(buf, "%ld", id);
  return string(buf);

}

static string IntToStr(int id)
{
  char buf[32];
  memset(buf, 0, 32);
  sprintf(buf, "%d", id);
  return string(buf);

}



void PhotoDataGatheringManagerI::SavePhotoSeqToDB(int userid,const ContentItemSeq& contentSeq)
{
  string idlist;
  if (contentSeq.size() == 0) {
    MCE_INFO("PhotoDataGatheringManagerI::SavePhotoSeqToDB contentSeq is Empty!!  userid: "<<userid<<" line: "<<__LINE__);
    idlist = "";
  } else {

    for (unsigned int i = 0; i < contentSeq.size() - 1; i++) {
      long photoid = contentSeq[i].photoid;
      int ownerid = contentSeq[i].ownerid;
      string str_pid = LongToStr(photoid);
      string str_oid = IntToStr(ownerid);
      idlist += str_pid;
      idlist += '_';
      idlist += str_oid;
      idlist += '_';
    }

    int end = contentSeq.size() - 1;
    long photoid = contentSeq[end].photoid;
    int ownerid = contentSeq[end].ownerid;
    string str_pid = LongToStr(photoid);
    string str_oid = IntToStr(ownerid);
    idlist += str_pid;
    idlist += '_';
    idlist += str_oid;
  }

  //Insert to table

  string table_name = GetPhotoSeqTableName(userid);

  mysqlpp::StoreQueryResult res;
  bool need_update = false;

  {
    Statement sql;
    sql << "select user from " << table_name << " where user=" << userid;

    res = QueryRunner(PhotoMemDBInstance, CDbRServer, table_name).store(sql);

    if (!res) {
      MCE_INFO("PhotoDataGatheringManagerI::SavePhotoSeqToDB DB select FAILED !! line: "<<__LINE__);
      MCE_INFO("FAILED_Number:4 userid:"<<userid<<" "<<"idlist: "<<idlist<<" line: "<<__LINE__);
      return;
    }

    if (res.num_rows() > 0) {
      need_update = true;
    }

  }

  {
    Statement sql;

    if (need_update) {
      sql << "update " << table_name << " set " << " photos = " << "\'"
          << idlist << "\'" << "where user= " << userid;

    } else {
      sql << "insert into " << table_name << "(user,photos)" << " values"
          << "( " << userid << ", " << "\'" << idlist << "\'" << " )";
    }

    try {
      mysqlpp::StoreQueryResult res = QueryRunner(PhotoMemDBInstance,
          CDbWServer, table_name).store(sql);
    } catch (const mysqlpp::Exception& e) {
      if (need_update) {
        MCE_INFO("PhotoDataGatheringManagerI::SavePhotoSeqToDB DB update Execption !!"<<e.what()<<" line: "<<__LINE__);
      } else {
        MCE_INFO("PhotoDataGatheringManagerI::SavePhotoSeqToDB DB insert Execption !!"<<e.what()<<" line: "<<__LINE__);
      }
      MCE_INFO("FAILED_Number:4 userid:"<<userid<<" "<<"idlist: "<<idlist<<" line: "<<__LINE__);
    }

  }
//  MCE_INFO("PhotoDataGatheringManagerI::SavePhotoSeqToDB  userid:"<<userid<<" "<<"idlist: "<<idlist<<" line: "<<__LINE__);
//    MCE_INFO("PhotoDataGatheringManagerI::SavePhotoSeqToDB  userid:"<<userid<<" "<<"idlist size: "<<contentSeq.size()<<" line: "<<__LINE__);
}

bool PhotoDataGatheringManagerI::LoadPhotoSeqFromDB(int userid,ContentItemSeq& seq)
{
  string table_name = GetPhotoSeqTableName(userid);
  Statement sql;

  sql << "select * from " << table_name << " where user=" << userid;

  mysqlpp::StoreQueryResult res = QueryRunner(PhotoMemDBInstance, CDbRServer,
      table_name).store(sql);

  if (!res) {
    MCE_INFO("PhotoMemoryManagerI::GetPhotoSeqFromDB DB select FAILED !!line: "<<__LINE__);
    return false;
  }

  if (res.num_rows() <= 0) {
    MCE_INFO("PhotoMemoryManagerI::GetPhotoSeqFromDB result is EMPTY!! line: "<<__LINE__);
    return true;
  }

  mysqlpp::Row row = res.at(0);
  string idlist = row["photos"].c_str();//main url or head url?
  unsigned int i = 0;
  while (i < idlist.size()) {
    ContentItem item;
    string tmpStr;
    while (idlist[i] != '_' && i < idlist.size()) {
      tmpStr += idlist[i];
      i++;
    }
    item.photoid = atol(tmpStr.c_str());
    tmpStr.clear();
    i++;
    while (idlist[i] != '_' && i < idlist.size()) {
      tmpStr += idlist[i];
      i++;
    }

    item.ownerid = atoi(tmpStr.c_str());
    tmpStr.clear();
    seq.push_back(item);
    i++;
  }
  return true;
}



void PhotoDataGatheringManagerI::Test(const Ice::Current&) {
  FlushPhotoContent();
  FlushPhotoSeq();
  FlushPhotoRankInfo();
}


}

}



