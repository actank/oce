#include "AdCache.h"

#include "QueryRunner.h"
#include "BuddyByIdReplicaAdapter.h"
#include "UserCountAdapter.h"
#include "FeedMemcProxy/client/ad_hot_message_client.h"

using namespace xce::ad;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace xce::buddy::adapter;
using namespace xce::usercount::adapter;

static const string db_instance = "ad_db";

const int kMaxHomeHotCount = 2;
const int kMaxWeeklyCount = 2;

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

void AdReceivedCache::Add(int uid, int ad_id) {
  IceUtil::RWRecMutex::WLock lock(received_mutex_);
  received_users_[ad_id].insert(uid);
}

void AdReceivedCache::Cancel(int uid, int ad_id) {
  IceUtil::RWRecMutex::WLock lock(received_mutex_);
  received_users_[ad_id].erase(uid);
}

bool AdReceivedCache::HasReceived(int uid, int ad_id) const {
  IceUtil::RWRecMutex::RLock lock(received_mutex_);
  map<int, set<int> >::const_iterator i = received_users_.find(ad_id);
  if (i == received_users_.end()) {
    return false;
  }
  return i->second.find(uid) != i->second.end();
}

void AdBlockedCache::Load(int ad_id) {
  Statement sql;
  mysqlpp::StoreQueryResult res;
  try {
    sql << "select uid from ad_hot_blocked where promotion_id = " << ad_id;
    res = QueryRunner(db_instance, CDbRServer, "ad_hot_blocked").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("ad " << ad_id << " load ad_hot_blocked err : " << e);
    return;
  } catch (std::exception& e) {
    MCE_WARN("ad " << ad_id << " load ad_hot_blocked err : " << e.what());
    return;
  }
  if (!res || res.empty()) {
    MCE_INFO("ad " << ad_id << " ad_hot_blocked empty");
    return;
  }
  set<int> blocked_users;
  for (size_t i = 0; i < res.size(); ++i) {
    blocked_users.insert(res.at(i)["uid"]);
  }

  MCE_INFO("ad " << ad_id << " ad_hot_blocked size : " << blocked_users.size());
  {
    IceUtil::RWRecMutex::WLock lock(blocked_mutex_);
    blocked_users_[ad_id].swap(blocked_users);
  }
}

void AdBlockedCache::Unblock(int uid, int ad_id) {
  IceUtil::RWRecMutex::WLock lock(blocked_mutex_);
  blocked_users_[ad_id].erase(uid);
}

void AdBlockedCache::Block(int uid, int ad_id) {
  IceUtil::RWRecMutex::WLock lock(blocked_mutex_);
  blocked_users_[ad_id].insert(uid);
}

bool AdBlockedCache::IsBlocked(int uid, int ad_id) const {
  IceUtil::RWRecMutex::RLock lock(blocked_mutex_);
  map<int, set<int> >::const_iterator i = blocked_users_.find(ad_id);
  if (i == blocked_users_.end()) {
    return false;
  }
  return i->second.find(uid) != i->second.end();
}

void ReloadTask::handle() {
  MCE_INFO("ReloadTask::handle()");
  CacheManager::instance().set_state(CacheManager::LOADING);
  CacheManager::instance().Reload();
  CacheManager::instance().set_state(CacheManager::READY);
}

void AdUnreadCache::MarkRead(int uid, int ad_id) {
  {
    IceUtil::RWRecMutex::WLock lock(unread_mutex_);
    set<int> & s = unread_users_[ad_id];
    set<int>::iterator it = s.find(uid);
    if (it == s.end()) {
      return;
    }
    s.erase(it);
  }
  int count = GetUnreadCount(uid);
  if (count < kMaxHomeHotCount) {
    UpdateHomeHotCount(uid, count);
  }
}

void AdUnreadCache::AddUnread(int uid, int ad_id) {
  {
    IceUtil::RWRecMutex::WLock lock(unread_mutex_);
    set<int> & s = unread_users_[ad_id];
    if (s.find(uid) != s.end()) {
      return;
    }
    s.insert(uid);
  }

  int count = GetUnreadCount(uid);
  if (count <= kMaxHomeHotCount) {
    UpdateHomeHotCount(uid, count);
  }
}

void AdUnreadCache::RemoveAd(int ad_id) {
  {
    IceUtil::RWRecMutex::RLock lock(unread_mutex_);
    map<int, set<int> >::const_iterator i = unread_users_.find(ad_id);
    if (i == unread_users_.end()) {
      MCE_INFO("ad " << ad_id << " no unread user.");
      return;
    }
    int total_unread = i->second.size();
    MCE_INFO("ad " << ad_id << " total unread users " << total_unread);
    int erased_count = 0;
    set<int>::const_iterator j = i->second.begin();
    for (; j != i->second.end(); ++j) {
      int uid = *j;
      // 将 AdReceivedCache 清空, 未读就认为未发送
      AdReceivedCache::instance().Cancel(uid, ad_id);
      UpdateHomeHotCount(uid, GetUnreadCountNoLock(uid) - 1);
      AdHotMessageClient::instance().Set(uid, ad_id, "");
      // 将 ReceivedCountCache 清空, 未读就认为未发送
      ReceivedCountCache::instance().DecWeeklyCount(uid);
      MCE_INFO("ad " << ad_id << " remove unread user " << uid << " - " << ++erased_count << "/" << total_unread);
    }
  }

  {
    set<int> empty;
    IceUtil::RWRecMutex::WLock lock(unread_mutex_);
    unread_users_.erase(ad_id);
    MCE_INFO("ad " << ad_id << " all unread user erased.");
  }
}

int AdUnreadCache::GetUnreadCountNoLock(int uid) {
  int count = 0;
  map<int, set<int> >::const_iterator i = unread_users_.begin();
  for (; i != unread_users_.end(); ++i) {
    if (i->second.find(uid) != i->second.end()) {
      ++count;
    }
  }
  return count;
}

int AdUnreadCache::GetUnreadCount(int uid) {
  IceUtil::RWRecMutex::RLock lock(unread_mutex_);
  return GetUnreadCountNoLock(uid);
}

void AdUnreadCache::UpdateHomeHotCount(int uid, int count) {
  // if (count > 0) {
  //   return;
  // }

  try {
    UserCountAdapter::instance().set(uid, CADNOTIFY, count);
  } catch (Ice::Exception& e) {
    MCE_WARN("UserCountAdapter.set() error " << e);
  } catch (std::exception& e) {
    MCE_WARN("UserCountAdapter.set() error " << e.what());
  }
  MCE_INFO("uid " << uid << " set home hot count = " << count);
}

bool CacheManager::Init() {
  MCE_WARN("CacheManager::Init()");
  if (state_ != READY) {
    MCE_WARN("please reload later");
    return false;
  }
  state_ = LOADING;
  TaskManager::instance().execute(new ReloadTask());
  return true;
}

template<class ItemType>
static string SerializeBuddies(const vector<ItemType> & ids) {
  string res;
  for(size_t i = 0; i< ids.size(); ++i) {
    res.append((const char*)&(ids[i]), sizeof(ItemType));
  }
  return res;
}

void CacheManager::Login(int uid) {
  if (state_ != READY) {
    MCE_INFO("login. uid " << uid << " : system is not ready " << state_);
    return;
  }

  int weekly_left = kMaxWeeklyCount - ReceivedCountCache::instance().GetWeeklyCount(uid);
  if (weekly_left <= 0) {
    MCE_INFO("login. uid " << uid << " : reached weekly limit " << kMaxWeeklyCount);
    return;
  }

  int unread_left = kMaxHomeHotCount - AdUnreadCache::instance().GetUnreadCount(uid);
  if (unread_left <= 0) {
    MCE_INFO("login. uid " << uid << " : reached max unread count " << kMaxHomeHotCount);
    return;
  }

  int quota_left = min(weekly_left, unread_left); // 剩余的配额数
  map<int, pair<int, vector<int> > > ad_info;     // 保存ad，level，参加的好友
  GetToPostAds(uid, quota_left, &ad_info);

  if (ad_info.empty()) {
    MCE_INFO("login. uid " << uid << " : receive no message");
  }
  
  // 推送 message
  for (map<int, pair<int, vector<int> > >::iterator mit = ad_info.begin(); mit != ad_info.end(); ++mit) {
    string message = SerializeBuddies(mit->second.second);
    if (message.empty()) {
      MCE_WARN("login. uid " << uid << " : empty message ad=" << mit->first);
      continue;
    }
    if (!AdHotMessageClient::instance().Set(uid, mit->first, message)) {
      MCE_WARN("login. uid " << uid << " : AdHotMessageClient::Set() error ad=" << mit->first);
      continue;
    }
    MCE_INFO("login. uid " << uid << " : receiving message ad=" << mit->first);
    AdReceivedCache::instance().Add(uid, mit->first); 
    AdUnreadCache::instance().AddUnread(uid, mit->first); 
    ReceivedCountCache::instance().IncWeeklyCount(uid); 
  }
}

void CacheManager::DelAd(int ad_id) {
  if (state_ != READY) {
    MCE_INFO("please remove ad " << ad_id << " later.");
    return;
  }
  state_ = REMOVING;
  int ret = 0;
  {
    IceUtil::RWRecMutex::WLock lock(ad_config_mutex_);
    ret = ad_config_map_.erase(ad_id);
  }
  if (ret == 1) {
    MCE_INFO("remove ad " << ad_id << " in memory");
    AdUnreadCache::instance().RemoveAd(ad_id);
  } else {
    MCE_INFO("remove ad " << ad_id << ". not in memory!");
  }

  state_ = READY;
}

AdConfigPtr CacheManager::LoadOneAd(int ad) {
  {
    IceUtil::RWRecMutex::RLock lock(ad_config_mutex_);
    if (ad_config_map_.find(ad) != ad_config_map_.end()) {
      MCE_WARN("ad " << ad << " already exist in memory.");
    }
  }

  // 先load该广告的block列表
  AdBlockedCache::instance().Load(ad);

  Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "select level, start_time, end_time, UNIX_TIMESTAMP(start_time) as start, UNIX_TIMESTAMP(end_time) "\
        " as end, min_partners, max_daily_post from ad_hot_promotion where id = "
      << ad << " and start_time < now() and end_time > now()";
  try {
    res = QueryRunner(db_instance, CDbRServer, "ad_hot_promotion").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("load table ad_hot_promotion : " << e);
  } catch (std::exception& e) {
    MCE_WARN("load table ad_hot_promotion : " << e.what());
  }

  if (!res) {
    MCE_WARN("Load ad " << ad << " from table ad_hot_promotion error");
    return NULL;
  }
  if (res.empty()) {
    MCE_WARN("ad " << ad << " doesn't exist in ad_hot_promotion or has expired");
    return NULL;
  }

  AdConfigPtr config = new AdConfig(ad);
  config->level_ = (int) res.at(0)["level"];
  config->start_time_ = (time_t) res.at(0)["start"];
  config->end_time_ = (time_t) res.at(0)["end"];
  config->min_partners_ = (int) res.at(0)["min_partners"];
  config->max_daily_post_ = (int) res.at(0)["max_daily_post"];
  string start_time = res.at(0)["start_time"].data();
  string end_time = res.at(0)["end_time"].data();

  sql.clear();
  res.clear();
  sql << "select uid from ad_hot_partner where promotion_id = " << ad;
  try {
    res = QueryRunner(db_instance, CDbWServer, "ad_hot_partner").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("load ad_hot_partner error : " << e);
  } catch (std::exception& e) {
    MCE_WARN("load ad_hot_partner error : " << e.what());
  }

  if (res) {
    for (size_t i = 0; i < res.size(); ++i) {
      config->partners_.insert(res.at(i)["uid"]);
    }
  }
  MCE_INFO("promotion " << ad << " load success, parnters=" << config->partners_.size());
  return config;
}

void CacheManager::Reload() {
  Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "select id from ad_hot_promotion where start_time < now() and end_time > now()"; //只load没过期的
  try {
    res = QueryRunner(db_instance, CDbRServer, "ad_hot_promotion").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("load ad_hot_promotion error : " << e);
    return;
  } catch (std::exception& e) {
    MCE_WARN("load ad_hot_promotion error : " << e.what());
    return;
  }

  if (!res) {
    MCE_WARN("load table ad_hot_promotion res NULL.");
    return;
  }

  map<int, AdConfigPtr> ad_config_map; // 广告信息
  for (size_t i = 0; i < res.size(); ++i) {
    AdConfigPtr config = LoadOneAd(res.at(i)["id"]);
    if (config) {
      ad_config_map.insert(make_pair(config->id_, config));
    }
  }

  MCE_INFO("total loaded ad count : " << ad_config_map.size());
  {
    IceUtil::RWRecMutex::WLock lock(ad_config_mutex_);
    ad_config_map_.swap(ad_config_map);
  }
}

void CacheManager::RemoveExpired() {
  if (state_ != READY) {
    MCE_WARN("please RemoveExpired() later");
    return;
  }

  int del_ad = -1;

  {
    IceUtil::RWRecMutex::RLock lock(ad_config_mutex_);
    map<int, AdConfigPtr>::iterator it = ad_config_map_.begin(); 
    while(it != ad_config_map_.end()) {
      if (it->second->Expired()) {
        MCE_INFO("ad " << it->first << " has expired");
        del_ad = it->first;
        break; // 如果有其他的需要下线，交给下次扫描
      } else {
        ++it;
      }
    }
  }

  if (del_ad >= 0) {
    DelAd(del_ad);
  }
}

void CacheManager::GetToPostAds(int uid, int quota_left, map<int, pair<int, vector<int> > > * to_post_ads) {
  IceUtil::RWRecMutex::RLock lock(ad_config_mutex_);
  vector<Ice::Int> buddies;

  for (map<int, AdConfigPtr>::iterator mit = ad_config_map_.begin(); mit != ad_config_map_.end(); ++mit) {
    if (to_post_ads->size() > quota_left) {
      return;
    }
    int ad_id = mit->first;
    AdConfigPtr & ad_config = mit->second;
    // 是活动参与者
    if (ad_config->partners_.find(uid) != ad_config->partners_.end()) {
      MCE_DEBUG("uid " << uid << " has participated ad " << ad_id);
      continue;
    }

    // 已经block这个活动
    if (AdBlockedCache::instance().IsBlocked(uid, ad_id)) {
      MCE_DEBUG("uid " << uid << " has blocked ad " << ad_id);
      continue;
    }

    // 已经接收过这个活动
    if (AdReceivedCache::instance().HasReceived(uid, ad_id)) {
      MCE_DEBUG("uid " << uid << " has received ad " << ad_id << " earlier");
      continue;
    }

    if (buddies.empty()) {
      try {
        buddies = BuddyByIdCacheAdapter::instance().getFriendListAsc(uid, 5000);
      } catch (Ice::Exception& e) {
        MCE_WARN("BuddyByIdCacheAdapter.getFriendListAsc() error : " << e);
      } catch (std::exception& e) {
        MCE_WARN("BuddyByIdCacheAdapter.getFriendListAsc() error : " << e.what());
      }
    }
    MCE_DEBUG("uid " << uid << " has " << buddies.size() << " friends");
    if (buddies.empty()) {
      return;
    }

    // 查找参加过该活动的好友
    vector<Ice::Int> active_buddies;
    for (vector<int>::iterator sit = buddies.begin(); sit != buddies.end(); ++sit) {
      if (ad_config->partners_.find(*sit) != ad_config->partners_.end()) {
        active_buddies.push_back(*sit);
        if (active_buddies.size() >= 40) {
          break;
        }
      }
    }

    MCE_DEBUG("uid " << uid << " has " << active_buddies.size() << " friends participated ad " << ad_id);

    // 好友中没有足够多参加该活动的
    if (active_buddies.size() < ad_config->min_partners_) {
      MCE_DEBUG("uid " << uid << " ad " << ad_id << " buddies " << active_buddies.size() << "/" << ad_config->min_partners_);
      continue;
    }

    if (ad_config->UnderDailyLimit()) {
      ++ad_config->daily_posted_;  // 更新日推送计数
      to_post_ads->insert(make_pair(ad_id, make_pair(ad_config->level_, active_buddies)));
    }
  }
}

void CacheManager::MarkReadByUid(int uid) {
  // 从memcached 删除
  {
    IceUtil::RWRecMutex::RLock lock(ad_config_mutex_);
    for (map<int, AdConfigPtr>::iterator mit = ad_config_map_.begin(); mit != ad_config_map_.end(); ++mit) {
      AdHotMessageClient::instance().Set(uid, mit->first, "");
      AdUnreadCache::instance().MarkRead(uid, mit->first);
    }
  }
}

void CacheManager::Block(int uid, int ad) {
  AdBlockedCache::instance().Block(uid, ad);

  Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "insert ignore ad_hot_blocked(uid,promotion_id) values (" << uid
      << "," << ad << ")";
  try {
    res = QueryRunner(db_instance, CDbWServer, "ad_hot_blocked").store(sql);
    MCE_INFO("ad_hot_blocked insert uid=" << uid << " ad=" << ad);
  } catch (std::exception& e) {
    MCE_INFO("ad_hot_blocked insert error. uid=" << uid << " ad=" << ad << " " << e.what());
  }
}
/*
void CacheManager::Unblock(int uid, int ad) {
  return;
  AdBlockedCache::instance().Unblock(uid, ad);

  Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "delete from ad_hot_blocked WHERER uid = " << uid << " AND promotion_id = " << ad;
  try {
    res = QueryRunner(db_instance, CDbWServer, "ad_hot_blocked").store(sql);
    MCE_INFO("ad_hot_blocked insert uid=" << uid << " ad=" << ad);
  } catch (std::exception& e) {
    MCE_INFO("ad_hot_blocked insert error. uid=" << uid << " ad=" << ad << " " << e.what());
  }
}
*/

bool CacheManager::ResetUserAd(Ice::Int uid, Ice::Int ad_id) {
  MCE_INFO("ResetUserAd() uid=" << uid << " ad=" << ad_id);
  ReceivedCountCache::instance().SetWeeklyCount(uid, 0);

  AdConfigPtr ad_config;
  {
    IceUtil::RWRecMutex::RLock lock(ad_config_mutex_);
    map<int, AdConfigPtr>::iterator mit = ad_config_map_.find(ad_id);
    if (mit != ad_config_map_.end()) {
      ad_config = mit->second;
    }
  }

  if (ad_config) {
    IceUtil::RWRecMutex::WLock lock(ad_config_mutex_);
    ad_config->partners_.erase(uid);
  }

  AdUnreadCache::instance().MarkRead(uid, ad_id);
  AdBlockedCache::instance().Unblock(uid, ad_id);
  AdReceivedCache::instance().Cancel(uid, ad_id);
  return false;
}

UserHotAdState CacheManager::GetUserAdState(Ice::Int uid, Ice::Int ad_id) {
  MCE_INFO("GetUserAdState() uid=" << uid << " ad=" << ad_id);
  UserHotAdState user_state;
  user_state.uid = uid;
  user_state.adId = ad_id;

  user_state.weeklyReceived = ReceivedCountCache::instance().GetWeeklyCount(uid);
  user_state.unreadCount = AdUnreadCache::instance().GetUnreadCount(uid);

  vector<Ice::Int> buddies;
  try {
    buddies = BuddyByIdCacheAdapter::instance().getFriendListAsc(uid, 5000);
  } catch (Ice::Exception& e) {
    MCE_WARN("BuddyByIdCacheAdapter.getFriendListAsc() error : " << e);
  } catch (std::exception& e) {
    MCE_WARN("BuddyByIdCacheAdapter.getFriendListAsc() error : " << e.what());
  }
  user_state.totalBuddies = buddies.size();
  user_state.partnerBuddies = -1;

  AdConfigPtr ad_config;
  {
    IceUtil::RWRecMutex::RLock lock(ad_config_mutex_);
    map<int, AdConfigPtr>::iterator mit = ad_config_map_.find(ad_id);
    if (mit != ad_config_map_.end()) {
      ad_config = mit->second;
    }
  }

  if (ad_config) {
    vector<Ice::Int> partner_buddies;
    for (vector<int>::iterator sit = buddies.begin(); sit != buddies.end(); ++sit) {
      if (ad_config->partners_.find(*sit) != ad_config->partners_.end()) {
        partner_buddies.push_back(*sit);
        if (partner_buddies.size() >= 40) {
          break;
        }
      }
    }
    user_state.partnerBuddies = partner_buddies.size();
    user_state.isPartner = ad_config->partners_.find(uid) != ad_config->partners_.end();
  }

  user_state.hasBlocked = AdBlockedCache::instance().IsBlocked(uid, ad_id);
  user_state.hasReceived = AdReceivedCache::instance().HasReceived(uid, ad_id);

  return user_state;
}

void CacheManager::ClearDailyPosted() {
  IceUtil::RWRecMutex::WLock lock(ad_config_mutex_);
  for (map<int, AdConfigPtr>::iterator it = ad_config_map_.begin(); it
        != ad_config_map_.end(); ++it) {
    it->second->daily_posted_ = 0;
  }
}

HotAdInfoSeq CacheManager::GetAdInfoSeq() {
  HotAdInfoSeq seq;
  {
    IceUtil::RWRecMutex::RLock lock(ad_config_mutex_);
    seq.reserve(ad_config_map_.size());
    map<int, AdConfigPtr>::const_iterator it = ad_config_map_.begin();
    for(; it != ad_config_map_.end(); ++it) {
      HotAdInfo ai;
      const AdConfigPtr & config = it->second;
      ai.ad = it->first;
      ai.state = kAsActive;
      ai.level = config->level_;
      ai.begintime = config->start_time_;
      ai.endtime = config->end_time_;
      ai.minPartners = config->min_partners_;
      ai.maxDailyPost = config->max_daily_post_;
      ai.dailyPosted = config->daily_posted_;
      ai.partnerCount = config->partners_.size();

      seq.push_back(ai);
    }
  }

  MCE_INFO("GetAdInfoSeq() size=" << seq.size());
  return seq;
}

// 定时扫描是否有已经过期的推广
void ScanTask::handle() {
  MCE_INFO("ScanTask::handle()");
  CacheManager::instance().RemoveExpired();
}

// 每天清空当天的 ad 日发送量记录
void ClearAdDailyPostedNumTask::handle() {
  MCE_INFO("ClearAdDailyPostedNumTask::handle()");
  this->_delay = 24 * 60 * 60 * 1000;
  TaskManager::instance().schedule(this);
  CacheManager::instance().ClearDailyPosted();

  // TODO : 将前一日配额用完被停发的广告重新激活
}

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

//-------------------------------------------------------------------------------------------
static bool IsNewWeek(time_t unixtime, time_t interval) {
  // 如果是周一的第一次检查，则是新的一周
  if (unixtime % 24 * 60 * 60 > interval) {
    return false;
  }

  struct tm unixtime_tm;
  localtime_r(&unixtime, &unixtime_tm);

  return unixtime_tm.tm_wday == 1;
}

void ClearUserWeeklyReceivedTask::handle() {
  MCE_INFO("ClearUserWeeklyReceivedTask::handle()");
  this->_delay = 5 * 60 * 1000;  //五分钟
 
  if (IsNewWeek(time(NULL), this->_delay / 1000)) {
    ReceivedCountCache::instance().ClearWeeklyCount();
    MCE_INFO("one week pass, clear user weekly received count");
  }

  TaskManager::instance().schedule(this);
}

