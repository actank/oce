#include <time.h>
#include "CardRelationI.h"

using namespace std;
using namespace xce::cardcache;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace xce::usercount::adapter;

/// @brief initialize 初始化
void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.registerObjectCache(CARD_RELATION_DATA,"CRD",new CardRelationFactoryI);
  service.getAdapter()->add(&CardRelationCacheI::instance(),service.createIdentity("M",""));
  std::string controllerEndpoint = service.getCommunicator()->getProperties()->getPropertyWithDefault("CardRelationCache.Endpoint","ControllerCardRelationCache");
  std::string tabName = service.getCommunicator()->getProperties()->getPropertyWithDefault("CardRelationCache.TableName","personal_address_book");
  int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CardRelationCache.Mod",10);
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CardRelationCache.Interval",5);
  xce::serverstate::ServerStateSubscriber::instance().initialize(controllerEndpoint,&CardRelationCacheI::instance(),mod,interval);
  CardRelationCacheI::instance().initialize();
}

/// @brief CardRelationCacheI 构造函数
CardRelationCacheI::CardRelationCacheI() {
  ServiceI& service = ServiceI::instance();
  /// @brief 获取表名
  tableName = service.getCommunicator()->getProperties()->getPropertyWithDefault("CardRelationCache.TableName","personal_address_book");
  /// @brief 获取表散成的份数
  tableCount = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CardRelationCache.TableCount",100);
  factory = new CardRelationFactoryI;
}

void CardRelationCacheI::initialize() {
  MCE_INFO("[CardRelationCacheI::initialize] start");
  for (int i = 0; i < CLUSTER; i++) {
    DispatcherPtr dispatchThread = new Dispatcher(this,i);
    dispatchThread->start(65535).detach();
    _dispatchThreads.push_back(dispatchThread);
  }
  MCE_INFO("[CardRelationCacheI::initialize] done _dispatchThreads.size:" << _dispatchThreads.size());
}

Ice::ObjectPtr CardRelationFactoryI::create(long id) {
  string table = CardRelationCacheI::instance().getTableName(id);
  Statement sql;
  sql << "SELECT "<<FIELD_FID <<","<<FIELD_FLAG<< "," << FIELD_TIME << " FROM " << table << " WHERE "<<FIELD_UID<<" = " << id;
  CardRelationDataPtr obj = new CardRelationData();
  try{
    size_t qsize = QueryRunner(DESC_ADDRESSBOOK,CDbRServer,table).query(sql, CardRelationResultHandlerI(obj));
    if(qsize == 0)
      return NULL;
  }catch(mysqlpp::Exception& e){
    MCE_WARN("[CardRelationFactoryI::create] "<< e.what() );
    return NULL;
  }catch(std::exception& e){
    MCE_WARN("[CardRelationFactoryI::create] "<< e.what() );
    return NULL;
  }catch(...){
    MCE_WARN("[CardRelationFactoryI::create] Unknown ");
    return NULL;
  }
  return obj;
}
  string xce::cardcache::CardRelationCacheI::getTableName(int userId){
    if(tableCount == 1)	
      return tableName; 
    ostringstream oss;
    oss<<tableName<<"_"<<userId%tableCount;
    return oss.str();
  }
  ::MyUtil::IntSeq
xce::cardcache::CardRelationCacheI::get(::Ice::Int userId,
    ::Ice::Int begin,
    ::Ice::Int limit,
    ::Ice::Int requestFlag,
    const Ice::Current& current)
{
  MCE_INFO("[CardRelationCacheI::get] entrance userId:"<<userId<<" begin:"<<begin<<" limit:"<<limit<<" state:"<<requestFlag);
  CardRelationDataPtr host = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, userId);
  Ice::ObjectPtr hObj;
  if(!host){
    hObj = factory->create(userId);
    if(hObj){
      ServiceI::instance().addObject(hObj, CARD_RELATION_DATA, userId);
      host = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, userId);
    }
  }

  if(host!=0)
    return host->get(begin,limit,requestFlag);
  else
    return MyUtil::IntSeq();
}

AllRelationPtr xce::cardcache::CardRelationCacheI::getAllRelation(Ice::Int userId, const Ice::Current& current){
  MCE_INFO("[CardRelationCacheI::getAllRelation] entrance userId:" << userId);
  CardRelationDataPtr host = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, userId);
  Ice::ObjectPtr hObj;
  if(!host){
    hObj = factory->create(userId);
    if(hObj){
      ServiceI::instance().addObject(hObj, CARD_RELATION_DATA, userId);
      host = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, userId);
    }
  }

  AllRelationPtr p = new AllRelation;
  if(!host)
    return p; 
  p->acceptList = host->get(0,-1,ACCEPT); 
  p->receiveList= host->get(0,-1,RECEIVE); 
  p->addList = host->get(0,-1,ADD);
  return p;
};

  ::Ice::Int
xce::cardcache::CardRelationCacheI::getSize(::Ice::Int userId,
    ::Ice::Int requestFlag,
    const Ice::Current& current)
{
  MCE_INFO("[CardRelationCacheI::getSize] entrance userId:" << userId << " requestFlag:" << requestFlag);
  CardRelationDataPtr host = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, userId);
  Ice::ObjectPtr hObj;
  if(!host){
    hObj = factory->create(userId);
    if(hObj){
      ServiceI::instance().addObject(hObj, CARD_RELATION_DATA, userId);
      host = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, userId);
    }
  }

  if(!host){
    return 0;
  }else{
    return host->getSize(requestFlag);
  }
}

  Ice::Int
xce::cardcache::CardRelationCacheI::set(const ::xce::cardcache::RelationInfoPtr& relation,
    const Ice::Current& current)
{
  MCE_INFO("[CardRelationCacheI::set] entrance");
  return	adjust(relation->userId, relation->friendId, relation->requestFlag);
}
void xce::cardcache::CardRelationCacheI::setBatch(const ::xce::cardcache::RelationInfoSeq& relationInfos,
    const Ice::Current& current){
  TimeCost tc = TimeCost::create("[CardRelationCacheI::setBatch]",1);
  MCE_INFO("[CardRelationCacheI::setBatch] entrance relationInfos.size:"<<relationInfos.size());

  for(RelationInfoSeq::const_iterator it=relationInfos.begin();it!=relationInfos.end();++it){
    adjust((*it)->userId, (*it)->friendId, (*it)->requestFlag);
  }
  tc.step("done!");
}
void xce::cardcache::CardRelationCacheI::remove(int userId,int friendId, const Ice::Current& current){
  MCE_INFO("[CardRelationCacheI::remove] entrance userId:"<< userId<<" friendId:"<<friendId);
  adjust(userId,friendId,REMOVE);
}
int xce::cardcache::CardRelationCacheI::getRelation(int userId,int friendId, const Ice::Current& current){

  MCE_INFO("[CardRelationCacheI::getRelation] entrance userId:"<< userId <<" friendId:"<< friendId);

  CardRelationDataPtr host = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, userId);
  Ice::ObjectPtr hObj;
  if(!host){
    hObj = factory->create(userId);
    if(hObj){
      ServiceI::instance().addObject(hObj, CARD_RELATION_DATA, userId);
      host = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, userId);
    }
  }

  if(!host)
    return REMOVE;

  if(host->has(friendId,ACCEPT)){
    return ACCEPT;
  }else if(host->has(friendId,RECEIVE)){
    return RECEIVE;
  }else if(host->has(friendId,ADD)){
    return ADD;
  }else
    return REMOVE;

}
int xce::cardcache::CardRelationCacheI::adjust(int userId,int friendId,int state){
  MCE_INFO("[CardRelationCacheI::adjust] userId:"<<userId<<" friendId:"<<friendId << " state:" << state);
  time_t t = time(0);
  CardRelationDataPtr host = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, userId);
  Ice::ObjectPtr hObj;	
  if(!host){
    hObj = factory->create(userId);	
    if(hObj){
      ServiceI::instance().addObject(hObj, CARD_RELATION_DATA, userId);
      host = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, userId);
    }
  }

  if(!hObj && !host && state!=REMOVE){
    host = new CardRelationData;
    ServiceI::instance().addObject(host, CARD_RELATION_DATA, userId);
  }

  CardRelationDataPtr guest = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, friendId);
  Ice::ObjectPtr gObj;	
  if(!guest){
    gObj = factory->create(friendId);
    if(gObj){
      ServiceI::instance().addObject(gObj, CARD_RELATION_DATA, friendId);
      guest = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, friendId);
    }
  }

  if(!gObj && !guest && state!=REMOVE){
    guest = new CardRelationData;
    ServiceI::instance().addObject(guest, CARD_RELATION_DATA, friendId);
  }

  if(state == ADD){
    if(host->has(friendId,ADD)){
      return ADD;
    }
    if(host->has(friendId,ACCEPT)) {
      return ACCEPT;
    }

    if(host->has(friendId,RECEIVE)){
      _dispatchThreads[userId % CLUSTER]->setDbInfo(userId,friendId,ACCEPT,t);
      _dispatchThreads[friendId % CLUSTER]->setDbInfo(friendId,userId,ACCEPT,t);
      if (host->remove(friendId,RECEIVE)) {
        UserCountAdapter::instance().dec(userId,TYPE);
      }
      guest->remove(userId,ADD);
      host->add(friendId,ACCEPT,t);
      setAcceptUser(t, userId, friendId );
      guest->add(userId,ACCEPT,t);
      setAcceptUser(t, friendId, userId );
      return ACCEPT;
    }else{
      _dispatchThreads[userId % CLUSTER]->setDbInfo(userId,friendId,ADD,t);
      _dispatchThreads[friendId % CLUSTER]->setDbInfo(friendId,userId,RECEIVE,t);
      host->add(friendId,ADD,t);
      guest->add(userId,RECEIVE,t);
      UserCountAdapter::instance().inc(friendId,TYPE);
      return ADD;
    }
  }else if(state == RECEIVE){
    _dispatchThreads[userId % CLUSTER]->setDbInfo(userId,friendId,ACCEPT,t);
    _dispatchThreads[friendId % CLUSTER]->setDbInfo(friendId,userId,ACCEPT,t);
    if (host->remove(friendId,RECEIVE)) {
      UserCountAdapter::instance().dec(userId,TYPE);
    }
    host->add(friendId,ACCEPT,t);
    setAcceptUser(t, userId, friendId );
    guest->remove(userId,ADD);
    guest->add(userId,ACCEPT,t);
    setAcceptUser(t, friendId, userId );
    return ACCEPT;
  }else if(state == ACCEPT){
    _dispatchThreads[userId % CLUSTER]->setDbInfo(userId,friendId,ACCEPT,t);
    _dispatchThreads[friendId % CLUSTER]->setDbInfo(friendId,userId,ACCEPT,t);
    host->remove(friendId,ADD);
    if (host->remove(friendId,RECEIVE)) {
      UserCountAdapter::instance().dec(userId,TYPE);
    }
    host->add(friendId,ACCEPT,t);
    setAcceptUser(t, userId, friendId );
    
    guest->remove(userId,ADD);
    if (guest->remove(userId,RECEIVE)) {
      UserCountAdapter::instance().dec(friendId,TYPE);
    }
    guest->add(userId,ACCEPT,t);
    setAcceptUser(t, friendId, userId );
    return ACCEPT;
  }else if(state == REMOVE) {
    _dispatchThreads[userId % CLUSTER]->setDbInfo(userId,friendId,REMOVE,t);
    _dispatchThreads[friendId % CLUSTER]->setDbInfo(friendId,userId,REMOVE,t);
    if(host){
      host->remove(friendId,ACCEPT);
      host->remove(friendId,ADD);
      if (host->remove(friendId,RECEIVE)) {
        UserCountAdapter::instance().dec(userId,TYPE);
      }
    }
    if(guest){
      guest->remove(userId,ACCEPT);
      guest->remove(userId,ADD);
      if (guest->remove(userId,RECEIVE)) {
        UserCountAdapter::instance().dec(friendId,TYPE);
      }
    }
    return REMOVE;
  }
  return -1;
}

MyUtil::IntSeq CardRelationCacheI::getReceive(Ice::Int userId, Ice::Int begin, Ice::Int limit, const ::Ice::Current&) {
  MCE_INFO("[CardRelationCacheI::getReceive] entrance userId:" << userId << " begin:" << begin << " limit:" << limit);
  CardRelationDataPtr host = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, userId);
  Ice::ObjectPtr hObj;
  if(!host){
    hObj = factory->create(userId);
    if(hObj){
      ServiceI::instance().addObject(hObj, CARD_RELATION_DATA, userId);
      host = ServiceI::instance().findObject<CardRelationDataPtr>(CARD_RELATION_DATA, userId);
    }
  }
  if(host!=0){
    return host->getReceive(userId,begin,limit,RECEIVE);
  }
  else {
    return MyUtil::IntSeq();
  }
}

bool CardRelationCacheI::ignoreReceive(Ice::Int userId, const MyUtil::IntSeq& friendIds, const Ice::Current&    current) {
  MCE_INFO("[CardRelationCacheI::ignoreReceive] entrance userId:"<<userId << " friendIds.size:" << friendIds.size());
  for(int i = 0; i < (int)friendIds.size(); ++i) {
    adjust(userId,friendIds[i],REMOVE);
  }
  return true;
}

bool CardRelationCacheI::ignoreAll(Ice::Int userId, const ::Ice::Current& current) {
  MCE_INFO("[CardRelationCacheI::ignoreAll] entrance userId:"<<userId);
  MyUtil::IntSeq friendIds = getReceive(userId,0,-1,current);
  for(int i = 0; i < (int)friendIds.size(); ++i) {
    adjust(userId,friendIds[i],REMOVE);
  }
  return true;
}

MyUtil::Int2IntSeqMap CardRelationCacheI::getAccept(Ice::Int getTime, Ice::Int lastId, Ice::Int limit, const Ice::Current&) {
  MCE_INFO("[CardRelationCacheI::getAccept] entrance lastId:" << lastId << " limit:" << limit);
  if (limit <= 0) {
    return MyUtil::Int2IntSeqMap();
  }
  MyUtil::Int2IntSeqMap results;
  time_t t = getTime;
  char time[64];
  strftime( time, sizeof(time), "%Y-%m-%d",localtime(&t) );
  IceUtil::Mutex::Lock lock(acceptUserMutex);
  std::map<std::string, std::map<int, std::set<int> > >::iterator it = acceptUsers.find(time);
  if (it != acceptUsers.end()) {
    std::map<int, std::set<int> > infos = it->second;
    std::map<int, std::set<int> >::iterator itt;
    itt = infos.upper_bound(lastId);
    int j = 0;
    for (; itt != infos.end(); itt++) {
      if (j >= limit) {
        break;
      }
      std::vector<int> users;
      std::set<int> friendIds = itt->second;
      std::set<int>::iterator itA = friendIds.begin();
      for (; itA != friendIds.end(); itA++) {
        users.push_back(*itA);
      }
      results[itt->first] = users;
      j++;
    }
    return results;
  } else {
    return MyUtil::Int2IntSeqMap();  
  }
}

void CardRelationCacheI::setAcceptUser(time_t time, int userId, int friendId ) {
  char time_str[64];
  strftime( time_str, sizeof(time_str), "%Y-%m-%d",localtime(&time) );
  std::map<std::string, std::map<int, std::set<int> > >::iterator it;
  std::map<int, std::set<int> >::iterator itt;
  IceUtil::Mutex::Lock lock(acceptUserMutex);
  if (acceptUsers.size() >= 3) {
    time_t t = time - 4 * 86000;
    char time_delete[64];
    strftime( time_delete, sizeof(time_delete), "%Y-%m-%d",localtime(&t) );
    it = acceptUsers.find(time_delete);
    if (it != acceptUsers.end()) {
      acceptUsers.erase(it);
    }
  }
  it = acceptUsers.find(time_str);
  if (it == acceptUsers.end()) {
    std::set<int> a;
    a.insert(friendId);
    std::map<int, std::set<int> > b;
    b.insert(std::pair<int,std::set<int> >(userId,a));
    acceptUsers.insert(std::pair<std::string, std::map<int, std::set<int> > >(time_str,b));
  } else {
    itt = (it->second).find(userId);
    if (itt == (it->second).end()) {
      std::set<int> a;
      a.insert(friendId);
      (it->second).insert(std::pair<int,std::set<int> >(userId,a));
    } else {
      (itt->second).insert(friendId);
    } 
  }
}

NotifyCardDbTask::NotifyCardDbTask(std::vector<dbInfo>& toInvokeCache, int mod) : _toInvokeCache(toInvokeCache),_mod(mod) {}

void NotifyCardDbTask::handle() {
  CardDBAdapter::AddRelations(_toInvokeCache,_mod);
}

void CardRelationCacheI::Dispatcher::setDbInfo(int userId, int friendId, int state, time_t recordTime) {
//  MCE_INFO("[CardRelationCacheI::Dispatcher::setDbInfo] userId:" << userId << " friendId:" << friendId << " state:" << state << " recordTime:" << recordTime);
  dbInfo info;
  info.userId = userId;
  info.friendId = friendId;
  info.state = state;
  info.recordTime = recordTime;
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(dbInfoMutex);
  _toInvokeCache.push_back(info);
}

void CardRelationCacheI::Dispatcher::run(){
  MCE_INFO("[CardRelationCacheI::Dispatcher] _dispatchThread started");
  while (true) {
    try {
      invoke();
      sleep(5);
    } catch (std::exception& e) {
      MCE_WARN("[CardRelationCacheI::Dispatcher] _dispatchThread exception " << e.what());
    }
  }
}

void CardRelationCacheI::Dispatcher::invoke() {
  if (shouldInvoke()){
    invokeAndClearCache();
  }
}

bool CardRelationCacheI::Dispatcher::shouldInvoke() {
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(dbInfoMutex);
  return (_toInvokeCache.size() > 1000) || (time(NULL) - _lastInvokeTime > 5);
}

void CardRelationCacheI::Dispatcher::invokeAndClearCache() {
  std::vector<dbInfo> toInvokeCache_swap;
  {
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(dbInfoMutex);
    _toInvokeCache.swap(toInvokeCache_swap);
  }
  _lastInvokeTime = time(NULL);
  CardDBAdapter::AddRelations(toInvokeCache_swap,_mod);
}


/*
void LoadTask::handle() {
  MCE_INFO("LoadTask::handle()");
  loadAcceptUsers();
  MCE_INFO("LoadTask::handle() after loadAcceptUsers");
}

void LoadTask::loadAcceptUsers() {
  MCE_INFO("LoadTask::loadAcceptUsers()");
  time_t t = time(0);
  time_t t0 = t - 86400;
  char time[64];
  strftime( time, sizeof(time), "%Y-%m-%d",localtime(&t0) );
  time_t t1 = t + 86400;
  char time1[64];
  strftime( time1, sizeof(time1), "%Y-%m-%d",localtime(&t1) );

  for (int i = 0; i < 5; i++) {
    string begintime = time;
    string maxtime = begintime;
    for (;;) {
      Statement sql;
      sql << "SELECT " << FIELD_UID << ", " << FIELD_FID << ", " << FIELD_TIME << " FROM " << tableName_ << "_" << i << " WHERE "<< FIELD_FLAG << "=" << ACCEPT << " and " << FIELD_TIME1 << ">=\"" << begintime << " 00:00:00\" and " << FIELD_TIME1 << "<=\"" << time1 << " 00:00:00\" limit 4000";
      LoadUserResultHandler handler(maxtime,acceptInfos_);
      QueryRunner(DESC_ADDRESSBOOK,CDbRServer).query(sql, handler);
      CardRelationCacheI::instance().setAcceptUsers(acceptInfos_);
      MCE_INFO("LoadTask::loadAcceptUsers() acceptInfos_.size():" << acceptInfos_.size());
      acceptInfos_.clear();
      if (begintime < maxtime) {
        continue;
      } else {
        break;
      }
    }
  MCE_INFO("LoadTask::loadAcceptUsers() done!");
}
*/
bool CardRelationResultHandlerI::handle(mysqlpp::Row& row) const {
  int fid = (int)row[FIELD_FID.c_str()];
  int flag = (int)row[FIELD_FLAG.c_str()];
  int time = (int)row[FIELD_TIME1.c_str()];
  result_->add(fid,flag,time);
  return true;
}

bool CardRelationData::add(int friendid,int state, int time){
  if(state>=0 && state < (int)stateList.size()){
    IceUtil::Mutex::Lock lock(*this);
    (stateList[state])[friendid]=time;
    return true;
  }
  return false;
}

bool CardRelationData::remove(int friendid,int state){
  if(state>=0 && state<(int)stateList.size()){
    IceUtil::Mutex::Lock lock(*this);	
    map<int, int>::iterator it = stateList[state].find(friendid);
    if(it!=stateList[state].end()){
      stateList[state].erase(it);
      return true;
    } else {
      return false;
    }
  }	
  return false;
}

bool CardRelationData::has(int friendid,int state){
  if(state>=0 && state<=(int)stateList.size()){
    IceUtil::Mutex::Lock lock(*this);

    map<int, int>::iterator it = stateList[state].find(friendid);
    if(it!=stateList[state].end()){
      return true;
    }
  }
  return false;
}
int CardRelationData::getSize(int state){
  if(state>=0 && state<(int)stateList.size()){
    IceUtil::Mutex::Lock lock(*this);
    return stateList[state].size();
  }
  return 0;
}
MyUtil::IntSeq CardRelationData::get(int begin,int limit,int state){
  if(state>=0 && state<(int)stateList.size()){
    IceUtil::Mutex::Lock lock(*this);
    vector<int> result;//(stateList[state].size());
    map<int, int>::iterator it;
    for (it = stateList[state].begin(); it != stateList[state].end(); it++) {
      result.push_back(it->first);
    }
    if(limit == -1)
      return result;
    else if(begin+limit>(int)result.size())
      limit = result.size()-begin;
    return vector<int>(result.begin()+begin,result.begin()+begin+limit);
  }
  return vector<int>();
}

MyUtil::IntSeq CardRelationData::getReceive(int userId,int begin,int limit,int state){
  if(state>=0 && state<(int)stateList.size()){
    IceUtil::Mutex::Lock lock(*this);
    if (state == RECEIVE) {
      int usercount = UserCountAdapter::instance().get(userId,TYPE);
      if (usercount != (int)stateList[state].size()) {
        UserCountAdapter::instance().set(userId,TYPE,stateList[state].size());
      }
    }

    RelationInfoNewSeq results;//(stateList[state].size());
    MyUtil::IntSeq res;
    map<int,int>::iterator it;
    for (it = stateList[state].begin(); it != stateList[state].end(); it++) {
      RelationInfoNewPtr ptr = new RelationInfoNew;
      ptr->friendId = it->first;
      ptr->recordTime = it->second;
      results.push_back(ptr);
    }

    sort(results.begin(), results.end(), CompareTime);
    for (int i = 0; i < (int)results.size(); ++i) {
      res.push_back(results[i]->friendId);
    }
    if(limit == -1) {
      return res;
    } else if (begin >= (int)res.size()) {
      return MyUtil::IntSeq();
    } else if (begin+limit>(int)res.size()) {
      limit = res.size()-begin;
    }
    return MyUtil::IntSeq(res.begin()+begin,res.begin()+ begin +limit);
  }
  return MyUtil::IntSeq();
}

