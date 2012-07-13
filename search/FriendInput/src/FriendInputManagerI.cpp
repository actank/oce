/*
 * =====================================================================================
 *
 *       Filename:  FriendInputManagerI.cpp
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
#include "UserCacheAdapter.h"
#include "BuddyByIdReplicaAdapter.h"
#include "FriendInputManagerI.h"
#include <cmath>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <algorithm>
#include "FriendOfFriendIndexerAdapter.h" 
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::services::FriendInput;
using namespace xce::buddy::adapter;
using namespace xce::usercache;
using namespace com::renren::search2::friendoffriend;
using namespace xce::serverstate;

// =============== MyUtil::initialize ====================================
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(FRIENDINPUT_DATA,"FI",new FriendInputFactoryI);
	service.getAdapter()->add(&FriendInputManagerI::instance(), service.createIdentity("Manager",""));
//    ServerStateSubscriber::instance().initialize("ControllerFriendInput",
//            &FriendInputManagerI::instance(), 10, 5);

}
// =============== FriendInputFactoryI ===================================
Ice::ObjectPtr FriendInputFactoryI::create(Ice::Int id) {
	ostringstream oss;
  oss<<"CreateIndex : "<< id ;
	TimeCost tc = TimeCost::create(oss.str(),1);
	MCE_INFO("create for:" << id );
	FriendInputDataIPtr obj=new FriendInputDataI(id);
	std::map<int, std::string> mm;
  //获取id对应的好友的名字, 存入mm中
	generateNameMap(id,mm);
	obj->add(mm);
	return obj;
}


/// @brief FriendInputFactoryI::generateNameMap 将uid对应的好友的名字放到friends中
/// 
/// @param uid 用户ID
/// @param friends 好友ID->好友姓名的映射
///
void FriendInputFactoryI::generateNameMap(Ice::Int uid, map<int, string>& friends) { 
	ostringstream oss;
	oss<<"GenerateNameMap : "<< uid ;
	TimeCost tc = TimeCost::create(oss.str(),1);
	MCE_INFO("generateNameMap for user:"<<uid);
	try{
			vector<int> friendsIds=BuddyByIdCacheAdapter::instance().getFriendListAsc(uid,6000);
			friendsIds.push_back(uid);
			//MCE_INFO("TimeCost getBuddyList: "<<friendsIds.size());
//			if(friendsIds.size() <= maxSize){
				map<int, UserCachePtr> users=UserCacheAdapter::instance().GetUserCache(friendsIds);
				for(map<int, UserCachePtr>::iterator it=users.begin();it!=users.end();++it) {
          //@modified by zhigang.wu@opi-corp.com: 加入归一化的代码, 将大写字母统一归一化为小写
          //解决如下bug: 1.当好友名字为武志刚，查询时输入WU没有结果 2. 当好友名字为"武志刚Reborn", 输入R能出来，输入r出不来
          string name = it->second->name();
          transform(name.begin(), name.end(), name.begin(), ::tolower);
					friends[it->first] = name;	
				}
/*
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
*/
	}catch(...){
		MCE_WARN("Buddy||User throw Exception");
			}
	MCE_INFO("Add uid:"<<uid<<" friends :"<<friends.size());
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

// =============== FriendInputManagerI ===================================
FriendInputManagerI::FriendInputManagerI()
{
	Statement sql;
	sql << "SELECT hanzi, pinyin FROM pinyin";
	PinYinResultHandler handler(newSet, dict_);
	QueryRunner(DB_FRIENDINPUT_PINYIN, CDbRServer).query(sql, handler);
}
MyUtil::IntSeq FriendInputManagerI::getFriendInput(Ice::Int hostID,const string& condition,Ice::Int limit,const Ice::Current& crt) {
	MCE_INFO("getFriendInput hostID= "<<hostID<<" condition="<<condition<<" limit="<<limit);
	//MCE_INFO(crt.adapter->getName()<<" "<<crt.facet<<" "<<crt.operation<<" "<<crt.requestId);
	if(condition.size()>15||condition.size()==0)
		return vector<int>();
	else  {
		FriendInputDataIPtr  server = ServiceI::instance().findObject<FriendInputDataIPtr>(FRIENDINPUT_DATA, (long)hostID);
		if(server!=NULL){
			time_t old = server->getTime();
			int duration = time(NULL) - old;
			MCE_INFO("duration is:"<<duration);
			if(duration >= TIMEOUT_CACHE)  {
				ServiceI::instance().removeObject(FRIENDINPUT_DATA, (long)hostID);
			}
		}
		return ServiceI::instance().locateObject<FriendInputDataIPtr>(FRIENDINPUT_DATA, (long)hostID)->search(condition,limit);
	}
};
void FriendInputManagerI::reLoadFriend(Ice::Int hostID,const Ice::Current& crt)
{
	MCE_INFO("reLoadFriend hostID="<<hostID);
	FriendInputDataIPtr obj = ServiceI::instance().findObject<FriendInputDataIPtr>(FRIENDINPUT_DATA, (long)hostID);
	if(!obj){
		 ServiceI::instance().addObject(obj, FRIENDINPUT_DATA, (long)hostID);
	}
};

vector<string> FriendInputManagerI::dict(const string& word){
	map<string,vector<string> >::const_iterator it = dict_.find(word);
	if(it != dict_.end()){
		return it->second;
	}
	return vector<string>();
}
// ============== FriendInputDataI ======================================
void FriendInputDataI::splitWord(const string& value, vector<string>& words)
{
	for(vector<string>::size_type i=0;i<value.size();++i) {
    //是否为汉字
		if((value[i]&0x80)!=0) {
			words.push_back(value.substr(i,3));
			i+=2;//累计已加3,即一个UTF8中文编码字符长度
		}else {
			if(value[i]!=' ')
				words.push_back(string(1,value[i]));		
		}		
	}
}
/// @brief FriendInputDataI::add 建立两个倒排索引(postingsHZ和postingsPY) 一个是纯拼音的，一个是非纯拼音的, 
/// 
/// @param result (in) 好友ID->好友名字
void FriendInputDataI::add(std::map<int,std::string>& result)
{
	MCE_INFO("addNameMap for "<<id_);
	ostringstream  oss;
  oss<<"AddNameMap : "<< id_;
	TimeCost tc = TimeCost::create(oss.str(),1);
  friends.swap(result);
	string name;
	unsigned int id;
	for(map<int,string>::iterator it=friends.begin();it!=friends.end();++it) {
		name=it->second;
		id=it->first;
		vector<string> words;
		splitWord(name,words);
		for(unsigned int i=0;i<words.size();++i) {
			if(words[i].size()==3) {
				vector<string> pinyins=FriendInputManagerI::instance().dict(words[i]);
				for(vector<string>::iterator it_vec=pinyins.begin();it_vec!=pinyins.end();++it_vec) {
					postingsPY[(*it_vec)].push_back(id);
				}
			}			
			postingsHZ[words[i]].push_back(id);
		}
	}
	old_ = time(NULL);
}
/*
struct cmp{
	bool operator()(pair<int,int> a, pair<int,int> b){
		if(a.second == b.second){
			return a.first < b.second;
		}else{
			return a.second < b.second;
		}
	};
};*/
MyUtil::IntSeq FriendInputDataI::search(const string& query,Ice::Int limit)
{
 	string transformed_query(query);
  transform(transformed_query.begin(), transformed_query.end(), transformed_query.begin(), ::tolower);    
	ostringstream oss;
	oss<<"Search : "<< id_<<","<<query;
	TimeCost tc = TimeCost::create(oss.str(),1);
	MCE_DEBUG("begin search " << id_ <<"."<< transformed_query);
//	set<int> value;
	map<int,int> value;
	vector<string> words;
	splitWord(transformed_query, words);
  //全都是小写字母
	if(isPinYin(words)) {
		vector<string> subStr(transformed_query.size());
		splitPY(transformed_query,0,0,subStr, value);
	}
	if(limit < 0 || value.size() < limit)
		searchHZ(words,value);
	return select(value,limit);
}

vector<int> FriendInputDataI::select(const map<int,int>& value,unsigned int limit) {
	vector<pair<int,int> > res;
	res.reserve(value.size());
	for(map<int,int>::const_iterator it=value.begin();it!=value.end();++it){
			res.push_back(make_pair<int>(it->second,it->first));
	}
	MCE_INFO("getFriends number : "<<value.size());
	
	sort(res.begin(),res.end());

	size_t count = 0;
	vector<int> result;
	for(vector<pair<int,int> >::iterator it=res.begin();it!=res.end() && count<limit;++it,++count){
		result.push_back(it->second);	
		MCE_INFO("select "<<it->second<<" "<<it->first);
	}
	
	return result;
}

/// @brief FriendInputDataI::reChoose 验证ID=doc的人名切分后是否与strs一致
/// 
/// @param doc 
/// @param strs 
/// @param len 
/// 
/// @return 
bool FriendInputDataI::reChoose(int doc,const vector<std::string>& strs,const int& len,int& first) {
	int inc=0;
	first = len;
	vector<string> words;
	splitWord(friends[doc], words);
	for(unsigned int i=0;i<words.size()&&inc<len;++i) {
		if(words[i].size()==3) {
			vector<string> pinyins=FriendInputManagerI::instance().dict(words[i]);
			if(!pinyins.empty()) {
				for(vector<string>::iterator it=pinyins.begin();it!=pinyins.end();++it) {
					if(startsWith(*(it),strs[inc])) {
						if(inc == 0){
							first = i;	
						}
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
bool FriendInputDataI::reChooseHZ(int doc,const vector<string>& str,int& first){
	string name = friends[doc];
	size_t pos = 0;
	first =  str.size();
	for(size_t i=0;i<str.size();++i){
		pos = name.find(str[i],pos);
		if(pos == string::npos){
			break;
		}else if(i == 0){
			first = pos;
		}
		pos += str[i].size();
	}
	if(pos!=string::npos){
		MCE_INFO("find hz doc "<<doc<<" "<<name);
	}
	return pos!=string::npos;
}
/// @brief FriendInputDataI::splitPY 将纯小写字母串pinyin切分成拼音串，具体推测并验证得到相应的ID
/// 
/// @param pinyin(in&out) 待切分的纯小写字母串
/// @param start(in) 字母串的起始切分位置
/// @param num(in) subStr的size
/// @param subStr(in) 由字母串切分而成的拼音
/// @param value(in&out) 存放符合条件的所有ID
void FriendInputDataI::splitPY(const string& pinyin,unsigned int start,int num,vector<string> subStr,map<int,int>& value) {
	if(start>=pinyin.size()) {
		vector<int> temp=getPrefix(subStr[0]);
		for(int i=1;i<num;i++) {
			intersect_vec(temp,getPrefix(subStr[i]));
			if(temp.empty())
				return;
		}
		for(vector<int>::iterator it=temp.begin();it!=temp.end();++it) {
			int pos;
			int id = *it;
			if(reChoose(id,subStr,num,pos)) {
				++pos;
				if(value[id] == 0 || value[id] > pos)
					value[id] = pos;
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
		
void FriendInputDataI::searchHZ(const vector<string>& words,map<int,int>& value) {
  if (words.empty()) {
    return;
  }
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
		int pos;
		int id = results[i];
		if(reChooseHZ(id,words,pos)){
        ++pos;
        if(value[id] == 0 || value[id] > pos)
          value[id] = pos;
    }
	}
}
