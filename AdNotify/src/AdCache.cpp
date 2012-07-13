/*
 * AdCache.cpp
 *
 *  Created on: Jun 25, 2010
 *      Author: yejingwei
 */

#include "AdCache.h"
#include "Allocator.h"
#include <stdio.h>
#include <set>
#include <vector>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "Singleton.h"
#include <algorithm>
#include <QueryRunner.h>
#include <DbDescriptor.h>
#include "UserCacheAdapter.h"
#include "UserCountAdapter.h"
#include "BuddyByIdReplicaAdapter.h"
#include "LogWrapper.h"

using namespace xce::ad;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace xce::buddy::adapter;
using namespace xce::usercache;
using namespace com::xiaonei::platform::storage;
using namespace xce::usercount::adapter;

map<int, AdConfigPtr> AdCache::ad_config_;
IceUtil::RWRecMutex AdCache::ad_config_mutex_;
bitset<kBitSetSize> AdCache::bitset_interval_;
bool AdCache::preload_done_flag_;

map<int, int> AdCache::ad_posted_num_;
IceUtil::RWRecMutex AdCache::ad_posted_num_mutex_;
set<int> AdCache::ad_reach_max_post_;//达到日最高发送量的ad

static string db_instance = "ad_db";

//判断输入时间所在的周和现在的周是否相同
static bool NewWeek(time_t & update_count_time_) {
  time_t now;
  char buffer1[10], buffer2[10];
  time(&now);
  struct tm now_tm;
  struct tm last_tm;
  localtime_r(&now, &now_tm);
  localtime_r(&update_count_time_, &last_tm);

  //  //--------------------测试用--------------------------------------
  //  int now_tens = now_tm.tm_hour * 6 + now_tm.tm_min / 10; //当天的第几个10分钟
  //  int last_tens = last_tm.tm_hour * 6 + last_tm.tm_min / 10;
  //
  //  if (now_tens != last_tens) {
  //    MCE_DEBUG("NewMonth --> it's new week = " << now_tens << ",last week = " << last_tens);
  //    update_count_time_ = now;
  //    return true;
  //  }
  //  MCE_DEBUG("NewWeek --> it's same week = " << now_tens );
  //  return false;
  //  //--------------------测试用--------------------------------------


  strftime(buffer1, 10, "%W", &now_tm);
  strftime(buffer2, 10, "%W", &last_tm);

  if (strcmp(buffer1, buffer2) != 0) {
    MCE_DEBUG("NewWeek --> it's new week = " << buffer1 << ",last week = " << buffer2);
    update_count_time_ = now;
    return true;
  }
  MCE_DEBUG("NewWeek --> it's same week = " << buffer1 );
  return false;
}

//判断输入时间所在的月和现在的月是否相同
static bool NewMonth(time_t & update_count_time_) {
  time_t now;
  time(&now);
  struct tm now_tm;
  struct tm last_tm;
  localtime_r(&now, &now_tm);
  localtime_r(&update_count_time_, &last_tm);

  //  //--------------------测试用--------------------------------------
  //  int now_tens = now_tm.tm_hour * 4 + now_tm.tm_min / 15; //当天的第几个15分钟
  //  int last_tens = last_tm.tm_hour * 4 + last_tm.tm_min / 15;
  //
  //  if (now_tens != last_tens) {
  //    MCE_DEBUG("NewMonth --> it's new Month = " << now_tens << ",last month = " << last_tens);
  //    update_count_time_ = now;
  //    return true;
  //  }
  //  MCE_DEBUG("NewWeek --> it's same Month = " << now_tens );
  //  return false;
  //  //--------------------测试用--------------------------------------


  int now_month = now_tm.tm_mon;
  int last_month = last_tm.tm_mon;
  if (last_month != last_month) {
    MCE_DEBUG("NewMonth --> it's new Month = " << now_month << ",last month = " << last_month);
    update_count_time_ = now;
    return true;
  }
  MCE_DEBUG("NewWeek --> it's same Month = " << now_month );
  return false;
}

template<typename T>
static string PrintHelper(const T & container) { //将容器内容打印成字符串
  ostringstream ios;
  ios << "(";
  bool first_item = true;
  for (typename T::const_iterator itr = container.begin(); itr
      != container.end(); ++itr) {
    if (first_item == true) {
      first_item = false;
    } else {
      ios << ",";
    }
    ios << *itr;
  }
  ios << ")";
  return ios.str();
}
//-------------------------------------------------------------------------------------------
//将db中count的更新时间改成现在
void UserInfo::UpdateDBTime(time_t update_count_time_) {
  Statement sql;
  sql << "update ad_notify_userconfig set update_count_time = FROM_UNIXTIME("
      << update_count_time_ << ") where uid = " << uid_;
  try {
    QueryRunner(db_instance, CDbWServer, "ad_notify_userconfig").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("UserInfo::UpdateDBTime:" << e);
  } catch (std::exception& e) {
    MCE_WARN("UserInfo::UpdateDBTime:" << e.what());
  }
}

void UserInfo::WriteMessages() {
  //获取好友
  vector<int> buddies = BuddyByIdCacheAdapter::instance().getFriendListAsc(uid_, 5000);
  MCE_DEBUG("UserInfo::WriteMessage --> getFriendListAsc , uid = " << uid_ << ",buddies size = " << buddies.size());

  //没有好友
  if (buddies.empty()) {
    return;
  }

  map<int, pair<int, vector<int> > > ad_info;//保存ad，level，参加的好友
  set<int> reach_max_post_ads;
  {
    IceUtil::RWRecMutex::RLock lock(AdCache::ad_config_mutex_);
    for (map<int, AdConfigPtr>::iterator mit = AdCache::ad_config_.begin(); mit
        != AdCache::ad_config_.end(); ++mit) {

      bool test_send = false;
      if (uid_ == 240599893) {
        MCE_INFO("promotion id " << mit->first << " pushed to test uid " << uid_);
        test_send = true;
      }

      //该用户本身就是活动参与者
      if (mit->second->partners.find(uid_) != mit->second->partners.end()) {
        MCE_DEBUG("UserInfo::WriteMessage --> user uid = " << uid_ << ",is already in parters of promotion id = " << mit->first);
        if (!test_send) {
          continue;
        }
      }

      //该用户已经block这个活动
      if (blocked_ads_.find(mit->first) != blocked_ads_.end()) {
        MCE_DEBUG("UserInfo::WriteMessage --> user uid = " << uid_ << ",has blocked promotion id = " << mit->first);
        if (!test_send) {
          continue;
        }
      }

      int ad_id = mit->first;
      int level = mit->second->level_;
      int max_daily_post = mit->second->max_daily_post_;
      //已经接收过这个活动
      //      MCE_DEBUG("UserInfo::WriteMessage --> user uid = " << uid_ << ", has received")
      if (received_ads_.find(ad_id) != received_ads_.end()) {
        MCE_DEBUG("UserInfo::WriteMessage --> user uid = " << uid_ << ", has received promotion id = " << mit->first);
        if (!test_send) {
          continue;
        }
      }

      //查找参加过该活动的好友
      vector<int> activebuddies;//参加该活动的好友

      if (test_send) {
        activebuddies.push_back(245226974);
      }

      for (vector<int>::iterator sit = buddies.begin(); sit != buddies.end(); ++sit) {
        if (mit->second->partners.find(*sit) != mit->second->partners.end()) {
          activebuddies.push_back(*sit);
          if (activebuddies.size() == 40) {
            break;
          }
        }
      }

      MCE_DEBUG("UserInfo::WriteMessage --> user uid = " << uid_ << ",has " << activebuddies.size() 
          << " buddies in parters of promotion id = " << mit->first << ",buddies is: " << PrintHelper(activebuddies));

      //如果好友中有参加该活动的，则要发送
      //((activebuddies.size() >= mit->second->min_partners_) && AdCache::CheckPostedInOneDay(ad_id, max_daily_post, reach_max_post_ads))
      if (test_send || (activebuddies.size() >= mit->second->min_partners_)) {
        ad_info.insert(make_pair(ad_id, make_pair(level, activebuddies)));
      }
    }
  }

  //发送message
  set<int> this_time_ads;//这次发送的ad
  int sql_success_flag = false;
  if (!ad_info.empty()) {
    Statement sql;
    sql << "insert ignore into ad_notify_message (uid,promotion_id,message ,level,recv_time ) values";

    bool first_item = true;//sql中第一项
    for (map<int, pair<int, vector<int> > >::iterator mit = ad_info.begin(); mit
        != ad_info.end(); ++mit) {

      string message = Formatter(mit->second.second);
      if (message.empty()) {
        continue;
      }

      if (first_item == true) {
        first_item = false;
      } else {
        sql << " , ";
      }

      sql << "(" << uid_ << "," << mit->first << ",'" << message << "',"
          << mit->second.first << ",now())";
      this_time_ads.insert(mit->first);
    }

    if (this_time_ads.empty() == false) {
      try {
        QueryRunner(db_instance, CDbWServer, "ad_notify_message").store(sql);
        sql_success_flag = true;
      } catch (Ice::Exception& e) {
        MCE_WARN("UserInfo::WriteMessages" << e);
      } catch (std::exception& e) {
        MCE_WARN("UserInfo::WriteMessages" << e.what());
      }
    }
  }

  if (sql_success_flag == false) { //写db失败
    return;
  }

  AdCache::UpdatePostedNum(ad_info);//更新发送ad的日发送量
  //map<int, AdConfigPtr>::iterator mit = AdCache::ad_config_.begin()
  {
	  IceUtil::RWRecMutex::WLock lock(AdCache::ad_config_mutex_);
    if(!reach_max_post_ads.empty()){
      set<int>::iterator it = reach_max_post_ads.begin();
      for(; it != reach_max_post_ads.end(); ++it){
    	  map<int, AdConfigPtr>::iterator mit = AdCache::ad_config_.find(*it);
    	  if(mit != AdCache::ad_config_.end()){
    		  AdCache::ad_config_.erase(mit);
          MCE_INFO("Id " << mit->first << "reach daily limit, erased.");
    	  }
      }
    }
  }

  MCE_INFO("UserInfo::WriteMessage --> user uid = " << uid_ << ", this_time_ads size = " 
      << this_time_ads.size() << ", new added ads = " << PrintHelper(this_time_ads) 
      << "old unread count = " << unread_ads_.size());
  //unread_count_ += this_time_ads.size();
  received_ads_.insert(this_time_ads.begin(), this_time_ads.end());
  unread_ads_.insert(this_time_ads.begin(), this_time_ads.end());

  // TODO 有问题，需要改
  if (AdCache::bitset_interval_.test(uid_) == false) { // 对于immediate用户，如果这次sql执行失败，则不需要更新usercount服务
    UserCountAdapter::instance().set(uid_, CADNOTIFY, unread_ads_.size());
    MCE_INFO("UserInfo::WriteMessage --> user uid = " << uid_ << ",config is IMMEDIATE , << set UserCount service = " << unread_ads_.size());
    return;
  }

  //根据用户配置的接收时间间隔，来决定是否更新UserCount
  switch (config_) {

  case Week:
    MCE_DEBUG("UserInfo::WriteMessage --> user uid = " << uid_ << " ,config is WEEK");
    //判断是不是本周第一次登录
    MCE_DEBUG(" before update_count_time = " << update_count_time_);
    if (NewWeek(update_count_time_)) {
      MCE_DEBUG(" after update_count_time = " << update_count_time_);
      UpdateDBTime(update_count_time_);
      MCE_INFO("UserInfo::WriteMessage --> user uid = " << uid_ << " ,config is WEEK and it is first login of this week ,unread_count_ is " 
          << unread_ads_.size() );
      UserCountAdapter::instance().set(uid_, CADNOTIFY, unread_ads_.size());
    }
    return;
  case Month:
    MCE_DEBUG("UserInfo::WriteMessage --> user uid = " << uid_ << " ,config is MONTH")
    ;
    //判断是不是本月第一次登录
    MCE_DEBUG(" before update_count_time = " << update_count_time_)
    ;
    if (NewMonth(update_count_time_)) {
      MCE_DEBUG(" after update_count_time = " << update_count_time_);
      UpdateDBTime(update_count_time_);
      MCE_INFO("UserInfo::WriteMessage --> user uid = " << uid_ << ",config is MONTH, and it is first login of this month,unread_count_ is " 
          << unread_ads_.size() );
      UserCountAdapter::instance().set(uid_, CADNOTIFY, unread_ads_.size());
    }
    return;
  default:
    MCE_WARN("UserInfo::WriteMessage --> user uid = " << uid_ << ",config is WRONG");
    return;
  }
}

string UserInfo::Formatter(const vector<int> & buddies) {
  MCE_DEBUG("UserInfo::Formatter --> activebuddies = " << PrintHelper(buddies));

  map<int, UserCachePtr> result = UserCacheAdapter::instance().GetUserCache(
      buddies);
  if (result.empty()) {
    MCE_DEBUG("UserInfo::Formatter --> GetUserCache, result empty");
    string s;
    return s;
  }
  MCE_DEBUG("UserInfo::Formatter --> GetUserCache, result size = " << result.size());

  ostringstream ios;
  for (map<int, UserCachePtr>::iterator mit = result.begin(); mit
      != result.end(); ++mit) {
    string tinyurl = mit->second->tinyurl();
    string fulltinyurl = HeadUrlUtil::instance().getHeadFullUrl(tinyurl);
    string name = mit->second->name();

    if (name.find(" ") != name.npos) { //删除空格
      name.erase(remove(name.begin(), name.end(), ' '), name.end());
    }
    if (name.find("\t") != name.npos) { //删除tab
      name.erase(remove(name.begin(), name.end(), '\t'), name.end());
    }

    //    //将\t替换成空格
    //    for (size_t pos = name.find("\t"); pos != string::npos; pos = name.find(
    //        "\t")) {
    //      MCE_DEBUG("UserInfo::Formatter --> name = " << name << ",pos = " << pos);
    //      name.replace(pos, 1, " ");
    //    }
    ios << mit->first << "\t" << name << "\t" << fulltinyurl << "\t";
  }

  return ios.str();
}

void UserInfo::Block(int ad) {
  blocked_ads_.insert(ad);
  MCE_DEBUG("UserInfo::Block --> uid = " << uid_ << ", promotion = " << ad << ", block promotion set = " << PrintHelper(blocked_ads_));
}

//-------------------------------------------------------------------------------------------
void PreloadTask::handle() {
  AdCache::preload_done_flag_ = false;
  AdCache::PreloadAds();
  AdCache::preload_done_flag_ = true;
}

void LoadOneAdTask::handle() {
  AdCache::preload_done_flag_ = false;//TODO
  AdCache::LoadOneAd(ad_);
  AdCache::preload_done_flag_ = true;
  MCE_DEBUG("LoadOneAdTask::handle --> load one ad success ,  preload_done_flag_ = " <<AdCache::preload_done_flag_);
}
//-------------------------------------------------------------------------------------------

void AdCache::Init() {
  bitset_interval_.set();
  MCE_DEBUG("CacheManager::Init --> bitset_interval_ size = " << bitset_interval_.count());
  TaskManager::instance().execute(new PreloadTask());
  AdCache::preload_done_flag_ = true;//TODO
}

void AdCache::Login(int uid) {
  MCE_DEBUG("AdCache::Login --> uid = " << uid);
  if (!preload_done_flag_) {
    MCE_DEBUG("AdCache::Login --> uid = " << uid <<",preloading is not completed ,return");
    return;
  }

  UserInfoPtr info = FindCache(uid);

  bool in_cache = true;
  if (!info) { //cache中没有
    in_cache = false;
    info = FindDB(uid);
    if (!info) {
      MCE_WARN("AdCache::Login --> FindDB fail, uid = " << uid);
      return;
    }
    MCE_DEBUG("AdCache::Login --> FindDB success, uid = " << uid);

  }

  if (in_cache == true) {//在内存中找到的，WriteMessages需要加锁
    IceUtil::Mutex::Lock lock(mutex_);
    info->WriteMessages();
  } else {
    info->WriteMessages();//info还未插入cache，WriteMessages不需要加锁
    InsertCache(info);
  }
}

void AdCache::SetInterval(int uid, Interval interval) {
  UserInfoPtr info = FindCache(uid);

  if (info) {
    MCE_DEBUG("AdCache::SetInterval --> userinfo is in cache, uid = " << uid);
    info->config_ = interval;
  }

  Statement sql;
  if (interval == Immediate) {
    bitset_interval_.reset(uid);
    sql << "delete from ad_notify_userconfig where uid = " << uid;
    try {
      QueryRunner(db_instance, CDbWServer, "ad_notify_userconfig").store(sql);
    } catch (Ice::Exception& e) {
      MCE_WARN("delete from ad_notify_userconfig error : " << e);
    } catch (std::exception& e) {
      MCE_WARN("delete from ad_notify_userconfig error : " << e.what());
    }
    return;
  }
  bitset_interval_.set(uid);
  sql.clear();
  int tmp = (interval == Week) ? 1 : 2;

  time_t now;
  time(&now);
  info->update_count_time_ = now;

  sql << "insert into ad_notify_userconfig (uid ,config, update_count_time) values("
      << uid << "," << tmp << ", from_unixtime(" << now
      << ")) on duplicate key  update config = values(config),update_count_time = values(update_count_time)";

  try {
    QueryRunner(db_instance, CDbWServer, "ad_notify_userconfig").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("insert into ad_notify_userconfig error : " << e);
  } catch (std::exception& e) {
    MCE_WARN("insert into ad_notify_userconfig error : " << e.what());
  }
}

//load一条推广的信息到内存，包括设置和所有的partner,static 函数
void AdCache::LoadOneAd(int ad) {
  {
    IceUtil::RWRecMutex::RLock lock(ad_config_mutex_);
    if (ad_config_.find(ad) != ad_config_.end()) {
      MCE_WARN("promotion id " << ad << " already exist in memory.");
      return;
    }
  }

  Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "select level, block_always,start_time, end_time, UNIX_TIMESTAMP(start_time) as start, UNIX_TIMESTAMP(end_time) "\
        " as end, min_partners, max_daily_post from ad_notify_promotion where promotion_id = "
      << ad << " and start_time < now() and end_time > now()";
  try {
    res = QueryRunner(db_instance, CDbRServer, "ad_notify_promotion").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdCache::LoadOneAd" << e);
  } catch (std::exception& e) {
    MCE_WARN("AdCache::LoadOneAd" << e.what());
  }

  if (!res) {
    MCE_WARN("Load promotion from talbe ad_notify_promotion error");
    return;
  }
  if (res.empty()) {
    MCE_WARN("AdCache::LoadOneAd --> promotion_id " << ad << " doesn't exist in ad_notify_promotion or has expired");
    return;
  }

  AdConfigPtr config = new AdConfig();
  config->level_ = (int) res.at(0)["level"];
  config->block_always_ = (bool) res.at(0)["block_always"];
  config->start_time_ = (time_t) res.at(0)["start"];
  config->end_time_ = (time_t) res.at(0)["end"];
  config->min_partners_ = (int) res.at(0)["min_partners"];
  config->max_daily_post_ = (int) res.at(0)["max_daily_post"];
  string start_str = res.at(0)["start_time"].data();
  string end_str = res.at(0)["end_time"].data();

  sql.clear();
  res.clear();
  sql << "select partner from ad_notify_partner where promotion_id = " << ad;
  try {
    res = QueryRunner(db_instance, CDbWServer, "ad_notify_partner").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdCache::LoadOneAd" << e);
  } catch (std::exception& e) {
    MCE_WARN("AdCache::LoadOneAd" << e.what());
  }
  if (!res) {
    MCE_WARN("Load promotion " << ad << " partener from talbe ad_notify_partner error");
    return;
  }
  if (res.empty()) {
    MCE_WARN("Promotion " << ad << " partener from talbe ad_notify_partner empty!");
    return;
  }

  for (size_t i = 0; i < res.size(); ++i) {
    config->partners.insert(res.at(i)["partner"]);
  }

  {
    IceUtil::RWRecMutex::WLock lock(ad_config_mutex_);
    ad_config_.insert(make_pair(ad, config));
  }

  MCE_INFO("AdCache::LoadOneAd --> load promotion " << ad << " success, parnter size = " << res.size() 
      << ", start_time = " << start_str << ", end_time = " << end_str 
      << ", block_always_ = " << config->block_always_);
}

void AdCache::PreloadAds() {
  Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "select promotion_id from ad_notify_promotion where start_time < now() and end_time > now()"; //只load没过期的
  try {
    res = QueryRunner(db_instance, CDbRServer, "ad_notify_promotion").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdCache::PreloadAds" << e);
  } catch (std::exception& e) {
    MCE_WARN("AdCache::PreloadAds" << e.what());
  }

  if (!res) {
    MCE_WARN("AdCache::PreloadAds select ad_notify_promotion error.");
    return;
  }
  if (res.empty()) {
    MCE_WARN("AdCache::PreloadAds select ad_notify_promotion empty.");
    return;
  }
  set<int> ads;
  for (size_t i = 0; i < res.size(); ++i) {
    ads.insert(res.at(i)["promotion_id"]);
    LoadOneAd(res.at(i)["promotion_id"]);
  }
  MCE_INFO("AdCache::PreloadAds --> preload promotion, promotions' ids = " << PrintHelper(ads));
}

bool AdCache::CheckPostedInOneDay(int ad, int max_post, set<int>& ads_set) {
  // 0 表示不限量
  if (max_post <= 0) {
    return true;
  }

  IceUtil::RWRecMutex::RLock lock(AdCache::ad_posted_num_mutex_);
  map<int, int>::const_iterator ite = AdCache::ad_posted_num_.find(ad);
  if(ite != AdCache::ad_posted_num_.end()) {
    bool isMax =  ite->second < max_post;
    if(!isMax){
    	AdCache::ad_reach_max_post_.insert(ad);
      ads_set.insert(ad);
    }
    return isMax;
  }
  return true;

}

void AdCache::UpdatePostedNum(map<int, pair<int, vector<int> > >& ad_infos) {
  IceUtil::RWRecMutex::WLock lock(AdCache::ad_posted_num_mutex_);
  map<int, int>::iterator ite_posted;
  if(!ad_infos.empty()){
    map<int, pair<int, vector<int> > >::iterator ite_info;
    for(ite_info = ad_infos.begin(); ite_info != ad_infos.end(); ++ite_info) {
      ite_posted = AdCache::ad_posted_num_.find(ite_info->first);
      if(ite_posted != AdCache::ad_posted_num_.end()) {
        ite_posted->second++;
      } else {
        ad_posted_num_[ite_info->first] = 1;
      }
    }
  }

}

void AdCache::ClearAdDailyPostedNum() {
  IceUtil::RWRecMutex::WLock lock(AdCache::ad_posted_num_mutex_);
  AdCache::ad_posted_num_.clear();
  MCE_DEBUG("AdCache::ClearAdDailyPostedNum, to clear ad posted num");
}

void AdCache::GetReachMaxPostAd(set<int> & ad_set){
	ad_set = AdCache::ad_reach_max_post_;
	AdCache::ad_reach_max_post_.clear();
}

void AdCache::Block(int uid, int ad) {
  UserInfoPtr info = FindCache(uid);
  if (!info) {
    MCE_DEBUG("AdCache::Block --> user not in cache , uid = " << uid );
  } else {
    info->Block(ad);
  }

  Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "insert ignore ad_notify_blocked (uid,promotion_id) values (" << uid
      << "," << ad << ")";
  try {
    res = QueryRunner(db_instance, CDbWServer, "ad_notify_blocked").store(sql);
    MCE_INFO("AdCache::Block --> user uid = " << uid << ",block ad = " << ad);
  } catch (Ice::Exception& e) {
    MCE_WARN("UserInfo::Block" << e);
  } catch (std::exception& e) {
    MCE_WARN("UserInfo::Block" << e.what());
  }

}

void AdCache::InsertCache(UserInfoPtr info) {

  MCE_DEBUG("AdCache::InsertCache --> uid = " << info->uid_);
  IceUtil::Mutex::Lock lock(mutex_);
  UserInfoPtr tmp = info;
  container_.push_front(info);
  info = tmp;
  Kick();

}

void AdCache::Kick() { //没有加锁
  while (container_.size() > kCacheSize) {
    MCE_DEBUG("AdCache::Kick --> Kicking uid = " << (*(container_.rbegin()))->uid_);
    container_.pop_back();
  }
  MCE_DEBUG("AdCache::kick --> after kick ,size = " << container_.size());
}

UserInfoPtr AdCache::FindCache(int uid) {
  MCE_DEBUG("AdCache::FindCache --> uid " << uid );
  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex & index = container_.get<1> ();
  UidIndex::iterator uit = index.find(uid);
  if (uit != index.end()) { //在cache中
    MCE_DEBUG("AdCache::FindCache --> user is in cache ,uid = " << uid );
    SeqIndex::iterator sit = container_.project<0> (uit);//新查的放到前面
    container_.relocate(container_.get<0> ().begin(), sit);
    return *uit;
  } else {
    MCE_DEBUG("AdCache::FindCache --> user is not in cache ,uid = " << uid );
    return 0;
  }
}

UserInfoPtr AdCache::FindDB(int uid) {
  MCE_DEBUG("AdCache::FindDB --> uid = " << uid );
  UserInfoPtr info = new UserInfo(uid);
  //load已经接收的ad
  Statement sql;
  sql << "select * from ad_notify_message where uid = " << uid;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(db_instance, CDbRServer, "ad_notify_message").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdCache::FindDB:" << e);
    return 0;
  } catch (std::exception& e) {
    MCE_WARN("AdCache::FindDB:" << e.what());
    return 0;
  }
  if (!res) {
    MCE_WARN("AdCache::FindDB --> ad_notify_message --> StoryQueryResult wrong --> uid = " << uid);
    return 0;
  } else {
    MCE_DEBUG("AdCache::FindDB --> uid = " << uid << ", received messages , size = " << res.size());
    for (size_t i = 0; i < res.size(); ++i) {
      info->received_ads_.insert(res.at(i)["promotion_id"]);
      if ((bool) res.at(i)["is_read"] == false) {
        info->unread_ads_.insert(res.at(i)["promotion_id"]);
      }
    }
    MCE_DEBUG("AdCache::FindDB --> uid = " << uid << ", received_ads_ size = " << info->received_ads_.size() 
        << ",unread_ads_ size = " << info->unread_ads_.size());

  }

  //load已经block的ad
  sql.clear();
  res.clear();
  sql << "select * from ad_notify_blocked where uid = " << uid;
  try {
    //    res = QueryRunner(db_instance, CDbRServer, "ad_notify_received").store(sql);
    res = QueryRunner(db_instance, CDbRServer, "ad_notify_blocked").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdCache::FindDB --> ad_notify_blocked " << e);
    return 0;
  } catch (std::exception& e) {
    MCE_WARN("AdCache::FindDB --> ad_notify_blocked " << e.what());
    return 0;
  }
  if (!res) {
    MCE_WARN("AdCache::FindDB --> ad_notify_blocked --> StoryQueryResult wrong --> uid = " << uid);
    return 0;
  } else {
    MCE_DEBUG("AdCache::FindDB --> uid = " << uid << ", block ad , size = " << res.size());
    for (size_t i = 1; i < res.size(); ++i) {
      info->blocked_ads_.insert(res.at(i)["promotion_id"]);
    }
  }

  if (bitset_interval_.test(uid) == false) { //用户是默认设置
    MCE_DEBUG("AdCache::FindDB --> uid = " << uid << ",bitset = 0 ,receive Immediate" );
    info->config_ = Immediate;
    return info;
  }

  //load用户接收时间间隔，和用户上次修改user count时间
  sql.clear();
  res.clear();
  sql
      << "select config , UNIX_TIMESTAMP(update_count_time) as time from ad_notify_userconfig where uid = "
      << uid;
  try {
    res = QueryRunner(db_instance, CDbRServer, "ad_notify_userconfig").store(
        sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdCache::FindDB --> ad_notify_userconfig :" << e);
    return 0;
  } catch (std::exception& e) {
    MCE_WARN("AdCache::FindDB --> ad_notify_userconfig :" << e.what());
    return 0;
  }
  if (!res) {
    MCE_WARN("AdCache::FindDB --> ad_notify_userconfig --> StoryQueryResult wrong --> uid = " << uid);
    return 0;
  } else {
    if (res.empty()) {
      MCE_DEBUG("AdCache::FindDB --> user uid = " << uid << ", does not set interval, set default");
      info->config_ = Immediate;
      bitset_interval_.reset(uid);
      return info;
    } else {
      int i = (int) res.at(0)["config"];
      MCE_DEBUG("AdCache::FindDB --> uid = " << uid << ",int interval  = " << i);
      if (i == 0) {
        info->config_ = Immediate; //应该不会有这种情况
        MCE_DEBUG("AdCache::FindDB --> interval = Immediate" );
      } else if (i == 1) {
        info->config_ = Week;
        MCE_DEBUG("AdCache::FindDB --> interval = Week" );
      } else if (i == 2) {
        info->config_ = Month;
        MCE_DEBUG("AdCache::FindDB --> interval = Month" );
      } else {
        info->config_ = Immediate;
        MCE_DEBUG("AdCache::FindDB --> interval = Immediate, fourth" );
      }
      info->update_count_time_ = res.at(0)["time"];
      return info;
    }
  }

}

void AdCache::MarkReadByUid(int uid) {//用户进入通知页面后，调用这个函数，标记所有的message已读，并且把usercount设为0
  MCE_DEBUG("AdCache::MarkReadByUid --> uid =:" << uid );

  Statement sql;
  sql
      << "update ad_notify_message set is_read = 1 where is_read = 0 and uid = "
      << uid;
  try {
    QueryRunner(db_instance, CDbWServer, "ad_notify_message").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdCache::MarkReadByUid:" << e);
  } catch (std::exception& e) {
    MCE_WARN("AdCache::MarkReadByUid:" << e.what());
  }

  UserInfoPtr info = FindCache(uid);

  if (info) { //cache中有
    IceUtil::Mutex::Lock lock(mutex_);
    MCE_DEBUG("AdCache::MarkReadByUid --> uid =:" << uid << ",is in cache, before reset, his unread message count = " << info->unread_ads_.size());
    //    info->unread_count_ = 0;
    info->unread_ads_.clear();
  }

  MCE_DEBUG("AdCache::MarkReadByUid --> uid =:" << uid << " erase user count");
  UserCountAdapter::instance().set(uid, CADNOTIFY, 0);
}

//non static , 删除userinfo中的指定ad
void AdCache::DelReceivedAdsInUserInfos(const set<int> & ads) {

  IceUtil::Mutex::Lock lock(mutex_);
  SeqIndex & sindex = container_.get<0> ();
  for (SeqIndex::iterator sit = sindex.begin(); sit != sindex.end(); ++sit) {

    for (set<int>::const_iterator itr = ads.begin(); itr != ads.end(); ++itr) {
      if ((*sit)->received_ads_.erase(*itr) == 1) { //received中有的,unread_ads中才可能有
        (*sit)->unread_ads_.erase(*itr);
      }
    }
    //  (*sit)->unread_count_ = (*sit)->unread_ads_.size();
    // MCE_DEBUG("AdCache::DelReceivedAdsInUserInfos --> Finally , unread count = " << (*sit)->unread_ads_.size());
    UserCountAdapter::instance().set((*sit)->uid_, CADNOTIFY,
        (*sit)->unread_ads_.size());
  }
  MCE_DEBUG("AdCache::DelReceivedAdsInUserInfos --> done");

}
//static
void AdCache::DelReceivedAdsInDB(const set<int> & ads) {
  string ads_str = PrintHelper(ads);
  //删除ad_notify_message
  Statement sql;

  sql << "delete from ad_notify_message where promotion_id in " << ads_str;
  //      << " and uid not in (225092473,220678672)";//TODO 测试用,待删除
  try {
    QueryRunner(db_instance, CDbWServer, "ad_notify_message").store(sql);
    MCE_INFO("AdCache::DelReceivedAdsInDB --> delete ads in db ,ads = " << ads_str);
  } catch (Ice::Exception& e) {
    MCE_WARN("ScanTask::handle:" << e);
  } catch (std::exception& e) {
    MCE_WARN("ScanTask::handle:" << e.what());
  }
}

//static 删除ad的配置和参与者，包括cache和db
void AdCache::DelAdConfigs(const set<int> & ads) {
  string ads_str = PrintHelper(ads);
  {
    IceUtil::RWRecMutex::WLock lock(ad_config_mutex_);
    for (set<int>::const_iterator itr = ads.begin(); itr != ads.end(); ++itr) {

      if (ad_config_.erase(*itr) == 1) {
        MCE_DEBUG("AdCache::DelAdConfigs --> found and delete ad in memory ,ad = " << *itr);
      } else {
        MCE_DEBUG("AdCache::DelAdConfigs --> not found ad in memory ,ad = " << *itr);
      }
    }
    MCE_DEBUG("AdCache::DelAdConfigs --> delete ads in memory, done. ads = " << ads_str);
  }

  Statement sql;
  sql << "delete from ad_notify_promotion where promotion_id in " << ads_str;
  try {
    QueryRunner(db_instance, CDbWServer, "ad_notify_promotion").store(sql);
    MCE_DEBUG("AdCache::DelAdConfigs --> delete ads in db ad_notify_promotion " << ads_str);
  } catch (Ice::Exception& e) {
    MCE_WARN("ScanTask::handle:" << e);
  } catch (std::exception& e) {
    MCE_WARN("ScanTask::handle:" << e.what());
  }

  sql.clear();

  sql << "delete from ad_notify_partner where promotion_id in " << ads_str;
  try {
    QueryRunner(db_instance, CDbWServer, "ad_notify_partner").store(sql);
    MCE_DEBUG("AdCache::DelAdConfigs --> delete ads in db ad_notify_partner,ads = " << ads_str);

  } catch (Ice::Exception& e) {
    MCE_WARN("ScanTask::handle:" << e);
  } catch (std::exception& e) {
    MCE_WARN("ScanTask::handle:" << e.what());
  }
}

void AdCache::DelNotBlockAlwaysAdsInDB(const set<int> & ads) {
  string ads_str = PrintHelper(ads);
  Statement sql;

  sql << "delete from ad_notify_blocked where promotion_id in " << ads_str;
  try {
    QueryRunner(db_instance, CDbWServer, "ad_notify_blocked").store(sql);
    MCE_DEBUG("AdCache::DelNotBlockAlwaysAdsInDB --> ads = " << ads_str);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdCache::DelNotBlockAlwaysAdsInDB:" << e);
  } catch (std::exception& e) {
    MCE_WARN("AdCache::DelNotBlockAlwaysAdsInDB:" << e.what());
  }

}

//static
set<int> AdCache::GetNotBlockAlwaysAds(const set<int> & ads) {
  set<int> not_block_always;
  {
    IceUtil::RWRecMutex::RLock lock(ad_config_mutex_);
    for (set<int>::const_iterator sit = ads.begin(); sit != ads.end(); ++sit) {
      map<int, AdConfigPtr>::iterator mit = ad_config_.find(*sit);
      if (mit != ad_config_.end()) {
        if (mit->second->block_always_ == false) {
          not_block_always.insert(*sit);
        }
      }
    }
  }
  MCE_DEBUG("AdCache::GetNotBlockAlwaysAds --> all expired ads = " << PrintHelper(ads) << ",not block always ads = " << PrintHelper(not_block_always));
  return not_block_always;
}

void AdCache::DelNotBlockAlwaysAdsInUserInfos(const set<int> & ads) {
  IceUtil::Mutex::Lock lock(mutex_);

  SeqIndex & sindex = container_.get<0> ();
  for (SeqIndex::iterator sit = sindex.begin(); sit != sindex.end(); ++sit) {
    for (set<int>::const_iterator itr = ads.begin(); itr != ads.end(); ++itr) {
      //      (*sit)->received_ads_.erase(*itr);
      (*sit)->blocked_ads_.erase(*itr);
    }
  }
  MCE_DEBUG("AdCache::DelNotBlockAlwaysAdsInUserInfos --> done");

}

//-------------------------------------------------------------------------------------------


void CacheManager::Init() {
  //  caches_.resize(kCache);
  AdCache::Init();
}

void CacheManager::Login(int uid) {
  MCE_DEBUG("CacheManager::Login --> uid = " << uid);

  caches_[uid % kCache].Login(uid);
}

void CacheManager::DelAds(const set<int> & ads) {

  set<int> not_block_always = AdCache::GetNotBlockAlwaysAds(ads);

  AdCache::DelAdConfigs(ads); // 删除配置，在cache和db中，内部加锁

  //内存中
  for (size_t i = 0; i < kCache; ++i) {
    caches_[i].DelReceivedAdsInUserInfos(ads);
    if (!not_block_always.empty()) {
      caches_[i].DelNotBlockAlwaysAdsInUserInfos(not_block_always);
    }
  }

  //db中
  AdCache::DelReceivedAdsInDB(ads);
  if (!not_block_always.empty()) {
    AdCache::DelNotBlockAlwaysAdsInDB(not_block_always);
  }
}

void CacheManager::LoadOneAd(int ad) {
  AdCache::LoadOneAd(ad);

}
void CacheManager::PreloadAds() {
  AdCache::PreloadAds();
}

void CacheManager::SetInterval(int uid, Interval interval) {
  caches_[uid % kCache].SetInterval(uid, interval);
}
void CacheManager::MarkReadByUid(int uid) {
  caches_[uid % kCache].MarkReadByUid(uid);
}
void CacheManager::Block(int uid, int ad) {
  caches_[uid % kCache].Block(uid, ad);
}

void CacheManager::ClearAdDailyPostedNum() {
  AdCache::ClearAdDailyPostedNum();
}

void CacheManager::GetReachMaxPostAd(set<int> & ad_set) {
  AdCache::GetReachMaxPostAd(ad_set);
}
//-------------------------------------------------------------------------------------------
//定时扫描是否有已经过期的推广
void ScanTask::handle() {

  time_t now;
  time(&now);
  struct tm now_tm;
  localtime_r(&now, &now_tm);
  char buffer[80];
  strftime(buffer, 80, "Now it's %c.", &now_tm);//buffer打印用

  //  if (now_tm.tm_hour < 3 || now_tm.tm_hour > 4) { //只有在凌晨3点到4点之间执行scan
  //    MCE_DEBUG("ScanTask::handle --> Now it's" << buffer << ",don't scan expired promotion");
  //    return;
  //  }

  MCE_DEBUG("ScanTask::handle --> Now it's" << buffer << ",scan expired promotion");

  //TODO 下面这一段可以从内存中读取，而不从db读取
  Statement sql;
  sql << "select * from ad_notify_promotion where end_time < now()";
  mysqlpp::StoreQueryResult res;
  try {
    res
        = QueryRunner(db_instance, CDbRServer, "ad_notify_promotion").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("ScanTask::handle:" << e);
  } catch (std::exception& e) {
    MCE_WARN("ScanTask::handle:" << e.what());
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  if (res.empty()) {
    MCE_INFO("ScanTask::handle --> no promotion expired");
    return;
  }

  set<int> expired_ads;

  for (size_t i = 0; i < res.size(); ++i) {
    int ad = res.at(i)["promotion_id"];
    expired_ads.insert(ad);
  }

  MCE_INFO("ScanTask::handle --> expired promotions = " << PrintHelper(expired_ads));

  CacheManager::instance().DelAds(expired_ads);

}

//-------------------------------------------------------------------------------------------
//每天清空当天的ad发送量记录
void ClearAdDailyPostedNumTask::handle() {

  this->_delay = 24*60*60*1000;
  TaskManager::instance().schedule(this);

  time_t now;
  time(&now);
  struct tm now_tm;
  localtime_r(&now, &now_tm);
  char buffer[80];
  strftime(buffer, 80, "Now it's %c.", &now_tm);//buffer打印用

  CacheManager::instance().ClearAdDailyPostedNum();
  MCE_DEBUG("ClearAdDailyPostedNumTask::handle --> Now it's" << buffer << ", clear ad posted num");

  set<int> ad_set;
  CacheManager::instance().GetReachMaxPostAd(ad_set);
  set<int>::iterator it = ad_set.begin();
  for(; it != ad_set.end(); ++it){
    CacheManager::instance().LoadOneAd(*it);
  }

}

//clock:0-23
int ClearAdDailyPostedNumTask::DelayToXClock(int clock) {
  time_t now;
  time(&now);
  struct tm now_tm;
  localtime_r(&now, &now_tm);
  int now_hour = now_tm.tm_hour;
  int now_min  = now_tm.tm_min;
  int now_sec  = now_tm.tm_sec;
  int between_hours = clock - now_hour;
  if(between_hours < 0) {
	  between_hours += 24;
  }
  int delay = between_hours*60*60 - now_min*60 - now_sec;
  if(delay < 0){
	  delay = delay + 24*60*60;
  }
  return delay * 1000;

}
