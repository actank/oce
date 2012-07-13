#include "ad_cache.h"
#include "Notify/util/url_encode.h"

#include "ServiceI.h"
#include "Date.h"

#include "ad_logger.h"
#include "aes_wrapper.h"
#include "url_base64.h"
#include "url_encrypt.h"

using namespace xce::ad;
using namespace std;
using namespace com::xiaonei::xce;
using namespace MyUtil;

const char * kDbInstance = "ad_db";
static const int kSearchAdId = 100008;

const Ice::Int kExclusiveAdTypeBase= 10000; // 单独展示的广告类型, 不和其他任何类型混排
//Ice::Int kAdTypeAllNonExclusive = 0;
//Ice::Int kAdTypeGame = 1;
//Ice::Int kAdTypeCommerce = 2;
Ice::Int kNuomiType = kExclusiveAdTypeBase + 1;

const char * g_aes_key = "!7a@k!Z#@$rr98";
size_t g_aes_key_len = strlen(g_aes_key);

AdTpl::AdTpl(Ice::Int tpl_type) : tpl_type_(tpl_type), template_(0), load_time_(0) {
}

Ice::Int AdTpl::MatchTplType(UnionAdPtr ad) {
  if (ad->img_url.empty()) {
    return TPL_TYPE_TEXT;
  } 
  if (ad->content.empty()) {
    return TPL_TYPE_IMAGE;
  }

  return TPL_TYPE_RICH;
}

UnionAdCache::UnionAdCache() {
}

UnionAdCache::~UnionAdCache() {
  // TODO : ctemplate 会自动释放这些模板吗
  //delete main_template_;
}

string AdTpl::GetTplNameByType(Ice::Int tpl_type) {
  string name = "../etc/ad_union_tpl/adunion_";
  name += boost::lexical_cast<string>(tpl_type);
  name += ".tpl";
  return name;
}

bool AdTpl::ReloadTemplate() {
  string filename = AdTpl::GetTplNameByType(tpl_type_);
  template_ = ctemplate::Template::GetTemplate(filename, ctemplate::DO_NOT_STRIP);
  if (!template_) {
    MCE_WARN("ctemplate load error : " << filename);
    return false;
  }
  if (template_->ReloadIfChanged()) {
    MCE_WARN("ctemplate reload error : " << filename);
    return false;
  }
  return true;
}

void LoadAdsResultHandler::Swap(AdTplMap & ad_tpl_map) {
  ad_tpl_map_.swap(ad_tpl_map);
}

bool LoadAdsResultHandler::handle(mysqlpp::Row & row) const {
  Ice::Int type = (Ice::Int)row["type"];
  Ice::Long id = (Ice::Long) row["id"];
  Ice::Int budget = (Ice::Int)row["budget"],
      click_count = (Ice::Int)row["click_count"],
      click_price = (Ice::Int)row["click_price"];

  if (budget <= click_count * click_price) {
    MCE_INFO("load : ad_id " << id << " budget out");
    return true;
  }

  UnionAdPtr ad = new UnionAd();
  ad->id = id;
  ad->type = type;
  ad->name = row["name"].c_str();
  ad->owner_id = (Ice::Long)row["owner_id"];
  ad->budget = budget;
  ad->daily_budget = (Ice::Int)row["daily_budget"];
  ad->click_price = click_price;
  ad->click_count = click_count;

  ad->title = row["title"].c_str();
  ad->content = row["content"].c_str();
  MCE_DEBUG("ad " << id << " content : " << ad->content);

  ad->img_url = row["img_url"].c_str();
  ad->dest_url = xce::notify::UrlEncode(row["dest_url"].c_str());

  ad->group_id = (Ice::Long)row["group_id"];
  ad->campaign_id = (Ice::Long)row["campaign_id"];

  MCE_INFO("load ad_id=" << id << " type=" << type << " success");

  Ice::Int tpl_type = AdTpl::MatchTplType(ad);
  ad->tpl_type = tpl_type;

  AdTplPtr ad_tpl = NULL;
  AdTplMap::const_iterator it = ad_tpl_map_.find(tpl_type);
  if (it == ad_tpl_map_.end()) {
    ad_tpl = new AdTpl(tpl_type);
    ad_tpl_map_[tpl_type] = ad_tpl;
  } else {
    ad_tpl = it->second;
  }

  if (type == kNuomiType) {
    MCE_INFO("ad_id=" << id << " kNuomiType");
    ad_tpl->default_nuomi_ads_.push_back(id);
  } else if (type < 0 || click_price <= 0) {
    ad_tpl->default_ads_.push_back(id);
  } else {
    ad_tpl->ad_type_index_[type].push_back(id);
    if (type > 0 && type < kExclusiveAdTypeBase) {
      ad_tpl->ad_type_index_[0].push_back(id);
    }
  }

  ad_tpl->ad_cache_[id] = ad;
  return true;
}

bool UnionAdCache::Reload() {
  ServiceI& service = ServiceI::instance();
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();

  // TODO : 表很大的时候，只加载最新更新的
  Statement sql;

  sql << "SELECT id, type, name, owner_id, group_id, campaign_id, budget, daily_budget, click_price, click_count, "\
    "title, content, img_url, dest_url FROM ad_union_ads ORDER BY id";
  ObjectResultPtr result = new ObjectResult; 
  LoadAdsResultHandler res_handler(result);
  try {
    QueryRunner(kDbInstance, CDbRServer).query(sql, res_handler);
  } catch (std::exception& e) {
    MCE_WARN("load db err : " << e.what());
    return false;
  } catch (...) {
    MCE_WARN("load db unknown err.");
    return false;
  }

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    res_handler.Swap(ad_tpl_map_);
    if (ad_tpl_map_.find(AdTpl::TPL_TYPE_RENREN_SEARCH) == ad_tpl_map_.end()) {
      ad_tpl_map_[AdTpl::TPL_TYPE_RENREN_SEARCH] = new AdTpl(AdTpl::TPL_TYPE_RENREN_SEARCH);
    }
  }

  for(AdTplMap::iterator it = ad_tpl_map_.begin(); it != ad_tpl_map_.end(); ++it) {
    it->second->ReloadTemplate();
  }

  MCE_INFO("UnionAdCache load success.");
  return true;
}

void UnionAdCache::UpdateNuomiCreative(Ice::Long id, const string& title, const string& content, const string& img_url, const string& dest_url) {
  AdTplPtr ad_tpl = GetAdTpl(1);
  if (ad_tpl) {
    ad_tpl->UpdateNuomiCreative(id, title, content, img_url, dest_url);
  }
}

void AdTpl::UpdateNuomiCreative(Ice::Long id, const string& title, const string& content, const string& img_url, const string& dest_url){
  AdCache::iterator it = ad_cache_.find(id);

  if(it != ad_cache_.end()){
    UnionAdPtr ad = it->second;
    IceUtil::RWRecMutex::WLock lock(ad->mutex_);
    ad->title = title;
    ad->content = content;
    ad->img_url = img_url;
    ad->dest_url = dest_url;
  }
}

AdTplPtr UnionAdCache::GetAdTpl(Ice::Int tpl_type) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  AdTplMap::iterator it = ad_tpl_map_.find(tpl_type);
  if (it == ad_tpl_map_.end()) {
    return NULL;
  }
  return it->second;
}

// 将某个广告挂起
void AdTpl::SuspendAd(UnionAdPtr ad) {
  IceUtil::RWRecMutex::WLock lock(index_mutex_);
  vector<Ice::Long> & v0 = ad_type_index_[0];
  for (size_t i = 0;  i < v0.size(); ++i) {
    if (v0[i] == ad->id) {
      v0.erase(v0.begin() + i);
      break;
    }
  }

  vector<Ice::Long> & vt = ad_type_index_[ad->type];
  for (size_t i = 0;  i < vt.size(); ++i) {
    if (vt[i] == ad->id) {
      vt.erase(vt.begin() + i);
      break;
    }
  }
}
  
string FormatedClickTime(time_t t) {
  struct tm lt;
  localtime_r(&t, &lt);

  const Ice::Int MAX_LEN = 24;
  char res[MAX_LEN];
  res[MAX_LEN - 1] = 0;
  snprintf(res, MAX_LEN - 1, "%d-%02d-%02d %02d:%02d:%02d", lt.tm_year + 1900,
      lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);

  return res;
}

bool UnionAdCache::UpdateNuomiAd(Ice::Long goods_id, Ice::Int tpl_type, const string & title, 
    const string & content, const string & img_url, const string & dest_url) {
  Ice::Long ad_id = goods_id + 1000008880000100000L + (10000 * tpl_type);
  AdTplPtr ad_tpl = GetAdTpl(tpl_type);
  if (!ad_tpl) {
    return false;
  }
  return ad_tpl->UpdateNuomiAd(ad_id, title, content, img_url, dest_url);
}

bool AdTpl::UpdateNuomiAd(Ice::Long ad_id, const string & title, 
    const string & content, const string & img_url, const string & dest_url) {
  AdCache::iterator it_ad = ad_cache_.find(ad_id);
  UnionAdPtr ad = NULL;

  if (dest_url == "del") {
    MCE_INFO("nuomi ad " << ad_id << " removed!");
    if (it_ad != ad_cache_.end()) {
      SuspendAd(it_ad->second);
    }
    return true;
  }

  if (it_ad == ad_cache_.end()) {
    MCE_INFO("nuomi ad " << ad_id << " created!");
    ad = new UnionAd;
    ad_cache_[ad_id] = ad;

    ad->id = ad_id;
    ad->type = kNuomiType;
    ad->name = title;
    ad->owner_id = 0;
    ad->budget = 1;
    ad->daily_budget = 1;
    ad->click_price = 0;
    ad->click_count = 0;

    ad->title = title;
    ad->content = content;

    ad->img_url = img_url;
    ad->dest_url = xce::notify::UrlEncode(dest_url);

    ad->group_id = 0;
    ad->campaign_id = 0;

    ad->tpl_type = tpl_type_;
    {
      IceUtil::RWRecMutex::RLock lock(index_mutex_);
      ad_type_index_[kNuomiType].push_back(ad_id);
    }
  } else {
    ad = it_ad->second;
    MCE_INFO("nuomi ad " << ad->id << " updated!");
    {
      IceUtil::RWRecMutex::WLock lock(ad->mutex_);
      ad->title = title;
      ad->content = content;

      ad->img_url = img_url;
      ad->dest_url = xce::notify::UrlEncode(dest_url);
    }

    bool found = false;
    {
      IceUtil::RWRecMutex::RLock lock(index_mutex_);
      for (size_t i = 0; i < ad_type_index_[kNuomiType].size(); ++i) {
        if (ad_type_index_[kNuomiType][i] == ad_id) {
          found = true;
          break;
        }
      }
    }
    if (!found) {
      MCE_INFO("nuomi ad " << ad->id << " reused!");
      IceUtil::RWRecMutex::WLock lock(index_mutex_);
      ad_type_index_[kNuomiType].push_back(ad_id);
    }
  }
  return true;
}

void UnionAdCache::ClickAd(Ice::Long ad_id, Ice::Int tpl_type, Ice::Int zone_id, Ice::Int site_id, const string & ip, bool cheat) {
  AdTplPtr ad_tpl = GetAdTpl(tpl_type);
  if (ad_tpl) {
    ad_tpl->ClickAd(ad_id, zone_id, site_id, ip, cheat);
  } else {
    MCE_WARN("TPL type " << tpl_type << " not found!");
  }
}

void AdTpl::ClickAd(Ice::Long ad_id, Ice::Int zone_id, Ice::Int site_id, const string & ip, bool cheat) {
  if (ad_id == kSearchAdId) {
    AD_INFO(kSearchAdId << ',' << kSearchAdId << ','
        << kSearchAdId << ',' << kSearchAdId << ',' << zone_id << ','
        << site_id << ',' << site_id << ',' << zone_id << ','
        << '0' << ',' << FormatedClickTime(time(NULL)) << ',' << '0' << ','
        << '0' << ',' << "1" << ',' << "lm" << ','
        << "0," << kSearchAdId << ',' << ip << ',' << kSearchAdId << "|0|1970-01-01|40|4|0|0_0|0|0|0|0086000000000000" << ',' << cheat);

    MCE_INFO("ad " << ad_id << " click");
    return;
  }

  bool budget_out = false;
  Ice::Int click_count = 0;

  AdCache::iterator it_ad = ad_cache_.find(ad_id);
  if (it_ad == ad_cache_.end()) {
    MCE_WARN("UnionAd::Click --> ad " << ad_id << " not found!");
    return;
  } 
  UnionAdPtr ad = it_ad->second;

  {
    IceUtil::RWRecMutex::WLock lock(ad->mutex_);
    click_count = ++ ad->click_count;
    if (ad->click_price * click_count >= ad->budget) {
      budget_out = true;
    }
  }

  if (budget_out) {
    MCE_INFO("ad " << ad_id << " budget out.");
    SuspendAd(ad);
  }

  AD_INFO(ad->id << ',' << ad->group_id << ','
      << ad->campaign_id << ',' << ad->owner_id << ',' << zone_id << ','
      << site_id << ',' << site_id << ',' << zone_id << ',' 
      << ad->click_price << ',' << FormatedClickTime(time(NULL)) << ',' << ad->click_price << ','
      << ad->click_price << ',' << "1" << ',' << "lm" << ','
      << "0," << ad->id << ',' << ip << ',' << ad->owner_id << "|0|1970-01-01|40|4|0|0_0|0|0|0|0086000000000000" << ',' << cheat);

  MCE_INFO("ad " << ad_id << " clicked " << click_count);

  TaskManager::instance().execute(new ClickSyncTask(ad_id, click_count, 0));
}

void UnionAdCache::GetAd(Ice::Int ad_type, Ice::Int tpl_type, Ice::Int slot, int ad_count, const string & dest_appendix, string * html,
   Ice::Int zone_id, Ice::Int site_id, const string & ip, const string & http_referer) {
  AdTplPtr ad_tpl = GetAdTpl(tpl_type);
  if (ad_tpl) {
    ad_tpl->GetAd(ad_type, slot, ad_count, dest_appendix, html, zone_id, site_id, ip, http_referer);
  } else {
    MCE_INFO("ad tpl_type " << tpl_type << " not found!");
  }
}

void AdTpl::GetAd(Ice::Int ad_type, Ice::Int slot, int ad_count, const string & dest_appendix, string * html,
   Ice::Int zone_id, Ice::Int site_id, const string & ip, const string & http_referer) {
  if (tpl_type_ == AdTpl::TPL_TYPE_IMAGE) {
    ad_count = 1;
  }

  MCE_DEBUG("GetAd type : " << ad_type << " tpl=" << tpl_type_);

  ctemplate::TemplateDictionary dict("");

  size_t cur_count = 0;
  vector<Ice::Long> ads;
  if (tpl_type_ != TPL_TYPE_RENREN_SEARCH) {
    IceUtil::RWRecMutex::RLock lock(index_mutex_);
    AdTypeIndex::const_iterator it = ad_type_index_.find(ad_type);

    for(int i = 0; i < ad_count; ++i) {
      Ice::Long ad_id = 0;
      if (it == ad_type_index_.end() || it->second.size() <= cur_count) {
        vector<Ice::Long> & dft_ads = (ad_type == kNuomiType) ? default_nuomi_ads_ : default_ads_;
        if (dft_ads.empty()) {
          MCE_WARN("no default ad!");
          break;
        }
        MCE_INFO("Use default ad " << ad_id);
        ad_id = dft_ads[slot++ % dft_ads.size()];
      } else {
        ad_id = it->second[slot++ % it->second.size()];
        ++ cur_count;
      }

      MCE_INFO("ad " << ad_id << " displayed : " << dest_appendix);
      ads.push_back(ad_id);
    }
  } else {
    //dict.SetIntValue("SITE_ID", site_id);

    string av = "r=";
    av += boost::lexical_cast<string>(rand());
    av += dest_appendix;
    av += "&aid=";
    av += boost::lexical_cast<std::string>(kSearchAdId);

    //AesWrapper aw((const unsigned char *)g_aes_key, g_aes_key_len);
    //Ice::Int sz = av.size();
    //unsigned char * ciphertext = aw.Encrypt((unsigned char *)av.c_str(), &sz);
    //av = url_base64_encode(reinterpret_cast<const unsigned char *>(ciphertext), sz);
    //free(ciphertext);

    dict.SetValue("AD_CIPHER", xce::ad::UrlEncrypt(av.c_str(), av.size()));
    MCE_INFO("ad " << kSearchAdId << " displayed : " << dest_appendix);
  }

  MCE_DEBUG("AdTpl " << tpl_type_ << " get ad : " << ads.size() << '/'<< ad_count);

  for(size_t i = 0; i < ads.size(); ++i) {
    BuildSubDict(ads[i], dest_appendix, &dict, zone_id, site_id, ip, http_referer);
  }

  if (template_) {
    template_->Expand(html, &dict);
  } else {
    MCE_DEBUG("template_ NULL!");
  }
}

bool AdTpl::BuildSubDict(Ice::Long ad_id, const string & appendix, ctemplate::TemplateDictionary* dict, 
    Ice::Int zone_id, Ice::Int site_id, const string & ip, const string & http_referer) const {
  AdCache::const_iterator it_ad = ad_cache_.find(ad_id);
  if (it_ad == ad_cache_.end()) {
    MCE_WARN("UnionAd::BuildSubDict --> object of ad " << ad_id << " not found!");
    return false;
  }

  //ctemplate::TemplateDictionary* sub_dict = dict->AddIncludeDictionary("AD_ITEMS");
  //sub_dict->SetFilename(kBasicTplFile);
  ctemplate::TemplateDictionary* sub_dict = dict->AddSectionDictionary("AD_ITEMS");

  const UnionAdPtr ad = it_ad->second;

  string av = "r=";
  av += boost::lexical_cast<string>(rand());
  av += appendix;
  av += "&aid=";
  av += boost::lexical_cast<string>(ad->id);

  AesWrapper aw((const unsigned char *)g_aes_key, g_aes_key_len);
  Ice::Int sz = av.size();
  unsigned char * ciphertext = aw.Encrypt((unsigned char *)av.c_str(), &sz);
  av = url_base64_encode(reinterpret_cast<const unsigned char *>(ciphertext), sz);
  free(ciphertext);

  sub_dict->SetValue("AD_CIPHER", av);
  sub_dict->SetValue("AD_TITLE", ad->title);
  sub_dict->SetValue("AD_CONTENT", ad->content);
  sub_dict->SetValue("AD_IMG_URL", ad->img_url);
  sub_dict->SetValue("AD_DEST_URL", ad->dest_url);
 
  AD_UNION_PV_INFO("ad_union_gate@10.3.21.45" << "||" << "10.3.21.45" << "||" << FormatedClickTime(time(NULL))
      << "||\"GET /union/get.html?userid=0&tt=0&type=.html&adzoneid=" << zone_id << "&count=0&rowoffset=0&ip=" 
        << ip << "&jbc=cookie|a|b&ubc=3000000_0|1|1970-01-01|30|4|0086000000000000|-|-|100101101|88888_1|0000000000000000 HTTP/1.0\"||200"
      << "||undefined||undefined||undefined||" << "2||" << "0"
      << "||none"
      << "||" << http_referer 
      << "||" << zone_id << "^B^B^B^B^B^B^B"
      << "||" << ad->id << "^B" << ad->owner_id << "^B" << ad->id << "^B" << 0 /*ad->click_price*/ << "^B" 
        << 0/*ad->click_price*/ << "^B" << ad->dest_url << "^B1" << "^B2^B" 
      << "||1"
      );
  return true;
}

void ClickSyncTask::handle() {
  Statement sql;
  sql << "UPDATE ad_union_ads SET click_count=" << click_count_ << " WHERE id=" << ad_id_; 
  try {
    mysqlpp::SimpleResult res = QueryRunner(kDbInstance, CDbWServer, "ad_union_ads").execute(sql);
  } catch (mysqlpp::Exception& e) {
    MCE_WARN("ClickSyncTask sql error --> " << e.what());
  } catch (...) {
    MCE_WARN("ClickSyncTask sql unknown error.");
  }
}

void ReloadNuomiCreativeTask::handle(){
  Statement sql;
  sql << "select * from ad_union_nuomi_creative";

  try {
    mysqlpp::StoreQueryResult res = QueryRunner(kDbInstance, CDbRServer).store(sql);

    if (!res) {
      MCE_WARN("select table ad_union_nuomi_creative error.");
      return ;
    }

    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long id = (Ice::Long) row["id"];
      sale_count_ = (Ice::Int) row["sale_count"];
      string title = row["title"].c_str();
      string content = row["content"].c_str();
      string img_url = row["img_url"].c_str();
      string dest_url = xce::notify::UrlEncode(row["dest_url"].c_str());
      string update_time = row["update_time"].c_str();

      ostringstream os;
      os << content << " [" << sale_count_ << "人已购买]";

      UnionAdCache::instance().UpdateNuomiCreative(id,title,os.str(),img_url,dest_url);
      if (Date(update_time).time() > creative_update_time_) {
        Statement sql;
        sql << "update ad_union_ads set title=" << mysqlpp::quote << title << ", content = " << mysqlpp::quote << content
            << ", img_url = " << mysqlpp::quote << img_url << ", dest_url = " << mysqlpp::quote << dest_url
            << " where update_time < '" << update_time << "' and id = " << id;
        QueryRunner(kDbInstance, CDbWServer).store(sql);
        creative_update_time_ = MyUtil::Date(update_time).time();
      }
    }
  } catch (std::exception& e) {
    MCE_WARN("load db err : " << e.what());
  } catch (...) {
    MCE_WARN("load db unknown err.");
  }
}

