#include "UserAdInfo.h"

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include "Allocator.h"
#include "AdConf.h"
#include "QueryRunner.h"
#include "Notify/util/url_encode.h"
#include "FeedMemcProxy/client/ad_hot_message_client.h"

using namespace com::xiaonei::platform::storage;
using namespace com::xiaonei::xce;

namespace xce {
namespace ad {

const static int kClearCacheInterval = 60 * 2;

template<class ItemType>
static vector<ItemType> DeserializeBuddies(const char * src, size_t len) {
  vector<ItemType> v;
  size_t total_size = len / sizeof(ItemType);
  v.reserve(total_size);
  ItemType item;
  for(size_t i = 0; i < total_size; ++i) {
    memcpy((char*)&item, src + i * sizeof(ItemType), sizeof(ItemType));
    v.push_back(item);
  }
  return v;
}

bool UserAdInfo::Load() {
  promotions_.clear();

  set<int> ad_ids = AdConfCache::instance().GetAdIds();
  MCE_DEBUG("ad_ids.size : " << ad_ids.size());

  for(set<int>::iterator it = ad_ids.begin(); it != ad_ids.end(); ++it) {
    Ice::Int ad_id = *it;
    string buddy_stream;
    // TODO : 目前 not found 和失败无法区分
    AdHotMessageClient::instance().Get(uid_, ad_id, &buddy_stream);

    if (buddy_stream.empty()) {
      MCE_DEBUG("no ad : uid=" << uid_ << " ad_id:" << ad_id);
      continue;
    }

    vector<Ice::Int> buddies = DeserializeBuddies<Ice::Int>(buddy_stream.data(), buddy_stream.size());
    MCE_INFO("show ad " << ad_id << " uid=" << uid_ << " buddies.size=" << buddies.size());
    
    int level = 1;
    time_t recv_time = time(NULL);

    PromotionItem ad_item;
    try {
      ad_item.buddies_ = UserCacheAdapter::instance().GetUserCache(buddies);
    } catch (Ice::Exception& e) {
      MCE_WARN("UserCacheAdapter::GetUserCache() error: " << e);
      continue;
    } catch (std::exception& e) {
      MCE_WARN("UserCacheAdapter::GetUserCache() error: " << e.what());
      continue;
    }
    ad_item.id_ = ad_id;
    ad_item.level_ = level;
    ad_item.recv_time_ = recv_time;

    promotions_.insert(ad_item);
  }

  MCE_INFO("uid total promotions : " << promotions_.size() << "  uid:" << uid_);
  
  while(promotions_.size() > 2) {  // 只保留两个用来展示
    promotions_.erase((++promotions_.rbegin()).base());
  }

  return true;
}

void UserAdInfo::FillDict(ctemplate::TemplateDictionary * dict) const {
  int hot_count = 0;
  for (set<PromotionItem>::const_iterator it = promotions_.begin();
      it != promotions_.end(); ++it) {
    if (it->FillItemDict(dict)) {
      MCE_INFO("user " << uid_ <<  " read promotion " << it->id_);
      ++hot_count;
    }
  }
  dict->SetIntValue("hot_info_count", promotions_.size());
}

UserAdInfoPtr AdInfoCache::GetUserAdInfo(Ice::Int uid) {
  {
    IceUtil::RWRecMutex::RLock lock(rw_mutex_);
    map<Ice::Int, UserAdInfoPtr>::const_iterator it = uid_ad_map_.find(uid);
    if (it != uid_ad_map_.end()) {
      it->second->set_pv_time(time(NULL));
      return it->second;
    }
  }

  UserAdInfoPtr p = new UserAdInfo(uid);
  if (p->Load()) {
    IceUtil::RWRecMutex::WLock lock(rw_mutex_);
    uid_ad_map_.insert(make_pair(uid, p));
  }

  return p;
}

void AdInfoCache::ClearUserAdInfo(Ice::Int uid) {
  MCE_INFO("uid " << uid << " erased from memory.");
  IceUtil::RWRecMutex::WLock lock(rw_mutex_);
  uid_ad_map_.erase(uid);
}

bool PromotionItem::FillItemDict(ctemplate::TemplateDictionary * root_dict) const {
  AdConfPtr ad_conf = AdConfCache::instance().Get(id_);
  if (!ad_conf) {
    return false;
  }

  // TODO : 合并到一个tpl文件
  ctemplate::TemplateDictionary * item_dict = root_dict->AddIncludeDictionary("hot_item");
  string path = "../etc/ad_hot/tpl/";
  bool is_new_template = false;

  if (is_new_template) {
    path += "hot_item2.tpl";
  } else {
    path += "hot_item.tpl";
  }
  item_dict->SetFilename(path.c_str());

  string message1 = ad_conf->message1_;
  if(is_new_template) {
    try {
      message1 = boost::regex_replace(message1, boost::regex("</a>"), string("</span>"));
      message1 = boost::regex_replace(message1, boost::regex("<a.*?>"), string("<span style=\"color:#005EAC\">"));
    } catch (std::exception& e) {
      MCE_WARN("AdHotRequest::Expand:boost::regex exception" << e.what());
      message1 = ad_conf->message1_;
    } catch (...) {
      MCE_WARN("AdHotRequest::Expand:boost::unknown exception");
      message1 = ad_conf->message1_;
    }
  }

  item_dict->SetValue("ad_pic_url", ad_conf->pic_url_);
  item_dict->SetValue("ad_pic_src", ad_conf->pic_src_);
  item_dict->SetValue("message1", message1);
  item_dict->SetValue("message2", ad_conf->message2_);
  item_dict->SetValue("join_button", ad_conf->join_button_);
  item_dict->SetValue("join_title", ad_conf->join_title_);
  item_dict->SetValue("not_join_title", ad_conf->not_join_title_);
  item_dict->SetIntValue("hot_id", id_);

  if (!ad_conf->related_posts_.empty()) {
    ctemplate::TemplateDictionary* related_dict = item_dict->AddSectionDictionary("RELATED_LIST");
    for (size_t i = 0; i < ad_conf->related_posts_.size(); ++i) {
      ctemplate::TemplateDictionary* item_dict = related_dict->AddSectionDictionary("RELATED_ITEM");
      item_dict->SetValue("RELATED_TITLE", ad_conf->related_posts_[i].first);
      item_dict->SetValue("RELATED_URL", ad_conf->related_posts_[i].second);
    }
  }
  if (buddies_.size() > 30) {
    item_dict->SetValue("friend_user_count",  "超过30");
  } else {
    item_dict->SetIntValue("friend_user_count",  buddies_.size());
  }

  if (is_new_template) {
    item_dict->SetValue("ad_pic_url_encode", xce::notify::UrlEncode(ad_conf->pic_url_));
  }

  int user_num_per_item  = buddies_.size();
  int display_user_num = 0;
  bool if_user_num_less_default = true;
  bool if_disply_expand_button = true;
  int default_visible_friend_count = (is_new_template ? 6 : 8);
  if (user_num_per_item <= default_visible_friend_count) {
    display_user_num = user_num_per_item;
  } else {
    display_user_num = default_visible_friend_count;
    if_user_num_less_default = false;
  }
  if (user_num_per_item > 0) {
    ctemplate::TemplateDictionary* has_friend_dict = item_dict->AddSectionDictionary("has_friend");
    // map<Ice::Int, UserCachePtr> buddies_;
    int count = 0;
    for(map<Ice::Int, UserCachePtr>::const_iterator it = buddies_.begin(); it != buddies_.end(); ++it) {
      const UserCachePtr & buddy = it->second;

      ctemplate::TemplateDictionary* friend_user_list_dict = has_friend_dict->AddSectionDictionary("friend_user_list");

      char buddy_page[64];
      buddy_page[63] = 0;
      snprintf(buddy_page, 63, "http://www.renren.com/profile.do?id=%u", it->first);

      friend_user_list_dict->SetValue("friend_user_url", buddy_page);
      friend_user_list_dict->SetValue("friend_user_src", HeadUrlUtil::instance().getHeadFullUrl(buddy->tinyurl()));
      friend_user_list_dict->SetValue("friend_user_name", buddy->name());

      if (!if_user_num_less_default && !is_new_template) {
        if (if_disply_expand_button) {
          has_friend_dict->ShowSection("if_expanded");
        } else {
          has_friend_dict->ShowSection("if_unexpanded");
        }
      }
      if (++count >= display_user_num) {
        break;
      }
    }
  }
  return true;
}

void AdInfoCache::ClearOutdated() {
  IceUtil::RWRecMutex::WLock lock(rw_mutex_);
  MCE_INFO("AdInfoCache::ClearCach --> cache size:" << uid_ad_map_.size());
  for (map<Ice::Int, UserAdInfoPtr>::iterator it = uid_ad_map_.begin(); it != uid_ad_map_.end();) {
    UserAdInfoPtr & user_ad_info = it->second;
    if (time(NULL) - user_ad_info->pv_time() > kClearCacheInterval) {
      MCE_INFO("ClearOutdated : uid " << it->first << " evicted.");
      uid_ad_map_.erase(it++);
    } else {
      ++ it;
    }
  }
}

ClearCacheTimer::ClearCacheTimer() : Timer(kClearCacheInterval * 1000) {
}

void ClearCacheTimer::handle() {
  AdInfoCache::instance().ClearOutdated();
}

}
}

