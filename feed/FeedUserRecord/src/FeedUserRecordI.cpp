#include<Ice/Ice.h>
#include"FeedUserRecordI.h"
#include"ServiceI.h"
#include<QueryRunner.h>

using namespace com::xiaonei::xce;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedUserRecordI::instance(), service.createIdentity("FeedUserRecord",""));
  TaskManager::instance().scheduleRepeated(new FlushDBTimer());
}


void FeedUserRecordDataWrapper::UpdateData(int type, long value) const{
  MCE_DEBUG("@@@@@@@@@@@@@22 --> " << type << " " << value << " " << data_.newsfeedid << " " << data_.favoritefeedid);
  if((type == 0) && (value > data_.newsfeedid)) {
    data_.newsfeedid = value;
    dirty_ = true;
  }
  if((type == 1) && (value > data_.favoritefeedid)) {
    data_.favoritefeedid = value;
    dirty_ = true;
  }
}


bool FeedUserRecordI::Load(int uid){
  FeedUserRecordData result_db = GetFromDB(uid);
  {
    IceUtil::Mutex::Lock lock(mutexs_[GetIndex(uid)]);
    FeedUserRecordDataWrapper record;
    record.uid_ = uid;
    record.data_ = result_db;
    MCE_DEBUG("FeedUserRecordI::Load-->userid:"<<record.uid_<<" maxnewsid:"<<result_db.newsfeedid<<" maxfavid:"<<result_db.favoritefeedid<<" load from db");
    SeqIndex &sindex = pool_[GetIndex(uid)].get<0>();
    pair<SeqIndex::iterator,bool> p = pool_[GetIndex(uid)].push_front(record);
    if(!p.second) {
      sindex.relocate(sindex.begin(), p.first);
    }
  }
  return true;
}

bool FeedUserRecordI::CheckLoad(int uid){
  {
    IceUtil::Mutex::Lock lock(mutexs_[GetIndex(uid)]);
    UidIndex & index = pool_[GetIndex(uid)].get<1> ();
    UidIndex::iterator uit = index.find(uid);
    if(uit != index.end()) {
      return false;
    }
  }
  return true;
}

void FeedUserRecordI::Set(::Ice::Int uid, ::Ice::Long feedid, ::Ice::Int type,  const Ice::Current&) {
  MCE_DEBUG("FeedUserRecordI::Set-->userid:"<<uid<<" need to set feedid:"<<feedid);
  if(CheckLoad(uid)){
    Load(uid);
  }
  {
    IceUtil::Mutex::Lock lock(mutexs_[GetIndex(uid)]);
    UidIndex & index = pool_[GetIndex(uid)].get<1> ();
    UidIndex::iterator uit = index.find(uid);
    if(uit != index.end()) {
      MCE_DEBUG("need to call UpdateData-->feedid:"<<feedid<<" uid"<< uit->uid_);
      uit->UpdateData(type, feedid); 
      SeqIndex::iterator sit = pool_[GetIndex(uid)].project<0> (uit);
      pool_[GetIndex(uid)].relocate(pool_[GetIndex(uid)].get<0> ().begin(),sit);
    }
  }
}

FeedUserRecordResPtr FeedUserRecordI::Get(const int uid, const Ice::Current&) {
  if(CheckLoad(uid)){
    Load(uid);
  }
  {
    IceUtil::Mutex::Lock lock(mutexs_[GetIndex(uid)]);
    UidIndex & index = pool_[GetIndex(uid)].get<1> ();
    UidIndex::iterator uit = index.find(uid);
    if(uit != index.end()) {
      SeqIndex::iterator sit = pool_[GetIndex(uid)].project<0> (uit);
      pool_[GetIndex(uid)].relocate(pool_[GetIndex(uid)].get<0> ().begin(),sit);
      return (*sit).ToFeedUserRecordRes();
    }
  }
}

void FlushDBTimer::handle() {
  FeedUserRecordI::instance().FlushDirtyData();
}

FeedUserRecordData FeedUserRecordI::GetFromDB(int uid) {
  Statement sql;
  sql<<"SELECT * FROM "<<_getFeedUserRecordTab(uid)<<" WHERE user_id ="<<uid;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(FeedDBInstance, CDbRServer,_getExpressTab(uid)).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedUserRecordI::GetFromDB --> error:" << e<<" DB:"<<_getExpressTab(uid)<<" Table:"<<_getFeedUserRecordTab(uid));
  } catch (std::exception& e) {
    MCE_WARN("FeedUserRecordI::GetFromDB --> error:" << e.what()<<" DB:"<<_getExpressTab(uid)<<" Table:"<<_getFeedUserRecordTab(uid));
  }
  if(!res) {
    MCE_WARN("FeedUserRecordI::GetFromDB --> StoryQueryResult wrong");
  }
  FeedUserRecordData data;
  data.newsfeedid = 0;
  data.favoritefeedid = 0;
  for(int i=0; i<res.num_rows(); i++){
    int type = res[i]["type"];
    if(type==0){
      data.newsfeedid = res[i]["value"];
    }
    if(type==1){
      data.favoritefeedid = res[i]["value"];
    }
    MCE_INFO("@@@ uid:" << uid << " " << type << " " << " " << res[i]["value"] << " " << _getExpressTab(uid)); 
  }
  return data;
}

void FeedUserRecordI::FlushDirtyData() {
  int upcount = 0;
  MCE_DEBUG("FeedUserRecordI::FlushDirtyData-->is start");
  for(int j=0;j<POOL_SIZE;j++){
      IceUtil::Mutex::Lock lock(mutexs_[j]);
      UidIndex &index = pool_[j].get<1>();
      int ItemCount = 0;
     
      Statement sql;
      ostringstream sql_info;
      for(UidIndex::iterator uit = index.begin(); uit!=index.end();){
        //int userid = uit->uid_;
        MCE_DEBUG("FeedUserRecordI::FlushDirtyData-->uid:"<<uit->uid_<<" dirty:"<<uit->dirty_);
        if(uit->dirty_){
          upcount++;
          uit->dirty_ = false;
          if(ItemCount == 0) {
            sql.clear();
            //sql << "INSERT INTO feed_user_record (userid, newsfeed, favoritefeed) VALUES "<<
            //"(" << uit->uid_ << "," << uit->data_.newsfeedid<<","<< uit->data_.favoritefeedid << ")";
            //<<"on duplicate key update newsfeed=values(newsfeed), favoritefeed=values(favoritefeed), time=values(time)";
            MCE_DEBUG("FeedUserRecordI::FlushDirtyData-->user_id:"<<uit->uid_<<" newsid:"<<uit->uid_<<" put DB:"<<_getExpressTab(j)<<" table-->"<<_getFeedUserRecordTab(j));
            sql << "INSERT INTO "<<_getFeedUserRecordTab(j)<<"(user_id, type, des, value ,update_time) VALUES "<<
                "(" << uit->uid_ << "," << 0 <<","<<"\"maxnewsfeedid\""<<","<< uit->data_.newsfeedid<< ","<<"CURRENT_TIMESTAMP"<<"),"
                "(" << uit->uid_ << "," << 1 <<","<<"\"maxfavfeedid\""<<","<< uit->data_.favoritefeedid <<","<<"CURRENT_TIMESTAMP"<< ")";
            ItemCount++;
            //---------------------------debug
             sql_info << "INSERT INTO "<<_getFeedUserRecordTab(j)<<"(user_id, type, des, value ,update_time) VALUES "<<
                "(" << uit->uid_ << "," << 0 <<","<<"\"maxnewsfeedid\""<<","<< uit->data_.newsfeedid<< ","<<"CURRENT_TIMESTAMP"<<"),"
                "(" << uit->uid_ << "," << 1 <<","<<"\"maxfavfeedid\""<<","<< uit->data_.favoritefeedid <<","<<"CURRENT_TIMESTAMP"<< ")";
            
          }else if(ItemCount < SqlItemCount) {
            //sql << "," << "(" << uit->uid_ << "," << uit->data_.newsfeedid<<","<< uit->data_.favoritefeedid << ")";
            ItemCount++;
            MCE_DEBUG("FeedUserRecordI::FlushDirtyData-->user_id:"<<uit->uid_<<" newsid:"<<uit->uid_<<" put DB:"<<_getExpressTab(j)<<" table-->"<<_getFeedUserRecordTab(j));

            sql << ",(" << uit->uid_ << "," << 0 <<","<<"\"maxnewsfeedid\""<<","<< uit->data_.newsfeedid<<","<<"CURRENT_TIMESTAMP"<<"),("
                << uit->uid_ << "," << 1 <<","<<"\"maxfavfeedid\""<<","<< uit->data_.favoritefeedid <<","<<"CURRENT_TIMESTAMP"<<")";

            //------------------------------debug
            sql_info << ",(" << uit->uid_ << "," << 0 <<","<<"\"maxnewsfeedid\""<<","<< uit->data_.newsfeedid<<","<<"CURRENT_TIMESTAMP"<<"),("
                << uit->uid_ << "," << 1 <<","<<"\"maxfavfeedid\""<<","<< uit->data_.favoritefeedid <<","<<"CURRENT_TIMESTAMP"<<")";


          }
        }
        ++uit;
        if((!sql.empty()) && (ItemCount == SqlItemCount || uit==index.end())) {
          try {
          	sql << "on duplicate key update value=values(value), update_time=values(update_time)";
          	sql_info << "on duplicate key update value=values(value), update_time=values(update_time)";
            MCE_DEBUG("FeedUserRecordI::FlushDirtyData:SQL-->"<<sql_info.str());
           // MCE_DEBUG("FeedUserRecordI::FlushDirtyData:SQLDB-->"<<_getExpressTab(j)<<" table-->"<<_getFeedUserRecordTab(j)<<" the userid:"<<userid);
            QueryRunner(FeedDBInstance, CDbWServer,_getExpressTab(j)).store(sql);
          } catch (Ice::Exception& e) {
            MCE_WARN("FeedUserRecordI::FlushDirtyData --> error:" << e<<" DB:"<<_getExpressTab(j)<<" Table:"<<_getFeedUserRecordTab(j));
          } catch (std::exception& e) {
            MCE_WARN("FeedUserRecordI::FlushDirtyData --> error:" << e.what()<<" DB:"<<_getExpressTab(j)<<" Table:"<<_getFeedUserRecordTab(j));
          }
          ItemCount = 0;
          sql.clear();
          sql_info.clear();
        }
      }
      sql.clear();
  } 
  MCE_DEBUG("FeedUserRecordI::FlushDirtyData-->need to update num:"<<upcount);
}
