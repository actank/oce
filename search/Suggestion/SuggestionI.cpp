#include <cmath>
#include "SuggestionI.h"

using com::xiaonei::services::Suggestion::SuggestionManagerI;
using com::xiaonei::services::Suggestion::IndexType;
using com::xiaonei::services::Suggestion::Int2StrMap;
using com::xiaonei::services::Suggestion::TypeHolder;
using com::xiaonei::services::Suggestion::PinYinResultHandler;
using com::xiaonei::services::Suggestion::LoadAppNameHandler;
using com::xiaonei::xce::ResultHandler;
using com::xiaonei::xce::Statement;
using com::xiaonei::xce::CDbRServer;
using com::xiaonei::xce::QueryRunner;
//using namespace com::xiaonei::xce;
//using namespace com::xiaonei::services::Suggestion;
void MyUtil::initialize() {

	ServiceI& service = ServiceI::instance();
	SuggestionManagerI::instance().initialize();
	SuggestionManagerI::instance().buildIndex();
	service.getAdapter()->add(&SuggestionManagerI::instance(), service.createIdentity("Manager", ""));
}

void TypeHolder::add(const Int2StrMap& id2Name, bool rebuild=true){
	if(rebuild){
		id2Name_.clear();
		hz2Id_.clear();
		py2Id_.clear();
	} 
	MCE_INFO("TypeHolder add "<<id2Name.size());
	for(map<int,string>::const_iterator it=id2Name.begin();it!=id2Name.end();++it){
		insert(it->first, it->second);	
	}
}

void TypeHolder::insert(int id, const string& name){
	if(id2Name_.find(id)==id2Name_.end()){
		vector<string> words;
		splitWord(name,words);
		id2Name_.insert(make_pair(id,name));
		insertHZ(id,words);
		insertPY(id,words);
	}
}

void TypeHolder::insertHZ(int id, const vector<string>& words){
	for(size_t i=0;i<words.size();++i){
		hz2Id_[words[i]].push_back(id);
	}
}

map<int,string> TypeHolder::searchHZ(const string& hanzi, int limit){
	MCE_INFO("[TypeHolder::searchHZ] " << hanzi);
	vector<string> words;
	splitWord(hanzi,words);
	vector<int> res;
	{
		map<string,vector<int> >::iterator it=hz2Id_.find(words[0]);
		if(it!=hz2Id_.end())
			res=it->second;
		for(size_t i=1;i<words.size()&&res.size()>0;++i){
			map<string,vector<int> >::iterator it = hz2Id_.find(words[i]);
			if(it!=hz2Id_.end())
				intersect(res, it->second);
		}
	}
	map<int,string> result;
	for(vector<int>::iterator it=res.begin();it!=res.end()&&limit-->0;++it)
		result[*it]=id2Name_[*it];
	return result;
}

void TypeHolder::insertPY(int id, const vector<string>& words){
	for(size_t i=0;i<words.size();++i){
		vector<string> pinyins = SuggestionManagerI::instance().getPinYin(words[i]);
		if(pinyins.size()){
			for(vector<string>::iterator it=pinyins.begin();it!=pinyins.end();++it){
				py2Id_[*it].push_back(id);
			}
		}
	}
}

bool TypeHolder::startsWith(const std::string& base, const std::string& pre) {
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

Int2StrMap TypeHolder::searchPY(const string& pinyin, int limit){
	MCE_INFO("[TypeHolder::searchPY ]"<<pinyin);
	set<int> res;
	vector<string> subs;
	subs.resize(pinyin.size());
	map<string,vector<int> >::iterator it;
	int dist;
	for(dist=min((int)pinyin.size(),5);dist>0;dist--){
		it=py2Id_.lower_bound(pinyin.substr(pinyin.size()-dist));
		if(it==py2Id_.end())
			continue;
		MCE_INFO(it->first<<" "<<pinyin.substr(pinyin.size()-dist));
		if(startsWith(it->first, pinyin.substr(pinyin.size()-dist))){
			splitPY(pinyin.substr(0,pinyin.size()-dist),0,0,subs, res);
			break;
		}
	}
	MCE_INFO("split into "<<pinyin.substr(0,pinyin.size()-dist)<<" "<<pinyin.substr(pinyin.size()-dist));
	MCE_INFO("rest size is "<<res.size());
	Int2StrMap result;
	if(dist>0){
		set<int> tmp;
		while(it!=py2Id_.end()){
			if(startsWith(it->first,pinyin.substr(pinyin.size()-dist)))
				tmp.insert(it->second.begin(),it->second.end());
			else
				break;
			++it;
		}
		vector<int> vec(min(tmp.size(),res.size()));
		if(dist==int(pinyin.size())){
			for(set<int>::iterator it=tmp.begin();it!=tmp.end();++it){
				result[*it]=id2Name_[*it];
			}
		}else{
			set_intersection(res.begin(),res.end(),tmp.begin(),tmp.end(),vec.begin());
			for(vector<int>::iterator it=vec.begin();it!=vec.end()&&limit-->0;++it)
				result[*it]=id2Name_[*it];
		}
	}
	return result;
}

void TypeHolder::splitPY(const string& pinyin,unsigned int start,int num,vector<string> subStr,set<int>& res) {
	if(start>=pinyin.size()) {
		vector<int> temp=py2Id_[subStr[0]];
		for(int i=1;i<num;++i) {
			intersect(temp,py2Id_[subStr[i]]);
			if(temp.empty())
				return;
		}
		res.insert(temp.begin(),temp.end());		
	}else {
		for(unsigned int end=start+1;end<=pinyin.size();++end) {
			string temp=pinyin.substr(start,end-start);
			if(py2Id_.find(temp)!=py2Id_.end()) {
				subStr[num]=temp;
				splitPY(pinyin,end,num+1,subStr,res);			
			}	
		}	
	}
}
void TypeHolder::intersect(vector<int>& A,const vector<int>& B)
{
	vector<int>::iterator itA=A.begin();
	vector<int>::const_iterator itB=B.begin();
	vector<int> res;
	while(itA!=A.end() && itB!=B.end()) {
		if((*itA)==(*itB)) {
			res.push_back(*itA);
			++itA;
			++itB;
		}else if((*itA)>(*itB)) {
			++itB;				
		}else {
			++itA;	
		}	
	}
	A.swap(res);
}

void TypeHolder::splitWord(const string& value, vector<string>& words)
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

void SuggestionManagerI::initialize(){
	Statement sql;
	sql << "SELECT hanzi, pinyin FROM pinyin";
	PinYinResultHandler handler(dict_);
	QueryRunner(DB_FRIENDINPUT_PINYIN, CDbRServer).query(sql, handler);
	swapIndex=false;
	holders_.insert(make_pair(App,new TypeHolder));
}
vector<string> SuggestionManagerI::getPinYin(const string& pinyin){
	map<string,vector<string> >::iterator it = dict_.find(pinyin);
	if(it==dict_.end()){
		return vector<string>();
	}else{
		return it->second;
	}
}
void SuggestionManagerI::buildIndex(IndexType type, const Ice::Current& current)
{
	Int2StrMap id2Name;
	switch(type)
	{
		case App:
			loadAppName(id2Name);
			break;
		default:
			break;
	}
	MCE_INFO("buildIndex add "<<type<<" size is :"<<id2Name.size() <<" "<<dict_.size());

	TypeHolderPtr thp = new TypeHolder;
	thp->add(id2Name);
	swapIndex=true;
	//    atomic_inc(&index_counter);
	holders_[type]=thp;
	swapIndex=false;
	//    atomic_dec(&index_counter);
}


Int2StrMap SuggestionManagerI::search(IndexType type, const ::std::string& condition, ::Ice::Int limit, const Ice::Current& current)
{
	MCE_INFO("[SuggestionManagerI::search] "<<condition<<" "<<limit);

	if(condition.size()==0 || swapIndex
			|| holders_.find(type)==holders_.end())
		return map<int,string>();
	size_t i=0;
	for(;i<condition.size();++i){
		if(condition[i]<'a'||condition[i]>'z')
			break;
	}
	if(i!=condition.size())
		return holders_[type]->searchHZ(condition, limit);
	else
		return holders_[type]->searchPY(condition, limit);
}
void SuggestionManagerI::loadAppName(Int2StrMap& id2Name){
	Statement sql;
	sql << "SELECT app_id,app_name FROM app_productlist";
	QueryRunner("app",CDbRServer).query(sql, LoadAppNameHandler(id2Name));
}
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
bool LoadAppNameHandler::handle(mysqlpp::Row& res) const {
#ifndef NEWARCH
	int id = int(res["app_id"]);
	string name = res["app_name"].get_string();
#else
	int id = int(res["app_id"]);
	string name = res["app_name"].c_str();
#endif
	id2Name_[id]=name;
	return true;
}
