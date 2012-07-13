/*
 * AdUrlUtil.h
 *
 *  Created on: 2010-9-13
 *      Author: ark
 */

#ifndef ADURLUTIL_H_
#define ADURLUTIL_H_

#include <curl/curl.h>
#include <string>
#include "AdEngine.h"
#include <Ice/Ice.h>
#include "IceLogger.h"
#include "Evictor.h"
#include "Singleton.h"
#include "QueryRunner.h"
#include "TaskManager.h"
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include "AdDemoQualityAdapter.h"
#include "AdDemoQuality.h"

namespace xce {
namespace ad {
using namespace std;
using namespace com::xiaonei::xce;

struct AdGroupInfo {
  AdGroupInfo():bid_unit(0),member_id(0),max_price(0) {};
  vector<Ice::Long> creatives;
  Ice::Long bid_unit;
  Ice::Long member_id;
  int max_price;
  Ice::Long widget_id;
};
struct AdGroupInfoAppend {
  AdGroupInfo group_info;
  string append_info;
  Ice::Long creative_id;
  int price;
  bool found_flag;
};


static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *data) {
  string *buff = (string *) data;
  buff->append((char *) ptr, size * nmemb);
  return size * nmemb;
}

class AdTwConfig : public MyUtil::Singleton<AdTwConfig> {
public:
  AdTwConfig():DB_("ade_db"), sys_min_price_(50), timeout_(1000), url_encode_(0) {
    MyUtil::TaskManager::instance().schedule(new xce::ad::AdTwConfig::AdTwConfigLoadTimer(0));
  }
  string DB() {
    return DB_;
  }
  void set_DB(string DB) {
    DB_ = DB;
  }
  int sys_min_price() {
    return sys_min_price_;
  }
  int timeout() {
    return timeout_;
  }
  bool url_encode() {
    return url_encode_;
  }
  string server_name() {
    return server_name_;
  }
  void set_server_name(int server_index) {
    ostringstream oss;
    oss << "AdTwS" << server_index << "@" << GetIp();
    server_name_ = oss.str();
  }
  void set_server_name(string server_name) {
    server_name_ = server_name;
  }
  bool LoadConfig();

private:
  string DB_;
  int sys_min_price_;
  int timeout_;
  string server_name_;
  bool url_encode_;

  string GetIp();

  class AdTwConfigLoadTimer : public MyUtil::Timer{
  public:
    AdTwConfigLoadTimer(int time = 0):Timer(time){}
    virtual void handle() {
      MyUtil::TaskManager::instance().schedule(new AdTwConfigLoadTimer(60*1000));//1分钟
      AdTwConfig::instance().LoadConfig();
    }
  };
};


struct UrlQueryEscape { std::string operator()(const std::string&) const; };

class AdGroupCache: public MyUtil::Singleton<AdGroupCache> {
public:
  AdGroupInfo get(const Ice::Long& group) {

    IceUtil::RWRecMutex::RLock lock(groups_mutex_);
    map<Ice::Long, AdGroupInfo>::iterator it = groups_map_.find(group);
    if (it != groups_map_.end()) {
      return it->second;
    } else {
      MCE_INFO("no group: " << group);
      return AdGroupInfo();
    }
  }

  void load() {
    string db = AdTwConfig::instance().DB();
    MCE_INFO("AdGroupCache load ...,db = " << db);
    Statement sql;
    mysqlpp::StoreQueryResult res;
    //load member
    MCE_INFO("load member ...");
    set<Ice::Long> member_set;
    sql << "select member_id from member where is_formal = 1 "
        "and validity = 1 and have_money = 1 and no_exceed_max = 1 and "
        "am_online_status = 1 and ma_online_status = 1 and delete_flag = 1";
    res = QueryRunner(db, CDbRServer).store(sql);
    MCE_INFO("[LOAD] member load size = " << res.size());
    for (size_t i = 0; i < res.num_rows(); i++) {
      mysqlpp::Row & row = res.at(i);
      Ice::Long member_id = boost::lexical_cast<Ice::Long>(row["member_id"]);
      member_set.insert(member_id);
    }
    // load campaign
    MCE_INFO("load campaign ...");
    set<Ice::Long> campaign_set;
    sql.clear();
    sql << "select campaign_id, member_id from campaign where begin_time < now() "
        "and (end_time>now() or end_time is NULL) and sys_status = 1 and "
        "member_status = 1 and am_status = 1 and delete_flag = 1";
    res = QueryRunner(db, CDbRServer).store(sql);
    MCE_INFO("[LOAD] campaign load size = " << res.size());
    for (size_t i = 0; i < res.num_rows(); i++) {
      mysqlpp::Row & row = res.at(i);
      Ice::Long campaign_id = boost::lexical_cast<Ice::Long>(row["campaign_id"]);
      Ice::Long member_id = boost::lexical_cast<Ice::Long>(row["member_id"]);
      if (member_set.find(member_id) != member_set.end()) {
        campaign_set.insert(campaign_id);
      }
    }
    // load adgroup
    MCE_INFO("load group ...");
    map<Ice::Long, int> group_map;
    sql.clear();
    sql << "select adgroup_id, campaign_id, member_id, max_price from adgroup "
        "where member_status = 1 and am_status = 1 and audit_status = 1 and delete_flag = 1 "
        "and display_type = 4 and daily_sys_status = 1 and total_sys_status = 1";
    res = QueryRunner(db, CDbRServer).store(sql);
    MCE_INFO("[LOAD] group load size = " << res.size());
    for (size_t i = 0; i < res.num_rows(); i++) {
      mysqlpp::Row & row = res.at(i);
      Ice::Long group_id = boost::lexical_cast<Ice::Long>(row["adgroup_id"]);
      Ice::Long campaign_id = boost::lexical_cast<Ice::Long>(row["campaign_id"]);
      Ice::Long member_id = boost::lexical_cast<Ice::Long>(row["member_id"]);
      int max_price = boost::lexical_cast<int>(row["max_price"]);
      if (campaign_set.find(campaign_id) != campaign_set.end()
          and member_set.find(member_id) != member_set.end()) {
        group_map.insert(pair<Ice::Long, int>(group_id,max_price));
      }
    }

    //load creative
    MCE_INFO("load creative ...");
    sql.clear();
    map<Ice::Long,Ice::Long> group_widget;
    sql << "select creative_id, adgroup_id, widget_id from creative where member_status = 1 and "
        "am_status = 1 and audit_status = 1 and delete_flag=1";
    map<Ice::Long, vector<Ice::Long> > creative_map;
    res = QueryRunner(db, CDbRServer).store(sql);
    MCE_INFO("[LOAD] creative load size = " << res.size());
    for (size_t i = 0; i < res.num_rows(); i++) {
      mysqlpp::Row & row = res.at(i);
      Ice::Long group = boost::lexical_cast<Ice::Long>(row["adgroup_id"]);
      Ice::Long creative = boost::lexical_cast<Ice::Long>(row["creative_id"]);
      Ice::Long widget_id = boost::lexical_cast<Ice::Long>(row["widget_id"]);
      map<Ice::Long, vector<Ice::Long> >::iterator it = creative_map.find(group);
      group_widget[group] = widget_id;
      if (it != creative_map.end()) {
        it->second.push_back(creative);
        MCE_DEBUG("group " << group << " has creative " << creative);
      } else if (group_map.find(group) != group_map.end()) {
        vector<Ice::Long> v;
        v.push_back(creative);
        creative_map.insert(pair<Ice::Long, vector<Ice::Long> >(group, v));
        MCE_DEBUG("group " << group << " has creative " << creative);
      }
    }
    
    //load bid_unit and prepare the new map
    MCE_INFO("load bid_unit ...");
    map<Ice::Long, AdGroupInfo> temp_map;
    sql.clear();
    sql << "select bid_unit_id, adgroup_id,member_id from bid_unit where delete_flag=1";
    map<Ice::Long, Ice::Long> bid_unit_map;
    res = QueryRunner(db, CDbRServer).store(sql);
    MCE_INFO("[LOAD] bid_unit load size = " << res.size());
    for (size_t i = 0; i < res.num_rows(); i++) {
      mysqlpp::Row & row = res.at(i);
      Ice::Long group = boost::lexical_cast<Ice::Long>(row["adgroup_id"]);
      Ice::Long bid_unit = boost::lexical_cast<Ice::Long>(row["bid_unit_id"]);
      Ice::Long member_id = boost::lexical_cast<Ice::Long>(row["member_id"]);
      //join the data
      Ice::Long widget_id = 0;
      map<Ice::Long, Ice::Long>::iterator itg = group_widget.find(group);
      if(itg != group_widget.end()){
        widget_id = itg->second;
      }
      map<Ice::Long, vector<Ice::Long> >::iterator it = creative_map.find(group);
      if (it != creative_map.end()) {
        AdGroupInfo value;
        value.creatives = it->second;
        value.bid_unit = bid_unit;
        value.max_price = group_map[group];
        value.member_id = member_id;
        value.widget_id = widget_id;
        MCE_DEBUG("group " << group << " has bid_unit " << bid_unit);
        temp_map.insert(pair<Ice::Long, AdGroupInfo>(group, value));
        MCE_DEBUG("group " << group << " inserted");
      } else {
        //
        //MCE_DEBUG("adgroup " << group << ", bid_unit " << bid_unit << " has no creative, maybe adgroup is not active");
      }
    }
    LoadAdgroupAdzoneRef(temp_map);

    IceUtil::RWRecMutex::WLock lock(groups_mutex_);
    groups_map_.swap(temp_map);
    MCE_INFO("AdGroupCache load ... done!!!");
  }

  void LoadAdgroupAdzoneRef(map<Ice::Long, AdGroupInfo>& group_pool) {
    //load adzone_ids;
    vector<AdGroupInfo> taobao_list;
    vector<AdGroupInfo> default_list;
    Statement sql;
    sql << "select adzone_id, member_id, adgroup_id,  member_status, am_status, delete_flag,show_flag from adgroup_adzone_ref where adzone_id in "
        " (100000000063, 100000000070, 100000000072) ";
    const string DB_INSTANCE  = AdTwConfig::instance().DB();
    //const string DB_INSTANCE = EngineConfig::instance().DB();
    mysqlpp::StoreQueryResult res;
    try{
      res = QueryRunner(DB_INSTANCE, CDbRServer, "adgroup_adzone_ref").store(sql);
    } catch(Ice::Exception& e) {
      MCE_WARN("AdGroupPool::LoadAdgroupAdzoneRef error : " << e);
    } catch(std::exception& e) {
      MCE_WARN("AdGroupPool::LoadAdgroupAdzoneRef std::exception error : " << e.what());
    } catch(...) {
      MCE_WARN("AdGroupPool::LoadAdgroupAdzoneRef unknown error.");
    }
    if (res && res.num_rows() > 0) {
      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        Ice::Long group_id = (Ice::Long) row["adgroup_id"];
        Ice::Long zone_id = (Ice::Long) row["adzone_id"];
        Ice::Long member_id = (Ice::Long) row["member_id"];
        int member_status = (int) row["member_status"];
        int am_status = (int) row["am_status"];
        int delete_flag = (int) row["delete_flag"];
        int show_flag = (int) row["show_flag"];
        if ((member_status == 1) && (am_status == 1) && (delete_flag == 1)&& (show_flag == 1)) {
          map<Ice::Long, AdGroupInfo>::iterator group_pool_it = group_pool.find(group_id);
          if (group_pool_it != group_pool.end()) {
            //if ((member_id == 100000006139)) {//加入淘宝默认广告
    //        if ((member_id == 0)) {//加入淘宝默认广告
    //          taobao_list.push_back(group_pool_it->second);
    //        }
            if ((member_id == 999)) {//加入默认广告
              default_list.push_back(group_pool_it->second);
            }
          }
        } else {

        }
      }
    }
    IceUtil::RWRecMutex::WLock lock(groups_mutex_);
    taobao_list_.swap(taobao_list);
    default_list_.swap(default_list);
    MCE_INFO("AdGroupCache LoadAdgroupAdzoneRef ... done!!!,taobao size:"<< taobao_list_.size()
        << ",default size:" << default_list_.size());
  }
  void GetTaobaoList(vector<AdGroupInfo>& group){
    IceUtil::RWRecMutex::RLock lock(groups_mutex_);
    group = taobao_list_;
  }
  void GetDefaultList(vector<AdGroupInfo>& group){
    IceUtil::RWRecMutex::RLock lock(groups_mutex_);
    group = default_list_;
  }

private:
  map<Ice::Long, AdGroupInfo> groups_map_;
  vector<AdGroupInfo> taobao_list_;
  vector<AdGroupInfo> default_list_;
  IceUtil::RWRecMutex groups_mutex_;
};

class AdGroupLoadTimer : public MyUtil::Timer{
  public:
    AdGroupLoadTimer(int time = 0):Timer(time){}
    virtual void handle() {

      MyUtil::TaskManager::instance().schedule(new xce::ad::AdGroupLoadTimer(5*60*1000));
      try{
        AdGroupCache::instance().load();
      } catch(Ice::Exception& e) {
        MCE_WARN("AdGroupCache::load error : " << e);
      } catch(...) {
        MCE_WARN("AdGroupCache::load error : unkown error ...");
      }

    }

};

class AdUrlUtil {
  public:
    static AdResultMap Get(const string& userProfile,
        const string& referer,
        const int server_index = 0);

    static void Click(int uid,
        const string& ip,
        const string& text,
        const string& referer,
        const string& click_url);

    static AdResultMap GetTest(const Ice::Int uid,
                                  const Ice::Int stage,
                                  const Ice::Int gender,
                                  const Ice::Int age,
                                  const ::std::string& ipArea,
                                  const Ice::Long school,
                                  const Ice::Int grade,
                                  const int count,
                                  const string & referer,
                                  const int rotate_index);

private:
  static bool UrlEncode(const string& input_url, string& output_url);
  static AdGroupInfo SelectDefaultTaobao(vector<AdGroupInfo>& taobao_list);
  static AdGroupInfo SelectDefaultAd(set<Ice::Long>& competed_groups, vector<AdGroupInfo>&default_list);
};

class TimeStat {
  timeval _begin, _end;
public:
  TimeStat() {
    reset();
  }
  void reset() {
    gettimeofday(&_begin, NULL);
  }
  float getTime() {
    gettimeofday(&_end, NULL);
    float timeuse = 1000000 * (_end.tv_sec - _begin.tv_sec) + _end.tv_usec - _begin.tv_usec;
    timeuse /= 1000;
    return timeuse;
  }
};

class LowQualityAdCache : public MyUtil::Singleton<LowQualityAdCache> {
public:

  void LoadGidsEcpmLevelPool();
  int GetAdEcpmLevel(Ice::Long group_id);
  bool IsLowQualityAd(Ice::Long group_id);

private:
  map<Ice::Long, int> gids_ecpm_level_;
  IceUtil::RWRecMutex mutex_;
};

class LowQualityAdCacheTimer : public MyUtil::Timer {
public:
  LowQualityAdCacheTimer() :
    Timer(2 * 60 * 1000) { //2分钟重新计算一次
  }
  virtual void handle() {
    MCE_INFO("LowQualityAdCacheTimer start");
    LowQualityAdCache::instance().LoadGidsEcpmLevelPool();
    MCE_INFO("LowQualityAdCacheTimer stop");
  }
};


class Widget2TypeCache : public MyUtil::Singleton<Widget2TypeCache> {
  public:
    int GetWidgetType(Ice::Long widget_id) {
      IceUtil::RWRecMutex::RLock lock(mutex_);
      map<Ice::Long, int>::iterator itdis = widget2Type_map_.find(widget_id);
      if(itdis != widget2Type_map_.end()) {
        return itdis->second;
      }   
      return 0;
    }
    void Load();
  private:
    map<Ice::Long, int> widget2Type_map_;
    IceUtil::RWRecMutex mutex_;
};

class Widget2TypeCacheTimer : public MyUtil::Timer {
  public:
    Widget2TypeCacheTimer() :
      Timer(5 * 60 * 1000) { //5分钟重新加载一次
    }
    virtual void handle() {
      MCE_INFO("Wiget2TypeCacheTimer begin");
      Widget2TypeCache::instance().Load();
      MCE_INFO("Wiget2TypeCacheTimer end");
    }   
};
/*
class TwEngineParametersCache : public MyUtil::Singleton<TwEngineParametersCache>{
  private:
    map<string, string> system_para_;
    IceUtil::RWRecMutex mutex_;
  public:
    void LoadParameters();
    string GetParameterByName(string para_name);
};
class TwEngineParametersTimer : public MyUtil::Timer {
  public:
    TwEngineParametersTimer() :
      Timer(3 * 60 * 1000){//3分钟重新加载读取一次
    }
    virtual void handle() {
      MCE_INFO(" TwEngineParametersTimer handle begin");
      TwEngineParametersCache::instance().LoadParameters();
      MCE_INFO(" TwEngineParametersTimer handle end");
    }

};
*/
}
}

#endif /* ADURLUTIL_H_ */
