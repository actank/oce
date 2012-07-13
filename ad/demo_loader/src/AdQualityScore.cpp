/*
 * AdQualityScore.cpp
 *
 *  Created on: 2011-9-21
 *      Author: shuang
 */
#include "DemoLoaderCache.h"
#include <connection.h>
#include <query.h>
#include <string>
#include <algorithm>
#include "AdConfig.h"
#include "QueryRunner.h"
#include "AdQualityUtil.h"
#include "AdQualityScore.h" 
#include <cmath>
#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/regex.hpp>
using namespace boost;
namespace xce{
namespace ad{
using namespace mysqlpp;

double ScoreAdCache::format_score(const double score,double base_point){
   double score_t=score;
   
   if(base_point < 0) 
     base_point *= -1;
   if(score > base_point)
     return 1;
   if(score < (-1)*base_point)
     return 0;     
   if(fabs(base_point) > 1e-6)
     return (score_t + base_point)/(base_point * 2);
   
   return 0;
}


void ScoreAdCache::LoadAvgCpc() {   //获取 avg_cpc
  map<Ice::Long, double> avg_pool;
  MCE_INFO("ScoreAdCache::LoadAvgCpcCache begin");
  
  try{

    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().quality_db_name().c_str();

    MCE_DEBUG("LoadAvgCpc db_host="<<db_host<<" db_name ="<<db_name);

    Connection conn(db_name,db_host,"ad","rrad");
    stringstream sql;
    sql << "select adzone_id,avg(price) as avg_cpc"
      <<" from adgroup_zone_factor"
      <<" where price >= 100"
      <<" group by adzone_id";
    MCE_INFO("LoadAvgCpcCache::Load try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("LoadAvgCpcCache Error " << sql.str() << ".  select  fail!");
      return;
    }

    MCE_INFO("LoadAvgCpcCache::Load Rows Size=: " << res.num_rows());

    map<Ice::Long, double>::iterator it; 
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long adzone_id = (Ice::Long) row["adzone_id"];
      double avg_cpc = (double) row["avg_cpc"];

      avg_pool[adzone_id] = avg_cpc;

    }

  }catch (Ice::Exception& e) {
    MCE_WARN("LoadAvgCpcCache::load --> call Database ERR  line:" << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("LoadAvgCpcCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e.what());
  } catch (...) {
    MCE_WARN("LoadAvgCpcCache::LoadDiscountCache error!");
  }

  IceUtil::RWRecMutex::WLock lock(mutex_score);
  avg_cpc.swap(avg_pool);  

  MCE_INFO("ScoreAdCache::LoadAvgCpcCache stop");
}

void ScoreAdCache::LoadMap_group_cam() {
  map<Ice::Long, Ice::Long> group_cam;
  MCE_INFO("ScoreAdCache::LoadLink_group_camCache begin");
  
  try{

    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().quality_db_name().c_str();
    MCE_DEBUG("LoadLink_group_cam db_host="<<db_host<<" db_name ="<<db_name);
    Connection conn(db_name,db_host,"ad","rrad");
    stringstream sql;
    sql << "select adgroup_id,campaign_id from adgroup_factor";
    MCE_INFO("LoadLink_group_camCache::Load try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("ScoreAdCache::Load LoadLink_group_camCache Error " << sql.str() << ".  select  fail!");
      return;
    }
    MCE_INFO("LoadLink_group_camCache::Load Rows Size=: " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long adgroup_id = (Ice::Long) row["adgroup_id"];
      Ice::Long campaign_id = (Ice::Long) row["campaign_id"];
      group_cam[adgroup_id] = campaign_id;
    }

  }catch (Ice::Exception& e) {
    MCE_WARN("LoadLink_group_camCache::load --> call Database ERR  line:" << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("LoadLink_group_camCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e.what());
  } catch (...) {
    MCE_WARN("LoadLink_group_camCache::LoadLink_group_camCache error!");
  }

  IceUtil::RWRecMutex::WLock lock(mutex_score);
  Map_group_cam.swap(group_cam);  
  MCE_INFO("ScoreAdCache::LoadLink_group_camCache stop");
}


void ScoreAdCache::LoadMap_cam_spon() { 
  map<Ice::Long, Ice::Long> cam_spon;
  map<Ice::Long,double> Campaign_score_tmp; 
  MCE_INFO("ScoreAdCache::Link_cam_sponCache begin");
  double WeightA1=0.2; 
  double WeightA2=0.2;
  string ret = DemoLoaderCache::instance().GetParameterByName("ad_weight_a1");
  if(ret != ""){
    WeightA1 = boost::lexical_cast<double>(ret);
  }

  ret = DemoLoaderCache::instance().GetParameterByName("ad_weight_a2");
  if(ret != ""){
    WeightA2 = boost::lexical_cast<double>(ret);
  }
  try{

    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().quality_db_name().c_str();
    MCE_DEBUG("LoadLink_cam_spon db_host="<<db_host<<" db_name ="<<db_name);
    Connection conn(db_name,db_host,"ad","rrad");
    stringstream sql;
    sql << "select campaign_id,sponsor_id,daily_budget,daily_consumption,budget_delta from campaign_factor";

    MCE_INFO("Link_cam_sponCache::Load try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("ScoreAdCache::Link_cam_sponCache Error " << sql.str() << ".  select  fail!");
      return;
    }

    MCE_INFO("Link_cam_sponCache::Load Rows Size=: " << res.num_rows());

    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);

      Ice::Long campaign_id = (Ice::Long) row["campaign_id"];
      Ice::Long sponsor_id = (Ice::Long) row["sponsor_id"];

      Ice::Long daily_budget = (Ice::Long) row["daily_budget"];
      Ice::Long daily_consumption = (Ice::Long) row["daily_consumption"];

      double budget_delta = (double) row["budget_delta"];

      double score = 0.0; 

      if (fabs(daily_budget) > 0){
        score= daily_consumption*1.0/(daily_budget*1.0);
        if(score > 1) score = 1;         //format campaign_score
        if(score <0 ) score = 0;
        score *= WeightA1;
      }
      score += WeightA2 * format_score(budget_delta,2.0);   //format budget_delta
      MCE_DEBUG(" cam_id="<<campaign_id<<" daily_score "<<WeightA1 * format_score(daily_consumption/daily_budget,1.0)
          <<" WeightA2*budget_delta="<<WeightA2*format_score(budget_delta,2.0));
      Campaign_score_tmp[campaign_id] = score;
      cam_spon[campaign_id] = sponsor_id;

    }

  }catch (Ice::Exception& e) {
    MCE_WARN("Link_cam_sponCache::load --> call Database ERR  line:" << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("Link_cam_sponCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e.what());
  } catch (...) {
    MCE_WARN("Link_cam_sponCache Load error!");
  }

  IceUtil::RWRecMutex::WLock lock(mutex_score);
  Map_cam_spon.swap(cam_spon);  
  Campaign_score.swap(Campaign_score_tmp);
  MCE_INFO("ScoreiAdCache::Link_cam_sponCache stop");
}

void ScoreAdCache::LoadSponsor_score() {   //获取 sponsor_id 和 质量分
  map<Ice::Long, double> Sponsor_score_tmp;
  MCE_INFO("ScoreAdCache::LoadSponsor_scoreCache begin");
  double WeightB1=0.1;
  string ret = DemoLoaderCache::instance().GetParameterByName("ad_weight_b1");
  if(ret != ""){
    WeightB1 = boost::lexical_cast<double>(ret);
  }
  try{

    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().quality_db_name().c_str();
    MCE_DEBUG("LoadSponsor_score db_host="<<db_host<<" db_name ="<<db_name);
    Connection conn(db_name,db_host,"ad","rrad");
    stringstream sql;
    sql << "select sponsor_id,total_paid from sponsor_factor";
    MCE_INFO("LoadSponsor_scoreCache::Load try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("ScoreAdCache::LoadSponsor_scoreCache Error " << sql.str() << ".  select  fail!");
      return;
    }
    MCE_INFO("LoadSponsor_scoreCache::Load Rows Size=: " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);

      Ice::Long sponsor_id = (Ice::Long) row["sponsor_id"];
      Ice::Long total_paid = (Ice::Long) row["total_paid"];
      double score = 0.0;
      score = log10(total_paid+1);
      //score = log(score +1);
      score /= 10;
      if(score>1.0) score=1;     // format sponsor_score
      if(score<0)   score=0;
      score *= WeightB1;
      Sponsor_score_tmp[sponsor_id] = score;
      MCE_DEBUG(" spon_id="<<sponsor_id<<" total_paid="<<total_paid<<" total_paid_score"<<score);
    }

  }catch (Ice::Exception& e) {
    MCE_WARN("LoadSponsor_scoreCache::load --> call Database ERR  line:" << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("LoadSponsor_scoreCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e.what());
  } catch (...) {
    MCE_WARN("ScoreAdCache::LoadSponsor_scoreCache error!");
  }

  IceUtil::RWRecMutex::WLock lock(mutex_score);
  Sponsor_score.swap(Sponsor_score_tmp);  
  MCE_INFO("ScoreAdCache::LoadSponsor_scoreCache stop");
}

void ScoreAdCache::UpdateScoreAdPool() {    // 更新 quality_score
  AdQualityScoreMap score_ad_update;
  /*int status=0;  // 0代表 全部用户 1代表 部分用户
    string ret = DemoLoaderCache::instance().GetParameterByName("quality_score_status");
    if(ret != ""){
    status = boost::lexical_cast<int>(ret);
    }  

    bool sign=false;   //判断用户是否按公式计算质量分 1 代表用公式计算 0代表质量分取默认值 1 */
  MCE_INFO("ScoreAdCache::UpdateScoreAdCache begin");
  try{
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().quality_db_name().c_str();
    Connection conn(db_name,db_host,"ad","rrad");
    stringstream sql;
    sql << "select adgroup_id,adzone_id,quality_score from quality_score_factor"; 
    MCE_INFO("ScoreAdCache::Load try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("ScoreAdCache::Load UpdateScoreCache Error " << sql.str() << ".  select  fail!");
      return;
    }
    MCE_INFO("ScoreAdCache::Load Rows Size=: " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long adgroup_id = (Ice::Long) row["adgroup_id"];
      Ice::Long adzone_id = (Ice::Long) row["adzone_id"];
      double quality_score = (double) row["quality_score"];
      MCE_DEBUG("adgroup_id ="<<adgroup_id<<" adzone_id="<<adzone_id<<" quality_score="<<quality_score);
      /*map<Ice::Long,Ice::Long>::iterator link_it;
        link_it=Map_group_cam.find(adgroup_id);
        if(link_it != Map_group_cam.end()){
        map<Ice::Long,Ice::Long>::iterator cam_spon_it;
        cam_spon_it = Map_cam_spon.find(link_it->second);
        if(cam_spon_it != Map_cam_spon.end()){
        map<Ice::Long,int>::iterator status_it; 
        status_it= score_ad_status.find(cam_spon_it->second);
        if(status_it!=score_ad_status.end() && status_it->second ==1){
        sign = true;
        }
        }
        }    

        if(0==status || sign){
        stringstream gidzid;
        gidzid<<adgroup_id<<adzone_id;
        string adgidzid= gidzid.str();
        score_ad_update[adgidzid] = quality_score;
        }*/

      stringstream gidzid;
      gidzid<<adgroup_id<<adzone_id;
      string adgidzid= gidzid.str();
      score_ad_update[adgidzid] = quality_score;
    }
  }catch (Ice::Exception& e) {
    MCE_WARN("ScoreAdCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("ScoreAdCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e.what());
  } catch (...) {
    MCE_WARN("ScoreAdCache::UpdateScoreCache error!");
  }
  MCE_INFO("score_ad_update size = "<<score_ad_update.size());
  IceUtil::RWRecMutex::WLock lock(mutex_score);
  AdQualityScoreMap::iterator it;
  for(it=score_ad_update.begin();it!=score_ad_update.end();it++){
    score_ad_pool_[it->first]=it->second;
  }
  MCE_INFO("ScoreAdCache::UpdateScoreCache stop");
}

void ScoreAdCache::LoadScore_status() {   //获取 sponsor_id 和 状态
  map<Ice::Long,int> status_pool;
  MCE_INFO("ScoreAdCache::LoadScore_statusCache begin");

  try{

    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().quality_db_name().c_str();
    MCE_DEBUG("LoadSponsor_score db_host="<<db_host<<" db_name ="<<db_name);
    Connection conn(db_name,db_host,"ad","rrad");
    stringstream sql;
    sql << "select sponsor_id,sponsor_status from sponsor_score_status";
    MCE_INFO("LoadScore_statusCache::Load try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("ScoreAdCache::LoadScore_status Error " << sql.str() << ".  select  fail!");
      return;
    }
    MCE_INFO("LoadScore_statusCache::Load Rows Size=: " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long sponsor_id = (Ice::Long) row["sponsor_id"];
      int status = (int) row["sponsor_status"];
      status_pool[sponsor_id] = status;
      MCE_DEBUG(" sponsor_id="<<sponsor_id<<" sponsor_status"<<status);
    }

  }catch (Ice::Exception& e) {
    MCE_WARN("LoadScore_statusCache::load --> call Database ERR  line:" << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("LoadScore_statusCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e.what());
  } catch (...) {
    MCE_WARN("ScoreAdCache::LoadScore_statusCache error!");
  }

  IceUtil::RWRecMutex::WLock lock(mutex_score);
  score_ad_status.swap(status_pool);  
  MCE_INFO("ScoreAdCache::LoadScore_statusCache stop");
}



void ScoreAdCache::LoadScoreAdPool() {    // 计算 quality_score
  LoadAvgCpc();
  LoadMap_group_cam();
  LoadMap_cam_spon();
  LoadSponsor_score();
  LoadScore_status(); 
  AdQualityScoreMap score_ad_pool;
  score_ad_pool_.clear();  
  double WeightA3=0.25; 
  double WeightA4=0.25; 
  string ret = DemoLoaderCache::instance().GetParameterByName("ad_weight_a3");
  if(ret != ""){
    WeightA3 = boost::lexical_cast<double>(ret);
  }

  ret = DemoLoaderCache::instance().GetParameterByName("ad_weight_a4");
  if(ret != ""){
    WeightA4 = boost::lexical_cast<double>(ret);
  }

  int status=0;  // 0代表 全部用户 1代表 部分用户
  ret = DemoLoaderCache::instance().GetParameterByName("quality_score_status");
  if(ret != ""){
    status = boost::lexical_cast<int>(ret);
  }  

  MCE_INFO("ScoreAdCache::LoadScoreAdCache begin");
  try{

    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().quality_db_name().c_str();
    MCE_DEBUG("LoadScoreAdPool db_host="<<db_host<<" db_name ="<<db_name);
    Connection conn(db_name,db_host,"ad","rrad");
    stringstream sql;
    sql << "select adgroup_id,adzone_id,price,price_delta from adgroup_zone_factor"; 
    MCE_INFO("ScoreAdCache::Load try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("ScoreAdCache::Load TmpCache Error " << sql.str() << ".  select  fail!");
      return;
    }
    MCE_INFO("ScoreAdCache::Load Rows Size=: " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long adgroup_id = (Ice::Long) row["adgroup_id"];
      Ice::Long adzone_id = (Ice::Long) row["adzone_id"];
      Ice::Long price = (Ice::Long) row["price"];
      double price_delta = (double) row["price_delta"];
      double score = 0.5;  // 质量分 加 0.5 
      bool sign=false;   //判断用户是否按公式计算质量分 1 代表用公式计算 0代表质量分取默认值 1 

      map<Ice::Long,double>::iterator it;
      it=avg_cpc.find(adzone_id);
      if(it != avg_cpc.end() && it->second > 0){
        double zone_score=(price*1.0/it->second - 1.0);
        zone_score = format_score(zone_score,2.0);   // format zone_score
        score +=WeightA4 * zone_score;
      }
      score += WeightA3 * format_score(price_delta,2.0);  //format price_delta
      double cam_score=0,spon_score=0;
      Ice::Long cam_id=0,spon_id=0;
      map<Ice::Long,Ice::Long>::iterator link_it;
      link_it=Map_group_cam.find(adgroup_id);
      if(link_it != Map_group_cam.end()){

        cam_id = link_it->second;   

        it = Campaign_score.find(cam_id);

        if(it != Campaign_score.end()){       
          score += it->second;
          cam_score=it->second;
        }

        map<Ice::Long,Ice::Long>::iterator cam_spon_it;
        cam_spon_it = Map_cam_spon.find(cam_id);
        if(cam_spon_it != Map_cam_spon.end()){
          spon_id = cam_spon_it->second;   
          map<Ice::Long,int>::iterator status_it; 
          status_it= score_ad_status.find(spon_id);
          if(status_it!=score_ad_status.end() && status_it->second ==1){
            sign = true;
          }
          it = Sponsor_score.find(spon_id);  
          if(it != Sponsor_score.end()){
            score += it->second;
            spon_score=it->second;
          }
        }
      }    
      MCE_DEBUG("adgroup_id ="<<adgroup_id<<" adzone_id="<<adzone_id<<" quality_score="<<score<<" cam_id="<<cam_id<<" spon_id="<<spon_id
          <<" cam_score="<<cam_score<<" spon_score="<<spon_score);
      stringstream gidzid;
      gidzid<<adgroup_id<<adzone_id;
      string adgidzid= gidzid.str();
      //modify begin 2011-10-14
      if(0==status || sign )
        score_ad_pool[adgidzid] = score;
      //modify end
    }
  }catch (Ice::Exception& e) {
    MCE_WARN("ScoreAdCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("ScoreAdCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e.what());
  } catch (...) {
    MCE_WARN("ScoreAdCache::LoadDiscountCache error!");
  }

  MCE_INFO("score_ad_pool size = "<<score_ad_pool.size());
  IceUtil::RWRecMutex::WLock lock(mutex_score);
  score_ad_pool_.swap(score_ad_pool);  
  UpdateScoreAdPool(); //更新quality_score
  clear();
  MCE_INFO("ScoreAdCache::LoadScoreAdCache stop");
}

void ScoreAdCache::LoadScoreAdMap(AdQualityScoreMap& score_ad_pool){
  IceUtil::RWRecMutex::RLock lock(mutex_score);
  score_ad_pool = score_ad_pool_;
}


}
}
