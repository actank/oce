#include "CardImportSearcher.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include <algorithm>

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::buddy::adapter;
using namespace xce::usercache;
using namespace xce::cardsearch;
using namespace xce::serverstate;
using namespace xce::cardcache;


/// @brief initialize 初始化
void MyUtil::initialize(){
  ServiceI& service = ServiceI::instance();
  service.registerObjectCache(CARD_IMPORT_SEARCHER,"CIS",new CardImportSearcherFactoryI);
  service.getAdapter()->add(&CardImportSearcherI::instance(),service.createIdentity("Manager",""));
  ServerStateSubscriber::instance().initialize("ControllerCardImportSearcher",&CardImportSearcherI::instance(),1,5);
}

void CardImportSearcherI::loadPY() {
  MCE_INFO("[CardImportSearcherI::loadPY]");
  Statement sql;
  sql << "SELECT hanzi, pinyin FROM pinyin";
  PinYinResultHandler handler(newSet, dict_);
  QueryRunner(DB_FRIENDINPUT_PINYIN, CDbRServer).query(sql, handler);
}

CardImportSearcherI::CardImportSearcherI(){
  try{
    MCE_INFO("Front [CardSearcherI::CardSearcherI()] ");
    loadPY();
    MCE_INFO("After [CardSearcherI::CardSearcherI()] ");
  }catch(std::exception& ex){
    cout<<ex.what()<<endl;
  }
}
Ice::ObjectPtr CardImportSearcherFactoryI::create(::Ice::Long userId) {
	MCE_INFO("[CardImportSearcherFactoryI::create] userId:"<<userId);
  CardImportSearcherDataPtr data = new CardImportSearcherData();

  try{
      /// @brief addIndex 创建索引 
      ImportInfoSeq importInfos = CardImportCacheAdapter::instance().get(userId,-1);
		  data->addIndex(importInfos);
      //增加另一个索引
      ImportInfoNewSeq importInfosNew = CardImportCacheAdapter::instance().getNew(userId,-1);
      data->addIndexNew(importInfosNew);
		  MCE_INFO("[CardImportSearcherFactoryI::create] create done");
  }catch(...){
	return NULL;
  }
  return data;
}

void CardImportSearcherData::addIndex(const ImportInfoSeq& infos) {
  MCE_INFO("[CardImportSearcherData::addIndex] infos.size():" << infos.size());
  //得到索引字段与值的map
  for(ImportInfoSeq::const_iterator it=infos.begin(); it!=infos.end(); ++it){
    ImportValuePtr ptr = new ImportValue;
    ptr->importDetail = (*it)->importDetail;
    ptr->importWay = (*it)->importWay;
    ptr->name = (*it)->name;
    if ((*it)->name != "") {
      MCE_INFO("[CardImportSearcherData::addIndex] name:" << (*it)->name);
      dict[(*it)->name].push_back(ptr);
    }
    if ((*it)->qq != "") {
      MCE_INFO("[CardImportSearcherData::addIndex] qq:" << (*it)->qq);
      dict[(*it)->qq].push_back(ptr);
    }
    if ((*it)->mobile != "") {
      MCE_INFO("[CardImportSearcherData::addIndex] mobile:" << (*it)->mobile);
      dict[(*it)->mobile].push_back(ptr);
    }
    if ((*it)->msn != "") {
      MCE_INFO("[CardImportSearcherData::addIndex] msn:" << (*it)->msn);
      dict[(*it)->msn].push_back(ptr);
    }
    if ((*it)->email != "") {
      MCE_INFO("[CardImportSearcherData::addIndex] email:" << (*it)->email);
      dict[(*it)->email].push_back(ptr);
    }
    if((*it)->name != ""){
      addPYIndex((*it)->name, ptr);
    }
  }	
  MCE_INFO("[CardImportSearcherData::addIndex] add done");
}

void CardImportSearcherData::addPYIndex(const std::string name, const ImportValuePtr& ptr) {
  MCE_INFO("[CardImportSearcherData::addPYIndex] name:" << name);
  vector<string> words;
  splitWord(name,words);

  for(unsigned int i=0;i<words.size();++i) {
    if(words[i].size()==3) {
//      MCE_INFO("words[i].size() " << words[i].size() << "; words[i] " << words[i]);
      vector<string> pinyins=CardImportSearcherI::instance().dict_[words[i]];
      MCE_INFO("[CardImportSearcherData::addPYIndex] words[i]:" << words[i] << "; pinyins.size():" << pinyins.size());
//      MCE_INFO("dict_[ " << words[i] << "]=" << pinyins);
      for(vector<string>::iterator it_vec=pinyins.begin();it_vec!=pinyins.end();++it_vec) {
        postingsPY[(*it_vec)].push_back(ptr);
      }
    }
  }
}

void CardImportSearcherData::addIndexNew(const ImportInfoNewSeq& infos) {
  MCE_INFO("[CardImportSearcherData::addIndexNew] infos.size():" << infos.size());
  //得到索引字段与值的map
  for(ImportInfoNewSeq::const_iterator it=infos.begin(); it!=infos.end(); ++it){
    ImportValueNewPtr ptr = new ImportValueNew;
    ptr->id = (*it)->id;
    ptr->importDetail = (*it)->importDetail;
    ptr->importWay = (*it)->importWay;
    ptr->name = (*it)->name;
    if ((*it)->name != "") {
      MCE_INFO("[CardImportSearcherData::addIndex] name:" << (*it)->name);
      dictNew[(*it)->name].push_back(ptr);
    }
    if ((*it)->qq != "") {
      MCE_INFO("[CardImportSearcherData::addIndex] qq:" << (*it)->qq);
      dictNew[(*it)->qq].push_back(ptr);
    }
    if ((*it)->mobile != "") {
      MCE_INFO("[CardImportSearcherData::addIndex] mobile:" << (*it)->mobile);
      dictNew[(*it)->mobile].push_back(ptr);
    }
    if ((*it)->msn != "") {
      MCE_INFO("[CardImportSearcherData::addIndex] msn:" << (*it)->msn);
      dictNew[(*it)->msn].push_back(ptr);
    }
    if ((*it)->email != "") {
      MCE_INFO("[CardImportSearcherData::addIndex] email:" << (*it)->email);
      dictNew[(*it)->email].push_back(ptr);
    }
    if((*it)->name != ""){
      addPYIndexNew((*it)->name, ptr);
    }
  }	
  MCE_INFO("[CardImportSearcherData::addIndex] add done");
}

void CardImportSearcherData::addPYIndexNew(const std::string name, const ImportValueNewPtr& ptr) {
  MCE_INFO("[CardImportSearcherData::addPYIndexNew] name:" << name);
  vector<string> words;
  splitWord(name,words);

  for(unsigned int i=0;i<words.size();++i) {
    if(words[i].size()==3) {
//      MCE_INFO("words[i].size() " << words[i].size() << "; words[i] " << words[i]);
      vector<string> pinyins=CardImportSearcherI::instance().dict_[words[i]];
      MCE_INFO("[CardImportSearcherData::addPYIndex] words[i]:" << words[i] << "; pinyins.size():" << pinyins.size());
//      MCE_INFO("dict_[ " << words[i] << "]=" << pinyins);
      for(vector<string>::iterator it_vec=pinyins.begin();it_vec!=pinyins.end();++it_vec) {
        postingsPYNew[(*it_vec)].push_back(ptr);
      }
    }
  }
}

bool CardImportSearcherData::startsWith(const std::string& base, const std::string& pre) {
  if (base.size()<pre.size()) {
    return false;
  }
  int pc=pre.size();
  int po=0;
  while (--pc >= 0) {
    if (base[po] != pre[po++] ) {
      return false;
    }
  }
  return true;
}
bool CardImportSearcherData::shouldOut(){
	if(time(NULL) - indexTime > OUTTIME){
		return true;
	}else{
		return false;
	}	
}

ImportValueSeq CardImportSearcherData::search(const string& query,int limit){
  MCE_INFO("[CardImportSearcherData::search] query:" << query);
  ImportValueSeq result, resultPY;
  string transformed_query(query);
  transform(transformed_query.begin(), transformed_query.end(), transformed_query.begin(), ::tolower);
  vector<string> words;
  splitWord(transformed_query, words);
  
  if(isPinYin(transformed_query, words)) {
    searchPY(resultPY, transformed_query, limit);
  }

  map<string,vector<ImportValuePtr> >::iterator it = dict.lower_bound(query);
  while(it!=dict.end()) {
    MCE_INFO("[CardImportSearcherData::search] key:"<<it->first<< " query:"<<query);
    if(startsWith(it->first,query)){
      for(vector<ImportValuePtr>::iterator itt=it->second.begin();itt!=it->second.end();++itt){
        if (!isExist(*itt, result)) {
          result.push_back(*itt);
        }
        if((int)result.size() >= limit && limit != -1) {
          break;
        }
      }
    }else {
      break;
    }
    ++it;
  }

  for (ImportValueSeq::iterator itPY = resultPY.begin(); itPY != resultPY.end(); ++itPY) {
    if (!isExist(*itPY, result)) {
      result.push_back(*itPY);
    }
  }

  return result;

}

ImportValueNewSeq CardImportSearcherData::searchNew(const string& query,int limit){
  MCE_INFO("[CardImportSearcherData::search] query:" << query);
  ImportValueNewSeq result, resultPY;
  string transformed_query(query);
  transform(transformed_query.begin(), transformed_query.end(), transformed_query.begin(), ::tolower);
  vector<string> words;
  splitWord(transformed_query, words);
  
  if(isPinYin(transformed_query, words)) {
    searchPYNew(resultPY, transformed_query, limit);
  }

  map<string,vector<ImportValueNewPtr> >::iterator it = dictNew.lower_bound(query);
  while(it!=dictNew.end()) {
    MCE_INFO("[CardImportSearcherData::search] key:"<<it->first<< " query:"<<query);
    if(startsWith(it->first,query)){
      for(vector<ImportValueNewPtr>::iterator itt=it->second.begin();itt!=it->second.end();++itt){
        if (!isExistNew(*itt, result)) {
          result.push_back(*itt);
        }
        if((int)result.size() >= limit && limit != -1) {
          break;
        }
      }
    }else {
      break;
    }
    ++it;
  }

  for (ImportValueNewSeq::iterator itPY = resultPY.begin(); itPY != resultPY.end(); ++itPY) {
    if (!isExistNew(*itPY, result)) {
      result.push_back(*itPY);
    }
  }

  return result;

}
//判断要插入的ImportValue是否存在，存在返回ture，否则返回false
bool CardImportSearcherData::isExist(const ImportValuePtr& ptr, const ImportValueSeq& values) {
  ImportValueSeq::const_iterator itR;
  unsigned char way = ptr->importWay;
  std::string detail = ptr->importDetail;
  std::string name = ptr->name;
  itR = std::find_if(values.begin(), values.end(), findImportInfo(way, detail, name));
  if (itR != values.end()) {
    return true;
  } else {
    return false;
  }
}

bool CardImportSearcherData::isExistNew(const ImportValueNewPtr& ptr, const ImportValueNewSeq& values) {
  ImportValueNewSeq::const_iterator itR;
  unsigned char way = ptr->importWay;
  std::string detail = ptr->importDetail;
  std::string name = ptr->name;
  itR = std::find_if(values.begin(), values.end(), findImportInfoNew(way, detail, name));
  if (itR != values.end()) {
    return true;
  } else {
    return false;
  }
}

void CardImportSearcherData::searchPY(ImportValueSeq& resultPY, const string& transformed_query,int limit){
  MCE_INFO("[CardSearcherData::searchPY] query:" << transformed_query);
  set<ImportValuePtr> value;
  vector<ImportValuePtr> res;
  vector<string> subStr(transformed_query.size());
  splitPY(transformed_query,0,0,subStr, value);
  if(!value.empty())  {
    MCE_INFO("[CardSearcherData::searchPY] value.size():" << value.size());
    select(resultPY,value,limit);
  }
}

void CardImportSearcherData::searchPYNew(ImportValueNewSeq& resultPY, const string& transformed_query,int limit){
  MCE_INFO("[CardSearcherData::searchPYNew] query:" << transformed_query);
  set<ImportValueNewPtr> value;
  vector<ImportValueNewPtr> res;
  vector<string> subStr(transformed_query.size());
  splitPYNew(transformed_query,0,0,subStr, value);
  if(!value.empty())  {
    MCE_INFO("[CardSearcherData::searchPYNew] value.size():" << value.size());
    selectNew(resultPY,value,limit);
  }
}


void CardImportSearcherData::splitWord(const string& value, vector<string>& words)
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

void CardImportSearcherData::splitPY(const string& pinyin,unsigned int start,int num,vector<string> subStr,set<ImportValuePtr>& value) {
  MCE_INFO("[CardSearcherData::splitPY] start:" << start << "; num:" << start << "; subStr.size():" << subStr.size() << "; value.size():" << value.size());
  string result = "";
  if(start>=pinyin.size()) {
      set<ImportValuePtr> temp=getPrefix(subStr[0]);
      result = subStr[0];
    for(int i=1;i<num;i++) {
      intersect_vec(temp,getPrefix(subStr[i]));
      result =result + " " + subStr[i];
      if(temp.empty())
        return;
    }
    MCE_INFO("[CardSearcherData::splitPY] RESULT:" <<  result);
    temp.swap(value);
    return;
  }else {
    for(unsigned int end=start+1;end<=pinyin.size();++end) {
      string temp=pinyin.substr(start,end-start);
      map<string,vector<ImportValuePtr> >::iterator it=postingsPY.lower_bound(temp);
      if(it!=postingsPY.end()&&startsWith(it->first,temp)) {
        subStr[num]=temp;
        splitPY(pinyin,end,num+1,subStr,value);
      }
    }
  }
}

void CardImportSearcherData::splitPYNew(const string& pinyin,unsigned int start,int num,vector<string> subStr,set<ImportValueNewPtr>& value) {
  MCE_INFO("[CardSearcherData::splitPYNew] start:" << start << "; num:" << start << "; subStr.size():" << subStr.size() << "; value.size():" << value.size());
  string result = "";
  if(start>=pinyin.size()) {
      set<ImportValueNewPtr> temp=getPrefixNew(subStr[0]);
      result = subStr[0];
    for(int i=1;i<num;i++) {
      intersect_vecNew(temp,getPrefixNew(subStr[i]));
      result =result + " " + subStr[i];
      if(temp.empty())
        return;
    }
    MCE_INFO("[CardSearcherData::splitPY] RESULT:" <<  result);
    temp.swap(value);
    return;
  }else {
    for(unsigned int end=start+1;end<=pinyin.size();++end) {
      string temp=pinyin.substr(start,end-start);
      map<string,vector<ImportValueNewPtr> >::iterator it=postingsPYNew.lower_bound(temp);
      if(it!=postingsPYNew.end()&&startsWith(it->first,temp)) {
        subStr[num]=temp;
        splitPYNew(pinyin,end,num+1,subStr,value);
      }
    }
  }
}
/*
bool CardSearcherData::reChoose(const int& doc,const vector<std::string>& strs,const int& len) {
  int inc=0;
  vector<string> words;
  splitWord(friends[doc], words);
  for(unsigned int i=0;i<words.size()&&inc<len;++i) {
    if(words[i].size()==3) {
      vector<string>& pinyins=FriendInputManagerI::instance().dict_[words[i]];
      if(!pinyins.empty()) {
        for(vector<string>::iterator it=pinyins.begin();it!=pinyins.end();++it) {
          if(startsWith(*(it),strs[inc])) {
            ++inc;
            break;
          }
        }
      }
    }
  }
  if(inc==len)
    return true;
  else
    return false;
}
*/

void CardImportSearcherData::select(vector<ImportValuePtr>& res, const set<ImportValuePtr>& mySet,unsigned int limit) {
  size_t count = 0;
  for(set<ImportValuePtr>::const_iterator it=mySet.begin();it!=mySet.end() && count<limit;++it,++count){
    res.push_back(*it);
  }
  MCE_INFO("[CardImportSearcherData::select] getFriends number:"<<res.size());
}

void CardImportSearcherData::selectNew(vector<ImportValueNewPtr>& res, const set<ImportValueNewPtr>& mySet,unsigned int limit) {
  size_t count = 0;
  for(set<ImportValueNewPtr>::const_iterator it=mySet.begin();it!=mySet.end() && count<limit;++it,++count){
    res.push_back(*it);
  }
  MCE_INFO("[CardImportSearcherData::selectNew] getFriends number:"<<res.size());
}


ImportValueSeq CardImportSearcherI::searchImport(Ice::Int userId, const std::string& query, Ice::Int limit, const ::Ice::Current& current) {
  ImportValueSeq res;
  CardImportSearcherDataPtr p = ServiceI::instance().locateObject<CardImportSearcherDataPtr>(CARD_IMPORT_SEARCHER, userId);
  if(p->shouldOut()){
    ServiceI::instance().removeObject(CARD_IMPORT_SEARCHER, userId);	
    p = ServiceI::instance().locateObject<CardImportSearcherDataPtr>(CARD_IMPORT_SEARCHER, userId);	
  }
  res = p->search(query,limit);
  MCE_INFO("[CardImportSearcherI::search] "<<userId<<" query:"<<query<<" result:"<<res.size() << " " << current.con->toString());
  return res;
}

ImportValueNewSeq CardImportSearcherI::searchImportNew(Ice::Int userId, const std::string& query, Ice::Int limit, const ::Ice::Current& current) {
  ImportValueNewSeq res;
  CardImportSearcherDataPtr p = ServiceI::instance().locateObject<CardImportSearcherDataPtr>(CARD_IMPORT_SEARCHER, userId);
  if(p->shouldOut()){
    ServiceI::instance().removeObject(CARD_IMPORT_SEARCHER, userId);	
    p = ServiceI::instance().locateObject<CardImportSearcherDataPtr>(CARD_IMPORT_SEARCHER, userId);	
  }
  res = p->searchNew(query,limit);
  MCE_INFO("[CardImportSearcherI::searchNew] "<<userId<<" query:"<<query<<" result:"<<res.size() << " " << current.con->toString());
  return res;
}

//================ PinYinResultHandler ===================================
bool PinYinResultHandler::handle(mysqlpp::Row& res) const {
  MCE_INFO("[PinYinResultHandler::handle]");
#ifndef NEWARCH
  string name=res["hanzi"].get_string();
  string pinyin=res["pinyin"].get_string();
#else
  string name=res["hanzi"].c_str();
  string pinyin=res["pinyin"].c_str();
#endif
  if(pinyin=="")
    return false;
  dict_[name].push_back(pinyin);
  MCE_INFO("name  " << name << "; pinyin  " << pinyin);
  return true;
}

