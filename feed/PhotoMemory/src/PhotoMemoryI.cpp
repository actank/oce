/*
 * PhotoMemoryI.cpp
 *
 *  Created on: 2010-11-17
 *      Author: wu
 */

#include "PhotoMemoryI.h"
#include "PhotoMemory.h"
#include "ServiceI.h"
#include "PhotoDataGatheringAdapter.h"
#include "BuddyFriendsAdapter.h"

#include "../../FeedMemcProxy/client/photo_memc_client.h"


#include "Allocator.h"
#include<stdlib.h>
#include<time.h>

using namespace com::xiaonei::platform::storage;
using namespace xce::feed;
using namespace com::xiaonei::xce;
using namespace xce::buddy::adapter;

#include<sys/stat.h>
#include   <unistd.h>
#define MONITOR(msg) LOG_INFO("monitor", msg);


inline void InitMonitorLogger(
  const std::string& name,
  const std::string& path,
  const std::string& loglevel = "WARN",
  const std::string& pattern = "%d{%Y-%m-%d %H:%M:%S} - %m%n")
{
  string log_dir= "../log/" + ServiceI::instance().getName()+ "/monitor";
  if(access(log_dir.c_str(),F_OK) != 0)
  {
    int res = mkdir(log_dir.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      if(res<0)
      {
        MCE_INFO("Make monitor log directory error : "<<res);
      }else{
        MCE_INFO("Make monitor log directory OK! res: "<<res);
      }
  }else{
    MCE_INFO("monitor log directory already exists! ");
  }

  log4cplus::SharedAppenderPtr append_1(new log4cplus::DailyRollingFileAppender(path, log4cplus::HOURLY, 1, true));
  append_1->setName(name);
  append_1->setLayout( std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)) );
  log4cplus::Logger::getInstance(name).addAppender(append_1);
  log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
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


void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	InitMonitorLogger("monitor", "../log/" + ServiceI::instance().getName() + "/monitor/monitor_log", "INFO");
	service.getAdapter()->add(&PhotoMemoryManagerI::instance(), service.createIdentity("M", ""));
}


namespace xce {

namespace feed {


void UserPhoto::Put(const ContentItemSeq& seq) const {
  for (unsigned int i = 0; i < seq.size(); i++) {
    photos.push_back(seq[i]);
//   MCE_INFO("UserPhoto::put --->  photoid: "<<seq[i].photoid<<"owner id"<<seq[i].ownerid<<" line: "<<__LINE__);
  }
}


ContentItemSeq UserPhoto::GetRandomSeq(int friendid,int begin, int limit) const {
  ContentItemSeq seq;
  srand((int) time(0));

  if (friendid < 0) {
    for (int i = begin; i < begin + limit && i < photos.size(); i++) {
      int pos = rand() % photos.size();
      seq.push_back(photos[pos]);
    }
  } else {
    ContentItemSeq tmpSeq;
    for (int i = 0;  i < photos.size(); i++) {
      if(photos[i].ownerid==friendid)
      {
        tmpSeq.push_back(photos[i]);
      }
    }

    set<int> posSet;
    for (int i = begin; i < begin + limit && i < tmpSeq.size(); i++) {
          int pos = rand() % tmpSeq.size();
          if(posSet.find(pos)==posSet.end())
          {
            posSet.insert(pos);
//            MCE_INFO("UserPhoto::GetRandomSeq  ownerid: "<<tmpSeq[pos].ownerid<<" line: "<<__LINE__);
            seq.push_back(tmpSeq[pos]);
          }
        }
  }

  return seq;
}

ContentItemSeq UserPhoto::Get(int friendid,int begin, int limit) const {
  ContentItemSeq seq;

  time_t now_time;
  time(&now_time);

  if(now_time-last_access_time_>UPDATE_TIME_INTERNAL)
  {
//    MCE_INFO("UserPhotoPool::Get New Photos from Memc --->  last_access_time_: "<<last_access_time_<<" line: "<<__LINE__);
//    MCE_INFO("UserPhotoPool::Get New Photos from Memc --->  uid: "<<userid_<<" line: "<<__LINE__);
    ContentItemSeq tmpSeq;
    tmpSeq = PhotoMemcClient::instance().GetPhotoSeq(userid_);
//    if(!tmpSeq.empty())
    {
      photos.clear();
      ContentItemSeq emptySeq;
      photos.swap(emptySeq);
      Put(tmpSeq);
    }
    last_access_time_ = now_time;
  }

  if (begin > photos.size() - 1) {
    return ContentItemSeq();
  }

  seq = GetRandomSeq(friendid,begin, limit);

  return seq;
}

void UserPhoto::Remove(Ice::Long photoid) const {
  for (ContentItemSeq::iterator it = photos.begin(); it != photos.end(); it++) {
    if (it->photoid == photoid) {
      photos.erase(it);
      break;
    }
  }

}


void UserPhoto::RemoveFrinendPhoto(int friendid) const {
  for (ContentItemSeq::iterator it = photos.begin(); it != photos.end(); it++) {
      if (it->ownerid == friendid) {
        photos.erase(it);
      }
    }
}



ContentItemSeq UserPhotoPool::Get(const int userid, int friendid,int begin, int limit)
{
//  MCE_INFO("Enter UserPhotoPool::Get--->  uid: "<<userid<<" begin "<<begin<<" limit "<<limit<<" line: "<<__LINE__);
  IceUtil::Mutex::Lock lock(mutex_);

  UidIndex& uid_index = users_.get<1> ();
  UidIndex::iterator it = uid_index.find(userid);
    if (it != uid_index.end()) {
      const UserPhoto* userPhoto = &(*it);
      if(userPhoto)
      {
        SeqIndex& seq_index = users_.get<0>();
        seq_index.relocate(seq_index.begin(), users_.project<0>(it));
        return userPhoto->Get(friendid,begin, limit);
      }
    }
  return ContentItemSeq();
}


void UserPhotoPool::Remove(int userid, Ice::Long photoid)
{
  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex& uid_index = users_.get<1> ();
  UidIndex::iterator it = uid_index.find(userid);
  if (it != uid_index.end()) {
    const UserPhoto* userPhoto = &(*it);
    if(userPhoto)
    {
      userPhoto->Remove(photoid);
    }
  }
}


void UserPhotoPool::RemoveFrinendPhoto(int userid, int friendid)
{
  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex& uid_index = users_.get<1> ();
  UidIndex::iterator it = uid_index.find(userid);
  if (it != uid_index.end()) {
    const UserPhoto* userPhoto = &(*it);
    if(userPhoto)
    {
      userPhoto->RemoveFrinendPhoto(friendid);
    }
  }
}


void UserPhotoPool::Put(int userid, const ContentItemSeq& seq)
{
  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex& uid_index = users_.get<1> ();
  UidIndex::iterator it = uid_index.find(userid);
  if (it != uid_index.end()) {
    const UserPhoto* userPhoto = &(*it);
    if (userPhoto) {
      userPhoto->Put(seq);
      return;
    }
  }

  time_t now_time;
  time(&now_time);

  UserPhoto user_photo(userid);
  user_photo.last_access_time_= now_time;
  user_photo.Put(seq);
//  MCE_INFO("UserPhotoPool::Put  --->     last_access_time_: "<<user_photo.last_access_time_<<" line: "<<__LINE__);

  pair<SeqIndex::iterator, bool> p = users_.push_front(user_photo);
  if (!p.second) {
    users_.relocate(users_.begin(), p.first);
  } else if (users_.size() > MAX_USER_COUNT) {
    users_.pop_back();
//    MCE_INFO("UserPhotoPool::Put users_ full pop_back line: "<<__LINE__);
  }

}

void PhotoMemoryManagerI::SetRead(int userid,Ice::Long  photoid, const Ice::Current&)
{

	MCE_INFO("Enter PhotoMemoryManagerI::SetRead --->  uid: "<<userid<<" photoid "<<photoid<<" line: "<<__LINE__);
	_pool[(userid >> 7) % POOL_SIZE].Remove(userid, photoid);
	PhotoDataGatheringAdapter::instance().SetRead(userid,photoid);
}

void PhotoMemoryManagerI::RemoveFriendPhotos(int userid, int friendid, const Ice::Current&)
{
  MCE_INFO("RemoveFriendPhoto--->  uid: "<<userid<<" friendid "<<friendid<<" line: "<<__LINE__);
  ContentItemSeq seq;
  ContentItemSeq tmpSeq;
  _pool[(userid >> 7) % POOL_SIZE].RemoveFrinendPhoto(userid, friendid);
  LoadPhotoSeqFromDB(userid,seq);


  for (ContentItemSeq::iterator it = seq.begin(); it != seq.end(); it++) {
    MCE_INFO("RemoveFriendPhoto uid: "<<userid<<"  ownerid: "<<it->ownerid<<" photo id: "<<it->photoid<<" line: "<<__LINE__);
    if (it->ownerid == friendid) {
      MCE_INFO("RemoveFriendPhoto--->erase photo..  uid: "<<userid<<" friendid "<<friendid<<" line: "<<__LINE__);
      continue;
    }
    tmpSeq.push_back(*it);
  }

  SavePhotoSeqToDB(userid,tmpSeq);
  PhotoMemcClient::instance().SetPhotoSeq(userid,tmpSeq);
}

void PhotoMemoryManagerI::RemoveDispatchedPhotos(int userid, long photoid, const Ice::Current&)
{
  MyUtil::IntSeq fidSeq = BuddyFriendsCacheAdapter::instance().getFriendListAsc(userid,0,2000);

  for (int i = 0; i < fidSeq.size(); i++) {
    int fid = fidSeq[i];
    ContentItemSeq seq;
    LoadPhotoSeqFromDB(fid, seq);

    for (ContentItemSeq::iterator it = seq.begin(); it != seq.end(); it++) {
      if (it->photoid == photoid) {
        MCE_INFO("RemoveDispatchedPhotos--->erase photo..  friend: "<<fid<<" photoid "<< photoid<<" line: "<<__LINE__);
        seq.erase(it);
        break;
      }
    }

    SavePhotoSeqToDB(fid, seq);
    PhotoMemcClient::instance().SetPhotoSeq(fid, seq);
  }
}


void PhotoMemoryManagerI::SavePhotoSeqToDB(int userid,const ContentItemSeq& contentSeq)
{
  string idlist;
  if (contentSeq.size() == 0) {
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

  string table_name = GetPhotoSeqTableName(userid);
  mysqlpp::StoreQueryResult res;
  Statement sql;
  sql << "update " << table_name << " set " << " photos = " << "\'" << idlist<< "\'" << "where user= " << userid;

  try {
    res = QueryRunner(PhotoMemDBInstance, CDbRServer, table_name).store(sql);
  } catch (const mysqlpp::Exception& e) {
    MCE_INFO("PhotoMemoryManagerI::SavePhotoSeqToDB DB update Execption !!"<<e.what()<<" line: "<<__LINE__);
  }

    MCE_INFO("PhotoMemoryManagerI::SavePhotoSeqToDB  userid:"<<userid<<" "<<"idlist: "<<idlist<<" line: "<<__LINE__);
}


ContentItemSeq PhotoMemoryManagerI::GetPhotoSeq(int uid)
{
//  MCE_INFO("Enter PhotoMemoryManagerI::GetPhotoSeq --->  uid: "<<uid<<" line: "<<__LINE__);
  ContentItemSeq seq = PhotoMemcClient::instance().GetPhotoSeq(uid);

  if(seq.empty())
  {
    LoadPhotoSeqFromDB(uid,seq);
  }
  /*else{
    MCE_INFO("PhotoMemoryManagerI::GetPhotoSeq Can get from MemCache--->  uid: "<<uid<<" line: "<<__LINE__);
  }*/
  return seq;

}


PhotoContentSeq PhotoMemoryManagerI::GetPhotoContentSeq(int userid,int begin, int limit,const Ice::Current& )
{
  MCE_INFO("Enter PhotoMemoryManagerI::GetPhotoContentSeq--->  uid: "<<userid<<" begin "<<begin<<" limit "<<limit<<" line: "<<__LINE__);
  MONITOR("Enter PhotoMemoryManagerI::GetPhotoContentSeq--->  uid: "<<userid<<" begin "<<begin<<" limit "<<limit<<" line: "<<__LINE__);
  return FetchPhotoContentSeq(userid, -1, begin, limit);

}

PhotoContentSeq PhotoMemoryManagerI::GetFriendPhotoContentSeq(int userid, int friendid, int begin, int limit,const Ice::Current& )
{
//  MCE_INFO("Enter PhotoMemoryManagerI::GetFriendPhotoContentSeq--->  uid: "<<userid<<" begin "<<begin<<" limit "<<limit<<" line: "<<__LINE__);
  return FetchPhotoContentSeq(userid, friendid, begin, limit);
}

PhotoContentSeq PhotoMemoryManagerI::FetchPhotoContentSeq(int userid, int friendid, int begin, int limit)
{

  ContentItemSeq contentItems = _pool[(userid>>7)%POOL_SIZE].Get(userid, friendid,begin,limit);

  if(contentItems.empty())
  {
    contentItems = GetPhotoSeq(userid);
    if(contentItems.empty())
    {
      return PhotoContentSeq();
    }
    _pool[(userid>>7)%POOL_SIZE].Put(userid,contentItems);
    contentItems.clear();
    contentItems = _pool[(userid>>7)%POOL_SIZE].Get(userid, friendid,begin,limit);
  }

  PhotoContentSeq contentSeq;
  vector<PhotoContentPtr> missedContents;

  MyUtil::LongSeq idSeq;
  for(unsigned int i=0;i<contentItems.size();i++)
  {
      idSeq.push_back(contentItems[i].photoid);
  }


  PhotoContentDict dict = PhotoMemcClient::instance().GetPhotoContentDict(idSeq);


  for (unsigned int i = 0; i < contentItems.size(); i++) {

    Ice::Long photoid = contentItems.at(i).photoid;
    int ownerid = contentItems.at(i).ownerid;
    if (ownerid == userid || (friendid>0 && friendid != ownerid) ) {
//      MCE_INFO("PhotoMemoryManagerI::GetPhotoContentSeq  filter!--->  uid: "<<userid<<" line: "<<__LINE__);
      continue;
    }



    if (dict.find(contentItems[i].photoid) == dict.end()) {
//      MCE_INFO("PhotoMemoryManagerI::GetPhotoContentSeq can not get from Memcache !!--->  photoid: "<<contentItems[i].photoid<<" line: "<<__LINE__);
      Ice::Long photoid = contentItems.at(i).photoid;
      int ownerid = contentItems.at(i).ownerid;

      PhotoContentPtr content = new PhotoContent();
      if (LoadPhotoContentFromDB(photoid, ownerid, content)) {
        content->ownerid = ownerid;
        PhotoMemcClient::instance().SetPhotoContent(content);
        contentSeq.push_back(content);
        //missedContents.push_back(content);
      }

    }else{
      PhotoContentDict::iterator it = dict.find(contentItems[i].photoid);
      contentSeq.push_back(it->second);
    }
  }
  return contentSeq;

}




bool PhotoMemoryManagerI::GetTagUsersFromDB(Ice::Long photoid,int ownerid, PhotoContentPtr content)
{
//  MCE_INFO("Enter PhotoMemoryManagerI::GetTagUsersFromDB --->  ownerid: "<<ownerid<<" photoid "<<photoid<<" line: "<<__LINE__);
  string table_name = GetPhotoTagTableName(ownerid);
  Statement sql;

  sql << "select targetId  from " << table_name << " where id=" << photoid
      << " and owner=" << ownerid;

  mysqlpp::StoreQueryResult res = QueryRunner("album_info", CDbRServer,
      table_name).store(sql);

  if (!res) {
    MCE_INFO("PhotoMemoryManagerI::GetTagUsersFromDB DB select FAILED !!line: "<<__LINE__);
    MCE_INFO("FAILED_Number:2 Photoid:"<<photoid<<" "<<"File"<<__FILE__);
    return false;
  }

  if (res.num_rows() <= 0) {
    MCE_INFO("PhotoMemoryManagerI::GetTagUsersFromDB result is EMPTY!! line: "<<__LINE__);
    return true;
  }

  for (unsigned int i = 0; i < res.num_rows(); i++) {

    mysqlpp::Row row = res.at(i);
    content->tagusers.push_back(row["targetId"]);
  }

  return true;
}


bool PhotoMemoryManagerI::GetBasicInfoFromDB(Ice::Long photoid,int ownerid,PhotoContentPtr content)
{

  if (!content) {
    return false;
  }

//  MCE_INFO("Enter PhotoMemoryManagerI::GetBasicInfoFromDB --->   photoid: "<<photoid<<" ownerid: "<<ownerid<<" line: "<<__LINE__);
  string table_name = GetPhotoTableName(ownerid);
  Statement sql;

  sql << "select id, album, title,owner, head_url from " << table_name
        << " where id=" << photoid;

  mysqlpp::StoreQueryResult res =
      QueryRunner(PhotoDBInstance, CDbRServer, table_name).store(sql);

  if (!res) {
    MCE_INFO("PhotoMemoryManagerI::GetBasicInfoFromDB DB select FAILED !!line: "<<__LINE__);
    MCE_INFO("FAILED_Number:2 Photoid:"<<photoid<<" "<<"File"<<__FILE__);
    return false;
  }

  if (res.num_rows() <= 0) {
    MCE_INFO("PhotoMemoryManagerI::GetBasicInfoFromDB result is EMPTY!! line: "<<__LINE__);
    return false;
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


bool PhotoMemoryManagerI::LoadPhotoContentFromDB(Ice::Long photoid,int ownerid,PhotoContentPtr content)
{
//  MCE_INFO("Enter PhotoMemoryManagerI::LoadPhotoSeqFromDB --->  photoid: "<<photoid<<" line: "<<__LINE__);
  bool res = GetBasicInfoFromDB(photoid,ownerid,content);

  if(!res)
  {
     return false;
  }
  GetTagUsersFromDB(photoid,ownerid,content);
  return true;
}


bool PhotoMemoryManagerI::LoadPhotoSeqFromDB(int userid,ContentItemSeq& seq)
{
//  MCE_INFO("Enter PhotoMemoryManagerI::LoadPhotoSeqFromDB --->  uid: "<<userid<<" line: "<<__LINE__);
  string table_name = GetPhotoSeqTableName(userid);
  Statement sql;

  sql << "select * from " << table_name << " where user=" << userid;

  mysqlpp::StoreQueryResult res = QueryRunner(PhotoMemDBInstance, CDbRServer,
      table_name).store(sql);

  if (!res) {
    MCE_INFO("PhotoMemoryManagerI::GetPhotoSeqFromDB DB select FAILED !!line: "<<__LINE__);
    //MCE_INFO("FAILED_Number:2 Photoid:"<<photoid<<" "<<"File"<<__FILE__);
    return false;
  }

  if (res.num_rows() <= 0) {
    MCE_INFO("PhotoMemoryManagerI::GetPhotoSeqFromDB result is EMPTY!! line: "<<__LINE__);
    return true;
  }

  //MCE_INFO("UserFeed::load() result record num: "<<res.num_rows()<<" line: "<<__LINE__);

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
//    MCE_INFO("PhotoMemoryManagerI::LoadPhotoSeqFromDB ---> photoid: "<<item.photoid<<" uid: "<<userid<<" line: "<<__LINE__);
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


string PhotoMemoryManagerI::GetAlbumFromDB(int albumid, int ownerid)
{
//    MCE_INFO("Enter PhotoDataGatheringManagerI::GetAlbumFromDB --->   albumid "<<albumid<<" ownerid: "<<ownerid<<" line: "<<__LINE__);

    string table_name = GetPhotoAlbumTableName(ownerid);
    Statement sql;

    sql << "select name from " << table_name << " where id="
        << albumid;

    mysqlpp::StoreQueryResult res =
        QueryRunner("album", CDbRServer, table_name).store(sql);

    if (!res) {
      MCE_INFO("PhotoDataGatheringManagerI::GetAlbumFromDB DB select FAILED !!line: "<<__LINE__);
      MCE_INFO("FAILED_Number:2 ownerid:"<<ownerid<<" "<<"File"<<__FILE__);
      return string();
    }

    if (res.num_rows() <= 0) {
      MCE_INFO("PhotoDataGatheringManagerI::GetAlbumFromDB result is EMPTY!! line: "<<__LINE__);
      return string();
    }

    //MCE_INFO("UserFeed::load() result record num: "<<res.num_rows()<<" line: "<<__LINE__);

    mysqlpp::Row row = res.at(0);
//    MCE_INFO("PhotoDataGatheringManagerI::GetAlbumFromDB album name:"<<row["name"]<<" "<<"File"<<__FILE__);
    return row["name"].c_str();

}

}

}


