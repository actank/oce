/*
 * AdUrlUtil.cpp
 *
 *  Created on: 2010-9-13
 *      Author: ark
 */

#include "AdUrlUtil.h"
//#include "boost/lexical_cast.hpp"
#include "feed/FeedMemcProxy/client/user_profile_client.h"
#include "Date.h"
#include "AdEngine.h"
#include "ad/enginejclient/src/engine_types.h"
#include "ad/enginejclient/src/AntiFraudClient.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include "AdLogSAdapter.h"
#include "AdData.h"
#include "AdUrlMaping.h"
#include <connection.h>
#include <query.h>

namespace xce {
namespace ad {
using namespace std;

//const string HOST[] = {"60.29.241.116", "60.29.241.121", "60.29.241.123", "60.29.241.124", "60.29.241.231", "60.29.241.232", "60.29.241.233"};
//const string HOST[] = {"60.29.241.116", "60.29.241.121", "60.29.241.123", "60.29.241.124"};
const string HOST[] = {"60.29.242.224"};

string FormatedClickTime()
{
  time_t t = time(NULL);
  struct tm lt;
  localtime_r(&t, &lt);

  const int MAX_LEN = 24;
  char res[MAX_LEN];
  res[MAX_LEN - 1] = 0;
  snprintf(res, MAX_LEN - 1, "%d-%02d-%02d %02d:%02d:%02d", lt.tm_year + 1900,
      lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
  return res;
}
AdGroupInfo AdUrlUtil::SelectDefaultTaobao(vector<AdGroupInfo>& taobao_list){
  AdGroupInfo result;
  if(taobao_list.empty()){
    return result;
  }
  int rand_index = rand()% taobao_list.size();
  result = taobao_list[rand_index];
  return result;
}
AdGroupInfo AdUrlUtil::SelectDefaultAd(set<Ice::Long>& competed_groups, vector<AdGroupInfo>&default_list){
  AdGroupInfo result;
  for(vector<AdGroupInfo>::iterator it = default_list.begin(); it != default_list.end(); ++it){
    Ice::Long group_id = (*it).creatives[0] / 100000;
    set<Ice::Long>::iterator itc = competed_groups.find(group_id);
    if(itc != competed_groups.end()){//已经存在，不能再次选入
      continue;
    } else{
      result = (*it);
      break;
    }
  }
  return result;
}

//text格式：  index_key_creative_bidid_cost_pos_maxprice_pv_click_pvtime_ubc_appendinfo
//           tw    0    xxx     xxx   xxx  xxx   xxx    0    0   xxx    xxx   xxx
AdResultMap AdUrlUtil::Get(const string& userProfile,
                         const string& referer_nextloadtime_refreshidx,
                         const int server_index) {

  AdResultMap result_map;
  UserProfile profile;
  UserProfileClient::instance().Deserialize(userProfile, &profile);
  //get ubc
  int uid = profile.id();
  if(uid <= 0) {
    uid = 0;
  }
  int ip = profile.ip();
  int age = profile.age();
  int gender = profile.gender();
  time_t now;
  now = time(NULL);
  int this_year = Date(now).year();
  int birth_year = this_year - age;
  int stage = profile.stage();
  string current_area = profile.current_area();
  if (current_area.empty()) {
    current_area = "-";
  }
  string ip_area = profile.ip_area();
  //MCE_INFO("Enter AdUrlUtil::Get() uid = " << uid << ", ip_area = " << ip_area);
  //如果没有查出ip，则使用全国
  if (ip_area.empty() || "0000000000000000" == ip_area) {
    ip_area = "0086000000000000";
  }
  Ice::Long school = profile.school();
  int grade = profile.grade();
  string valid_area;
  if (stage == 2) {
    valid_area = current_area;
  } else {
    valid_area = ip_area;
  }
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  string referer, next_load_time, refresh_idx;
  vector<string> ref_load_refresh;
  boost::algorithm::split_regex(ref_load_refresh, referer_nextloadtime_refreshidx, boost::regex("\\^C"));
  if (ref_load_refresh.size() >= 3) {
    referer = ref_load_refresh[0];
    next_load_time = ref_load_refresh[1];
    refresh_idx = ref_load_refresh[2];
  } else if (ref_load_refresh.size() >= 1) {
    referer = ref_load_refresh[0];
  }

  ostringstream ubc_oss;
  ubc_oss << "1000000_" << uid << "|" << gender << "|" << birth_year
      << "-01-01" << "|" << age << "|" << stage << "|" << valid_area << "|"
      << school << "_" << current_area << "|" << grade << "|0|0|" << ip_area;
  string ubc = ubc_oss.str();
  /*string referer_copy = "\"" + referer + "\"";
  for (int i = 0; i < referer_copy.length(); i++) {
    if (referer_copy[i] == '|') {
      referer_copy[i] = '-';
    }
  }*/
  string referer_copy;
  UrlEncode(referer, referer_copy);
  int a, b, c, d; //将整数ip转化成string
  unsigned ip_u = (unsigned) ip;
  a = ip_u / 16777216;
  b = (ip_u - a * 16777216) / 65536;
  c = (ip_u - a * 16777216 - b * 65536) / 256;
  d = (ip_u - a * 16777216 - b * 65536 - c * 256);
  ostringstream ip_oss;
  ip_oss << a << "." << b << "." << c << "." << d;
  string host = HOST[server_index];
  int timeout = AdTwConfig::instance().timeout();
  //得到广告位
  set<Ice::Long> adzone_ids;
  AdUrlMaping::instance().GetAdZone(referer, profile.stage(), adzone_ids);
  set<TwZonePtr> zone_counts;
  TwZonePool::instance().GetAdZone(adzone_ids, zone_counts);
  //循环请求多个广告位
  //MCE_INFO("before curl_easy_init url = " << referer);
  CURL* handle = curl_easy_init();
  //MCE_INFO("after curl_easy_init url = " << referer << ", zone size = " << zone_counts.size());
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
  //以毫秒为单位的超时
  curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, timeout);
  map<TwZonePtr, string> data_map;
  for(set<TwZonePtr>::iterator zone_iter = zone_counts.begin();
      zone_iter != zone_counts.end(); ++zone_iter) {
    Ice::Long adzone_id = (*zone_iter)->id();
    int count = (*zone_iter)->count();
    string data;
    //string url= "http://60.29.241.122/bwad/ADIDViewer.aspx?adzoneid=100000000001&count=3&url=http%3a%2f%2fhk.news.yahoo.com%2f080218%2f12%2f2p1nn.html&ubc=1000000_728593419|0|1987-1-1|23|4|0086000000000000|-|-|-|88888_1|0000000000000000";
    ostringstream url_oss;

    url_oss << "http://" << host << "/bwad/ADIDViewer.aspx?adzoneid="
        << adzone_id << "&count=" << count <<"&url="
        << referer << "&ubc=" << ubc;
    string url = url_oss.str();

    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

    TimeStat ts;
    CURLcode success;
    success = curl_easy_perform(handle);
    MCE_INFO("[CURL] curl_easy_perform use time: " << ts.getTime() << " ms.");
    MCE_INFO("try to get: " << url << ", data from tw server: return = " << success << ", data = " << data);
    if (success != 0) {
      continue;
    } else {
      data_map[*zone_iter] = data;
    }
  }

  if(handle) {
    curl_easy_cleanup(handle);
  }
  vector<string> log_seq;
  for(map<TwZonePtr, string>::iterator data_iter = data_map.begin();
      data_iter != data_map.end(); ++data_iter) {
    // process the http result
    //10000057480001^55^1_Ag_161_201009250951|10000057570001^59^1_Ag_161_201009250951|10000057560001^58^1_Ag_161_201009250951|10000057550001^52^1_Ag_161_201009250951|10000057490001^55^1_Ag_161_201009250951|10000057710001^50^spare|10000057700001^50^spare|10000036870003^50^spare|10000036870001^50^spare|10000036870002^50^spare
    Ice::Long adzone_id = data_iter->first->id();
    int count = data_iter->first->count();

    ostringstream http_oss;
    http_oss << "/show?userid=" << uid << "&tt=0&type=.js&adzoneid="
      << adzone_id << "&count=" << count;
    if (!next_load_time.empty()) {
      http_oss << "&next_load_time=" << next_load_time;
    }
    if (!refresh_idx.empty()) {
      http_oss << "&refresh_idx=" << refresh_idx;
    }
    http_oss << "&rowoffset=0&ip=" << ip_oss.str() << "&jbc=cookie|a|b&ubc=" << ubc;

    string data = data_iter->second;
    AdInfoSeq result;

    boost::char_separator<char> sep("|");
    tokenizer tokens(data, sep);
    int pos = 1;
    MyUtil::Date date(time(NULL));
    vector<AdGroupInfoAppend> selected_groups;
    set<Ice::Long> competed_groups;

    Ice::Long group_id = 0;
    Ice::Long bid_unit_id = 0;
    Ice::Long creative_id = 0;
    Ice::Long member_id = 0;
    int max_price = 0, price = 0;
    /////////////////////////////////////////////////////////////
    for(tokenizer::iterator tok_iter = tokens.begin();
        tok_iter != tokens.end() && pos <= count;
        ++tok_iter) {
      ostringstream log;
      string one_group = *tok_iter;
      boost::char_separator<char> sep2("^");
      tokenizer tokens2(one_group, sep2);
      Ice::Long group_id = 0;
      Ice::Long bid_unit_id = 0;
      Ice::Long creative_id = 0;
      Ice::Long member_id = 0;
      Ice::Long widget_id = 0;
      int max_price = 0, price = 0;
      int num = 0;
      string append_info;
      for (tokenizer::iterator tok_iter2 = tokens2.begin();
          tok_iter2 != tokens2.end(); ++tok_iter2, ++num) {
        if (num == 0) {//group_id
          group_id = boost::lexical_cast<Ice::Long>(*tok_iter2);
          AdGroupInfo info = AdGroupCache::instance().get(group_id);
          bid_unit_id = info.bid_unit;
          max_price = info.max_price;
          member_id = info.member_id;
          widget_id = info.widget_id;

          if (max_price == 0 || bid_unit_id == 0) {
            //not active
            creative_id = 0;
            //break;
          } else {
            creative_id = info.creatives[0];
          }
        } else if (num == 1) {//price
          price = boost::lexical_cast<int>(*tok_iter2);
        } else if (num == 2) {// append_info
          append_info = (*tok_iter2);
          if (max_price == 0 && bid_unit_id == 0) {
            append_info += "_NOTFOUND";
          }
        }
      }
      //如果价格大于出价，取出价作为价格
      if(max_price != 0 && price > max_price) {
        price = max_price;
      }
      
      //判定当前广告类型
      int type = Widget2TypeCache::instance().GetWidgetType(widget_id);
      //MCE_INFO("AdUrlUtil:: ad: " << type <<",widget_id:" << widget_id << ",groupid:" << group_id <<",count:" << count);
      if((2 == type) && (100000000072 == adzone_id)) {//如果是VIP客户，出的个数为1
        count = 1 ;
        MCE_INFO("AdUrlUtil::Get vip:" << type <<",widget_id:" << widget_id << ",groupid:" << group_id <<",count:" << count);
      }
      //台湾的pvlog里，如果是NOTFOUND，使用初始值0,否则使用真实值
      int tw_pos = 0;
      if (num == 3 && bid_unit_id != 0) {
        if (!LowQualityAdCache::instance().IsLowQualityAd(group_id)) {
          competed_groups.insert(group_id);

          ostringstream text_oss;
          //text格式：  index^key^creative^bidid^cost^pos^maxprice^member^adzone_id^pvtime^ubc^appendinfo
          //           tw    0    xxx     xxx   xxx  xxx   xxx     xxx     xxx    xxx   xxx   xxx
          text_oss << "tw^0^" << creative_id << "^" << bid_unit_id << "^"
              << price << "^" << pos << "^" << max_price << "^" << member_id
              << "^" << adzone_id << "^" << now << "^" << ubc << "^"
              << append_info;
          if (!next_load_time.empty()) {
            text_oss << "^" << next_load_time;
          } else {
            text_oss << "^0";
          }
          if (!refresh_idx.empty()) {
            text_oss << "^" << refresh_idx;
          } else {
            text_oss << "^0";
          }

          AdInfo ad;
          ad.creativeid = creative_id;
          ad.text = text_oss.str();
          MCE_INFO("ad: pos = " << pos << ", adgroup_id = "
              << group_id << ", creative_id = " << creative_id
              << ", text = " << ad.text);
          result.push_back(ad);
          tw_pos = pos++;

        } else {//低质量广告
          
          append_info += "_LOWQUALITY";//低质量广告，LOWQUALITY
        }

      } else if (num != 3) {
        //该group有问题，跳过pv
        MCE_INFO("tw return bad data: " << data << ", bad group info format: " << one_group);
        continue;
      }
      //数据正常但有可能不在线，需要打pv_log，但是不返回
      log << AdTwConfig::instance().server_name() << "||" << ip_oss.str() << "||" << date.toDateTime()
            << "||\"GET " << http_oss.str() << " HTTP/1.0\"||200||undefined||undefined||undefined||2||1||none||"
            << referer_copy << "||" << adzone_id << "^B^B^B^B^B^B^B||" << creative_id
            <<  "^B" << member_id << "^B" << bid_unit_id << "^B0^B" << max_price << "^B" << referer_copy
            << "^B" << tw_pos << "^B1^B2||4||" << append_info;

      log_seq.push_back(log.str());
      if (pos > count) {
        break;
      }
    }

    //////补默认广告
    int need_count = count - result.size();
    if (need_count > 0 && adzone_id != 100000000001) { //如果个数不够补默认广告
      vector<AdGroupInfo> default_list;
      AdGroupCache::instance().GetDefaultList(default_list);
      for (int i = 0; i < need_count; ++i) {
        ostringstream log;
        int tw_pos = 0;
        AdGroupInfo replace_gid =
            SelectDefaultAd(competed_groups, default_list); //选择默认广告，保证于已经存在的默认广告不同
        //////////////////////////////////////////////////
        if (replace_gid.member_id > 0) {
          
          creative_id = replace_gid.creatives[0];
          member_id = replace_gid.member_id;
          bid_unit_id = replace_gid.bid_unit;
          price = 0;
          max_price = 0;
          string append_info = "DEFAULT";
          Ice::Long group_id = creative_id / 100000;
          competed_groups.insert(group_id);
          //替换广告
          ostringstream text_oss;
          //text格式：  index^key^creative^bidid^cost^pos^maxprice^member^adzone_id^pvtime^ubc^appendinfo
          //           tw    0    xxx     xxx   xxx  xxx   xxx     xxx     xxx    xxx   xxx   xxx
          text_oss << "tw^0^" << creative_id << "^" << bid_unit_id << "^"
              << price << "^" << pos << "^" << max_price << "^" << member_id
              << "^" << adzone_id << "^" << now << "^" << ubc << "^"
              << append_info;
          if (!next_load_time.empty()) {
            text_oss << "^" << next_load_time;
          } else {
            text_oss << "^0";
          }
          if (!refresh_idx.empty()) {
            text_oss << "^" << refresh_idx;
          } else {
            text_oss << "^0";
          }
          AdInfo ad;
          ad.creativeid = creative_id;
          ad.text = text_oss.str();
          MCE_INFO("ad: replace pos = " << pos << ", adgroup_id = "
              << group_id << ", creative_id = " << creative_id
              << ", text = " << ad.text);
          result.push_back(ad);
          tw_pos = pos++;
          //////////////////////////////////////////////////
          log << AdTwConfig::instance().server_name() << "||" << ip_oss.str()
              << "||" << date.toDateTime() << "||\"GET " << http_oss.str()
              << " HTTP/1.0\"||200||undefined||undefined||undefined||2||1||none||"
              << referer_copy << "||" << adzone_id << "^B^B^B^B^B^B^B||"
              << creative_id << "^B" << member_id << "^B" << bid_unit_id
              << "^B0^B" << max_price << "^B" << referer_copy << "^B" << tw_pos
              << "^B1^B2||4||" << append_info;
          log_seq.push_back(log.str());
          if (pos > count) {
            break;
          }
        }
      }
    }
    result_map[adzone_id]=result;
  } // end for 每个广告位
  try {
    AdLogAdapter::instance().PvBatch(log_seq);
  } catch(Ice::Exception& e) {
    MCE_WARN("AdLogAdapter::Pv error : " << e << " uid=" << uid);
  } catch(std::exception& e) {
    MCE_WARN("AdLogAdapter::Pv error : " << e.what() << " uid=" << uid);
  }
  return result_map;
} //end Get

//text格式：  index^key^creative^bidid^cost^pos^maxprice^pv^click^pvtime^ubc^appendinfo
//           tw    0    xxx     xxx   xxx  xxx  xxx     0    0   xxx    xxx   xxx
void AdUrlUtil::Click(int uid,
    const string& ip,
    const string& text,
    const string& referer,
    const string& click_url) {

  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep("^");
  tokenizer tok(text, sep);
  int num = 0;
  //get info from parameter: text
  string append_info;
  MCE_INFO("click text = " << text);
  Ice::Long group_id = 0;
  Ice::Long bid_unit_id = 0;
  Ice::Long creative_id = 0;
  Ice::Long member_id = 0;
  Ice::Long adzone_id = 0;
  int max_price = 0;
  int pos = 0;
  int cost = 0;
  string ubc;
  string next_load_time, refresh_idx;
  MyUtil::Date date(time(NULL));
  string pv_time;
  //text格式：  index^key^creative^bidid^cost^pos^maxprice^member^adzone_id^pvtime^ubc^appendinfo
  //           tw    0    xxx     xxx   xxx  xxx  xxx      xxx     xxx   xxx    xxx   xxx
  //text = tw_0_1000004590000100001_1000004590000100001_100_2_100_0_0_1284634831_1000000_287341024|1|1982-01-01|28|4|0000000000000000|0_0086000000000000|0|0|0|0000000000000000_1_Ag_4544_201009161838
  //ubc = 1000000_287341024|1|1982-01-01|28|4|0000000000000000|0_0086000000000000|0|0|0|0000000000000000
  //append_info = 1_Ag_4544_201009161838
  for(tokenizer::iterator tok_iter = tok.begin(); tok_iter != tok.end(); ++tok_iter, ++num) {
    if (num == 2) {//creative_id
      creative_id = boost::lexical_cast<Ice::Long>(*tok_iter);
      group_id = creative_id / 100000;
    } else if (num == 3) {//bid_unit_id
      bid_unit_id = boost::lexical_cast<Ice::Long>(*tok_iter);
    } else if (num == 4) { //cost
      cost = boost::lexical_cast<int>(*tok_iter);
    } else if (num == 5) {
      pos = boost::lexical_cast<int>(*tok_iter);
    } else if (num ==6) {
      max_price = boost::lexical_cast<int>(*tok_iter);
    } else if (num == 7) {
      member_id = boost::lexical_cast<Ice::Long>(*tok_iter);
    } else if (num == 8) {
      adzone_id = boost::lexical_cast<Ice::Long>(*tok_iter);
    } else if (num == 9) {//pvtime
      pv_time = *tok_iter;
    } else if (num == 10) {//ubc
      ubc = *tok_iter;
      if (uid == 0) {
        //如果没有 uid，则从cookie里边取
        //287341024|1|1982-01-01|28|4|0000000000000000|0
        string sub_ubc = *tok_iter;
        boost::char_separator<char> sub_sep("|");
        tokenizer sub_tok(sub_ubc, sub_sep);
        if (sub_tok.begin() != sub_tok.end()) {
          //注意，改变的是形式参数
          uid = boost::lexical_cast<int>(*sub_tok.begin());
        }
      }
    } else if (num == 11) {//append_info
      append_info = *tok_iter;
    } else if (num == 12) {//next_load_time
      next_load_time = *tok_iter;
    } else if (num == 13) {//refresh_idx
      refresh_idx = *tok_iter;
    }
  }

  bool fraud_flag = false; //默认为真实点击
// construct ClickAction  for AntiFraudServer 
/*
  ClickAction clickAction ;
  clickAction.userId = uid;
  clickAction.ip = ip;
  clickAction.referer = referer;
  clickAction.clickUrl = click_url;
  clickAction.context = text;
  AntiFraudClient antiFraudClient("10.7.18.59",9096); // ad45
  bool fraud_flag = false; //默认为真实点击
  try{
    if (!antiFraudClient.check(clickAction)){
      fraud_flag = true;
    }
    MCE_INFO("antifraud response: " << " uid = " << uid << " zone_id = "<<adzone_id<<" ip = " << ip << (fraud_flag ? " fraud-click" : " true-click"));
  }catch(std::exception& e){
    MCE_WARN("Call Antifraud Service Error : " << e.what());
  }
*/
// antifauder call end!

  ostringstream clicklog, chargelog;
  chargelog << creative_id << ',' << group_id << ','
      << /*campaign_id*/group_id/10000 << ',' << member_id << ',' << adzone_id
      << ",0,0," << adzone_id << ',' << cost << ',' << FormatedClickTime()
      << ',' << cost << ',' << cost << ",1,lm," << AdTwConfig::instance().sys_min_price() << "," << bid_unit_id << ',' << ip
      << ',' << uid << ',' << (fraud_flag ? 1 : 0);
  MCE_DEBUG("chargelog = " << chargelog.str());
  MyUtil::Date pv_date(boost::lexical_cast<time_t>(pv_time));
    char url_sep = '&';
    if (click_url.find_first_of('?') == string::npos) {
      url_sep = '?';
    }
  ostringstream log_url;
  string encoded_referer;
  UrlEncode(referer, encoded_referer);
  log_url << click_url << url_sep << "next_load_time=" << next_load_time
          << "&refresh_idx=" << refresh_idx;
  string encoded_click_url;
  UrlEncode(log_url.str(), encoded_click_url);
  clicklog << creative_id << ',' << MyUtil::Date(time(NULL)).toDateTime() << "," << bid_unit_id
        << ',' << cost << ',' << ip << ',' << adzone_id << ",1000000_" << uid << ',' << ubc << ','
        << encoded_click_url << ',' << pos << ",-," << member_id << ",," << max_price << ",2," << encoded_referer << ",,"
        << pv_date.toDateTime() << ',' << uid << ",2,1,1," << ip << ",,," << max_price << ",1,lm,"<< AdTwConfig::instance().sys_min_price() << ","
        << bid_unit_id << ","<< (fraud_flag ? 1 : 0) <<",1,4," << append_info;
  MCE_DEBUG("clicklog = " << clicklog.str());
  try{
    AdLogAdapter::instance().ClickForBp(clicklog.str());
    MCE_INFO("[MAIN_PROCESS] AdUrlUtil::Click --> ClickForBp, clicklog = " << clicklog.str());
  } catch(Ice::Exception& e){
    MCE_WARN("AdUrlUtil::Click --> call AdLogAdapter.ClickForBp ERR  line:"<<__LINE__<<" err:"<<e);
  } catch(std::exception& e){
    MCE_WARN("AdUrlUtil::Click --> call AdLogAdapter.ClickForBp ERR  line:"<<__LINE__<<" err:"<<e.what());
  }

  try{
    AdLogAdapter::instance().ClickForCharge(chargelog.str());
    MCE_INFO("[MAIN_PROCESS] AdUrlUtil::Click --> ClickForCharge = " << chargelog.str());
  } catch(Ice::Exception& e){
    MCE_WARN("AdUrlUtil::Click --> call AdLogAdapter.ClickForCharge ERR  line:"<<__LINE__<<" err:"<<e);
  } catch(std::exception& e){
    MCE_WARN("AdUrlUtil::Click --> call AdLogAdapter.ClickForCharge ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
  }

//text格式：  index_key_creative_bidid_cost_pos_maxprice_pv_click_pvtime_ubc_appendinfo
//           tw    0    xxx     xxx   xxx  xxx   xxx    0    0   xxx    xxx   xxx

AdResultMap AdUrlUtil::GetTest(const Ice::Int uid,
                               const Ice::Int stage,
                               const Ice::Int gender,
                               const Ice::Int age,
                               const ::std::string& ipArea,
                               const Ice::Long school,
                               const Ice::Int grade,
                               const int count_not_use,
                               const string & referer_nextloadtime_refreshidx,
                               const int server_index) {

  //MCE_INFO("Enter new AdUrlUtil::Get() url = " << referer_nextloadtime_refreshidx);
  AdResultMap result_map;
  //UserProfile profile;
  //UserProfileClient::instance().Deserialize(userProfile, &profile);
  //get ubc
  //int uid = profile.id();
  //int ip = profile.ip();
  //int age = profile.age();
  //int gender = profile.gender();
  time_t now;
  now = time(NULL);
  int this_year = Date(now).year();
  int birth_year = this_year - age;
  /*//int stage = profile.stage();
  //MCE_DEBUG("uid = " << uid << ", ip = " << ip << ", age = " << age
  //    << ", gender = " << gender << ", this_year = " << this_year << ", birth_year = " << birth_year << ", stage = " << stage);
  //string current_area = profile.current_area();
  //if (current_area.empty()) {
  //  current_area = "-";
  //}
  string ip_area = profile.ip_area();
  if (ip_area.empty()) {
    ip_area = "-";
  }
  //Ice::Long school = profile.school();
  //int grade = profile.grade();
  string valid_area;
  if (stage == 2) {
    valid_area = current_area;
  } else {
    valid_area = ip_area;
  }*/
  string valid_area = ipArea;
  string current_area = ipArea;

  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  string referer, next_load_time, refresh_idx;
  vector<string> ref_load_refresh;
    boost::algorithm::split_regex(ref_load_refresh, referer_nextloadtime_refreshidx, boost::regex("\\^C"));
    if (ref_load_refresh.size() >= 3) {
      referer = ref_load_refresh[0];
      next_load_time = ref_load_refresh[1];
      refresh_idx = ref_load_refresh[2];
    } else if (ref_load_refresh.size() >= 1) {
      referer = ref_load_refresh[0];
    }

  ostringstream ubc_oss;
  ubc_oss << "1000000_" << uid << "|" << gender << "|" << birth_year
      << "-01-01" << "|" << age << "|" << stage << "|" << valid_area << "|"
      << school << "_" << current_area << "|" << grade << "|0|0|" << ipArea;
  string ubc = ubc_oss.str(); 
  string referer_copy = "\"" + referer + "\"";
  for (int i = 0; i < referer_copy.length(); i++) {
    if (referer_copy[i] == '|') {
      referer_copy[i] = '-';
    }
  }
  /*int a, b, c, d; //将整数ip转化成string
  unsigned ip_u = (unsigned) ip;

  a = ip_u / 16777216;
  b = (ip_u - a * 16777216) / 65536;
  c = (ip_u - a * 16777216 - b * 65536) / 256;
  d = (ip_u - a * 16777216 - b * 65536 - c * 256);
  ostringstream ip_oss;
  ip_oss << a << "." << b << "." << c << "." << d;
  if (a == 255 && b == 255 && c == 255 && d == 255) {
    MCE_INFO("[IP_DEBUG] : (unsigned)ip = " << ip_u << ", ip_oss = " << ip_oss.str());
  }*/
  ostringstream ip_oss;
  ip_oss << "127.0.0.1";
  string host = HOST[server_index];
  int timeout = AdTwConfig::instance().timeout();
  //得到广告位
  set<Ice::Long> adzone_ids;
  AdUrlMaping::instance().GetAdZone(referer, stage, adzone_ids);
  set<TwZonePtr> zone_counts;
  TwZonePool::instance().GetAdZone(adzone_ids, zone_counts);
  //循环请求多个广告位
  //MCE_INFO("before curl_easy_init url = " << referer);
  CURL* handle = curl_easy_init();
  //MCE_INFO("after curl_easy_init url = " << referer << ", zone size = " << zone_counts.size());
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
  //以毫秒为单位的超时
  curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, timeout);
  map<TwZonePtr, string> data_map;
  for(set<TwZonePtr>::iterator zone_iter = zone_counts.begin();
      zone_iter != zone_counts.end(); ++zone_iter) {
    Ice::Long adzone_id = (*zone_iter)->id();
    int count = (*zone_iter)->count();
    string data;
    //string url= "http://60.29.241.122/bwad/ADIDViewer.aspx?adzoneid=100000000001&count=3&url=http%3a%2f%2fhk.news.yahoo.com%2f080218%2f12%2f2p1nn.html&ubc=1000000_728593419|0|1987-1-1|23|4|0086000000000000|-|-|-|88888_1|0000000000000000";
    ostringstream url_oss;

    url_oss << "http://" << host << "/bwad/ADIDViewer.aspx?adzoneid="
        << adzone_id << "&count=" << count <<"&url="
        << referer << "&ubc=" << ubc;
    string url = url_oss.str();

    MCE_INFO("try to get: " << url << ", timeout = " << timeout);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

    TimeStat ts;
    CURLcode success;
    success = curl_easy_perform(handle);
    MCE_INFO("[CURL] curl_easy_perform use time: " << ts.getTime() << " ms.");
    MCE_INFO("data from tw server: return = " << success << ", data = " << data);
    if (success != 0) {
      continue;
    } else {
      data_map[*zone_iter] = data;
    }
  }

  if(handle) {
    curl_easy_cleanup(handle);
  }
  for(map<TwZonePtr, string>::iterator data_iter = data_map.begin();
      data_iter != data_map.end(); ++data_iter) {
    // process the http result
    //10000057480001^55^1_Ag_161_201009250951|10000057570001^59^1_Ag_161_201009250951|10000057560001^58^1_Ag_161_201009250951|10000057550001^52^1_Ag_161_201009250951|10000057490001^55^1_Ag_161_201009250951|10000057710001^50^spare|10000057700001^50^spare|10000036870003^50^spare|10000036870001^50^spare|10000036870002^50^spare
    Ice::Long adzone_id = data_iter->first->id();
    int count = data_iter->first->count();

    ostringstream http_oss;
    http_oss << "/show?userid=" << uid << "&tt=0&type=.js&adzoneid="
      << adzone_id << "&count=" << count;
    if (!next_load_time.empty()) {
      http_oss << "&next_load_time=" << next_load_time;
    }
    if (!refresh_idx.empty()) {
      http_oss << "&refresh_idx=" << refresh_idx;
    }
    http_oss << "&rowoffset=0&ip=" << ip_oss.str() << "&jbc=cookie|a|b&ubc=" << ubc;

    string data = data_iter->second;
    AdInfoSeq result(count, AdInfo());

    boost::char_separator<char> sep("|");
    tokenizer tokens(data, sep);
    int pos = 1;
    //vector<string> append_info_seq;
    //MCE_DEBUG("tokened");
    MyUtil::Date date(time(NULL));
    for(tokenizer::iterator tok_iter = tokens.begin();
        tok_iter != tokens.end() && pos <= count;
        ++tok_iter) {
      ostringstream log;
      string one_group = *tok_iter;
      boost::char_separator<char> sep2("^");
      tokenizer tokens2(one_group, sep2);
      Ice::Long group_id = 0;
      Ice::Long bid_unit_id = 0;
      Ice::Long creative_id = 0;
      Ice::Long member_id = 0;
      int max_price = 0, price = 0;
      int num = 0;
      string append_info;
      for (tokenizer::iterator tok_iter2 = tokens2.begin();
          tok_iter2 != tokens2.end(); ++tok_iter2, ++num) {
        if (num == 0) {//group_id
          group_id = boost::lexical_cast<Ice::Long>(*tok_iter2);
          AdGroupInfo info = AdGroupCache::instance().get(group_id);
          bid_unit_id = info.bid_unit;
          max_price = info.max_price;
          member_id = info.member_id;
          if (max_price == 0 || bid_unit_id == 0) {
            //not active
            creative_id = 0;
            //break;
          } else {
            creative_id = info.creatives[0];
          }
        } else if (num == 1) {//price
          price = boost::lexical_cast<int>(*tok_iter2);
        } else if (num == 2) {// append_info
          append_info = (*tok_iter2);
          if (max_price == 0 && bid_unit_id == 0) {
            append_info += "_NOTFOUND";
          }
        }
      }
      //MCE_DEBUG(__LINE__);
      //台湾的pvlog里，如果是NOTFOUND，使用初始值0,否则使用真实值
      int tw_pos = 0;
      if (num == 3 && bid_unit_id != 0) {
        ostringstream text_oss;
        //text格式：  index^key^creative^bidid^cost^pos^maxprice^member^adzone_id^pvtime^ubc^appendinfo
        //           tw    0    xxx     xxx   xxx  xxx   xxx     xxx     xxx    xxx   xxx   xxx
        text_oss << "tw^0^" << creative_id << "^" << bid_unit_id << "^"
            << price << "^" << pos << "^" << max_price << "^" << member_id
            << "^" << adzone_id << "^" << now << "^" << ubc << "^" << append_info;
        //MCE_DEBUG(__LINE__);
        if (!next_load_time.empty()) {
          text_oss << "^" << next_load_time;
        } else {
          text_oss << "^0";
        }
        if (!refresh_idx.empty()) {
          text_oss << "^" << refresh_idx;
        } else {
          text_oss << "^0";
        }

        AdInfo ad;
        ad.creativeid = creative_id;
        ad.text = text_oss.str();
        MCE_INFO("ad: replace pos = " << pos << ", adgroup_id = "
                 << group_id << ", creative_id = " << creative_id
                 << ", text = " << ad.text);
        //测试代码，调整tw广告返回的顺序
        if(adzone_id == 100000000001) {
          tw_pos = (pos+2)%count + 1;
        } else {
          tw_pos = pos;
        }
        result[(tw_pos-1)%result.size()] = ad;
        pos++;
        //result.push_back(ad);
        //tw_pos = pos++;
      } else if (num != 3) {
        //该group有问题，跳过pv
        MCE_INFO("tw return bad data: " << data << ", bad group info format: " << one_group);
        continue;
      }
      //数据正常但有可能不在线，需要打pv_log，但是不返回

      log << AdTwConfig::instance().server_name() << "||" << ip_oss.str() << "||" << date.toDateTime()
            << "||\"GET " << http_oss.str() << " HTTP/1.0\"||200||undefined||undefined||undefined||2||1||none||"
            << referer_copy << "||" << adzone_id << "^B^B^B^B^B^B^B||" << creative_id
            <<  "^B" << member_id << "^B" << bid_unit_id << "^B0^B" << max_price << "^B" << referer_copy
            << "^B" << tw_pos << "^B1^B2||4||" << append_info;
      /*try {
        AdLogAdapter::instance().Pv(log.str());
        MCE_INFO("PVLOG");
      } catch(Ice::Exception& e) {
        MCE_WARN("AdLogAdapter::Pv error : " << e << " uid=" << uid);
      } catch(std::exception& e) {
        MCE_WARN("AdLogAdapter::Pv error : " << e.what() << " uid=" << uid);
      }*/

      MCE_DEBUG("pv log: " << log.str());
      //MCE_DEBUG("after push_back");
      if (pos > count) {
        break;
      }
      //MCE_DEBUG(__LINE__);
    }

    result_map[adzone_id]=result;
  } // end for 每个广告位
  return result_map;
} //end Get

bool AdUrlUtil::UrlEncode(const string& input_url, string& output_url) {
  if(AdTwConfig::instance().url_encode()) {
    CURL *handle = curl_easy_init();
    char *encodedURL = curl_easy_escape(handle, input_url.c_str(), 0);
    if (encodedURL){
      MCE_DEBUG("Encoded URL: " << encodedURL);
      output_url = encodedURL;
      curl_free(encodedURL);
    } else {
      MCE_DEBUG("URL encoding failed");
    }
    curl_easy_cleanup(handle);
  } else {
    output_url = input_url;
    for (int i = 0; i < (int)output_url.length(); i++) {
      if (output_url[i] == '|') {
        output_url.replace(i, 1, "-");
      }
    }
  }
  output_url = "\"" + output_url + "\"";

}

bool AdTwConfig::LoadConfig() {
  string file_name = "../etc/ad_tw.config";
  //string db_instance="no db";
  //int sys_min_price = 50;
  try {
    ifstream config(file_name.c_str());
    string line;

    if (config) {
      while (config >> line) {
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep("=");
        tokenizer tokens(line, sep);
        string key;
        string value;
        int n = 0;
        for(tokenizer::iterator it = tokens.begin(); it != tokens.end(); ++it, ++n) {
          if (0==n) {
            key = *it;
          } else if(1==n) {
            value = *it;
          }
        }
        if (2 == n) {
          if (key.compare("sys_min_price") == 0) {
            sys_min_price_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] sys_min_price = " << sys_min_price_);
          } else if (key.compare("timeout") == 0) {
            timeout_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] timeout = " << timeout_);
          } else if (key.compare("url_encode") == 0) {
            url_encode_ = boost::lexical_cast<bool>(value);
            MCE_INFO("[LOAD_CONFIG] url_encode = " << url_encode_);
          }
        }
      }
    } else {
      MCE_INFO("no file " << file_name);
      //db_instance = "ade_db";
    }
    if (config) {
      config.close();
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("AdTwConfig::LoadConfig error : " << e);
  } catch (...) {
    MCE_WARN("AdTwConfig::LoadConfig error : unkown error \"...\"");
  }
  return true;
}

string AdTwConfig::GetIp()
{
    int result = 0;
    char hostName[100];
    struct hostent* host = NULL;
    result = gethostname(hostName, 100);
    host = gethostbyname(hostName);
    return string(inet_ntoa(*((struct in_addr *)host->h_addr_list[0])));
}

void LowQualityAdCache::LoadGidsEcpmLevelPool() {
  MCE_INFO("[loader],LoadGidsEcpmLevelPool::Load start");
  map<Ice::Long, int> gids_ecpm_level;
  try {
    gids_ecpm_level = AdDemoQualityAdapter::instance().GetLimitedAdGroupMap(0);
  }catch (Ice::Exception& e) {
    MCE_WARN("LowQualityAdCache::LoadGidsEcpmLevelPool --> call LoadGidsEcpmLevelPool  ERR  line:"
        << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("LowQualityAdCache::LoadGidsEcpmLevelPool --> call LoadGidsEcpmLevelPool  ERR  line:"
        << __LINE__ << " err:" << e.what());
  }
  IceUtil::RWRecMutex::WLock lock(mutex_);
  gids_ecpm_level_.swap(gids_ecpm_level);
  MCE_INFO("[cpm loader]: Load stop, LoadGidsEcpmLevelPool size is:"  << gids_ecpm_level_.size());
}

int LowQualityAdCache::GetAdEcpmLevel(Ice::Long group_id){
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<Ice::Long, int>::iterator itg = gids_ecpm_level_.find(group_id);
  if(itg != gids_ecpm_level_.end()){
    return itg->second;
  }
  return 3;
}

bool LowQualityAdCache::IsLowQualityAd(Ice::Long group_id){
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<Ice::Long, int>::iterator itg = gids_ecpm_level_.find(group_id);
  if(itg != gids_ecpm_level_.end()){
    return true;
  }
  return false;
}

void Widget2TypeCache::Load() {
  MCE_INFO("[Widget2Type:],Widget2TypeCache::Load start");
  try {
    using namespace mysqlpp;
    //online db
    const char* db_host = "report.db.jebe.d.xiaonei.com";
    //test db
    //const char* db_host = "10.22.206.99";
    const char* db_name = "jebe_main";
    Connection conn(db_name,db_host,"ad","adjb###");
    stringstream sql; 
    sql << "select * from widget_type";
    MCE_INFO("Widget2TypeCache::Load try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("Widget2TypeCache::Load Widget2TypeCache Error " << sql.str() << ".  select  fail!");
      return;
    }    
    MCE_INFO("Widget2TypeCache::Load Widget2TypeCache, Widget2TypeCache Size=: " << res.num_rows());
    map<Ice::Long, int> pool;
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long widget_id = (Ice::Long) row["widget_id"];
      int type = (int)row["type"];
      pool[widget_id] = type;
    }    
    IceUtil::RWRecMutex::WLock lock(mutex_);
    widget2Type_map_.swap(pool);
    MCE_INFO("End: Widget2TypeCache::Load size = " << widget2Type_map_.size());

  }catch (Ice::Exception& e) { 
    MCE_WARN("Widget2TypeCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e);
  } catch (std::exception& e) { 
    MCE_WARN("Widget2TypeCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e.what());
  } catch (...) {
    MCE_WARN("Widget2TypeCache::LoadDiscountCache error!");
  }    
}

/*
void TwEngineParametersCache::LoadParameters() {
  MCE_INFO("TwEngineParametersCache::LoadParameters begin");
  map<string,string> system_para;
  try{
    using namespace mysqlpp;
    const char* db_host = "10.3.23.69";
    const char* db_name = "ad_strategy";
    Connection conn(db_name, db_host,"ad","rrad");
    MCE_INFO("TwEngineParametersCache::LoadParameters, db host:" << db_host<< ",db_name:" << db_name);

    stringstream sql;
    sql.str("");
    sql << " select * from system_parameter";
    MCE_INFO("try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res || res.num_rows() <= 0) {
      MCE_WARN("TwEngineParametersCache::LoadParameters Error " << sql.str() << ".  select  fail!");
    }
    MCE_INFO("TwEngineParametersCache::LoadParameters res size : " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      string key = (string) row["key"];
      string value = (string) row["value"];
      MCE_INFO("TwEngineParametersCache::LoadParameters key=" << key << ", value=" << value);
      system_para[key] = value;
    }
    MCE_INFO("TwEngineParametersCache::LoadParameters size is " << system_para.size());
  } catch (std::exception& e) {
    MCE_WARN("TwEngineParametersCache::LoadParameters error: " << e.what());
  } catch (...) {
    MCE_WARN("TwEngineParametersCache::LoadParameters parameters error!");
  }

  IceUtil::RWRecMutex::WLock lock(mutex_);
  system_para_.swap(system_para);
  MCE_INFO("TwEngineParametersCache::LoadParameters stop,system_para size:" << system_para.size());
}

string TwEngineParametersCache::GetParameterByName(string para_name) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<string, string>::iterator its = system_para_.find(para_name);
  if(its != system_para_.end()) {
    return its->second;
  }
  return "";
}
*/

}
}
