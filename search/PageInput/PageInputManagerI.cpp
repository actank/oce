/*
 * =====================================================================================
 *
 *       Filename:  PageInputManagerI.cpp
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
#include "PageInputManagerI.h"
#include <cmath>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include "util/cpp/TimeCost.h"
#include <algorithm>

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::PageInput;


// =============== MyUtil::initialize ====================================
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(PAGEINPUT_DATA,"FI",new PageInputFactoryI);
	service.getAdapter()->add(&PageInputManagerI::instance(), service.createIdentity("Manager",""));
}
// =============== PageInputFactoryI ===================================
PageInputFactoryI::PageInputFactoryI() {
	reload();
	thread_ = new UpdateThread(this);
	thread_->start().detach();
}

Ice::ObjectPtr PageInputFactoryI::create(Ice::Int id) {
	TimeCost tc= TimeCost::create("createIndex",1);
	MCE_INFO("create for:" << id );
	PageInputDataIPtr obj=new PageInputDataI;
	std::map<int, std::string> mm;
	generateNameMap(id,mm);
	obj->add(mm);
	return obj;
}
void PageInputFactoryI::generateNameMap(Ice::Int uid, map<int, string>& id2PageName) { 
	TimeCost tc = TimeCost::create("GeneratePageNameMap",1);
	MCE_DEBUG("generatePageNameMap for user:"<<uid);
	vector<int> pageIds;
	selectFansPages(uid,pageIds);
	vector<int> missIds;
	ostringstream oss;
	oss<<"load fans from db size:"<<pageIds.size();
	tc.step(oss.str());
	{
	IceUtil::RWRecMutex::RLock lock(mutex_);
	for(size_t i=0;i<pageIds.size();++i){
			map<int,pair<string,unsigned char> >::iterator it=id2Name_.find(pageIds[i]);
			if(it!=id2Name_.end()){
					if ((it->second).second == (unsigned char)0) {
							id2PageName[it->first]=(it->second).first;
					}
			}else{
					MCE_INFO("not find "<<pageIds[i] << ", reload");
					missIds.push_back(pageIds[i]);
			}
	}
	}
	{
	if(!missIds.empty()){
		IceUtil::RWRecMutex::WLock lock(mutex_);
		reloadIds(missIds);
		for(size_t i=0;i<missIds.size();++i){
			pair<map<int,pair<string,unsigned char> >::iterator,bool> res = id2Name_.insert(make_pair<int,pair<string,unsigned char> >(missIds[i],pair<string,unsigned char>("",1)));
			if(res.second){
				MCE_WARN("Warning Remain not find pageid : "<<missIds[i]);
			}
		}
	}
	}
	
}
void PageInputFactoryI::selectFansPages(int uid, vector<int>& ids) { 
	ostringstream oss;
	oss<<"fans_pages_"<<uid%100;
	Statement sql;
	sql << "SELECT page_id FROM fans_pages_"<<uid%100<<" WHERE user_id="<<uid<<" and state=0";
	FansPagesResultHandler handler(ids);
	QueryRunner(DB_FANS_PAGES, CDbRServer, oss.str()).query(sql, handler);
	for(size_t i=0;i<ids.size();++i)
	{
		MCE_INFO("select fans_pages for uid:"<<uid<<" pid:"<<ids[i]);
	}

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
	dict_[name].push_back(pinyin);
	return true;
}
//================ PageNameResultHandler ===================================
bool PageNameResultHandler::handle(mysqlpp::Row& res) const {
  	pair<string, unsigned char> name2state;
  	int id=(int)res["id"];
  	string name=res["name"].c_str();
	unsigned char state = (unsigned char)res["state"];
	if(!name.empty()){
  		name2state = make_pair(name,state);
		id2Name_.insert(make_pair<int,pair<string,unsigned char> >(id,name2state));
	}
	return true;
}
//================ FansPagesResultHandler ===================================
bool FansPagesResultHandler::handle(mysqlpp::Row& res) const {
	int id=(int)res["page_id"];
	ids_.push_back(id);
	return true;
}
// =============== PageInputManagerI ===================================
PageInputManagerI::PageInputManagerI()
{
	try{
		Statement sql;
		sql << "SELECT hanzi, pinyin FROM pinyin";
		PinYinResultHandler handler(newSet, dict_);
		QueryRunner(DB_FRIENDINPUT_PINYIN, CDbRServer).query(sql, handler);
	}catch(...){
		MCE_WARN("initialize error");
	}
//	reload();
}
void PageInputFactoryI::reload(){
	TimeCost tc = TimeCost::create("reload",1);
	MCE_INFO("reload all from db");
	map<int,pair<string,unsigned char> > id2NameNew_;
	try{
		Statement sql;
		sql << "SELECT id, name, state FROM page";
		PageNameResultHandler handler(id2NameNew_);
		QueryRunner(DB_PAGE_NAME, CDbWServer).query(sql, handler);
		IceUtil::RWRecMutex::WLock lock(mutex_);
    id2Name_ = id2NameNew_;
//		for(map<int,pair<string,unsigned char> >::iterator it=id2Name_.begin();it!=id2Name_.end();++it){
//			MCE_INFO("reload id:"<<it->first<<" name:"<<(it->second).first);
//		}
	}catch(exception& ex){
		MCE_WARN(ex.what());
	}catch(...){
		MCE_WARN("reload error");
	}
}
void PageInputFactoryI::reloadIds(const vector<int>& ids){
	MCE_INFO("reload ids from db");
	map<int,pair<string,unsigned char> > id2NameNew_;
	ostringstream oss;
	for(int i=0;i<ids.size();++i){
		if(i>0)
			oss<<",";
		oss<<ids[i];
	}
	try{
		Statement sql;
		sql << "SELECT id, name, state FROM page WHERE id in ( "<<oss.str()<<" )";
		PageNameResultHandler handler(id2Name_);
		QueryRunner(DB_PAGE_NAME, CDbWServer).query(sql, handler);
	}catch(exception& ex){
		MCE_WARN(ex.what());
	}catch(...){
		MCE_WARN("reload error");
	}
}
//map<int,string> PageInputManagerI::getPageName(){
//	return id2NameNew_;
//}
MyUtil::IntSeq PageInputManagerI::getPageInput(Ice::Int hostID,const string& condition,Ice::Int limit,const Ice::Current& crt) {
	MCE_INFO("getPageInput hostID= "<<hostID<<" condition="<<condition<<" limit="<<limit);
	//MCE_INFO(crt.adapter->getName()<<" "<<crt.facet<<" "<<crt.operation<<" "<<crt.requestId);
	if(condition.size()>15||condition.size()==0)
		return vector<int>();
	else
		return ServiceI::instance().locateObject<PageInputDataIPtr>(PAGEINPUT_DATA, (long)hostID)->search(condition,limit);
};

// ============== PageInputDataI ======================================
void PageInputDataI::splitWord(const string& value, vector<string>& words)
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
void PageInputDataI::add(std::map<int,std::string>& result)
{
	TimeCost tc = TimeCost::create("AddNameMap",1);
	friends.swap(result);
	string name;
	unsigned int id;
	for(map<int,string>::iterator it=friends.begin();it!=friends.end();++it) {
		name=it->second;
		id=it->first;
		vector<string> words;
		splitWord(name,words);
		for(unsigned int i=0;i<words.size();++i) {
			postingsHZ[words[i]].push_back(id);
		}
		for(unsigned int i=0;i<words.size();++i) {
			if(words[i].size()==3) {
				vector<string> pinyins=PageInputManagerI::instance().dict_[words[i]];
				for(vector<string>::iterator it_vec=pinyins.begin();it_vec!=pinyins.end();++it_vec) {
					postingsPY[(*it_vec)].push_back(id);
				}
			}			
		}
	}
}
	bool PageInputDataI::isPinYin(const std::string& str, const std::vector<std::string> words) {
		if(words.size()<str.size())
		{
			return false;
		}
		else {
			for(std::size_t i=0;i<words.size();++i) {
				if(words[i]<"a"||words[i]>"z")
				{
					return false;
				}
			}
			return true;
		}
	}


vector<int> PageInputDataI::getPrefix(string query) {
	vector<int> tmp;
	map<string,vector<int> >::iterator it=postingsPY.lower_bound(query);
	while(it!=postingsPY.end()) {
		if(startsWith(it->first,query))
			union_vec(tmp,it->second);	
		else
			break;
		++it;
	}
	return tmp;
}

void PageInputDataI::intersect_vec(std::vector<int>& A,const std::vector<int>& B)
{
	vector<int> C;
	vector<int>::iterator itA=A.begin();
	vector<int>::const_iterator itB=B.begin();
	while(itA!=A.end()&&itB!=B.end()) {
		if((*itA)==(*itB)) {
			C.push_back(*itA);	
			++itA;
			++itB;
		}else if((*itA)>(*itB)) {
			++itB;				
		}else {
			++itA;				
		}	
	}
	A.swap(C);
}

void PageInputDataI::union_vec(std::vector<int>&A, const std::vector<int>& B) {
	vector<int> C;
	vector<int>::iterator itA=A.begin();
	vector<int>::const_iterator itB=B.begin();
	while(itA!=A.end()&&itB!=B.end()){
		if((*itA)==(*itB)){
			C.push_back(*itA);
			++itA;
			++itB;
		}else if((*itA)<(*itB)){
			C.push_back(*itA);
			++itA;
		}else{
			C.push_back(*itB);
			++itB;
		}
	}
	while(itA!=A.end()){
		C.push_back(*itA);
		++itA;
	}
	while(itB!=B.end()){
		C.push_back(*itB);
		++itB;
	}
	A.swap(C);
}
bool PageInputDataI::startsWith(const std::string& base, const std::string& pre) {
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


MyUtil::IntSeq PageInputDataI::search(const string& query,Ice::Int limit)
{
	TimeCost tc = TimeCost::create("Search");
	MCE_DEBUG("begin search " << query);
	set<int> value;
	vector<string> words;
	splitWord(query, words);
	if(isPinYin(query, words)) {
		MCE_DEBUG("begin pinyin search");
		vector<string> subStr(query.size());
		splitPY(query,0,0,subStr, value);
		if(!value.empty())
			return select(value,limit);
	}
	return searchHZ(query,limit);
}

vector<int> PageInputDataI::select(const set<int>& mySet,unsigned int limit) {
	size_t count = 0;
	vector<int> res;
	for(set<int>::const_iterator it=mySet.begin();it!=mySet.end() && count<limit;++it,++count){
		res.push_back(*it);	
		MCE_INFO("Find PY:"<<res[count]<<" "<<friends[res[count]]);
	}
	MCE_INFO("getPagesnumber : "<<res.size());
	return res;
}

bool PageInputDataI::reChoose(const int& doc,const vector<std::string>& strs,const int& len) {
	int inc=0;
	vector<string> words;
	splitWord(friends[doc], words);
	for(unsigned int i=0;i<words.size()&&inc<len;++i) {
		if(words[i].size()==3) {
			vector<string>& pinyins=PageInputManagerI::instance().dict_[words[i]];
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
void PageInputDataI::splitPY(const string& pinyin,unsigned int start,int num,vector<string> subStr,set<int>& value) {
	if(start>=pinyin.size()) {
		vector<int> temp=getPrefix(subStr[0]);
		for(int i=1;i<num;i++) {
			intersect_vec(temp,getPrefix(subStr[i]));
			if(temp.empty())
				return;
		}
		for(vector<int>::iterator it=temp.begin();it!=temp.end();++it) {
			if(reChoose(*it,subStr,num)) {
				value.insert(*it);
			}
		}
		temp.clear();
		return;
	}else {
		for(unsigned int end=start+1;end<=pinyin.size();++end) {
			string temp=pinyin.substr(start,end-start);
			map<string,vector<int> >::iterator it=postingsPY.lower_bound(temp);
			if(it!=postingsPY.end()&&startsWith(it->first,temp)) {
				subStr[num]=temp;
				splitPY(pinyin,end,num+1,subStr,value);			
			}		
		}	
	}
}		

vector<int> PageInputDataI::searchHZ(const string& name,const int& limit) {
	map<string,vector<int> >::iterator it;
	vector<string> words;
	splitWord(name, words);
	vector<int> results=postingsHZ[words[0]];
	for(unsigned int i=1;i<words.size();++i) {
		it=postingsHZ.find(words[i]);
		if( it==postingsHZ.end() ){
			return vector<int>();
		}
		intersect_vec(results,it->second);
		if(results.empty())
			return results;
	}
	if(results.size()>(size_t)limit)
		results.resize(limit);
	for(size_t i=0;i<results.size();++i){
		MCE_INFO("Find HZ: "<<results[i]<<" "<<friends[results[i]]);
	}
	MCE_INFO("get Pages number : "<<results.size());
	return results;
}
