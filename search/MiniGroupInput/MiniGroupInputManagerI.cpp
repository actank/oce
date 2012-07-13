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
#include "MiniGroupInputManagerI.h"
#include <cmath>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include "util/cpp/TimeCost.h"
#include <algorithm>

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::minigroup;


// =============== MyUtil::initialize ====================================
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(MINIGROUPINPUT_DATA,"FI",new MiniGroupInputFactoryI);
	service.getAdapter()->add(&MiniGroupInputManagerI::instance(), service.createIdentity("Manager",""));
}
// =============== MiniGroupInputFactoryI ===================================
MiniGroupInputFactoryI::MiniGroupInputFactoryI() {
	reload();
	thread_ = new UpdateThread(this);
	thread_->start().detach();
}

Ice::ObjectPtr MiniGroupInputFactoryI::create(Ice::Int id) {
	TimeCost tc= TimeCost::create("createIndex",1);
	MCE_INFO("create for:" << id );
	MiniGroupInputDataIPtr obj=new MiniGroupInputDataI;
	std::map<int, std::string> mm;
	generateNameMap(id,mm);
	obj->add(mm);
	return obj;
}
void MiniGroupInputFactoryI::generateNameMap(Ice::Int uid, map<int, string>& id2PageName) { 
	TimeCost tc = TimeCost::create("GeneratePageNameMap",1);
	MCE_DEBUG("generatePageNameMap for user:"<<uid);
	vector<int> pageIds;
	selectFansMiniGroups(uid,pageIds);
	vector<int> missIds;
	ostringstream oss;
	oss<<"load fans from db size:"<<pageIds.size();
	tc.step(oss.str());
	{
	IceUtil::RWRecMutex::RLock lock(mutex_);
	for(size_t i=0;i<pageIds.size();++i){
			map<int,pair<string,unsigned char> >::iterator it=id2Name_.find(pageIds[i]);
			if(it!=id2Name_.end()){
					id2PageName[it->first]=(it->second).first;
			}else{
					MCE_INFO("not find "<<pageIds[i] << ", reload");
					missIds.push_back(pageIds[i]);
			}
	}
	}
//	{
//	if(!missIds.empty()){
//		IceUtil::RWRecMutex::WLock lock(mutex_);
//		reloadIds(missIds);
//		for(size_t i=0;i<missIds.size();++i){
//			pair<map<int,pair<string,unsigned char> >::iterator,bool> res = id2Name_.insert(make_pair<int,pair<string,unsigned char> >(missIds[i],pair<string,unsigned char>("",1)));
//			if(res.second){
//				MCE_WARN("Warning Remain not find pageid : "<<missIds[i]);
//			}
//		}
//	}
//	}
	
}
void MiniGroupInputFactoryI::selectFansMiniGroups(int uid, vector<int>& ids) { 
	ostringstream oss;
	oss<<"member_minigroup_"<<uid%100;
	Statement sql;
  sql << "SELECT minigroup_id from member_minigroup_"<<uid%100<<" WHERE member_id="<<uid<<"  AND (access_id=1 or access_id=2 or access_id=3)";
	FansMiniGroupsResultHandler handler(ids);
	QueryRunner(DB_MINIGROUP_SOURCE, CDbRServer).query(sql, handler);
	for(size_t i=0;i<ids.size();++i)
	{
		MCE_INFO("select minigroup for uid:"<<uid<<" gid:"<<ids[i]);
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
bool MiniGroupNameResultHandler::handle(mysqlpp::Row& res) const {
  	pair<string, unsigned char> name2state;
  	int id=(int)res["id"];
  	string name=res["name"].c_str();
		unsigned char state = 0;
		if(!name.empty()){
  		name2state = make_pair(name,state);
			id2Name_.insert(make_pair<int,pair<string,unsigned char> >(id,name2state));
		}
		return true;
}
//================ FansMiniGroupsResultHandler ===================================
bool FansMiniGroupsResultHandler::handle(mysqlpp::Row& res) const {
	int id=(int)res["minigroup_id"];
	ids_.push_back(id);
	return true;
}
// =============== PageInputManagerI ===================================
MiniGroupInputManagerI::MiniGroupInputManagerI()
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
void MiniGroupInputFactoryI::reload(){
	TimeCost tc = TimeCost::create("reload",1);
	MCE_INFO("reload all from db");
	map<int,pair<string,unsigned char> > id2NameNew_;
  for(int i=0;i<100;++i){
		try{
					Statement sql;
					sql << "select id,name from minigroup_"<<i<<" where status=1";
					MiniGroupNameResultHandler handler(id2NameNew_);
					QueryRunner(DB_MINIGROUP_SOURCE, CDbWServer).query(sql, handler);
									//		for(map<int,pair<string,unsigned char> >::iterator it=id2Name_.begin();it!=id2Name_.end();++it){
									//			MCE_INFO("reload id:"<<it->first<<" name:"<<(it->second).first);
									//		}
		}catch(exception& ex){
					MCE_WARN(ex.what());
		}catch(...){
					MCE_WARN("reload error");
		}
  }
	IceUtil::RWRecMutex::WLock lock(mutex_);
	id2Name_.swap(id2NameNew_);
}
/*
void MiniGroupInputFactoryI::reloadIds(const vector<int>& ids){
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
}*/
//map<int,string> PageInputManagerI::getPageName(){
//	return id2NameNew_;
//}
MyUtil::IntSeq MiniGroupInputManagerI::getPageInput(Ice::Int hostID,const string& condition,Ice::Int limit,const Ice::Current& crt) {
	MCE_INFO("getMiniGroup hostID= "<<hostID<<" condition="<<condition<<" limit="<<limit);
	//MCE_INFO(crt.adapter->getName()<<" "<<crt.facet<<" "<<crt.operation<<" "<<crt.requestId);
	if(condition.size()>15||condition.size()==0)
		return vector<int>();
  else  {
    MiniGroupInputDataIPtr  server = ServiceI::instance().findObject<MiniGroupInputDataIPtr>(MINIGROUPINPUT_DATA, (long)hostID);
    if(server!=NULL){
      time_t old = server->getTime();
      int duration = time(NULL) - old;
      MCE_INFO("duration is:"<<duration);
      if(duration >= TIMEOUT_CACHE)  {
        ServiceI::instance().removeObject(MINIGROUPINPUT_DATA, (long)hostID);
      }
    }

		return ServiceI::instance().locateObject<MiniGroupInputDataIPtr>(MINIGROUPINPUT_DATA, (long)hostID)->search(condition,limit);
	}
};

// ============== MiniGroupInputDataI ======================================
void MiniGroupInputDataI::splitWord(const string& value, vector<string>& words)
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
void MiniGroupInputDataI::add(std::map<int,std::string>& result)
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
				vector<string> pinyins=MiniGroupInputManagerI::instance().dict_[words[i]];
				for(vector<string>::iterator it_vec=pinyins.begin();it_vec!=pinyins.end();++it_vec) {
					postingsPY[(*it_vec)].push_back(id);
				}
			}			
		}
	}
	old_ = time(NULL);

}
	bool MiniGroupInputDataI::isPinYin(const std::string& str, const std::vector<std::string> words) {
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


vector<int> MiniGroupInputDataI::getPrefix(string query) {
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

void MiniGroupInputDataI::intersect_vec(std::vector<int>& A,const std::vector<int>& B)
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

void MiniGroupInputDataI::union_vec(std::vector<int>&A, const std::vector<int>& B) {
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
bool MiniGroupInputDataI::startsWith(const std::string& base, const std::string& pre) {
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


MyUtil::IntSeq MiniGroupInputDataI::search(const string& query,Ice::Int limit)
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
	}
  if(limit<0 || limit > value.size())
    searchHZ(words,value);
  return select(value,limit);
}

vector<int> MiniGroupInputDataI::select(const set<int>& mySet,unsigned int limit) {
	size_t count = 0;
	vector<int> res;
	for(set<int>::const_iterator it=mySet.begin();it!=mySet.end() && count<limit;++it,++count){
		res.push_back(*it);	
		MCE_INFO("Find Res:"<<res[count]<<" "<<friends[res[count]]);
	}
	MCE_INFO("get res number : "<<res.size());
	return res;
}

bool MiniGroupInputDataI::reChoose(const int& doc,const vector<std::string>& strs,const int& len) {
	int inc=0;
	vector<string> words;
	splitWord(friends[doc], words);
	for(unsigned int i=0;i<words.size()&&inc<len;++i) {
		if(words[i].size()==3) {
			vector<string>& pinyins=MiniGroupInputManagerI::instance().dict_[words[i]];
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

bool MiniGroupInputDataI::reChooseHZ(int doc,const vector<string>& str){
  string name = friends[doc];
  size_t pos = 0;
  for(size_t i=0;i<str.size();++i){
    pos = name.find(str[i],pos);
    pos += str[i].size();
    if(pos == string::npos){
      break;
    }
  }
  if(pos!=string::npos){
    MCE_INFO("find hz doc "<<doc<<" "<<name);
  }
  return pos!=string::npos;
}

void MiniGroupInputDataI::splitPY(const string& pinyin,unsigned int start,int num,vector<string> subStr,set<int>& value) {
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

void MiniGroupInputDataI::searchHZ(const vector<string>& words,set<int>& mySet) {
	map<string,vector<int> >::iterator it = postingsHZ.find(words[0]);
	vector<int> results;
	if(it!=postingsHZ.end()){
		results = it->second;	
	}else{
		return;
	}
	for(size_t i=1;i<words.size() && !results.empty();++i)  {
		it = postingsHZ.find(words[i]);
		if(it!=postingsHZ.end()){
			intersect_vec(results,it->second);
		}
	}
	MCE_INFO("get hz result "<<results.size());
	for(size_t i=0;i<results.size();++i){
		if(reChooseHZ(results[i],words)){
			mySet.insert(results[i]);
		}
	}
}
