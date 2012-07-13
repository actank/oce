/*
 * AdUrlMaping.cpp
 *
 *  Created on: 2010-12-13
 *      Author: ark
 */

#include "IceLogger.h"
#include "QueryRunner.h"
#include "UrlMaping.h"

namespace xce {
namespace ad {
using namespace std;
using namespace com::xiaonei::xce;

AdUrlMaping::AdUrlMaping() {
  // TODO Auto-generated constructor stub
  //MyUtil::TaskManager::instance().schedule(new xce::ad::AdUrlMaping::AdUrlMapingLoadTimer());
}

AdUrlMaping::~AdUrlMaping() {
  // TODO Auto-generated destructor stub
}

void AdUrlMaping::GetAdZone(const string& Url, const int stage, set<Ice::Long>& AdZones) {
  vector<string> refers;
  PreProcess(Url, refers);
  IceUtil::RWRecMutex::RLock lock(mutex_);
  for(vector<string>::iterator it = refers.begin(); it != refers.end(); ++it) {
    pair<string, int> p = make_pair(*it, stage);
    map<pair<string,int>, set<Ice::Long> >::iterator mit = map_.find(p);
    if(mit != map_.end()) {
      AdZones = mit->second;
      /*MCE_DEBUG("[UrlMaping] find " << AdZones.size() << " adzone at refer = " << (*it));
      for(set<Ice::Long>::iterator zit = AdZones.begin(); zit != AdZones.end(); ++zit) {
        MCE_DEBUG("[UrlMaping] adzone_id = " << (*zit));
      }*/
      break;//找到，结束
    }
  }
}

bool AdUrlMaping::Load(const string& db) {
  MCE_INFO("AdUrlMaping load ..., db = " << db);
  Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "select url,stage,adzone_id from url_adzone_maping";
  res = QueryRunner(db, CDbRServer).store(sql);
  map<pair<string,int>, set<Ice::Long> > temp_map;
  for (size_t i = 0; i < res.num_rows(); i++) {
    mysqlpp::Row & row = res.at(i);
    string url = boost::lexical_cast<string>(row["url"]);
    int stage = boost::lexical_cast<int>(row["stage"]);
    Ice::Long adzone_id = boost::lexical_cast<Ice::Long>(row["adzone_id"]);

    pair<string, int> p = make_pair(url, stage);
    map<pair<string,int>, set<Ice::Long> >::iterator it = temp_map.find(p);
    if(it != temp_map.end()) {
      it->second.insert(adzone_id);
    } else {
      set<Ice::Long> s;
      s.insert(adzone_id);
      temp_map[p] = s;
    }
    //MCE_DEBUG("url = " << url << ", stage = " <<  stage << ", adzone_id = " << adzone_id);
  }
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    map_.swap(temp_map);
  }
  return true;
}

void AdUrlMaping::PreProcess(const string& Url, vector<string>& UrlSeq) {
  //第一步：原始匹配
  UrlSeq.push_back(Url);
  //第二步：去掉问号和#后的内容再匹配
  size_t found = Url.find_first_of("?");
  string ref;
  if(found != string::npos) {
    ref = Url.substr(0, found);
    UrlSeq.push_back(ref);
  } else {
    ref = Url;
  }
  found = ref.find_first_of("#");
  if(found != string::npos) {
    ref = ref.substr(0, found);
    UrlSeq.push_back(ref);
  }
  //第三步：去掉数字进行匹配
  size_t len = ref.size();
  string ref_with_star;
  for(size_t i = 0; i < len; ++i) {
    char c = ref[i];
    if(!isdigit(c)) {
      ref_with_star.append(&c,1);
    } else {
      if(ref_with_star[ref_with_star.size()-1] != '*') {
        ref_with_star.append("*");
      }
    }
  }
  if(!ref_with_star.empty() && ref_with_star.compare(ref) != 0) {
    UrlSeq.push_back(ref_with_star);
  }
  //第四步：逐级去掉目录匹配
  if(ref_with_star.empty()) {
    ref_with_star = ref;
  }
  len = ref_with_star.size();
  for(int i = len-1; i >=0; --i) {
    if(ref_with_star[i] == '/') {
      string s = ref_with_star.substr(0, i);
      if(s.compare("http:/") != 0) {
        UrlSeq.push_back(s);
      } else {
        break;
      }
    }
  }
  MCE_DEBUG("AdUrlMaping::PreProcess() --> UrlSeq.size() = " << UrlSeq.size());
  for(vector<string>::iterator it = UrlSeq.begin(); it != UrlSeq.end(); ++it) {
    MCE_DEBUG("AdUrlMaping::PreProcess() --> Url = " << (*it));
  }
}

}
}
