/*
 * =====================================================================================
 *
 *       Filename:  SearchManagerI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年07月29日 15时03分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tanbokan (), bokan.tan@opi-corp.com
 *        Company:  opi-corp.com
 *
 * =====================================================================================
 */
#include "SearchManagerI.h"
#include <cmath>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include "IceExt/src/Channel.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace MyUtil;
using namespace xce::serverstate;
using namespace com::xiaonei::xce;
using namespace xce::generalsuggestion;

// =============== MyUtil::initialize ====================================
void MyUtil::initialize() {
	MCE_INFO("init dict");
	Dictionary::instance().init();
  MCE_INFO("load data");
  SearchManagerI::instance().initialize();
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&SearchManagerI::instance(), service.createIdentity("M",""));
	MCE_INFO("init HA");
    ServerStateSubscriber::instance().initialize("ControllerGeneralSuggestion",&SearchManagerI::instance(), 10, 5, new MyUtil::GadgetsChannel);
	MCE_INFO("init done");

}
//================ PinYinResultHandler ===================================
bool PinYinResultHandler::handle(mysqlpp::Row& res) const {
#ifndef NEWARCH
	string name=res["hanzi"].get_string();
	string pinyin=res["pinyin"].get_string();
#else
	string name=res["hanzi"].c_str();
	string pinyin=res["pinyin"].c_str();
#endif
	if(pinyin=="")
	  return false;
	pinyin_.insert(pinyin);
	map<string,StringSeqPtr>::iterator it=dict_.find(name);
	if(it!=dict_.end()){
		(it->second)->seq_.push_back(pinyin);
	}else{
		StringSeqPtr p = new StringSeq;	
		p->seq_.push_back(pinyin);
		dict_[name] = p;
	}
	return true;
}

// =============== SearchManagerI ===================================

SuggestionResultPtr SearchManagerI::search(int userid,const string& query,int begin,int limit,const StrSeq& types,const Ice::Current& crt) {
  TimeCost tc = TimeCost::create("SearchQuery: " + query, 1);
	SuggestionResultPtr result = new SuggestionResult;
	for(vector<string>::const_iterator it=types.begin();it!=types.end();++it){
		string key = StrUtil::getKey(*it, userid);
		MCE_INFO("search "+key);
		SearchDataIPtr sp = getCachedData(key);
    if (!sp) {
      load(*it, userid);
      sp = getCachedData(key);
    }
		if(sp){
			IndexItemPtr p = new IndexItem;
			p->type = *it;
      TimeCost tc2 = TimeCost::create("SearchType: [" + *it + "] Query: " + query, 1);
			p->docs = sp->search(query,begin,limit);
      tc2.step("Time: ");
			result->results.push_back(p);
			if(sp->timeout()){
				reload(*it, userid);
			}
		}else{
			result->missIndexs.push_back(*it);
		}
	}
  tc.step("SearchTime: ");
	return result;
};
void SearchManagerI::load(int userid,const IndexItemSeq& indexItems, const Str2IntMap& config,const Ice::Current& crt) {
  if (indexItems.empty()  || config.empty()) {
    ostringstream os;
    os << "Load Friend and May Knows of User: " << userid;
    TimeCost tc = TimeCost::create(os.str(), 1);
    load(FRIEND, userid);
    load(RECOMMEND_USER, userid);
    tc.step("Time: ");
    return;
  }
  for(vector<IndexItemPtr>::const_iterator it=indexItems.begin();it!=indexItems.end();++it){
    string type = (*it)->type;
    string key = StrUtil::getKey(type, userid);
    int sizeT = (*it)->docs.size();
    MCE_INFO("load:" << key << " size:" << sizeT << " Type: " << type);

    SearchDataIPtr p = SearchDataIUtil::getSearchDataIPtr(type);
    if (!p) {
      MCE_INFO("Invalide Type: " << type);
      return;
    }
    p->add((*it)->docs,config, DYNAMIC_DATA_TIMEOUT);
    if (FRIEND == type || RECOMMEND_USER == type) {
      dynamic_cache_.add(p,key);
    } else {
      static_cache_.add(p,key);
    }
  }
}

void SearchManagerI::initialize() {
  MCE_INFO("load static data: Page");
  SearchDataIPtr page = new PageSearchDataI;
  page->load();
  string key = StrUtil::getKey(PAGE, 0);
  static_cache_.add(page, key);


  MCE_INFO("load static data: App");
  SearchDataIPtr app = new AppSearchDataI;
  app->load();
  static_cache_.add(app, StrUtil::getKey(APP, 0));

  MCE_INFO("load static data: University");
  SearchDataIPtr univ = new UniversitySearchDataI;
  univ->load();
  static_cache_.add(univ, StrUtil::getKey(UNIVERSITY, 0));

  MCE_INFO("load static data: Workplace");
  SearchDataIPtr workplace = new WorkplaceSearchDataI;
  workplace->load();
  static_cache_.add(workplace, StrUtil::getKey(WORKPLACE, 0));

  MCE_INFO("load static data over!");
}

void SearchManagerI::load(string type, int id) {
  string key = StrUtil::getKey(type, id);
  SearchDataIPtr sp = getCachedData(key);
  if (sp) {
    if (sp->timeout()) {
      reload(type, id);
    }
    return;
  }
  if (type == FRIEND || type == RECOMMEND_USER) {
    sp = SearchDataIUtil::getSearchDataIPtr(type);
  } else {
    MCE_INFO("Invalide Type: " << type);
    return;
  }
  if (sp) {
    sp->load(id);
    dynamic_cache_.add(sp, key);
  }
}

void SearchManagerI::reload(string type, int id) {
  MCE_INFO("begin LoadThread... " << type);
  LoadThreadPtr thread = new LoadThread(type, id);
  thread->start();
}

