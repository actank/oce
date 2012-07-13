#include "ad_creative_cache.h"

#include <boost/algorithm/string/predicate.hpp>

#include "Notify/util/url_encode.h"
#include "Notify/util/json_escape.h"

#include "ServiceI.h"
#include "QueryRunner.h"
//#include "AdEngineAdapter.h"
#include "AdGateConfig.h"
#include "AdGateData.h"

using namespace xce::ad;
using namespace std;
using namespace com::xiaonei::xce;
using namespace MyUtil;

static const char * kMainTplFile = "../etc/ad_gate_tpl/html.tpl";
static const char * kJsonTplFile = "../etc/ad_gate_tpl/json.tpl";

AdCreativeCache::AdCreativeCache() : main_template_(0), json_template_(0) {
}

AdCreativeCache::~AdCreativeCache() {
  delete main_template_;
}

bool AdCreativeCache::ReloadById(long id) {
  Statement sql;
  sql << "SELECT creative_id, adgroup_id, member_id, widget, widget_id, widget_version, title, content, click_url, media_uri, last_update_time "\
         "FROM creative WHERE creative_id=" << id;

  try {
    mysqlpp::StoreQueryResult res = QueryRunner(GateConfig::instance().DB(), CDbRServer).store(sql);
    
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

      ad->title = row["title"].c_str();
      ad->content = row["content"].c_str();
      ad->img_url = string("http://jebe.xnimg.cn") + row["media_uri"].c_str();
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

  set<long> active_campaigns;
  set<long> active_groups;

  string db = GateConfig::instance().DB();
  {
    Statement sql;
    sql << "SELECT campaign_id FROM campaign WHERE begin_time < now() AND (end_time>now() OR end_time is NULL) AND sys_status = 1 AND "
       "member_status = 1 AND am_status = 1 AND delete_flag = 1";
    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db, CDbRServer).store(sql);
      
      if (!res) {
        MCE_WARN("select table campaign error.");
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
    sql << "SELECT adgroup_id FROM adgroup WHERE member_status = 1 AND am_status = 1 AND audit_status = 1 AND delete_flag = 1 "
      "AND display_type != 1 AND daily_sys_status = 1 AND total_sys_status = 1";

    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db, CDbRServer).store(sql);
      
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

  multimap<string, long> url_zone_map;
  {
    Statement sql;
    sql << "SELECT url, adzone_id FROM url_adzone_maping";
    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db, CDbRServer).store(sql);
      
      if (res) {
        for (size_t i = 0; i < res.num_rows(); ++i) {
          string url = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)res[i]["url"];
          url_zone_map.insert(make_pair(url, (long)res[i]["adzone_id"]));
        }
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
    sql << "SELECT adzone_id, count FROM adzone";
    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db, CDbRServer).store(sql);
      
      if (res) {
        for (size_t i = 0; i < res.num_rows(); ++i) {
          zone_count_map.insert(make_pair((long)res[i]["adzone_id"], (size_t)res[i]["count"]));
        }
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

  map<long, set<long> > default_group_zones;
  {
    // 加载 默认 ad group 的 zone 信息
    Statement sql;
    sql << "SELECT adzone_id, adgroup_id FROM adgroup_adzone_ref WHERE member_id=999 AND member_status=1 AND am_status=1 AND delete_flag=1";
    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db, CDbRServer).store(sql);
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
        "widget_id, widget_version, title, content, click_url, media_uri, "
        "last_update_time FROM creative where member_status = 1 and am_status = 1 "
        "and audit_status = 1 and delete_flag = 1";
    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db, CDbRServer).store(sql);
      
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

        /*if (active_campaigns.find((long)row["campaign_id"]) == active_campaigns.end()) {
          MCE_DEBUG("ad id campaign inactive << " << id);
          continue;
        }
        if (active_groups.find(group_id) == active_groups.end()) {
          MCE_DEBUG("ad id group inactive << " << id);
          continue;
        }*/

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
            map<long, set<long> >::iterator it = default_group_zones.find(group_id);
            if (it != default_group_zones.end()) {
              // 一个默认 group 可能在多个 zone 上展示. zone 和 group 是多对多的
              for(set<long>::iterator zone_it = it->second.begin(); zone_it != it->second.end(); ++ zone_it) {
                default_ads[*zone_it].push_back(id);
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

          ad->title = row["title"].c_str(  );

          ad->content = row["content"].c_str();
          ad->img_url = string("http://jebe.xnimg.cn") + row["media_uri"].c_str();
          ad->dest_url = xce::notify::UrlEncode(row["click_url"].c_str());
          ad->update_time = (time_t)((mysqlpp::DateTime)row["last_update_time"]);
          ad_cache[id] = ad;

          // MCE_INFO("Load ad " << id << " title=" << row["title"].c_str() << " widget=" << ad->widget);
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
    ad_cache_.swap(ad_cache);
    url_zone_map_.swap(url_zone_map);
    zone_count_map_.swap(zone_count_map);

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
        MCE_WARN("template " << kJsonTplFile << " reloaded successfully.");
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

void AdCreativeCache::GetZonesByReferer(const string & referer, set<long> * zones) {
  size_t pos = referer.find('?');
  string path = referer.substr(0, pos);
  typedef  multimap<string, long>::iterator ItType;

  IceUtil::RWRecMutex::RLock lock(mutex_);
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

void AdCreativeCache::PushDefault(int uid, const string & ip, const string & referer, AdResultMap * ad_reault_map) {
  size_t pos = referer.find('?');
  string referer_path = referer.substr(0, pos);
  set<long> zones;
  GetZonesByReferer(referer, &zones);

  IceUtil::RWRecMutex::RLock lock(mutex_);
  for (set<long>::iterator i = zones.begin(); i != zones.end(); ++i) {
    long zone_id = *i;
    map<long, size_t>::iterator zone_count_it = zone_count_map_.find(zone_id);
    if (zone_count_it == zone_count_map_.end()) {
      MCE_DEBUG(" zone " << zone_id << " need no ad");
      continue;
    }
    size_t count = zone_count_it->second;
    MCE_DEBUG(" zone " << zone_id << " need " << count << " ads");
    AdInfoSeq & ads = (*ad_reault_map)[zone_id];
    while (ads.size() < count && default_ads_[zone_id].size()) {
      AdInfo ai;
      ++ default_ad_indices_[zone_id];
      if (default_ad_indices_[zone_id] >= default_ads_[zone_id].size()) {
        default_ad_indices_[zone_id] %= default_ads_[zone_id].size();
      }

      ai.creativeid = default_ads_[zone_id][default_ad_indices_[zone_id]];
      ai.updateTime = 0;
      ai.text = "0^C0^C";
      ai.text += boost::lexical_cast<string>(ai.creativeid);
      ai.text += "^C0^C0";
      MCE_DEBUG("AdCreativeCache::PushDefault --> add default ad");
      ads.push_back(ai);
      
      {
        AdCache::const_iterator it = ad_cache_.find(ai.creativeid);
        AdCreativePtr ad ;
        if (it != ad_cache_.end()) {
          ad = it->second;
        }
        // if (ad) {
        //   DefaultAd da;
        //   da.groupId = ad->group_id;
        //   da.crtvId = ad->id;
        //   da.bidId = ad->id;
        //   da.adzoneId = ad->zone_id;
        //   da.refer = referer_path;
        //   dseq.push_back(da);
        // }
      }

      MCE_INFO("use default ad " << ai.creativeid);
    }
  }

  // if (dseq.empty()) {
  //   MCE_WARN("no default ad to use!");
  //   return;
  // }

  // MCE_INFO("ReportDefault size : " << dseq.size());
  // try {
  //   AdMatchAdapter::instance().ReportDefault(uid, ip, count, dseq);
  // } catch(Ice::Exception& e) {
  //   MCE_WARN("ReportDefault Ice error : " << e);
  // } catch(std::exception& e) {
  //   MCE_WARN("ReportDefault error : " << e.what());
  // }
}

void AdCreativeCache::Get(const AdResultMap & ad_resutl_map, string * html, int refresh_idx)  const {
  ctemplate::TemplateDictionary dict("html");
  IceUtil::RWRecMutex::RLock lock(mutex_);
  
  for(AdResultMap::const_iterator it = ad_resutl_map.begin(); it != ad_resutl_map.end(); ++it) {
    ctemplate::TemplateDictionary* zone_dict = dict.AddSectionDictionary("AD_ZONES");
    zone_dict->SetIntValue("AD_ZONE_ID", it->first);
    const AdInfoSeq & ads = it->second;
    MCE_INFO("ad_zone " << it->first << " ad count " << ads.size());

    for(size_t i = 0; i < ads.size(); ++i) {
      ctemplate::TemplateDictionary* item_dict = zone_dict->AddSectionDictionary("AD_ITEMS");
      MCE_DEBUG("ad " << ads[i].creativeid << " mc " << ads[i].text);
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
        MCE_DEBUG("ad " << ads[i].creativeid << " title " << ad->title);
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

  if (main_template_) {
    main_template_->Expand(html, &dict);
  }
  MCE_DEBUG("ad html size : " << html->size());
}

void AdCreativeCache::GetJson(const string & referer, const AdResultMap & zone_ads_map, string * html, int refresh_idx) const {
  ctemplate::TemplateDictionary root_dict("json");

//  typedef std::map<long, ctemplate::TemplateDictionary*> AdZoneDictMap;
//  AdZoneDictMap dict_map;

  IceUtil::RWRecMutex::RLock lock(mutex_);
  bool item_sep = false;
  for(AdResultMap::const_iterator map_it = zone_ads_map.begin(); map_it != zone_ads_map.end(); ++ map_it) {
    const AdInfoSeq & ads = map_it->second;
    long zone_id = map_it->first;

    ctemplate::TemplateDictionary* ad_zone_dict = root_dict.AddSectionDictionary("SEC_AD_ZONES");
    ad_zone_dict->SetIntValue("AD_ZONE_ID", zone_id);
    ad_zone_dict->SetIntValue("NEXT_LOAD_TIME", RefreshTime::instance().next_load_time(zone_id, refresh_idx));
    if (map_it != zone_ads_map.begin()) {
      ad_zone_dict->SetValue("AD_ZONE_SEP", ",");
    }
    ad_zone_dict->SetValue("REFERER", xce::notify::UrlEncode(referer));

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
      ad_item_dict->SetValue("AD_DEST_URL", ad->dest_url);
      ad_item_dict->SetValue("AD_MATCHER_COOKIE", xce::notify::UrlEncode(ads[i].text));

      ad_item_dict->SetIntValue("AD_CREATIVE_ID", ads[i].creativeid);
      ad_item_dict->SetValue("AD_TITLE", xce::notify::JsonEscape(ad->title));
      ad_item_dict->SetValue("AD_CONTENT", xce::notify::JsonEscape(ad->content));
      ad_item_dict->SetValue("AD_MEDIA_URI", ad->img_url);
    }
  }

  if (json_template_) {
    json_template_->Expand(html, &root_dict);
    // MCE_INFO("json content : " << *html);
  }
}

void AdCreativeCache::GetJson(const string & referer, const AdZone2Creatives& zone_ads_map, string * html, int refresh_idx) const {
  ctemplate::TemplateDictionary root_dict("json");

  MCE_DEBUG("AdCreativeCache::GetJson --> enter zone_ads_map.size() = " << zone_ads_map.size());
  IceUtil::RWRecMutex::RLock lock(mutex_);
  bool item_sep = false;
  for(AdZone2Creatives::const_iterator map_it = zone_ads_map.begin(); map_it != zone_ads_map.end(); ++ map_it) {
    const AdCreativeSeq& ads = map_it->second;
    long zone_id = map_it->first;

    ctemplate::TemplateDictionary* ad_zone_dict = root_dict.AddSectionDictionary("SEC_AD_ZONES");
    ad_zone_dict->SetIntValue("AD_ZONE_ID", zone_id);
    ad_zone_dict->SetIntValue("NEXT_LOAD_TIME", RefreshTime::instance().next_load_time(zone_id, refresh_idx));
    if (map_it != zone_ads_map.begin()) {
      ad_zone_dict->SetValue("AD_ZONE_SEP", ",");
    }
    ad_zone_dict->SetValue("REFERER", xce::notify::UrlEncode(referer));

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

      MCE_DEBUG("AdCreativeCache::GetJson --> creative_id = " << ads[i].creativeid << ", widget_id = " << ads[i].widgetid);
      if(ads[i].widgetid) {
        ad_item_dict->SetIntValue("AD_WIDGET_ID", ads[i].widgetid);
      } else {
        ad_item_dict->SetIntValue("AD_WIDGET_ID", ad->widget_id);
      }
      ad_item_dict->SetValue("AD_WIDGET_VERSION", ad->widget_version);
      ad_item_dict->SetValue("AD_WIDGET", xce::notify::JsonEscape(ad->widget));
      ad_item_dict->SetValue("AD_DEST_URL", ad->dest_url);
      ad_item_dict->SetValue("AD_MATCHER_COOKIE", xce::notify::UrlEncode(ads[i].text));

      ad_item_dict->SetIntValue("AD_CREATIVE_ID", ads[i].creativeid);
      ad_item_dict->SetValue("AD_TITLE", xce::notify::JsonEscape(ad->title));
      ad_item_dict->SetValue("AD_CONTENT", xce::notify::JsonEscape(ad->content));
      ad_item_dict->SetValue("AD_MEDIA_URI", ad->img_url);
    }
  }

  if (json_template_) {
    json_template_->Expand(html, &root_dict);
    // MCE_INFO("json content : " << *html);
  }
}


void ReloadCreativeTask::handle() {
  if(!AdCreativeCache::instance().ReloadById(creative_id_)) {
    MCE_INFO("ReloadCreativeTask executed. id=" << creative_id_);
  } else {
    MCE_WARN("ReloadCreativeTask error. id=" << creative_id_);
  }
}

