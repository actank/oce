#include <cmath>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include "SearchDataI.h"
#include "BuddyByIdReplicaAdapter.h"
#include "UserCacheAdapter.h"


using namespace std;
using namespace MyUtil;
using namespace xce::serverstate;
using namespace com::xiaonei::xce;
using namespace xce::search;
using namespace xce::suggestiondata;
using namespace xce::buddy::adapter;
using namespace xce::usercache;


// ============== SearchDataI ======================================

void SearchDataI::getSubStr(const string& pinyin,unsigned int start,int num,vector<string> subStr, vector<string>& resultStr) {
  ostringstream osstmp;
  for (unsigned int k = 0; k < subStr.size(); k++) {
    osstmp << subStr[k] << " ";
  }
  MCE_DEBUG("getSubStr: pingyin:" << pinyin << " start:" << start << " num:" << num << " subStr:" << osstmp.str());
  if(start>=pinyin.size()) {
    resultStr.clear();
    ostringstream oss;
    for(int i=0;i<num;++i){
      oss << subStr[i] << " ";
      MCE_DEBUG("subStr[" << i << "]:" << subStr[i]);
      resultStr.push_back(subStr[i]);
    }
    MCE_DEBUG("split res: "<<oss.str() << " subStr.size:" << subStr.size() << " resultStr.size:" << resultStr.size());
  }else {
    for(unsigned int end=start+1;end<=pinyin.size();++end) {
      string temp = pinyin.substr(start,end-start);
      if(Dictionary::instance().checkSyntax(temp)){
        subStr[num]=temp;
        getSubStr(pinyin,end,num+1,subStr,resultStr);
      }
    }
  }
}

void PageSearchDataI::splitPY(const string& pinyin,unsigned int start,int num,vector<string> subStr,CollectorPtr collector) {
  TimeCost tc = TimeCost::create("PageSearchDataI::splitPY",1);
  vector<string> resultStr;
  getSubStr(pinyin, start, num, subStr, resultStr);
  ostringstream osstmp;
  for (unsigned int k = 0; k < resultStr.size(); k++) {
    osstmp << resultStr[k] << " ";
    MCE_DEBUG("resultStr[" << k << "]:" << resultStr[k]);
  }
  tc.step(osstmp.str());
  MCE_DEBUG("PageSearchDataI::splitPY: pingyin:" << pinyin << " start:" << start << " num:" << num << " resultStr:" << osstmp.str() << " resultStr.size:" << resultStr.size());
  //ScorerPtr sp = pyIndex_.getScorer(resultStr,resultStr.size());
  ScorerPtr sp = getScorer(resultStr,resultStr.size());
  if(sp)
    sp->score(collector);
  tc.step(pinyin);
}

void WorkplaceSearchDataI::splitPY(const string& pinyin,unsigned int start,int num,vector<string> subStr,CollectorPtr collector) {
  TimeCost tc = TimeCost::create("WorkplaceSearchDataI::splitPY",1);
  vector<string> resultStr;
  getSubStr(pinyin, start, num, subStr, resultStr);
  ostringstream osstmp;
  for (unsigned int k = 0; k < resultStr.size(); k++) {
    osstmp << resultStr[k] << " ";
    MCE_DEBUG("resultStr[" << k << "]:" << resultStr[k]);
  }
  tc.step(osstmp.str());
  MCE_DEBUG("WorkplaceSearchDataI::splitPY: pingyin:" << pinyin << " start:" << start << " num:" << num << " resultStr:" << osstmp.str() << " resultStr.size:" << resultStr.size());
  //ScorerPtr sp = pyIndex_.getScorer(resultStr,resultStr.size());
  ScorerPtr sp = getScorer(resultStr,resultStr.size());
  if(sp)
    sp->score(collector);
  tc.step(pinyin);
}

void SearchDataI::splitPY(const string& pinyin,unsigned int start,int num,vector<string> subStr,CollectorPtr collector) {
  ostringstream osstmp;
  for (unsigned int k = 0; k < subStr.size(); k++) {
    osstmp << subStr[k] << " ";
  }
  MCE_DEBUG("splitPY: pingyin:" << pinyin << " start:" << start << " num:" << num << " subStr:" << osstmp.str());
  if(start>=pinyin.size()) {
    ostringstream oss;
    for(int i=0;i<num;++i){
      oss << subStr[i] << " ";
    }
    MCE_DEBUG("split res: "<<oss.str());
    ScorerPtr sp = pyIndex_.getScorer(subStr,num);
    if(sp)
      sp->score(collector);
    return;
  }else {
    for(unsigned int end=start+1;end<=pinyin.size();++end) {
      string temp = pinyin.substr(start,end-start);
      if(Dictionary::instance().checkSyntax(temp)){
        subStr[num]=temp;
        splitPY(pinyin,end,num+1,subStr,collector);
      }
    }
  }
}

Str2StrMapSeq SearchDataI::search(const string& query, int begin, int limit)
{
  TimeCost tc = TimeCost::create("SearchDataI::Search",1);
  vector<string> words;
  splitWord(query,words);
  CollectorPtr collector = new Collector(100);
  if(StrUtil::mayPinyin(query,words)){
    MCE_DEBUG("start py search: " << query);
    vector<string> subStr(query.size());
    splitPY(query,0,0,subStr,collector);
    MCE_DEBUG("end py search");
  }
  ScorerPtr sp = hzIndex_.getScorer(words,words.size());
  if(sp == 0){ 
    MCE_DEBUG("zero");
  }else{
    MCE_DEBUG("get "<<sp.get());
    sp->score(collector);
  }
  vector<int> ids = collector->topDocs();
  int size = ids.size();
  if (size <= begin) {
    Str2StrMapSeq res;
    return res;
  }
  int total = begin + limit;
  if (total > size) {
    total = size;
  }

  Str2StrMapSeq res;
  for(int i = begin; i < total; ++i){
    res.push_back(stores_[ids[i]]);
  } 
  return res;
}   

/// @brief SearchDataI::add 建立两个倒排索引(postingsHZ和postingsPY) 一个是纯拼音的，一个是非纯拼音的, 
///// 
///// @param result (in) 好友ID->好友名字
void SearchDataI::add(const Str2StrMapSeq& docs,const Str2IntMap& config, int timeout)
{
  unsigned int id=0;
  stores_ = Str2StrMapSeq(0);
  for(Str2StrMapSeq::const_iterator it=docs.begin();it!=docs.end();++it,++id) {
    int total = 0;
    Str2StrMap storeMap;
    for(Str2StrMap::const_iterator itt=it->begin();itt!=it->end();itt++){
      Str2IntMap::const_iterator itC = config.find(itt->first);
      if(itC == config.end()){
        continue;
      } 
      int cfg = itC->second;
      bool pyIndex = isPyIndex(cfg);
      bool hzIndex = isHzIndex(cfg);
      bool store = isStore(cfg);
      if(!store && !pyIndex && !hzIndex)
        continue;
      string word = itt->second;
      vector<string> words;
      splitWord(word,words);
      for(unsigned int i=0;i<words.size();++i) {
        if(words[i].size() == 3)  {
          if(hzIndex) {
            // MCE_INFO("AddTerm " << words[i]);
            hzIndex_.addTerm(words[i],id,total+i);
          }
          if(pyIndex){
            StringSeqPtr ssp = Dictionary::instance().getPinyin(words[i]);
            if (ssp) {
              for(vector<string>::iterator it=ssp->seq_.begin();it!=ssp->seq_.end();++it){
                pyIndex_.addTerm(*it,id,total+i);
                //MCE_INFO("AddPY "<<words[i]<<" : "<<(*it)<<" id: "<<id);
              }
            }
          }
        }else if(words[i].size() == 1)  {
          if(hzIndex)
            hzIndex_.addTerm(words[i],id,total+i);
        }
      }
      if(store){
        storeMap.insert(*itt);
      }
      if(pyIndex || hzIndex)
        total +=words.size()+1;
    }
    stores_.push_back(storeMap);
  }
  outTime_ = time(NULL) + timeout;
}

void SearchDataI::splitWord(const string& value, vector<string>& words)
{
  for(vector<string>::size_type i=0;i<value.size();++i) {
    if((value[i]&0x80)!=0) {
      words.push_back(value.substr(i,3));
      i+=2;
    }else {
      words.push_back(string(1,value[i]));
    }   
  }
}


//=================FriendSearchDataI================================
void FriendSearchDataI::load() {}

void FriendSearchDataI::load(int id) {
  ostringstream in;
  in << "Load Friends of User: " << id;
  TimeCost tc = TimeCost::create(in.str(), 1);
  std::map<int, std::string> friends;
  getUserName(id,friends);
  vector<map<string, string> > docs;
  map<string, int> config;
  
  for (map<int, string>::iterator it = friends.begin(); it != friends.end(); ++it) {
    int id = it->first;
    string name = it->second;
    map<string, string> doc;
    doc["id"] = boost::lexical_cast<string>(id);
    doc["name"] = name;
    docs.push_back(doc);
  } 
  // 只存储
  config["id"] = 4;
  // 索引并存储
  config["name"] = 7;

  add(docs, config, DYNAMIC_DATA_TIMEOUT);
  ostringstream os;
  os << "FriendCount: " << docs.size() << " Time:";
  tc.step(os.str());
}

/// @brief FriendInputFactoryI::generateNameMap 将uid对应的好友的名字放到friends中
/// 
/// @param uid 用户ID
/// @param friends 好友ID->好友姓名的映射
void FriendSearchDataI::getUserName(int uid, map<int, string>& friends) {
  MCE_DEBUG("generateNameMap for user:"<<uid);
  try{
    vector<int> friendsIds=BuddyByIdCacheAdapter::instance().getFriendListAsc(uid,6000);
    MCE_DEBUG("TimeCost getBuddyList: "<<friendsIds.size());
    if(friendsIds.size() <= maxSize){
      map<int, UserCachePtr> users=UserCacheAdapter::instance().GetUserCache(friendsIds);
      for(map<int, UserCachePtr>::iterator it=users.begin();it!=users.end();++it) {
        //@modified by zhigang.wu@opi-corp.com: 加入归一化的代码, 将大写字母统一归一化为小写
        //解决如下bug: 1.当好友名字为武志刚，查询时输入WU没有结果 2. 当好友名字为"武志刚Reborn", 输入R能出来，输入r出不来
        string name = it->second->name();
        transform(name.begin(), name.end(), name.begin(), ::tolower);
        friends[it->first] = name;
      }
    }else{
      //若好友数较多，则分多次取ID对应的名字
      vector<int>::iterator itS=friendsIds.begin();
      vector<int>::iterator itE=itS+maxSize;
      while(itS!=friendsIds.end()){
        vector<int> part_ids(itE-itS);
        copy(itS,itE,part_ids.begin());
        itS = itE;
        itE += maxSize;
        if(itE > friendsIds.end())
          itE=friendsIds.end();
        map<int, UserCachePtr> part_users = UserCacheAdapter::instance().GetUserCache(part_ids);
        for(map<int, UserCachePtr>::iterator it=part_users.begin();it!=part_users.end();++it){
          string name = it->second->name();
          transform(name.begin(), name.end(), name.begin(), ::tolower);
          friends[it->first] = name;
        }
      }
    }
  }catch(...){
    MCE_WARN("Buddy||User throw Exception");
  }
  MCE_INFO("Add uid:"<<uid<<" friends :"<<friends.size());
}

//================RecommendUserSearchDataI======================
void RecommendUserSearchDataI::load(int id) {

}

void RecommendUserSearchDataI::load(){
}

//===============PageDataResultHandler==========================
bool PageDataResultHandler::handle(mysqlpp::Row& res) const {
  int id = (int)res["id"];
  int ori_type = (int)res["ori_type"];
  if (ori_type == 5) {
    return true;
  }
  int state = (int)res["state"];
  if (state != 0) {
    return true;
  }
#ifndef NEWARCH
  string name=res["name"].get_string();
#else
  string name=res["name"].c_str();
#endif
  id2Page[id] = name;
  return true;
}

//=================PageSearchDataI================================
void PageSearchDataI::load(int id) {
  load();
}

void PageSearchDataI::load() {
  TimeCost tc = TimeCost::create("Load Page", 1);
  string DB_SEARCH2_PAGE = "search2_page";
  map<int, string> id2Name;
  Statement sql;
  try{
    sql << "SELECT id, name, ori_type, state FROM page";
    PageDataResultHandler handler(id2Name);
    QueryRunner(DB_SEARCH2_PAGE, CDbRServer).query(sql, handler);
  }catch(exception& ex){
    MCE_ERROR("ERROR" << ex.what());
  }catch(...){
    MCE_ERROR("ERROR: reload error");
  }

  vector<map<string, string> > docs;
  for(map<int,string>::iterator it = id2Name.begin(); it != id2Name.end(); ++it) {
    string id = boost::lexical_cast<string>(it->first);
    string name = it->second;
    map<string, string> doc;
    doc["id"] = id;
    doc["name"] = name;
    docs.push_back(doc);
  }

  map<string, int> config;
  // 只存储
  config["id"] = 4;
  // 索引存储
  config["name"] = 7;

  add(docs, config, STATIC_DATA_TIMEOUT);
  ostringstream os;
  os << "TotalCount: " << docs.size() << " Time:";
  tc.step(os.str());
}

//================AppDataResultHandler===========================
bool AppDataResultHandler::handle(mysqlpp::Row& res) const {
  int id=(int)res["app_id"];
#ifndef NEWARCH
  string name=res["app_name"].get_string();
#else
  string name=res["app_name"].c_str();
#endif
  if(name=="")
    return true;
  id2App[id] = name;
  return true;
}
//=================AppSearchDataI================================
void AppSearchDataI::load(int id) {
  load();
}

void AppSearchDataI::load() {
  TimeCost tc = TimeCost::create("Load App", 1);
  string DB_SEARCH2_APP = "search2_app";
  map<int, string> id2Name;
  Statement sql;
  sql << "SELECT app_id, app_name FROM app_productlist";
  AppDataResultHandler handler(id2Name);
  QueryRunner(DB_SEARCH2_APP, CDbRServer).query(sql, handler);

  vector<map<string, string> > docs;
  for(map<int,string>::iterator it = id2Name.begin(); it != id2Name.end(); ++it) {
    string id = boost::lexical_cast<string>(it->first);
    string name = it->second;
    map<string, string> doc;
    doc["id"] = id;
    doc["name"] = name;
    docs.push_back(doc);
  }

  map<string, int> config;
  // 只存储
  config["id"] = 4;
  // 索引存储
  config["name"] = 7;
  add(docs, config, STATIC_DATA_TIMEOUT);
  ostringstream os;
  os << "TotalCount: " << docs.size() << " Time:";
  tc.step(os.str());
}
//=================UniversityDataResultHandler====================
bool UniversityDataResultHandler::handle(mysqlpp::Row& res) const {
  int id=(int)res["id"];
#ifndef NEWARCH
  string name=res["name"].get_string();
#else
  string name=res["name"].c_str();
#endif
  if(name != "") {
    id2Universities[id] = name;
  }
  return true;
}

//=================UniversityDataI================================
void UniversitySearchDataI::load(int id) {
  load();
}

void UniversitySearchDataI::load() {
  TimeCost tc = TimeCost::create("Load University", 1);
  string DB_SEARCH2_UNIVERSITY = "search2_userbasic";
  map<int, string> id2University;
  Statement sql;
  sql << "SELECT id, name FROM university where is_expire=0";
  UniversityDataResultHandler handler(id2University);
  QueryRunner(DB_SEARCH2_UNIVERSITY, CDbRServer).query(sql, handler);

  vector<map<string, string> > docs;
  for(map<int,string>::iterator it = id2University.begin(); it != id2University.end(); ++it) {
    string id = boost::lexical_cast<string>(it->first);
    string name = it->second;
    map<string, string> doc;
    doc["id"] = id;
    doc["name"] = name;
    docs.push_back(doc);
  }

  map<string, int> config;
  // 只存储
  config["id"] = 4;
  // 索引存储
  config["name"] = 7;
  add(docs, config, STATIC_DATA_TIMEOUT);
  ostringstream os;
  os << "TotalCount: " << docs.size() << " Time:";
  tc.step(os.str());
}
//=================WorkplaceDataResultHandler====================
bool WorkplaceDataResultHandler::handle(mysqlpp::Row& res) const {
  int id=(int)res["id"];
#ifndef NEWARCH
  string name=res["name"].get_string();
#else
  string name=res["name"].c_str();
#endif
  if(name != "") {
    id2Workplaces[id] = name;
  }
  return true;
}

//=================WorkplaceSearchDataI==========================
void WorkplaceSearchDataI::load(int id) {
  load();
}

void WorkplaceSearchDataI::load() {
  TimeCost tc = TimeCost::create("Load Workplace", 1);
  string DB_SEARCH2_WORKPLACE = "search2_userbasic";
  map<int, string> id2Workplace;
  Statement sql;
  sql << "SELECT id, name FROM workplace";
  WorkplaceDataResultHandler handler(id2Workplace);
  QueryRunner(DB_SEARCH2_WORKPLACE, CDbRServer).query(sql, handler);

  vector<map<string, string> > docs;
  for(map<int,string>::iterator it = id2Workplace.begin(); it != id2Workplace.end(); ++it) {
    string id = boost::lexical_cast<string>(it->first);
    string name = it->second;
    map<string, string> doc;
    doc["id"] = id;
    doc["name"] = name;
    docs.push_back(doc);
  }

  map<string, int> config;
  // 只存储
  config["id"] = 4;
  // 索引存储
  config["name"] = 7;
  add(docs, config, STATIC_DATA_TIMEOUT);
  ostringstream os;
  os << "TotalCount: " << docs.size() << " Time:";
  tc.step(os.str());
}

