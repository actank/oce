#include "ad_creative_cache.h"
#include "Notify/util/url_encode.h"
#include "Notify/util/json_escape.h"

#include "ServiceI.h"
#include "QueryRunner.h"
#include "AdMatchAdapter.h"

using namespace xce::ad;
using namespace std;
using namespace com::xiaonei::xce;
using namespace MyUtil;

static const char * kDbInstance = "ad_db";
static const char * kMainTplFile = "../etc/ad_ebp_tpl/ebp.tpl";
static const char * kJsonTplFile = "../etc/ad_ebp_tpl/json.tpl";

AdCreativeCache::AdCreativeCache() : default_index_(0), main_template_(0), json_template_(0) {
}

AdCreativeCache::~AdCreativeCache() {
  delete main_template_;
}

bool AdCreativeCache::ReloadById(long id) {
  Statement sql;
  sql << "SELECT creative_id, member_id, widget_id, title, content, click_url, media_uri, update_time "\
         "FROM creative WHERE creative_id=" << id;

  try {
    mysqlpp::StoreQueryResult res = QueryRunner(kDbInstance, CDbRServer).store(sql);
    
    if (!res || res.num_rows() <= 0) {
      MCE_WARN("select table creative error. id = " << id);
      return false;
    }

    mysqlpp::Row row = res.at(0);

    long id = (long)row["creative_id"];
    //int member_id = (int)row["member_id"];
    {
      IceUtil::RWRecMutex::WLock lock(mutex_);
      //if (member_id == 999) {
      //  default_ads.push_back(id);
      //}

      // 有点偷懒
      ad_cache_[id].id = id;
      ad_cache_[id].zone_id = 100000000001;
      ad_cache_[id].widget_id = (long)row["widget_id"];

      ad_cache_[id].title = row["title"].c_str();
      ad_cache_[id].content = row["content"].c_str();
      ad_cache_[id].img_url = string("http://jebe.xnimg.cn") + row["media_uri"].c_str();
      ad_cache_[id].dest_url = xce::notify::UrlEncode(row["click_url"].c_str());
      ad_cache_[id].update_time = (time_t)(mysqlpp::DateTime)row["update_time"];
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
  vector<long> default_ads;

  set<long> active_campaigns;
  set<long> active_groups;

  {
    Statement sql;
    sql << "SELECT campaign_id FROM ad_campaign WHERE is_active > 0";

    try {
      mysqlpp::StoreQueryResult res = QueryRunner(kDbInstance, CDbRServer).store(sql);
      
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
    sql << "SELECT group_id FROM ad_group WHERE is_active > 0";

    try {
      mysqlpp::StoreQueryResult res = QueryRunner(kDbInstance, CDbRServer).store(sql);
      
      if (!res) {
        MCE_WARN("select table ad_group error.");
        return false;
      }

      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        active_groups.insert((long)row["group_id"]);
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

  // 加载 data_schemas 表信息
  {
    Statement sql;

    sql << "SELECT creative_id, adgroup_id, campaign_id, member_id, widget_id, title, content, click_url, media_uri, update_time FROM creative";

    try {
      mysqlpp::StoreQueryResult res = QueryRunner(kDbInstance, CDbRServer).store(sql);
      
      if (!res) {
        MCE_WARN("select table creative error.");
        return false;
      }

      MCE_DEBUG("loaded ad size : " << res.num_rows());
      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);

        long id = (long)row["creative_id"];
        long member_id = (long)row["member_id"];

        // if (active_campaigns.find((long)row["campaign_id"]) == active_campaigns.end()) {
        //   MCE_DEBUG("ad id campaign inactive << " << id);
        //   continue;
        // }

        // if (active_groups.find((long)row["adgroup_id"]) == active_groups.end()) {
        //   MCE_DEBUG("ad id group inactive << " << id);
        //   continue;
        // }

        {
          if (member_id == 999) {
            if (active_campaigns.find((long)row["campaign_id"]) != active_campaigns.end()
                && active_groups.find((long)row["adgroup_id"]) != active_groups.end()) {
              default_ads.push_back(id);
            }
          }

          // 有点偷懒
          ad_cache[id].id = id;
          ad_cache[id].zone_id = 100000000001;
          ad_cache[id].widget_id = (long)row["widget_id"];
          ad_cache[id].group_id = (long)row["adgroup_id"];

          ad_cache[id].title = row["title"].c_str();
          ad_cache[id].content = row["content"].c_str();
          ad_cache[id].img_url = string("http://jebe.xnimg.cn") + row["media_uri"].c_str();
          ad_cache[id].dest_url = xce::notify::UrlEncode(row["click_url"].c_str());
          ad_cache[id].update_time = (time_t)((mysqlpp::DateTime)row["update_time"]);
        }

        MCE_INFO("load ad creative id=" << id << " success");
      }
    } catch (std::exception& e) {
      MCE_WARN("load db err : " << e.what());
      return false;
    } catch (...) {
      MCE_WARN("load db unknown err.");
      return false;
    }
  }

  if (default_ads.size() < 3) {
    MCE_WARN("too few default_ads : " << default_ads.size());
  } else {
    MCE_INFO("default_ads size : " << default_ads.size());
  }

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    default_ads_.swap(default_ads);
    ad_cache_.swap(ad_cache);

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
      //json_template_= ctemplate::Template::GetTemplate(kJsonTplFile, ctemplate::STRIP_WHITESPACE);
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

void AdCreativeCache::PushDefault(int uid, const string & ip, const string & referer, AdInfoSeq * ads, int count) {
  DefaultSeq dseq;

  size_t pos = referer.find('?');
  string referer_path = referer.substr(0, pos);
  while (ads->size() < count && default_ads_.size()) {
    AdInfo ai;
    ++ default_index_;
    if (default_index_ >= default_ads_.size()) {
      default_index_ = 0;
    }

    ai.creative = default_ads_[default_index_ % default_ads_.size()];
    ai.updateTime = 0;
    ai.text = "0_0_";
    ai.text += boost::lexical_cast<string>(ai.creative);
    ai.text += "_0_0";
    ads->push_back(ai);
    
    {
      IceUtil::RWRecMutex::RLock lock(mutex_);
      AdCache::const_iterator it = ad_cache_.find(ai.creative);
      const AdCreative * ad = 0;
      if (it != ad_cache_.end()) {
        ad = &it->second;
      }
      if (ad) {
        DefaultAd da;
        da.groupId = ad->group_id;
        da.crtvId = ad->id;
        da.bidId = ad->id;
        da.adzoneId = ad->zone_id;
        da.refer = referer_path;
        dseq.push_back(da);
      }
    }

    MCE_INFO("use default ad " << ai.creative << " uid=" << uid);
  }

  if (ads->size() < count) {
    MCE_WARN("no adequate default ad : " << dseq.size() << '/' << count - ads->size() << " uid=" << uid);
    return;
  }

  MCE_INFO("ReportDefault size : " << dseq.size() << " uid=" << uid);
  try {
    AdMatchAdapter::instance().ReportDefault(uid, ip, count, dseq);
  } catch(Ice::Exception& e) {
    MCE_WARN("ReportDefault Ice error : " << e);
  } catch(std::exception& e) {
    MCE_WARN("ReportDefault error : " << e.what());
  }
}

void AdCreativeCache::Get(const AdInfoSeq & ads, string * html) const {
  ctemplate::TemplateDictionary dict("html");
  
  IceUtil::RWRecMutex::RLock lock(mutex_);

  for(size_t i = 0; i < ads.size(); ++i) {
    ctemplate::TemplateDictionary* sub_dict = dict.AddSectionDictionary("AD_ITEMS");
    sub_dict->SetValue("AD_MATCHER_COOKIE", xce::notify::UrlEncode(ads[i].text));
    sub_dict->SetIntValue("AD_ID", ads[i].creative);

    AdCache::const_iterator it = ad_cache_.find(ads[i].creative);
    const AdCreative * ad = 0;
    if (it != ad_cache_.end()) {
      ad = &it->second;
    }

    if (ads[i].updateTime > ad->update_time) {
      TaskManager::instance().execute(new ReloadCreativeTask(ad->id));
    }

    if (ad) {
      sub_dict->SetValue("AD_TITLE", ad->title);
      sub_dict->SetValue("AD_CONTENT", ad->content);
      sub_dict->SetValue("AD_IMG_URL", ad->img_url);
      sub_dict->SetValue("AD_DEST_URL", ad->dest_url);
    } else {
      MCE_INFO("ad " << ads[i].creative << " not found in memory.");
    }
  }

  if (main_template_) {
    main_template_->Expand(html, &dict);
  }
  MCE_DEBUG("ad html size : " << html->size());
}

void AdCreativeCache::GetJson(const AdInfoSeq & ads, string * html) const {
  ctemplate::TemplateDictionary dict("html");

  ctemplate::TemplateDictionary* list_item_dict = dict.AddSectionDictionary("SEC_LIST_ITEM");
  list_item_dict->SetIntValue("AD_ZONE_ID", 100000000001);

  IceUtil::RWRecMutex::RLock lock(mutex_);
  int item_count = 0;
  for(size_t i = 0; i < ads.size(); ++i) {
    AdCache::const_iterator it = ad_cache_.find(ads[i].creative);
    const AdCreative * ad = 0;
    if (it != ad_cache_.end()) {
      ad = &it->second;
    }

    if (ad) {
      ctemplate::TemplateDictionary* ad_item_dict = list_item_dict->AddSectionDictionary("SEC_AD_ITEM");

      if (item_count++ > 0) {
        ad_item_dict->SetValue("AD_ITEM_SEP", ",");
      }
      ad_item_dict->SetIntValue("AD_CREATIVE_ID", ads[i].creative);
      ad_item_dict->SetIntValue("AD_WIDGET_ID", ad->widget_id);
      ad_item_dict->SetValue("AD_WIDGET_VER", "1.0");

      ad_item_dict->SetValue("AD_TITLE", xce::notify::JsonEscape(ad->title));
      ad_item_dict->SetValue("AD_CONTENT", xce::notify::JsonEscape(ad->content));
      ad_item_dict->SetValue("AD_MEDIA_URI", ad->img_url);

      stringstream url;
      url << "http://ebp.renren.com/ebp/report.html?aid=" << ad->id
          << "&url=" << ad->dest_url
          << "&mc=" << ads[i].text;

      ad_item_dict->SetValue("AD_CLICK_URL", url.str());
    } else {
      MCE_INFO("ad " << ads[i].creative << " not found in memory.");
    }
  }

  if (json_template_) {
    json_template_->Expand(html, &dict);
  }
}

void ReloadCreativeTask::handle() {
  if(!AdCreativeCache::instance().ReloadById(creative_id_)) {
    MCE_INFO("ReloadCreativeTask executed. id=" << creative_id_);
  } else {
    MCE_WARN("ReloadCreativeTask error. id=" << creative_id_);
  }
}

