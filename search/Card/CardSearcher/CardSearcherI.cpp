#include "CardSearcherI.h"
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
  service.registerObjectCache(CARD_SEARCHER,"CS",new CardSearcherFactoryI);
  service.getAdapter()->add(&CardSearcherI::instance(),service.createIdentity("Manager",""));
  ServerStateSubscriber::instance().initialize("ControllerCardSearcher",&CardSearcherI::instance(),1,5);
}

void CardSearcherI::loadPY() {
  MCE_INFO("CardSearcherI::loadPY");
  Statement sql;
  sql << "SELECT hanzi, pinyin FROM pinyin";
  PinYinResultHandler handler(newSet, dict_);
  QueryRunner(DB_FRIENDINPUT_PINYIN, CDbRServer).query(sql, handler);
}

CardSearcherI::CardSearcherI(){
  try{
    MCE_INFO("Front [CardSearcherI::CardSearcherI()] ");
    loadPY();
    MCE_INFO("After [CardSearcherI::CardSearcherI()] ");
  }catch(std::exception& ex){
    cout<<ex.what()<<endl;
  }
}
Ice::ObjectPtr CardSearcherFactoryI::create(::Ice::Long userId) {
	MCE_INFO("[CardSearcherFactoryI::create] "<<userId);
  CardSearcherDataPtr data = new CardSearcherData();

  try{
		  //取出用户所有好友的id（按姓名排序）
		  IntSeq allFriends = BuddyByNameCacheAdapter::instance().getFriendList(userId,-1);

		  MCE_INFO("get friends size:"<<allFriends.size());
      //取出用户好友的信息
      map<int, UserCachePtr> users;
		  CardSearcherI::instance().getUserCache(allFriends, users);
		  MCE_INFO("get users size: "<<users.size());
      //取出用户好友的名片信息 
      CardInfoSeq infos = CardInfoCacheAdapter::instance().get(allFriends);

		  //获取已经交换名片的好友ID
		  IntSeq exchangeFriends = CardRelationCacheAdapter::instance().get(userId,0,-1,ACCEPT);
      //获取用户发送交换请求的好友ID
		  IntSeq addFriends = CardRelationCacheAdapter::instance().get(userId,0,-1,ADD);
      //获取用户收到的请求的好友ID 
		  IntSeq receiveFriends = CardRelationCacheAdapter::instance().get(userId,0,-1,RECEIVE);

		  IntSeq A;//对应ACCEPT
		  IntSeq B;//对应RECEIVE 
		  IntSeq C;//对应ADD
		  IntSeq D;//对应REMOVE

		  for(vector<int>::iterator it=allFriends.begin(); it!=allFriends.end(); ++it){
				  if(binary_search(exchangeFriends.begin(),exchangeFriends.end(), *it)){
						  A.push_back(*it);
				  }else if(binary_search(receiveFriends.begin(), receiveFriends.end(), *it)){
						  B.push_back(*it);
				  }else if(binary_search(addFriends.begin(),addFriends.end(), *it)){
						  C.push_back(*it);
				  }else
						  D.push_back(*it);
		  }
      /// @brief addIndex 创建索引 
		  addIndex(data, A, users, infos, ACCEPT);	
		  addIndex(data, B, users, infos, RECEIVE);
		  addIndex(data, C, users, infos, ADD);
		  addIndex(data, D, users, infos, REMOVE);
		  MCE_INFO("create done");
  }catch(...){
	return NULL;
  }
  return data;
}

void CardSearcherFactoryI::addIndex(CardSearcherDataPtr& data,const vector<int>& ids,const map<int, UserCachePtr>& users,const CardInfoSeq& infos, int type) {
  //得到索引字段与值的map
  for(vector<int>::const_iterator it=ids.begin(); it!=ids.end(); ++it){
    Str2StrMap props;
    map<int, UserCachePtr>::const_iterator itA = users.find(*it);
    if(itA!=users.end()){
      string name = itA->second->name();
      transform(name.begin(), name.end(), name.begin(), ::tolower);
      props.insert(make_pair<string,string>(NAME, name));
    }	
    CardInfoPtr p = new CardInfo;
    p->userId = *it;
    vector<CardInfoPtr>::const_iterator itB=lower_bound(infos.begin(), infos.end(), p, lessCardInfo);
    if(itB == infos.end()){
      continue;
    }
    if((*itB)->userId == *it){
      props.insert(make_pair<string,string>(QQ,(*itB)->qq));
      props.insert(make_pair<string,string>(MOBILE,(*itB)->mobile));
      props.insert(make_pair<string,string>(MSN,(*itB)->msn));
      props.insert(make_pair<string,string>(EMAIL,(*itB)->email));
      //    props.insert(ADDRESS,(*itB)->address);
    }
    data->addBaseIndex(*it, props, type);
    if(itA!=users.end()){
      string name = itA->second->name();
      data->addPYIndex(*it, name, type);
    }
  }	
}

void CardSearcherData::addPYIndex(int userId, const std::string name, int type) {
//  MCE_INFO("CardSearcherData::addPYIndex: userId " << userId << "; name " << name << "; type " << type);
  vector<string> words;
  splitWord(name,words);

  for(unsigned int i=0;i<words.size();++i) {
    if(words[i].size()==3) {
//      MCE_INFO("words[i].size() " << words[i].size() << "; words[i] " << words[i]);
      vector<string> pinyins=CardSearcherI::instance().dict_[words[i]];
      MCE_INFO("words[i]: " << words[i] << "; pinyins.size(): " << pinyins.size());
//      MCE_INFO("dict_[ " << words[i] << "]=" << pinyins);
      for(vector<string>::iterator it_vec=pinyins.begin();it_vec!=pinyins.end();++it_vec) {
        postingsPY[(*it_vec)].push_back(userId);
        MCE_INFO("pinyin: " << *it_vec << "; userId: " << userId);
      }
    }
  }
  //id2typePY[userId] = type;
}

void CardSearcherData::addBaseIndex(int userId, const Str2StrMap& props,int type){
  //建立倒排索引
  for(map<string,string>::const_iterator it=props.begin();it!=props.end();++it){
    dict[it->second].push_back(userId);
    MCE_INFO("add index key:"<<it->second<<" person:"<<userId);
  }
  //存储ID对应的名片交换状态
  id2type[userId] = type;
}
bool CardSearcherData::startsWith(const std::string& base, const std::string& pre) {
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
bool CardSearcherData::shouldOut(){
	if(time(NULL) - indexTime > OUTTIME){
		return true;
	}else{
		return false;
	}	
}

Int2IntMap CardSearcherData::search(const string& query,int limit){
  MCE_INFO("CardSearcherData::search: query " << query);
  string transformed_query(query);
  transform(transformed_query.begin(), transformed_query.end(), transformed_query.begin(), ::tolower);
  vector<string> words;
  Int2IntMap result, resultPY;
  splitWord(transformed_query, words);
  
  if(isPinYin(transformed_query, words)) {
    searchPY(resultPY, transformed_query, limit);
  }

  map<string,vector<int> >::iterator it = dict.lower_bound(query);
  while(it!=dict.end()) {
    MCE_INFO("key:"<<it->first<< " query:"<<query);
    if(startsWith(it->first,query)){
      for(vector<int>::iterator itt=it->second.begin();itt!=it->second.end();++itt){
        result[*itt] = id2type[*itt];
        if((int)result.size() >= limit && limit != -1)
          //return result;
          break;
      }
    }else
      break;
    ++it;
  }

  for (map<int,int>::iterator itPY = resultPY.begin(); itPY != resultPY.end(); ++itPY) {
    MCE_INFO("key:"<<itPY->first<< " query:"<<query);
    result[itPY->first] = itPY->second;
  }

  return result;

}

void CardSearcherData::searchPY(Int2IntMap& resultPY, const string& transformed_query,int limit){
  MCE_INFO("CardSearcherData::searchPY: query " << transformed_query);
  set<int> value;
  vector<int> res;
  vector<string> subStr(transformed_query.size());
  splitPY(transformed_query,0,0,subStr, value);
  if(!value.empty())  {
    MCE_INFO("CardSearcherData::searchPY: value.size() " << value.size());
    select(res,value,limit);
  }
  for (vector<int>::iterator itR = res.begin(); itR != res.end(); ++itR) {
    resultPY[*itR] = id2type[*itR];
  }
}

void CardSearcherData::splitWord(const string& value, vector<string>& words)
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

void CardSearcherData::splitPY(const string& pinyin,unsigned int start,int num,vector<string> subStr,set<int>& value) {
  MCE_INFO("CardSearcherData::splitPY: start " << start << "; num " << start << "; subStr.size()" << subStr.size() << "; value.size() " << value.size());
  string result = "";
  if(start>=pinyin.size()) {
      set<int> temp=getPrefix(subStr[0]);
      result = subStr[0];
    for(int i=1;i<num;i++) {
      intersect_vec(temp,getPrefix(subStr[i]));
      result =result + " " + subStr[i];
      if(temp.empty())
        return;
    }
    MCE_INFO("RESULT " <<  result);
    temp.swap(value);
    return;
  }else {
    for(unsigned int end=start+1;end<=pinyin.size();++end) {
      string temp=pinyin.substr(start,end-start);
//      MCE_INFO("temp: " << temp <<";  end: " << end);
      map<string,vector<int> >::iterator it=postingsPY.lower_bound(temp);

//      MCE_INFO("postingsPY.size() " << postingsPY.size());
      if(it!=postingsPY.end()&&startsWith(it->first,temp)) {
//        MCE_INFO("num " << num <<"; temp " << temp);
        subStr[num]=temp;
        splitPY(pinyin,end,num+1,subStr,value);
      }
    }
  }
//  MCE_INFO("subStr[0]" << subStr[0]);
//  MCE_INFO("subStr[1]" << subStr[1]);
//  MCE_INFO("subStr[2]" << subStr[2]);
//  MCE_INFO("subStr[3]" << subStr[3]);
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

void CardSearcherData::select(vector<int>& res, const set<int>& mySet,unsigned int limit) {
  size_t count = 0;
  for(set<int>::const_iterator it=mySet.begin();it!=mySet.end() && count<limit;++it,++count){
    res.push_back(*it);
  }
  MCE_INFO("getFriends number : "<<res.size());
}
  
  Int2IntMap
xce::cardsearch::CardSearcherI::search(::Ice::Int userId,
    const ::std::string& query,
    ::Ice::Int limit,
    const Ice::Current& current)
{
  Int2IntMap res;
  CardSearcherDataPtr p = ServiceI::instance().locateObject<CardSearcherDataPtr>(CARD_SEARCHER, userId);
  if(p->shouldOut()){
    ServiceI::instance().removeObject(CARD_SEARCHER, userId);	
    p = ServiceI::instance().locateObject<CardSearcherDataPtr>(CARD_SEARCHER, userId);	
  }
  res = p->search(query,limit);
  MCE_INFO("[CardSearcherI::search] "<<userId<<" query:"<<query<<" result:"<<res.size());
  return res;
}

void CardSearcherI::getUserCache(const vector<int>&friends ,map<int, UserCachePtr>& users){
		if(friends.size() <= MAXBATCH){
				users=UserCacheAdapter::instance().GetUserCache(friends);
		}else{
				//若好友数较多，则分多次取ID对应的名字
				vector<int>::const_iterator itS=friends.begin();
				vector<int>::const_iterator itE=itS+MAXBATCH;
				while(itS!=friends.end()){
						vector<int> part_ids(itE-itS);
						copy(itS,itE,part_ids.begin());
						itS = itE;
						itE += MAXBATCH;
						if(itE > friends.end())
								itE=friends.end();
						map<int, UserCachePtr> part_users = UserCacheAdapter::instance().GetUserCache(part_ids);
						users.insert(part_users.begin(),part_users.end());
				}
		}
}
PartInfoPtr CardSearcherI::getPartOfCardInfo(int userId,const PartOfCardInfoSeq& infos, const string& name){
	PartInfoPtr p = new PartInfo;
	p->userId = userId;
	vector<PartOfCardInfoPtr>::const_iterator it=lower_bound(infos.begin(),infos.end(), p, lessPartOfCardInfo); 
	if(it!=infos.end() && (*it)->userId == userId){
		p->qq = (*it)->qq;
		p->mobile = (*it)-> mobile;
		p->privacy = (*it)->privacy;
		p->name = name;
		return p;
	}else{
		return NULL;
	}
}

xce::cardsearch::FriendCardListPtr CardSearcherI::getFriendList(Ice::Int userId, const Ice::Current& current){
	FriendCardListPtr p = new FriendCardList;
 	TimeCost tc = TimeCost::create("[getFriendList]",1);
  //获取用户所有与所有好友的名片交换关系
  AllRelationPtr relation = CardRelationCacheAdapter::instance().getAllRelation(userId);
	tc.step("[getRelationCache]");
  //去用户的好友列表
  IntList friends = BuddyByNameCacheAdapter::instance().getFriendList(userId, 2000);
	tc.step("[getBuddyByName]");
  //获取用户好友的信息
    map<int, UserCachePtr> users;

   	getUserCache(friends, users); 
	tc.step("[getUserCache]");
	//获取用户好友的名片信息
  PartOfCardInfoSeq infos = CardInfoCacheAdapter::instance().getPart(friends);
	tc.step("[getPartOfCacheInfo]");
	for(vector<int>::iterator it=friends.begin();it!=friends.end();++it){
		PartInfoPtr part = getPartOfCardInfo(*it, infos, users[*it]->name());
    if(binary_search(relation->acceptList.begin(),relation->acceptList.end(),*it)){
      //判断好友id是否在交换名片的列表中,在则把信息插入到交换信息列表中
			p->acceptList.push_back(part);
		}else if(binary_search(relation->receiveList.begin(),relation->receiveList.end(),*it)){
      //在收到交换请求列表中则插入到响相应列表
      p->receiveList.push_back(part);
		}else if(binary_search(relation->addList.begin(),relation->addList.end(),*it)){
      //发送交换请求列表
			p->addList.push_back(part);
		}else{
      //没有任何名片交换操作列表
			p->friendList.push_back(part);
		}
	}
	tc.step("[zuzhuangResult]");
	return p;
};

xce::cardcache::PartNewInfoSeq CardSearcherI::getFriendsList(Ice::Int userId, const Ice::Current& current) {
  PartNewInfoSeq results;
  TimeCost tc = TimeCost::create("[getFriendsList]",1);
  //获取用户所有与所有好友的名片交换关系
  AllRelationPtr relation = CardRelationCacheAdapter::instance().getAllRelation(userId);
  tc.step("[getRelationCache]");
  //去用户的好友列表
  IntList friends = BuddyByNameCacheAdapter::instance().getFriendList(userId, 2000);

  tc.step("[getBuddyByName]");
  //获取用户好友的信息
  map<int, UserCachePtr> users;

  getUserCache(friends, users); 
  tc.step("[getUserCache]");
  //获取用户好友的名片信息
  PartOfCardInfoSeq infos = CardInfoCacheAdapter::instance().getPart(friends);
  tc.step("[getPartOfCacheInfo]");
  for(vector<int>::iterator it=friends.begin();it!=friends.end();++it){
    PartNewInfoPtr p = new PartNewInfo;
    p->userId = *it;
    p->name = users[*it]->name();
    vector<PartOfCardInfoPtr>::iterator itP=lower_bound(infos.begin(),infos.end(), p, lessPartOfCardInfo);
    if(itP!=infos.end() && (*itP)->userId == *it){
      p->qq = (*itP)->qq;
      p->mobile = (*itP)-> mobile;
      p->privacy = (*itP)->privacy;
    }

//     MCE_INFO("Infos : "<<userId<<" "<<p->qq<<" "<<p->mobile<<" "<<p->privacy<<" "<<(itP!=infos.end()?(*itP)->userId:-1));
    if(binary_search(relation->acceptList.begin(),relation->acceptList.end(),*it)){
      //判断好友id是否在交换名片的列表中,在则把信息插入到交换信息列表中
      p->type = 2;
    }else if(binary_search(relation->receiveList.begin(),relation->receiveList.end(),*it)){
      //在收到交换请求列表中则插入到响相应列表
      p->type = 1;
    }else if(binary_search(relation->addList.begin(),relation->addList.end(),*it)){
      //发送交换请求列表
      p->type = 0;
    }else{
      //没有任何名片交换操作列表
      p->type = 3;
    }
    results.push_back(p);
  }
  tc.step("[zuzhuangResult]");
  return results;

}

MyUtil::IntList CardSearcherI::getUnExchangeList(Ice::Int userId, const Ice::Current& current) {
	MyUtil::IntList result;

	AllRelationPtr relation = CardRelationCacheAdapter::instance().getAllRelation(userId);
	IntList friends = BuddyByNameCacheAdapter::instance().getFriendList(userId, 2000);
	PartOfCardInfoSeq infos = CardInfoCacheAdapter::instance().getPart(friends);

	for(vector<int>::iterator it=friends.begin();it!=friends.end();++it){
		if(binary_search(relation->acceptList.begin(),relation->acceptList.end(),*it)){
		}else if(binary_search(relation->receiveList.begin(),relation->receiveList.end(),*it)){
		}else if(binary_search(relation->addList.begin(),relation->addList.end(),*it)){
		}else{
			result.push_back(*it);
		}
	}
	return result;
}

//================ PinYinResultHandler ===================================
bool PinYinResultHandler::handle(mysqlpp::Row& res) const {
  MCE_INFO("PinYinResultHandler::handle");
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

