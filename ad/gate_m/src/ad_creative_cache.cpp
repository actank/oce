#include "ad_creative_cache.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "Notify/util/url_encode.h"
#include "Notify/util/json_escape.h"
#include "ad_update_interval.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include "AdMatchNAdapter.h"

using namespace xce::ad;
using namespace std;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace boost;

static const char * kMainTplFile = "../etc/ad_ebpn_tpl/html.tpl";
static const char * kJsonTplFile = "../etc/ad_ebpn_tpl/json.tpl";

AdCreativeCache::AdCreativeCache() : main_template_(0), json_template_(0) {
}

AdCreativeCache::~AdCreativeCache() {
  delete main_template_;
}

bool AdCreativeCache::ReloadById(long id) {
  Statement sql;
  sql << "SELECT creative_id, adgroup_id, member_id, widget, widget_id, widget_version, creative_name, content, click_url, media_uri, last_update_time "\
         "FROM creative WHERE creative_id=" << id;

  try {
    MCE_INFO("AdCreativeCache::ReloadById() --> db_instance = " << db_instance);
    mysqlpp::StoreQueryResult res = QueryRunner(db_instance, CDbRServer).store(sql);
    
    if (!res || res.num_rows() <= 0) {
      MCE_WARN("select table creative error. id = " << id);
      return false;
    }

    mysqlpp::Row row = res.at(0);

    long id = (long)row["creative_id"];

      //if (member_id == 999) {
      //  default_ads.push_back(id);
      //}

      AdCreativePtr  ad = new AdCreative();
      ad->id = id;
      ad->widget = row["widget"].c_str();
      ad->widget_id = (long)row["widget_id"];
      ad->widget_version = row["widget_version"].c_str();

      ad->title = row["creative_name"].c_str();
      ad->content = row["content"].c_str();
      // ad->img_url = string("http://jebe.xnimg.cn") + row["media_uri"].c_str();
      ad->img_url = row["media_uri"].c_str();
      if (ad->img_url.compare("NULL") == 0) {
        ad->img_url = "";
      }
      ad->dest_url = xce::notify::UrlEncode(row["click_url"].c_str());
      ad->update_time = (time_t)(mysqlpp::DateTime)row["last_update_time"];
    {
      IceUtil::RWRecMutex::WLock lock(mutex_);
      ad_cache_[id] = ad;
    }
  } catch (std::exception& e) {
    MCE_WARN("load db err : " << e.what());
    return false;
  } catch (...) {
    MCE_WARN("load db unknown err.");
    return false;
  }

  MCE_INFO("creative ReloadById id=" << id << " success");
  return true;
}

bool AdCreativeCache::Reload() {
  // TODO : template表很大的时候，只加载最新更新的template
  AdCache ad_cache;
  map<long, vector<long> > default_ads;
  map<long, size_t> default_ad_indices;

  set<long> active_campaigns;
  set<long> active_groups;
  set<long> mobile_groups;

  MCE_INFO("AdCreativeCache::Reload() --> db_instance = " << db_instance);

  multimap<string, long> url_zone_map;
  ostringstream adzones;
  {
    Statement sql;
    sql << "SELECT url, adzone_id FROM url_adzone_maping where url like '%3g.renren.com%'";
    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db_instance, CDbRServer).store(sql);
      
      if (res) {
        adzones << " (";
        for (size_t i = 0; i < res.num_rows(); ++i) {
          string url = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)res[i]["url"];
          url_zone_map.insert(make_pair(url, (long)res[i]["adzone_id"]));
          if(i!=0) {
            adzones << ", ";
          }
          adzones << res[i]["adzone_id"];
        }
        adzones << ")";
      } else {
        MCE_WARN("select table url_adzone_maping error.");
      }
    } catch (std::exception& e) {
      MCE_WARN("load db err : " << e.what());
      return false;
    } catch (...) {
      MCE_WARN("load db unknown err.");
      return false;
    }
  }

  map<long, size_t> zone_count_map;
  {
    Statement sql;
    sql << "SELECT adzone_id, count FROM adzone where adzone_id in " << adzones.str();
    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db_instance, CDbRServer).store(sql);
      
      if (res) {
        for (size_t i = 0; i < res.num_rows(); ++i) {
          zone_count_map.insert(make_pair((long)res[i]["adzone_id"], (size_t)res[i]["count"]));
        }
      } else {
        MCE_WARN("select table adzone error.");
      }
    } catch (std::exception& e) {
      MCE_WARN("load db err : " << e.what());
      return false;
    } catch (...) {
      MCE_WARN("load db unknown err.");
      return false;
    }
  }

  {
    Statement sql;
    sql << "SELECT campaign_id FROM campaign WHERE begin_time < now() AND (end_time>now() OR end_time is NULL) AND sys_status = 1 AND "
       "member_status = 1 AND am_status = 1 AND audit_status = 1 AND delete_flag = 1";

    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db_instance, CDbRServer).store(sql);
      
      if (!res) {
        MCE_WARN("select table ad_campaign error.");
        return false;
      }

      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        active_campaigns.insert((long)row["campaign_id"]);
      }
    } catch (std::exception& e) {
      MCE_WARN("load db err : " << e.what());
      return false;
    } catch (...) {
      MCE_WARN("load db unknown err.");
      return false;
    }
  }
  if (active_campaigns.empty()) {
    MCE_WARN("no active campaign !");
    return false;
  }
  MCE_INFO("active campaign_id size " << active_campaigns.size());

  {
    Statement sql;
    sql << "SELECT adgroup_id FROM adgroup_adzone_ref WHERE delete_flag = 1 AND adzone_id in " << adzones.str();

    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db_instance, CDbRServer).store(sql);
      
      if (!res) {
        MCE_WARN("select table adgroup_adzone_ref error.");
        return false;
      }

      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        mobile_groups.insert((long)row["adgroup_id"]);
      }
    } catch (std::exception& e) {
      MCE_WARN("load db err : " << e.what());
      return false;
    } catch (...) {
      MCE_WARN("load db unknown err.");
      return false;
    }
  }
  if (mobile_groups.empty()) {
    MCE_WARN("no 3g group!");
    return false;
  }
  MCE_INFO("3g group_id size " << mobile_groups.size());

  {
    Statement sql;
    sql << "SELECT adgroup_id FROM adgroup WHERE member_status = 1 AND am_status = 1 AND audit_status = 1 AND delete_flag = 1 "
      "AND daily_sys_status = 1 AND total_sys_status = 1";

    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db_instance, CDbRServer).store(sql);
      
      if (!res) {
        MCE_WARN("select table ad_group error.");
        return false;
      }

      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        active_groups.insert((long)row["adgroup_id"]);
      }
    } catch (std::exception& e) {
      MCE_WARN("load db err : " << e.what());
      return false;
    } catch (...) {
      MCE_WARN("load db unknown err.");
      return false;
    }
  }
  if (active_groups.empty()) {
    MCE_WARN("no active group!");
    return false;
  }
  MCE_INFO("active group_id size " << active_groups.size());

  map<long, set<long> > default_group_zones;
  {
    // 加载 默认 ad group 的 zone 信息
    Statement sql;
    sql << "SELECT adzone_id, adgroup_id FROM adgroup_adzone_ref WHERE member_id=999 AND member_status=1 AND am_status=1 AND delete_flag=1 AND adzone_id in " << adzones.str();
    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db_instance, CDbRServer).store(sql);
      if (res) {
        for (size_t i = 0; i < res.num_rows(); ++i) {
          mysqlpp::Row row = res.at(i);
          long zone_id = (long)row["adzone_id"];
          long group_id = (long)row["adgroup_id"];
          default_group_zones[group_id].insert(zone_id);
        }
      }
    } catch (std::exception& e) {
      MCE_WARN("load table adgroup_adzone_ref err : " << e.what());
      return false;
    } catch (...) {
      MCE_WARN("load table adgroup_adzone_ref unknown err.");
      return false;
    }
  }

  {
    // 加载 creative 信息
    Statement sql;

    sql << "SELECT creative_id, adgroup_id, campaign_id, member_id, widget, "
        "widget_id, widget_version, creative_name, content, click_url, media_uri, "
        "last_update_time FROM creative where member_status = 1 and am_status = 1 "
        "and audit_status = 1 and delete_flag = 1";

    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db_instance, CDbRServer).store(sql);
      
      if (!res) {
        MCE_WARN("select table creative error.");
        return false;
      }

      MCE_DEBUG("loaded ad size : " << res.num_rows());

      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);

        long id = (long)row["creative_id"];
        long member_id = (long)row["member_id"];
        long group_id = (long)row["adgroup_id"];

        {
          if (member_id == 999) {
            if (active_campaigns.find((long)row["campaign_id"]) == active_campaigns.end()) {
              MCE_DEBUG("ad id campaign inactive << " << id);
              continue;
            }
            if (active_groups.find(group_id) == active_groups.end()) {
              MCE_DEBUG("ad id group inactive << " << id);
              continue;
            }
            if (mobile_groups.find(group_id) == mobile_groups.end()) {
              MCE_DEBUG("ad id group not for 3g: " << id);
              continue;
            }
            map<long, set<long> >::iterator it = default_group_zones.find(group_id);
            if (it != default_group_zones.end()) {
              // 一个默认 group 可能在多个 zone 上展示. zone 和 group 是多对多的
              for(set<long>::iterator zone_it = it->second.begin(); zone_it != it->second.end(); ++ zone_it) {
                default_ads[*zone_it].push_back(id);
                default_ad_indices[*zone_it] = 0;
              }
            }
          }
          AdCreativePtr  ad = new AdCreative();

          ad->id = id;
          ad->widget = row["widget"].c_str();
          ad->widget_id = (long)row["widget_id"];
          ad->widget_version = row["widget_version"].c_str();
          ad->group_id = group_id;
          ad->member_id = member_id;

          ad->title = row["creative_name"].c_str();

          ad->content = row["content"].c_str();
          // ad->img_url = string("http://jebe.xnimg.cn") + row["media_uri"].c_str();
          ad->img_url = row["media_uri"].c_str();
          if (ad->img_url.compare("NULL") == 0) {
            ad->img_url = "";
          }
          ad->dest_url = xce::notify::UrlEncode(row["click_url"].c_str());
          ad->update_time = (time_t)((mysqlpp::DateTime)row["last_update_time"]);
          ad_cache[id] = ad;

          // MCE_INFO("Load ad " << id << " title=" << row["creative_name"].c_str() << " widget=" << ad->widget);
        }

        // MCE_INFO("load ad creative id=" << id << " success");
      }
    } catch (std::exception& e) {
      MCE_WARN("load db err : " << e.what());
      return false;
    } catch (...) {
      MCE_WARN("load db unknown err.");
      return false;
    }
  }

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    default_ads_.swap(default_ads);
    default_ad_indices_.swap(default_ad_indices);
    ad_cache_.swap(ad_cache);
    url_zone_map_.swap(url_zone_map);
    zone_count_map_.swap(zone_count_map);
  }

  {
    IceUtil::RWRecMutex::WLock lock(tpl_mutex_);

    if (main_template_) {
      if (main_template_->ReloadIfChanged()) {
        MCE_WARN("template " << kMainTplFile << " reloaded successfully.");
      }
    } else {
      main_template_= ctemplate::Template::GetTemplate(kMainTplFile, ctemplate::DO_NOT_STRIP);
    }
    if (!main_template_) {
      MCE_WARN("load template " << kMainTplFile << " fail.");
      return false;
    }

    if (json_template_) {
      if (json_template_->ReloadIfChanged()) {
        MCE_INFO("template " << kJsonTplFile << " reloaded successfully.");
      }
    } else {
      // json_template_= ctemplate::Template::GetTemplate(kJsonTplFile, ctemplate::STRIP_WHITESPACE);
      json_template_= ctemplate::Template::GetTemplate(kJsonTplFile, ctemplate::DO_NOT_STRIP);
    }
    if (!json_template_) {
      MCE_WARN("load template " << kJsonTplFile << " fail.");
      return false;
    }
  }

  MCE_INFO("AdCreativeCache load success.");
  return true;
}

void AdCreativeCache::GetZonesByReferer(const string & referer, set<long> * zones) const {
  size_t pos = referer.find('?');
  string path = referer.substr(0, pos);
  typedef  multimap<string, long>::const_iterator ItType;

  pair<ItType, ItType> it_range = url_zone_map_.equal_range(path);
  if (it_range.first == it_range.second) {
    size_t pos = 0;

    if (boost::starts_with(path, "http://")) {
      pos = path.find_first_of('/', 8);
    } else {
      pos = path.find_first_of('/');
    }

    string host = path.substr(0, pos);
    MCE_DEBUG("referer host : " << host);

    it_range = url_zone_map_.equal_range(host);
  }

  for (ItType it = it_range.first; it != it_range.second; ++it) {
    zones->insert(it->second);
  }
}

void AdCreativeCache::PushDefault(int uid, const string & ip, const string & referer, AdResultMap * ad_reault_map) const {
  size_t pos = referer.find('?');
  string referer_path = referer.substr(0, pos);
  set<long> zones;

  IceUtil::RWRecMutex::RLock lock(mutex_);

  GetZonesByReferer(referer, &zones);
  for (set<long>::iterator i = zones.begin(); i != zones.end(); ++i) {
    long zone_id = *i;
    map<long, size_t>::const_iterator zone_count_it = zone_count_map_.find(zone_id);
    if (zone_count_it == zone_count_map_.end()) {
      MCE_DEBUG(" zone " << zone_id << " need no ad");
      continue;
    }
    size_t count = zone_count_it->second;
    MCE_DEBUG(" zone " << zone_id << " need " << count << " ads");
    AdInfoSeq & ads = (*ad_reault_map)[zone_id];
    map<long, vector<long> >::const_iterator it_default = default_ads_.find(zone_id);
    if (it_default == default_ads_.end()) {
      continue;
    }
    map<long, size_t>::iterator it_default_index = default_ad_indices_.find(zone_id);
    if (it_default_index == default_ad_indices_.end()) {
      continue;
    }

    const vector<long> & default_ids = it_default->second;
    size_t & default_index = it_default_index->second;

    while (ads.size() < count && default_ids.size()) {
      AdInfo ai;
      if (default_index >= default_ids.size()) {
        default_index = 0;
      }

      ai.creativeid = default_ids[default_index];
      ai.updateTime = 0;
      ai.text = "0^C0^C";
      ai.text += boost::lexical_cast<string>(ai.creativeid);
      ai.text += "^C0^C0";
      ads.push_back(ai);
      
      AdCache::const_iterator it = ad_cache_.find(ai.creativeid);
      AdCreativePtr ad ;
      if (it != ad_cache_.end()) {
        ad = it->second;
      }
      ++default_index;
      MCE_INFO("use default ad " << ai.creativeid);
    }
  }
}

void AdCreativeCache::Get(const AdResultMap & ad_resutl_map, string * html)  const {
  ctemplate::TemplateDictionary dict("html");
  {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  
  for(AdResultMap::const_iterator it = ad_resutl_map.begin(); it != ad_resutl_map.end(); ++it) {
    ctemplate::TemplateDictionary* zone_dict = dict.AddSectionDictionary("AD_ZONES");
    zone_dict->SetIntValue("AD_ZONE_ID", it->first);
    const AdInfoSeq & ads = it->second;
    MCE_INFO("ad_zone " << it->first << " ad count " << ads.size());

    for(size_t i = 0; i < ads.size(); ++i) {
      ctemplate::TemplateDictionary* item_dict = zone_dict->AddSectionDictionary("AD_ITEMS");
      MCE_INFO("ad " << ads[i].creativeid << " mc " << ads[i].text);
      item_dict->SetValue("AD_MATCHER_COOKIE", xce::notify::UrlEncode(ads[i].text));
      item_dict->SetIntValue("AD_ID", ads[i].creativeid);
      AdCache::const_iterator itc = ad_cache_.find(ads[i].creativeid);
      AdCreativePtr ad ;
      if (itc != ad_cache_.end()) {
        ad = itc->second;
      } else {
        MCE_WARN("AdCreativeCache::Get --> creativeid:" << ads[i].creativeid  << " not found");
        continue;
      }
      if (ads.at(i).updateTime > ad->update_time) {
        TaskManager::instance().execute(new ReloadCreativeTask(ad->id));
      }
      if (ad) {
        MCE_INFO("ad " << ads[i].creativeid << " title " << ad->title);
        item_dict->SetValue("AD_TITLE", ad->title);
        item_dict->SetIntValue("AD_WIDGET_ID", ad->widget_id);
        item_dict->SetValue("AD_WIDGET_VERSION", ad->widget_version);
        item_dict->SetValue("AD_WIDGET", ad->widget);
        item_dict->SetValue("AD_DEST_URL", ad->dest_url);
      } else {
        MCE_INFO("ad " << ads[i].creativeid << " not found in memory.");
      }
    }
  }
  }

  {
    IceUtil::RWRecMutex::RLock lock(tpl_mutex_);
    if (main_template_) {
      main_template_->Expand(html, &dict);
    }
  }
  MCE_DEBUG("ad html size : " << html->size());
}

void AdCreativeCache::GetHtml(const string & referer, const AdResultMap & ad_resutl_map, string & html)  const {
  ostringstream result;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);

    for(AdResultMap::const_iterator it = ad_resutl_map.begin(); it != ad_resutl_map.end(); ++it) {
      const AdInfoSeq & ads = it->second;
      MCE_INFO("ad_zone " << it->first << " ad count " << ads.size());

      for(size_t i = 0; i < ads.size(); ++i) {
        MCE_INFO("ad " << ads[i].creativeid << " mc " << ads[i].text);
        ostringstream click_url;
        click_url << "http://ebp.renren.com/ebpm/click.html?aid=" << ads[i].creativeid 
          << "&mc=" << xce::notify::UrlEncode(ads[i].text) << "&ref=" << referer;
        AdCache::const_iterator itc = ad_cache_.find(ads[i].creativeid);
        AdCreativePtr ad ;
        if (itc != ad_cache_.end()) {
          ad = itc->second;
        } else {
          MCE_WARN("AdCreativeCache::GetHtml --> creativeid:" << ads[i].creativeid  << " not found");
          continue;
        }
        if (ads.at(i).updateTime > ad->update_time) {
          TaskManager::instance().execute(new ReloadCreativeTask(ad->id));
        }
        if (ad) {
          MCE_INFO("ad " << ads[i].creativeid << " title " << ad->title << " content -->" << ad->content);
          //click_url << "&url=" << ad->dest_url;
          click_url << "&url=";
          string content = ad->content;
          if(contains(content, "http://3g.renren.com/go.do?to=")) {
            replace_all(content, "@jebe@", xce::notify::UrlEncode(click_url.str()));
          } else {
            replace_all(content, "@jebe@", click_url.str());
          }

          result << content;
        } else {
          MCE_INFO("ad " << ads[i].creativeid << " not found in memory.");
        }
      }
    }
  }
  html = result.str();
}

void AdCreativeCache::GetJson(const string & referer, int refresh_source, int refresh_idx, const map<long, EngineTypes> & self_zones, const AdResultMap & zone_ads_map, string * html) const {
  ctemplate::TemplateDictionary dict("json");
  pair<int, int> update_interval = AdUpdateIntervalCache::instance().GetUpdateInterval(referer);

  dict.SetIntValue("UPDATE_INTERVAL", update_interval.first);
  dict.SetIntValue("MAX_UPDATE_COUNT", update_interval.second);
  dict.SetIntValue("REFRESH_SOURCE", refresh_source);
  dict.SetIntValue("REFRESH_IDX", refresh_idx);
  dict.SetValue("REFERER", xce::notify::UrlEncode(referer));
//  dict.SetValue("EM_TAG", emtag);

  {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  bool item_sep = false;
  for(AdResultMap::const_iterator map_it = zone_ads_map.begin(); map_it != zone_ads_map.end(); ++ map_it) {
    const AdInfoSeq & ads = map_it->second;
    long zone_id = map_it->first;

    ctemplate::TemplateDictionary* ad_zone_dict = dict.AddSectionDictionary("SEC_AD_ZONES");
    ad_zone_dict->SetIntValue("AD_ZONE_ID", zone_id);
    if (map_it != zone_ads_map.begin()) {
      ad_zone_dict->SetValue("AD_ZONE_SEP", ",");
    }

    map<long, EngineTypes>::const_iterator t_it = self_zones.find(zone_id);
    if (t_it != self_zones.end()) {
      ad_zone_dict->SetIntValue("ENGINE_TYPE", t_it->second);
    }

    item_sep = false;
    for(size_t i = 0; i < ads.size(); ++i) {
      AdCache::const_iterator itc = ad_cache_.find(ads[i].creativeid);
      AdCreativePtr ad;
      if (itc != ad_cache_.end()) {
        ad = itc->second;
      }
      if (!ad) {
        MCE_INFO("ad " << ads[i].creativeid << " not found in memory.");
        continue;
      }

      ctemplate::TemplateDictionary* ad_item_dict = ad_zone_dict->AddSectionDictionary("SEC_AD_ITEMS");

      if (item_sep) {
        ad_item_dict->SetValue("AD_ITEM_SEP", ",");
      }
      item_sep = true;

      // ad_item_dict->SetValue("AD_TITLE", ad->title);
      ad_item_dict->SetIntValue("AD_WIDGET_ID", ad->widget_id);
      ad_item_dict->SetValue("AD_WIDGET_VERSION", ad->widget_version);
      ad_item_dict->SetValue("AD_WIDGET", xce::notify::JsonEscape(ad->widget));
      MCE_DEBUG("ad " << ad->id <<  " DEST_URL " << ad->dest_url);
      ad_item_dict->SetValue("AD_DEST_URL", ad->dest_url);
      ad_item_dict->SetValue("AD_MATCHER_COOKIE", xce::notify::UrlEncode(ads[i].text));

      ad_item_dict->SetIntValue("AD_CREATIVE_ID", ads[i].creativeid);
      ad_item_dict->SetValue("AD_TITLE", xce::notify::JsonEscape(ad->title));
      ad_item_dict->SetValue("AD_CONTENT", xce::notify::JsonEscape(ad->content));
      ad_item_dict->SetValue("AD_MEDIA_URI", xce::notify::UrlEncode(ad->img_url));
    }
  }
  }

  {
    IceUtil::RWRecMutex::RLock lock(tpl_mutex_);
  if (json_template_) {
    json_template_->Expand(html, &dict);
    // MCE_DEBUG("json content : " << *html);
  }
  }
}

void ReloadCreativeTask::handle() {
  if(!AdCreativeCache::instance().ReloadById(creative_id_)) {
    MCE_INFO("ReloadCreativeTask executed. id=" << creative_id_);
  } else {
    MCE_WARN("ReloadCreativeTask error. id=" << creative_id_);
  }
}

