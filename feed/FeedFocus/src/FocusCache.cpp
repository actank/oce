/*
 * FocusCache.h
 *
 *  Created on: May 23, 2010
 *      Author: yejingwei
 */

#include "FocusCache.h"
#include <exception>
#include <iostream>
#include <ostream>
#include <vector>
#include <QueryRunner.h>
#include "ServiceI.h"
#include <Ice/Exception.h>
#include <exception>
#include <stdexcept>
#include "FeedUserRecord.h"
#include "FeedUserRecordAdapter.h"

using namespace std;
using namespace xce::feed;
using namespace com::xiaonei::xce;
using namespace boost::multi_index;
using namespace boost;

//---------------------------------------------------------------------------
//初始化，默认所有用户都已经设定了关注好友
void FocusCache::Init() {
  bitset_.set();
  MCE_DEBUG("FocusCache::Init --> bitset_ size = " << bitset_.count());
}

//获取source设定的关注好友
set<int> FocusCache::GetTargets(int source) {
  set<int> result;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    if (bitset_.test(source) == false) {//没有设定任何关注。
      MCE_DEBUG("FocusCache::GetTargets --> source = " << source << ", bit is FALSE " << ",multi_index size = " << container_.size());
      return result;
    }
    MCE_DEBUG("FocusCache::GetTargets --> source = " << source << ", bit is TRUE ");
    UidIndex & index = container_.get<1> ();
    UidIndex::iterator uit = index.find(source);
    if (uit != index.end()) { //在cache中
      result = (*uit)->targets();

      SeqIndex::iterator sit = container_.project<0> (uit);//新查的放到前面
      container_.relocate(container_.get<0> ().begin(), sit);
      bitset_.set(source);//此时bitset_中可能已经设置过，也可能没设置过。
      MCE_DEBUG("FocusCache::GetTargets --> source = " << source << " in multi_index , targets size = " << result.size() << ",multi_index size = " << container_.size());
      return result;
    }
  }
  //不在cache中
  //  vector<int> targets_db = GetDB(source);
  vector<int> targets_db;
  bool db_success = GetDB(source, targets_db);

  if (!db_success) { //查询数据库失败
    MCE_WARN("FocusCache::GetTargets --> call GetDB fail , source = " << source);
    return set<int> ();
  }

  {
    FeedUserRecordResPtr record;
    if (!targets_db.empty()) {
      try {
        record = FeedUserRecordAdapter::instance().Get(source);
      } catch (Ice::Exception& e) {
        MCE_WARN("FocusCache::GetTargets --> call FeedUserRecord err, " << e << ",source = " << source);
        record = 0;//说明调用Adapter错误
      }
    }

    IceUtil::Mutex::Lock lock(mutex_);

    //该用户没有关注，清除cache中该用户
    if (targets_db.empty()) { //数据库该source为空
      bitset_.reset(source);
      UidIndex & uindex = container_.get<1> ();
      UidIndex::iterator uit = uindex.find(source);
      if (uit != uindex.end()) {
        uindex.erase(uit);
      }
      MCE_DEBUG("FocusCache::GetTargets --> bitset_ size = " << bitset_.count() << ",bitset_ reset count = " << bitset_.size() - bitset_.count() << ",multi_index size = " << container_.size());
      return set<int> ();
    }

    //该用户有关注
    bitset_.set(source);
    FocusConfigPtr config = new FocusConfig();
    config->source_ = source;
    //---------------------------------------------------
    //FeedUserRecordResPtr record; 
    //try{
    //  record = FeedUserRecordAdapter::instance().Get(source);
    //}catch(Ice::Exception& e){
    //  MCE_WARN("FocusCache::GetTargets --> call FeedUserRecord err, " << e);
    //}
    if (!record) {
      config->maxfeed_ = 0x7FFFFFFFFFFFFFFF;
      MCE_WARN("FocusCache::GetTargets --> FeedUserRecord empty ,source = " << source);
    } else {
      config->maxfeed_ = record->data.favoritefeedid;
      MCE_INFO("@@@load from DB-->userid:"<<source<<"  maxid:"<<record->data.favoritefeedid);
    }
    //---------------------------------------------
    //MCE_INFO("the user:"<<source<<"is new,the init unreadcount is:"<<config->unreadcount_<<"the init time is:"<<config->lastAccesstime_);
    SeqIndex & sindex = container_.get<0> ();
    pair<SeqIndex::iterator, bool> p = container_.push_front(config);

    for (size_t i = 0; i < targets_db.size(); ++i) {
      (*p.first)->targets_.insert(targets_db.at(i));
    }
    if (!p.second) {

      sindex.relocate(sindex.begin(), p.first);
      //          sindex.replace(p.first,config);
    } else {
      Kick();
    }
    MCE_DEBUG("FocusCache::GetTargets --> bitset_ size = " << bitset_.count() << ",bitset_ reset count = " << bitset_.size() - bitset_.count() << ",multi_index size = " << container_.size());
    return (*p.first)->targets_;
  }
}
//添加source新增的关注好友
int FocusCache::AddTargets(int source, int target) {
  //  AddDB(source, targets);
  //  UpdateCache(source, targets, true);
  set<int> targets = GetTargets(source);
  if (targets.find(target) != targets.end()) {
    return 2;//失败返回值2：已关注过该好友，添加失败
  }

  {
    IceUtil::Mutex::Lock lock(mutex_);
    bitset_.set(source);

    FocusConfigPtr config = new FocusConfig();
    config->source_ = source;

    SeqIndex & sindex = container_.get<0> ();
    pair<SeqIndex::iterator, bool> p = container_.push_front(config);

    if((*p.first)->targets_.size() >= 20){
    	return 1;//失败返回值1：关注数量已达上限，添加失败
    }
    (*p.first)->targets_.insert(target);

    if (!p.second) {
      sindex.relocate(sindex.begin(), p.first);
      //          sindex.replace(p.first,config);
    } else {
      Kick();
    }

  }

  AddDB(source, target);
  return 0;
}

//删除source设定的关注好友
void FocusCache::DelTargets(int source, int target) {
  //    DelDB(source, targets);
  //    UpdateCache(source, targets, false);
  {
    IceUtil::Mutex::Lock lock(mutex_);

    UidIndex & uindex = container_.get<1> ();
    UidIndex::iterator uit = uindex.find(source);

    if (uit != uindex.end()) {
      (*uit)->Del(target);
      if ((*uit)->Empty()) {
        uindex.erase(uit);
        bitset_.reset(source);
        MCE_INFO("FocusCache::DelTargets --> source = " << source << ",target = " << target << ", source delete last target,source is deleted from cache");
      } else {
        MCE_INFO("FocusCache::DelTargets --> source = " << source << ",target = " << target << ", source is still in cache");
      }
    } else {
      MCE_INFO("FocusCache::DelTargets --> source = " << source << ",target = " << target << ", source is not in cache");
      //      bitset_.reset(source);
    }
  }
  DelDB(source, target);
}
//------------------------------------------------------------------------------------------------------

bool FocusCache::IsGetIncCount(int user) {
  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex& uindex = container_.get<1> ();
  UidIndex::iterator uit = uindex.find(user);
  if (uit != uindex.end()) {
    if (time(NULL) - (*uit)->lastAccesstime_ < 5 * 60) {
      return false;
    }
    (*uit)->lastAccesstime_ = time(NULL);
    SeqIndex::iterator sit = container_.project<0> (uit);
    container_.relocate(container_.get<0> ().begin(), sit);
    return true;
  }
  return false;
}
int FocusCache::GetUnreadCount(int user) {
  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex& uindex = container_.get<1> ();
  UidIndex::iterator uit = uindex.find(user);
  if (uit != uindex.end()) {
    return (*uit)->unreadcount_;
  }
  return 0;
}
void FocusCache::SetUnreadCount(int user, int count) {
  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex& uindex = container_.get<1> ();
  UidIndex::iterator uit = uindex.find(user);
  if (uit != uindex.end()) {
    (*uit)->unreadcount_ = count;
  }
}
void FocusCache::UpdateMaxFeed(int user, FeedItemSeqSeq& itemseqseq) {
  IceUtil::Mutex::Lock lock(mutex_);
  if (bitset_.test(user) == false) {
    return;
  }
  MCE_INFO("UpdateMaxFeed-->");
  UidIndex& uindex = container_.get<1> ();
  UidIndex::iterator uit = uindex.find(user);
  if (uit != uindex.end()) {
    long maxfeed = (*uit)->maxfeed_;
    for (FeedItemSeqSeq::iterator item_seq_itr = itemseqseq.begin(); item_seq_itr
        != itemseqseq.end(); ++item_seq_itr) {
      if (item_seq_itr->empty()) {
        MCE_DEBUG("FocusCache::UpdateMaxFeed --> item_seq_empty");
        continue;
      }
      for (FeedItemSeq::iterator item_itr = item_seq_itr->begin(); item_itr
          != item_seq_itr->end(); ++item_itr) {
        if (item_itr->feed > maxfeed) {
          maxfeed = item_itr->feed;
        }
      }
    }
    if (maxfeed > (*uit)->maxfeed_) {
      (*uit)->maxfeed_ = maxfeed;
      try {
        FeedUserRecordAdapter::instance().Set(user, maxfeed, 1);
      } catch (Ice::Exception &e) {
        MCE_WARN("FocusCache::UpdateMaxFeed--> call FeedUserRecord err:"<<e);
      }
      //FeedUserRecordAdapter::instance().Set(user, maxfeed, 1);
      MCE_INFO("UpdateMaxFeed-->need to write maxfeedid:"<<maxfeed<<" userid:"<<user);
    }
  }
}

int FocusCache::GetIncCount(int user, FeedItemSeqSeq& itemseqseq) {
  IceUtil::Mutex::Lock lock(mutex_);
  if (bitset_.test(user) == false) {
    return 0;
  }
  UidIndex& uindex = container_.get<1> ();
  UidIndex::iterator uit = uindex.find(user);
  if (uit != uindex.end()) {
    int count = 0;
    long maxfeed = (*uit)->maxfeed_;
    long oldmaxfeed = (*uit)->maxfeed_;
    MCE_INFO("the user:"<<user<<" old maxfeed:"<<oldmaxfeed);
    for (FeedItemSeqSeq::iterator item_seq_itr = itemseqseq.begin(); item_seq_itr
        != itemseqseq.end(); ++item_seq_itr) {
      if (item_seq_itr->empty()) {
        MCE_DEBUG("FocusCache::GetIncCount --> item_seq_empty");
        continue;
      }

      bool iscount = false;
      for (FeedItemSeq::iterator item_itr = item_seq_itr->begin(); item_itr
          != item_seq_itr->end(); ++item_itr) {
        if (item_itr->feed > oldmaxfeed) {
          iscount = true;
        }
        if (item_itr->feed > maxfeed) {
          maxfeed = item_itr->feed;
        }
      }
      if (iscount) {
        count++;
      }
    }
    if ((*uit)->maxfeed_ == 0) {//not sucess load from DB or the service  restart
      count = 0;
      (*uit)->maxfeed_ = maxfeed;
      try {
        FeedUserRecordAdapter::instance().Set(user, maxfeed, 1);
      } catch (Ice::Exception& e) {
        MCE_WARN("FocusCache::GetIncCount -->call FeedUserRecord err:"<<e);
      }
      //FeedUserRecordAdapter::instance().Set(user, maxfeed, 1);
    }

    //    (*uit)->maxfeed_ = maxfeed;
    //    count += (*uit)->unreadcount_;
    (*uit)->unreadcount_ = count;

    SeqIndex::iterator sit = container_.project<0> (uit);
    container_.relocate(container_.get<0> ().begin(), sit);
    return count;
  }
  return 0;
}
//------------------------------------------------------------------------------------------------------
//更新到内存,并且更新bitset
//vector<int> FocusCache::UpdateCache(int source, const vector<int> & targets,
//    bool add_del) {
//  if (!targets.empty()) { //说明是AddTargets或DelTargets调用
//    IceUtil::Mutex::Lock lock(mutex_);
//    {
//      UidIndex & uindex = container_.get<1> ();
//      UidIndex::iterator uit = uindex.find(source);
//      if (uit != uindex.end()) { //在cache中
//        if (add_del == true) {
//          MCE_DEBUG("FocusCache::UpdateCache --> source = " << source << " in multi_index " << ", added targets size = " << targets.size() );
//          (*uit)->Add(targets);
//        } else {
//          (*uit)->Del(targets);
//          MCE_DEBUG("FocusCache::UpdateCache --> delete source = " << source << " in multi_index " <<", deleted targets size = " << targets.size() );
//          if ((*uit)->Empty()) {
//            uindex.erase(uit);
//            bitset_.reset(source);
//            vector<int> result;
//            MCE_DEBUG("FocusCache::UpdateCache --> source = " << source << ",targets zero , delete this source" );
//            return result; //返回空
//          }
//        }
//        SeqIndex::iterator sit = container_.project<0> (uit); //放到第一个
//        container_.relocate(container_.get<0> ().begin(), sit);
//        bitset_.set(source);
//        vector<int> result((*uit)->targets_.begin(), (*uit)->targets_.end());
//        MCE_DEBUG("FocusCache::UpdateCache --> bitset_ size = " << bitset_.count() << "bitset_ reset count = " << bitset_.size() - bitset_.count() << ",multi_index size = " << container_.size());
//        return result;
//      }
//    }
//  }
//
//  vector<int> targets_db = GetDB(source);
//
//  {
//    IceUtil::Mutex::Lock lock(mutex_);
//
//    //该用户没有关注，清除cache中该用户
//    if (targets_db.empty()) { //数据库该source为空
//      bitset_.reset(source);
//      UidIndex & uindex = container_.get<1> ();
//      UidIndex::iterator uit = uindex.find(source);
//      if (uit != uindex.end()) {
//        uindex.erase(uit);
//      }
//      MCE_DEBUG("FocusCache::UpdateCache --> bitset_ size = " << bitset_.count() << ",bitset_ reset count = " << bitset_.size() - bitset_.count() << ",multi_index size = " << container_.size());
//      return targets_db;
//    }
//
//    //该用户有关注
//    bitset_.set(source);
//    FocusConfigPtr config = new FocusConfig();
//    config->source_ = source;
//    for (size_t i = 0; i < targets_db.size(); ++i) {
//      config->targets_.insert(targets_db.at(i));
//    }
//    SeqIndex & sindex = container_.get<0> ();
//    pair<SeqIndex::iterator, bool> p = container_.push_front(config);
//    if (!p.second) {
//      sindex.relocate(sindex.begin(), p.first);
//    } else {
//      Kick();
//    }
//    MCE_DEBUG("FocusCache::UpdateCache --> bitset_ size = " << bitset_.count() << ",bitset_ reset count = " << bitset_.size() - bitset_.count() << ",multi_index size = " << container_.size());
//    return targets_db;
//  }
//
//}

bool FocusCache::GetDB(int source, vector<int> & targets) { //返回false说明查询数据库失败

  Statement sql;
  sql << "SELECT target FROM relation_feed_high WHERE source = " << source;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner("feedwb", CDbRServer).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" FocusCache::GetDB --> error:" << e << ",source = " << source);
    return false;
  } catch (std::exception& e) {
    MCE_WARN(" FocusCache::GetDB --> error:" << e.what() << ",source = " << source);
    return false;
  }
  //  vector<int> result;
  if (!res) {
    MCE_WARN("FocusCache::GetDB --> StoryQueryResult wrong " << ",source = " << source);
    return false;
  }
  if (res.empty()) {
    return true;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    int target = res.at(i).at(0);
    targets.push_back(target);
  }
  MCE_INFO("FocusCache::GetDB --> source = " << source << ", result targets size = " << targets.size() );
  return true;
}

void FocusCache::AddDB(int source, int target) {

  Statement sql;
  sql << "INSERT IGNORE relation_feed_high (source,target) VALUE (" << source
      << "," << target << ")";

  try {
    QueryRunner("feedwb", CDbWServer,"relation_feed_high").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" FocusCache::AddDB --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" FocusCache::AddDB --> error:" << e.what());
  }
}

void FocusCache::DelDB(int source, int target) {

  Statement sql;
  sql << "DELETE FROM relation_feed_high WHERE source = " << source << " AND "
      << " target = " << target;
  try {
    QueryRunner("feedwb", CDbWServer,"relation_feed_high").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" FocusCache::DelDB --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" FocusCache::DelDB --> error:" << e.what());
  }
}

void FocusCache::Kick() {
  while (container_.size() > kFocusSize) { //在外层加锁
    MCE_DEBUG("FocusCache::Kick --> Kicking source = " << (*(container_.rbegin()))->source_);
    container_.pop_back();
  }
}

//---------------------------------------------------------------------------

void PrintTask::handle() {
  MCE_INFO("PrintTask::handle --> bitset_ reset count = " << FocusCache::instance().bitset_.size() - FocusCache::instance().bitset_.count()
      << ",multi_index size = " << FocusCache::instance().container_.size());
}
