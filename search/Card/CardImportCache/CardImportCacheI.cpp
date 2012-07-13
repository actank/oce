#include "../share/CardDBAdapter.h"
#include "CardImportCacheI.h"
#include <ServiceI.h>
#include <map>
#include <algorithm>
#include "QueryRunner.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"

using namespace MyUtil;
using namespace xce::cardcache;
using namespace xce::serverstate;
using namespace com::xiaonei::xce;
using namespace xce::serverstate;
using namespace xce::buddy::adapter;

//=================MyUtil::Initialize()=============
////初始化
void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.registerObjectCache(CARD_IMPORT_CACHE, "CardImportCahce", new CardImportCacheFactory);
  service.getAdapter()->add(&CardImportCacheI::instance(), service.createIdentity(CARD_IMPORT_CACHE_STR, ""));
  std::string controllerEndpoint = service.getCommunicator()->getProperties()->getPropertyWithDefault("CardImportCache.Endpoint", "ControllerCardImportCache");
  int mode = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CardImportCache", 1);
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CardImportCache.Interval", 5);
  ServerStateSubscriber::instance().initialize(controllerEndpoint, &CardImportCacheI::instance(), mode, interval);
}

//====================CardImportCacheI=================

CardImportCacheI::CardImportCacheI() {
  try{
    MCE_INFO("[CardImportCacheI::CardImportCacheI()] start");
    TaskManager::instance().execute(new LoadImportTask());
    MCE_INFO("[CardImportCacheI::CardImportCacheI()] done");
  }catch(std::exception& ex){
    cout<<ex.what()<<endl;
  }

}

//从CardImportCache中根据userId取导入信息
//userId: 用户id
//begin: 开始位置
//limit：限制个数
//返回结果：用户id所对应的import信息的集合
ImportInfoSeq CardImportCacheI::get(Ice::Int userId, Ice::Int limit, const Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::get] entrance userId:"<<userId<< " limit:"<<limit << " " << current.con->toString());
  
  if (userId <= 0) {
    return ImportInfoSeq();
  }

  CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE,userId);
  if(cip) {
    return cip->get(limit);
  } else {
    return ImportInfoSeq();
  }
}

ImportInfoNewSeq CardImportCacheI::getNew(Ice::Int userId, Ice::Int limit, const Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::getNew] entrance  userId:"<<userId<< " limit:"<<limit << " " << current.con->toString());
  if (userId <= 0) {
    return ImportInfoNewSeq();
  }

  CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE,userId);
  if(cip) {
    return cip->getNew(limit);
  } else {
    return ImportInfoNewSeq();
  }
}

ImportInfoPtr CardImportCacheI::getDetail(Ice::Int userId, Ice::Byte importWay, const std::string& importDetail, const ::Ice::Current& current){
  MCE_INFO("[CardImportCacheI::getDetail] entrance  userId:"<<userId<< " importWay:"<<importWay << " importDetail:" << importDetail << " " << current.con->toString());
  if (userId <= 0) {
    return NULL;
  }

  CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE,userId);
  if(cip) {
    return cip->getDetail(importWay, importDetail);
  } else {
    return NULL;
  }  
}

ImportInfoNewPtr CardImportCacheI::getDetailNew(Ice::Int userId, Ice::Byte importWay, const std::string& importDetail, const ::Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::getDetailNew] entrance  userId:"<<userId<< " importWay:"<<importWay << " importDetail:" << importDetail << " " << current.con->toString());
  if (userId <= 0) {
    return NULL;
  }

  CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE,userId);
  if(cip) {
    return cip->getDetailNew(importWay, importDetail);
  } else {
    return NULL;
  }
}

ImportInfoNewPtr CardImportCacheI::getDetailByEmail(Ice::Int userId, const std::string& email, const :: Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::getDetailByEmail] entrance  userId:"<< userId << " email:" << email << " " << current.con->toString());
  if (userId <= 0) {
    return NULL;
  }

  CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE,userId);
  if(cip) {
    return cip->getDetailByEmail(email);
  } else {
    return NULL;
  }
}


ImportValueSeq CardImportCacheI::getList(Ice::Int userId, const Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::getList] entrance userId:"<<userId << " " << current.con->toString());

  if (userId <= 0) {
    return ImportValueSeq();
  }

  CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE,userId);
  if(cip) {
    return cip->getList();
  } else {
    return ImportValueSeq();
  }
}

ImportValueNewSeq CardImportCacheI::getListNew(Ice::Int userId, const ::Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::getListNew] entrance userId:"<<userId << " " << current.con->toString());

  if (userId <= 0) {
    return ImportValueNewSeq();
  }

  CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE,userId);
  if(cip) {
    return cip->getListNew();
  } else {
    return ImportValueNewSeq();
  }
}

ImportValueIISeq CardImportCacheI::getListII(Ice::Int userId, const ::Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::getListII] entrance userId:"<<userId << " " << current.con->toString());

  if (userId <= 0) {
    return ImportValueIISeq();
  }

  CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE,userId);
  if(cip) {
    return cip->getListII();
  } else {
    return ImportValueIISeq();
  }
}

// 添加或修改ImportInfo的值，以userId为Key,将整个ImportInfoSeq添加到Cache中
// 如果给定的userId已经存在，则将其删除，添加新的userId
bool CardImportCacheI::set(const ImportInfoSeq& infos, const Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::set] entrance infos.size():"<<infos.size() << " " << current.con->toString());
  Ice::Long uid;
  if (infos.empty()) {
    return false;
  }
  TimeCost tc = TimeCost::create("[CardImportCacheI::set Before Write DB]",1);
  if (!CardDBAdapter::AddImport(infos)) {
    return false;
  }
  tc.step("[CardImportCacheI::set Write DB Done]");
  // 分类，将具有相同userId的ImportInfo对象放在一起
  std::map<int, ImportInfoNewSeq> id2ImportData;
  for (size_t i = 0; i < infos.size(); ++i) {
    uid = infos[i]->userId;
    ImportInfoNewPtr ptr = new ImportInfoNew;
    ptr->importWay = infos[i]->importWay;
    ptr->importDetail = infos[i]->importDetail;
    ptr->name = infos[i]->name;
    ptr->userId = infos[i]->userId;
    ptr->msn = infos[i]->msn;
    ptr->email = infos[i]->email;
    ptr->mobile = infos[i]->mobile;
    ptr->qq = infos[i]->qq;
    ptr->address = infos[i]->address;
    ptr->extra = infos[i]->extra;
    ptr->requestCount = infos[i]->requestCount;
    id2ImportData[uid].push_back(ptr);
  } 
  tc.step("[CardImportCacheI::set ZuZhuang]");
  for (std::map<int, ImportInfoNewSeq>::iterator it = id2ImportData.begin(); it != id2ImportData.end(); ++it) {
    CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE, it->first);
    CardImportDataPtr ptr = new CardImportData();
    ptr->infos = it->second;
    if (!cip) {
      ServiceI::instance().addObject(ptr, CARD_IMPORT_CACHE, it->first);
    } else {
      cip->set(it->second);
    }
  }
  tc.step("[CardImportCacheI::set Done]");
  return true;
}

bool CardImportCacheI::setNew(const ImportInfoNewSeq& infos, const Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::setNew] entrance infos.size():"<<infos.size() << " " << current.con->toString());
  Ice::Long uid;
  if (infos.empty()) {
    return false;
  }
  TimeCost tc = TimeCost::create("[CardImportCacheI::set Before Write DB]",1);
  if (!CardDBAdapter::AddImportNew(infos)) {
    return false;
  }
  tc.step("[CardImportCacheI::set Write DB Done]");
  // 分类，将具有相同userId的ImportInfo对象放在一起
  std::map<int, ImportInfoNewSeq> id2ImportData;
  for (size_t i = 0; i < infos.size(); ++i) {
    uid = infos[i]->userId;
    id2ImportData[uid].push_back(infos[i]);
  }
  tc.step("[CardImportCacheI::set ZuZhuang]");
  for (std::map<int, ImportInfoNewSeq>::iterator it = id2ImportData.begin(); it != id2ImportData.end(); ++it) {
    CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE, it->first);
    CardImportDataPtr ptr = new CardImportData();
    ptr->infos = it->second;
    if (!cip) {
      ServiceI::instance().addObject(ptr, CARD_IMPORT_CACHE, it->first);
    } else {
      cip->setNew(it->second);
    }
  }
  tc.step("[CardImportCacheI::set Done]");
  return true;
}

bool CardImportCacheI::load(const ImportInfoSeq& infos, const Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::load] entrance infos.size():"<<infos.size() << " " << current.con->toString());
  Ice::Long uid;
//  TimeCost tc = TimeCost::create("[CardImportCacheI::load Before Write DB]",1);
  if (infos.empty()) {
    return false;
  }
  MCE_INFO("Before write DB");
  if (!CardDBAdapter::AddImport(infos)) {
    return false;
  }
  MCE_INFO("After write DB");
//  tc.step("[CardImportCacheI::load Write DB Done]");
  // 分类，将具有相同userId的ImportInfo对象放在一起
  std::map<int, ImportInfoNewSeq> id2ImportData;
  for (size_t i = 0; i < infos.size(); ++i) {
    uid = infos[i]->userId;
    ImportInfoNewPtr ptr = new ImportInfoNew;
    ptr->importWay = infos[i]->importWay;
    ptr->importDetail = infos[i]->importDetail;
    ptr->name = infos[i]->name;
    ptr->userId = infos[i]->userId;
    ptr->msn = infos[i]->msn;
    ptr->email = infos[i]->email;
    ptr->mobile = infos[i]->mobile;
    ptr->qq = infos[i]->qq;
    ptr->address = infos[i]->address;
    ptr->extra = infos[i]->extra;
    ptr->requestCount = infos[i]->requestCount;
    id2ImportData[uid].push_back(ptr);
  } 
//  tc.step("[CardImportCacheI::load ZuZhuang]");
  IceUtil::Mutex::Lock lock(_importUserMutex);
  for (std::map<int, ImportInfoNewSeq>::iterator it = id2ImportData.begin(); it != id2ImportData.end(); ++it) {
    CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE, it->first);
    CardImportDataPtr ptr = new CardImportData();
    ptr->infos = it->second;
    if (!cip) {

      ServiceI::instance().addObject(ptr, CARD_IMPORT_CACHE, it->first);
    } else {
      if (cip->load(it->first,it->second)) {
        vector<int>::iterator lowIt=lower_bound(importUser.begin(),importUser.end(),it->first);
        if(lowIt!=importUser.end() && *lowIt!=it->first){
          importUser.insert(lowIt,it->first);
        }else if(lowIt == importUser.end()){
          importUser.push_back(it->first);
        }
      }
    }
  }
  //tc.step("[CardImportCacheI::load Done]");
  return true;
}

bool CardImportCacheI::loadNew(const ImportInfoNewSeq& infos, const Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::loadNew] entrance infos.size():"<<infos.size() << " " << current.con->toString());
  Ice::Long uid;
  if (infos.empty()) {
    return false;
  } 
  TimeCost tc = TimeCost::create("[CardImportCacheI::loadNew Before Write DB]",1);
  if (!CardDBAdapter::AddImportNew(infos)) {
    return false;
  } 
  tc.step("[CardImportCacheI::loadNew Write DB Done]");
  std::map<int, ImportInfoNewSeq> id2ImportData;
  for (size_t i = 0; i < infos.size(); ++i) {
    uid = infos[i]->userId;
    MCE_INFO("[CardImportCacheI::loadNew] userId " << uid << " importWay " << int(infos[i]->importWay));
    id2ImportData[uid].push_back(infos[i]);
  }
  tc.step("[CardImportCacheI::loadNew ZuZhuang]");
  for (std::map<int, ImportInfoNewSeq>::iterator it = id2ImportData.begin(); it != id2ImportData.end(); ++it) {
    CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE, it->first);
    CardImportDataPtr ptr = new CardImportData();
    ptr->infos = it->second;
    if (!cip) {

      ServiceI::instance().addObject(ptr, CARD_IMPORT_CACHE, it->first);
    } else {
      if (cip->loadNew(it->first,it->second)) {
        IceUtil::Mutex::Lock lock(_importUserMutex);
        vector<int>::iterator lowIt=lower_bound(importUser.begin(),importUser.end(),it->first);
        if(lowIt!=importUser.end() && *lowIt!=it->first){
          importUser.insert(lowIt,it->first);
        }else if(lowIt == importUser.end()){
          importUser.push_back(it->first);
        }
      }
    }
  }
  tc.step("[CardImportCacheI::loadNew Done]");
  return true;
}

ImportInfoNewSeq CardImportCacheI::loadII(const ImportInfoNewSeq& infos, const Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::loadII] entrance infos.size():"<<infos.size() << " " << current.con->toString());
  Ice::Long uid;
  ImportInfoNewSeq results;
  if (infos.empty()) {
    return ImportInfoNewSeq();
  }
  TimeCost tc = TimeCost::create("[CardImportCacheI::loadII Before Write DB]",1);
  if (!CardDBAdapter::AddImportNew(infos)) {
    return ImportInfoNewSeq();;
  }
  tc.step("[CardImportCacheI::loadII Write DB Done]");
  std::map<int, ImportInfoNewSeq> id2ImportData;
  for (size_t i = 0; i < infos.size(); ++i) {
    uid = infos[i]->userId;
    MCE_INFO("[CardImportCacheI::loadII] userId " << uid << " importWay " << int(infos[i]->importWay));
    id2ImportData[uid].push_back(infos[i]);
  }
  tc.step("[CardImportCacheI::loadII ZuZhuang]");
  for (std::map<int, ImportInfoNewSeq>::iterator it = id2ImportData.begin(); it != id2ImportData.end(); ++it) {
    CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE, it->first);
    CardImportDataPtr ptr = new CardImportData();
    ptr->infos = it->second;
    if (!cip) {
      ServiceI::instance().addObject(ptr, CARD_IMPORT_CACHE, it->first);
    } else {
      if (cip->loadNew(it->first,it->second)) {
        IceUtil::Mutex::Lock lock(_importUserMutex);
        vector<int>::iterator lowIt=lower_bound(importUser.begin(),importUser.end(),it->first);
        if(lowIt!=importUser.end() && *lowIt!=it->first){
          importUser.insert(lowIt,it->first);
        }else if(lowIt == importUser.end()){
          importUser.push_back(it->first);
        }
      }
    }
  }

  for (size_t i = 0; i < infos.size(); ++i) {
    ImportInfoNewPtr ptr = getDetailNew(infos[i]->userId,infos[i]->importWay,infos[i]->importDetail,current);
    if (ptr) {
      results.push_back(ptr);
    }
  }

  tc.step("[CardImportCacheI::loadII Done]");
  return results;
}

ImportIdImportDetailSeq CardImportCacheI::getIdImportDetail(Ice::Int userId, const Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::getIdImportDetail] entrance  userId " << userId << " " << current.con->toString());
  if (userId <= 0) {
    return ImportIdImportDetailSeq();
  }

  CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE,userId);
  if(cip) {
    return cip->getIdImportDetail();
  } else {
    return ImportIdImportDetailSeq();
  }
}

bool CardImportCacheI::setSubscribeFlag(const ImportInfoNewSeq& infos, const Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::setSubscribeFlag] entrance infos.size() " << infos.size() << " " << current.con->toString());
  Ice::Long uid;
  if (infos.empty()) {
    return false;
  }

  if (!CardDBAdapter::SetSubscribeFlag(infos)) {
    return false;
  }

  std::map<int, ImportInfoNewSeq> id2ImportData;
  for (size_t i = 0; i < infos.size(); ++i) {
    uid = infos[i]->userId;
    id2ImportData[uid].push_back(infos[i]);
  }

  for (std::map<int, ImportInfoNewSeq>::iterator it = id2ImportData.begin(); it != id2ImportData.end(); ++it) {
    CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE, it->first);
    if (!cip) {
      return false;
    } else {
      cip->setSubscribeFlag(it->second);
    }
  }

  return true;
}

bool CardImportCacheI::isContact(Ice::Int userId, const std::string& email, Ice::Int eType, const ::Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::isContact] entrance  userId " << userId << " email " << email << " eType " << eType << " " << current.con->toString());
  if (userId <= 0) {
    return false;
  }

  CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE,userId);
  if(cip) {
    return cip->isContact(email,eType);
  } else {
    return false;
  }
}

// 从数据库和Cache中删除给定用户的Import信息
// ids：给定的用户id集合
void CardImportCacheI::remove(const ::xce::cardcache::ImportInfoSeq& infos, const Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::remove] entrance INFOS.SIZE() " << infos.size() << " " << current.con->toString());
  int deletenum = 0;
  if (infos.empty()) {
    return;
  }
  CardDBAdapter::RemoveImport(infos);
  MCE_INFO("[CardImportCacheI::remove] after delete from DB ");
  for (ImportInfoSeq::const_iterator it = infos.begin(); it != infos.end(); ++it) {
    MCE_INFO("[CardImportCacheI::remove] userId " << (*it)->userId);
    CardImportDataPtr cip = ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE, (*it)->userId);
    if (cip) {
      deletenum = cip->remove((*it)->importDetail,(*it)->importWay);
    }
    if (deletenum == 0) {
      vector<int>::iterator lowIt=lower_bound(importUser.begin(),importUser.end(),(*it)->userId);
      if(lowIt!=importUser.end() && *lowIt==(*it)->userId){
        MCE_INFO("[CardImportCacheI::remove] remove from importUser");
        importUser.erase(lowIt);
      }
    }
  }
}

IntSeq CardImportCacheI::getImportUser(Ice::Int userId, const Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::getImportUser] entrance userId " << userId << " " << current.con->toString());
  IntSeq results;
  int count = 0;
  int countImportUser = 0;
  std::set<int> friends;
  IntSeq::iterator ret;
  srand(time(NULL));
  MCE_INFO("[CardImportCacheI::getImportUser] entrance userId " << userId);
  if (userId <= 0) {
    return MyUtil::IntSeq();
  }
  IntSeq allFriends = BuddyByIdCacheAdapter::instance().getFriendListAsc(userId,-1);
//  IntSeq allFriends;
//  allFriends.push_back(11111);
//  allFriends.push_back(67067835);
  IceUtil::Mutex::Lock lock(_importUserMutex);
  for (IntSeq::iterator it = allFriends.begin(); it != allFriends.end(); ++it) {
    if(binary_search(importUser.begin(),importUser.end(), *it)) {
      results.push_back(*it);
      ++count;
    }
    if (count >= 20) {
      break;
    }
  }
  int numFriends = int(importUser.size() - results.size());
  MCE_INFO("numFriends " << numFriends);
  while (results.size() < 20 && countImportUser < numFriends) {
    int i = rand()%(importUser.size());
    MCE_INFO("[CardImportCacheI::getImportUser] i " << i << " importUser.size() " << importUser.size());
    ret = std::find(results.begin(),results.end(),importUser[i]);
    if (ret == results.end()) {
      results.push_back(importUser[i]);
      MCE_INFO("countImportUser " << countImportUser << " results.size() " << results.size());
      countImportUser++;
    }
  }
  return results;
}

bool CardImportCacheI::isImported(Ice::Int userId, const Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::isImported] entrance userId " << userId << " " << current.con->toString());
  IceUtil::Mutex::Lock lock(_importUserMutex);
  if(binary_search(importUser.begin(),importUser.end(), userId)) {
    return true;
  } else {
    return false;
  }
}

IntSeq CardImportCacheI::friendsImported(const IntSeq& friendIds, const Ice::Current& current) {
  IntSeq results;
  MCE_INFO("[CardImportCacheI::friendsImported] entrance friendIds.size() " << friendIds.size() << " " << current.con->toString());
  if (friendIds.empty()) {
    MyUtil::IntSeq();
  }
  IceUtil::Mutex::Lock lock(_importUserMutex);
  for (IntSeq::const_iterator it = friendIds.begin(); it != friendIds.end(); ++it) {
    if(binary_search(importUser.begin(),importUser.end(), *it)) {
      results.push_back(*it);
    }
  }
  return results;
}

void LoadImportTask::handle() {
  MCE_INFO("LoadImportTask::handle()");
  loadImportUser();
  MCE_INFO("LoadImportTask::handle() after loadImportUser");
  CardImportCacheI::instance().union_vec(importUser_);
  importUser_.clear();
}

void LoadImportTask::loadImportUser() {
  MCE_INFO("LoadImportTask::loadImportUser()");
  int beginId = 0;
  int maxId = beginId;
  int lastId = -1;
  for (;;) {
    beginId = maxId;
    Statement sql;
    sql << "SELECT user_id from " << IMPORT_TABLE_NAME << " where user_id>" << beginId << " limit 4000";
    ImportUserResultHandler handler(importUser_,lastId);
    QueryRunner(IMPORT_TABLE_DATA_SOURCE, CDbRServer).query(sql, handler);
    maxId = importUser_.back();
    MCE_INFO("LoadImportTask::loadImportUser maxId:" << maxId << " lastId:" << lastId <<" importUser_.size():" << importUser_.size());
    if (beginId < maxId) {
      continue;
    } else {
      break;
    }
  }
  MCE_INFO("LoadImportTask::loadImportUser done! importUser_.size()=" << importUser_.size());
}

ImportEmailSeq CardImportCacheI::getImportEmail(Ice::Int offset, Ice::Int limit, const Ice::Current& current) {
  TimeCost tc = TimeCost::create("[CardImportCacheI::getImportEmail] start",1);
  importEmails.clear();
  tc.step("[CardImportCacheI::getImportEmail] clear importEmails");
  MCE_INFO("[CardImportCacheI::getImportEmail] entrance offset " << offset << " limit " << limit << " " << current.con->toString());
  Statement sql;
  tc.step("[CardImportCacheI::getImportEmail] before select from db");
  sql << "SELECT id, user_id, email from " << IMPORT_TABLE_NAME << " limit " << offset << "," << limit;
  ImportEmailResultHandler handler(importEmails);
  QueryRunner(IMPORT_TABLE_DATA_SOURCE, CDbRServer).query(sql, handler);
  tc.step("[CardImportCacheI::getImportEmail] select db done");
  MCE_INFO("[CardImportCacheI::getImportEmail] importEmails.size() " << importEmails.size());
  return importEmails;
}

MyUtil::IntSeq CardImportCacheI::getImportUserId(Ice::Int begin, Ice::Int limit, const ::Ice::Current& current) {
  MCE_INFO("[CardImportCacheI::getImportUserId] entrance begin " << begin << " limit " << limit << " " << current.con->toString());
  IceUtil::Mutex::Lock lock(_importUserMutex);
  if(limit == -1) {
    MCE_INFO("[CardRelationData::getImportUserId] return all");
    return importUser;
  } else if (begin >= (int)importUser.size()) {
    return MyUtil::IntSeq();
  } else if (begin+limit>(int)importUser.size()) {
    limit = importUser.size()-begin;
  }
  MCE_INFO("[CardRelationData::getImportUserId] return limit");
  return MyUtil::IntSeq(importUser.begin()+begin,importUser.begin()+ begin +limit);
}

//===============ImportInfoCacheFactory===========
//id: user的id
Ice::ObjectPtr CardImportCacheFactory::create(Ice::Long id) {
  MCE_INFO("[CardImportCacheFactory::create] id:" << id);
  Statement sql;
  CardImportDataPtr obj = new CardImportData();

  sql << "SELECT id,user_id,import_way, import_detail, msn, email, name, phone, qq, address, extra,request_count,subscribe_flag from " << IMPORT_TABLE_NAME << " WHERE user_id = " << id;
  size_t size = QueryRunner(IMPORT_TABLE_DATA_SOURCE, CDbRServer).query(sql, LoadPersistentByAddressImportInfoHandler(obj));
  if (1 > size) {
    MCE_INFO("RETURN NULL");  
  } else {
    MCE_INFO("HAVE RESULT");
  }
  return obj;
}

ImportInfoSeq CardImportData::get(Ice::Int limit) {
  IceUtil::Mutex::Lock lock(*this);
  MCE_INFO("[CardImportData::get] limit:"<<limit);
  ImportInfoSeq results(infos.size());
  for (int i = 0; i < (int)infos.size(); ++i) {
    ImportInfoPtr ptr = new ImportInfo;
    ptr->userId = infos[i]->userId;
    ptr->importWay = infos[i]->importWay;
    ptr->importDetail = infos[i]->importDetail;
    ptr->name = infos[i]->name;
    ptr->msn = infos[i]->msn;
    ptr->email = infos[i]->email;
    ptr->mobile = infos[i]->mobile;
    ptr->qq = infos[i]->qq;
    ptr->address = infos[i]->address;
    ptr->extra = infos[i]->extra;
    ptr->requestCount = infos[i]->requestCount;
    results[i] = ptr;
  }

  if(limit == -1 || limit>=(int)results.size())
    return results;
  return ImportInfoSeq(results.begin(),results.begin()+limit);
}

ImportInfoNewSeq CardImportData::getNew(Ice::Int limit) {
  IceUtil::Mutex::Lock lock(*this);
  MCE_INFO("[CardImportData::getNew] limit:"<<limit);
  if(limit == -1 || limit>=(int)infos.size())
    return infos;
  return ImportInfoNewSeq(infos.begin(),infos.begin()+limit);
}

ImportInfoPtr CardImportData::getDetail(Ice::Byte importWay, const std::string& importDetail) {
  IceUtil::Mutex::Lock lock(*this);
  MCE_INFO("[CardImportData::getDetail] importWay:"<<(unsigned char)importWay << " importDetail:" << importDetail);
  ImportInfoNewSeq::iterator it;
  it = std::find_if(infos.begin(), infos.end(), findImportInfo((unsigned char)importWay, importDetail));
  if(it != infos.end()) {
    ImportInfoPtr ptr = new ImportInfo;
    ptr->userId = (*it)->userId;
    ptr->importWay = (*it)->importWay;
    ptr->importDetail = (*it)->importDetail;
    ptr->name = (*it)->name;
    ptr->msn = (*it)->msn;
    ptr->email = (*it)->email;
    ptr->mobile = (*it)->mobile;
    ptr->qq = (*it)->qq;
    ptr->address = (*it)->address;
    ptr->extra = (*it)->extra;
    ptr->requestCount = (*it)->requestCount;
    return ptr;
  }
  return NULL;
}

ImportInfoNewPtr CardImportData::getDetailNew(Ice::Byte importWay, const std::string& importDetail) {
  IceUtil::Mutex::Lock lock(*this);
  MCE_INFO("[CardImportData::getDetailNew] importWay:"<<(unsigned char)importWay << " importDetail:" << importDetail);
  ImportInfoNewSeq::iterator it;
  it = std::find_if(infos.begin(), infos.end(), findImportInfo((unsigned char)importWay, importDetail));
  if(it != infos.end()) {
    return *it;
  }
  return NULL;
}

ImportInfoNewPtr CardImportData::getDetailByEmail(const std::string& email) {
  IceUtil::Mutex::Lock lock(*this);
  MCE_INFO("[CardImportData::getDetailByEmail] email:" << email);
  ImportInfoNewSeq::iterator it;
  it = std::find_if(infos.begin(), infos.end(), findEmail(email));
  if(it != infos.end()) {
    return *it;
  }
  return NULL;
}

ImportValueSeq CardImportData::getList() {
  ImportValueSeq result(infos.size());
  IceUtil::Mutex::Lock lock(*this);
  MCE_INFO("[CardImportData::getList] infos.size():" << infos.size() << " result.size():" << result.size());
  for (int i = 0; i < int(infos.size()); ++i) {
    ImportValuePtr ptr = new ImportValue;
    ptr->importDetail = infos[i]->importDetail;
    ptr->importWay = infos[i]->importWay;
    ptr->name = infos[i]->name;
    result[i] = ptr;
  }
  return result;
}

ImportValueNewSeq CardImportData::getListNew() {
  ImportValueNewSeq result(infos.size());
  IceUtil::Mutex::Lock lock(*this);
  MCE_INFO("[CardImportData::getListNew] infos.size():" << infos.size() << " result.size():" << result.size());
  for (int i = 0; i < int(infos.size()); ++i) {
    ImportValueNewPtr ptr = new ImportValueNew;
    ptr->id = infos[i]->id;
    ptr->importDetail = infos[i]->importDetail;
    ptr->importWay = infos[i]->importWay;
    ptr->name = infos[i]->name;
    result[i] = ptr;
  }
  return result;
}

ImportValueIISeq CardImportData::getListII() {
  ImportValueIISeq result(infos.size());
  IceUtil::Mutex::Lock lock(*this);
  MCE_INFO("[CardImportData::getListII] infos.size():" << infos.size() << " result.size():" << result.size());
  for (int i = 0; i < int(infos.size()); ++i) {
    ImportValueIIPtr ptr = new ImportValueII;
    ptr->id = infos[i]->id;
    ptr->importDetail = infos[i]->importDetail;
    ptr->importWay = infos[i]->importWay;
    ptr->name = infos[i]->name;
    ptr->subscribeFlag = infos[i]->subscribeFlag;
    result[i] = ptr;
  }
  return result;
}

bool CardImportData::load(int userId, const ImportInfoNewSeq& result) {
  MCE_INFO("[CardImportData::load] userId " << userId << " result.size() " << result.size());
  IceUtil::Mutex::Lock lock(*this);
  ServiceI::instance().removeObject(CARD_IMPORT_CACHE, userId);
  ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE, userId);
  return true;
}

bool CardImportData::loadNew(int userId, const ImportInfoNewSeq& result) {
  MCE_INFO("[CardImportData::loadNew] userId " << userId << " result.size() " << result.size());
  IceUtil::Mutex::Lock lock(*this);
  ServiceI::instance().removeObject(CARD_IMPORT_CACHE, userId);
  ServiceI::instance().locateObject<CardImportDataPtr>(CARD_IMPORT_CACHE, userId);
  return true;
}

bool CardImportData::set(const ImportInfoNewSeq& result) {
  MCE_INFO("[CardImportData::set] result.size() " << result.size());
  MCE_INFO("[CardImportData::set] infos.size() " << infos.size());
  TimeCost tc = TimeCost::create("[CardImportData::set Before Set Cache]",1);
  IceUtil::Mutex::Lock lock(*this);
  ImportInfoNewSeq::iterator it;
  for (int i = 0; i < int(result.size()); ++i) {
    unsigned char way = result[i]->importWay;
    std::string detail = result[i]->importDetail;
    it = std::find_if(infos.begin(), infos.end(), findImportInfo(way, detail));
    if (it != infos.end()) {
      (*it)->msn = result[i]->msn;
      (*it)->email = result[i]->email;
      (*it)->name = result[i]->name;
      (*it)->mobile = result[i]->mobile;
      (*it)->qq = result[i]->qq;
      (*it)->address = result[i]->address;
      (*it)->extra = result[i]->extra;
      (*it)->requestCount = result[i]->requestCount;
    } else {
      ImportInfoNewPtr ptr = new ImportInfoNew;
      ptr->userId = result[i]->userId;
      ptr->importWay = result[i]->importWay;
      ptr->importDetail = result[i]->importDetail;
      ptr->msn = result[i]->msn;
      ptr->email = result[i]->email;
      ptr->name = result[i]->name;
      ptr->mobile = result[i]->mobile;
      ptr->qq = result[i]->qq;
      ptr->address = result[i]->address;
      ptr->extra = result[i]->extra;
      ptr->requestCount = result[i]->requestCount;
      infos.push_back(ptr);
    }
  }
  tc.step("[CardImportData::set Set Cache Done]");
  return true;
}

bool CardImportData::setNew(const ImportInfoNewSeq& result) {
  MCE_INFO("[CardImportData::setNew] result.size() " << result.size());
  MCE_INFO("[CardImportData::setNew] infos.size() " << infos.size());
  TimeCost tc = TimeCost::create("[CardImportData::setNew Before Set Cache]",1);
  IceUtil::Mutex::Lock lock(*this);
  ImportInfoNewSeq::iterator it;
  for (int i = 0; i < int(result.size()); ++i) {
    unsigned char way = result[i]->importWay;
    std::string detail = result[i]->importDetail;
    it = std::find_if(infos.begin(), infos.end(), findImportInfo(way, detail));
    if (it != infos.end()) {
      (*it)->msn = result[i]->msn;
      (*it)->email = result[i]->email;
      (*it)->name = result[i]->name;
      (*it)->mobile = result[i]->mobile;
      (*it)->qq = result[i]->qq;
      (*it)->address = result[i]->address;
      (*it)->extra = result[i]->extra;
      (*it)->requestCount = result[i]->requestCount;
      (*it)->subscribeFlag = result[i]->subscribeFlag;
    } else {
      ImportInfoNewPtr ptr = new ImportInfoNew;
      ptr->id = result[i]->id;
      ptr->userId = result[i]->userId;
      ptr->importWay = result[i]->importWay;
      ptr->importDetail = result[i]->importDetail;
      ptr->msn = result[i]->msn;
      ptr->email = result[i]->email;
      ptr->name = result[i]->name;
      ptr->mobile = result[i]->mobile;
      ptr->qq = result[i]->qq;
      ptr->address = result[i]->address;
      ptr->extra = result[i]->extra;
      ptr->requestCount = result[i]->requestCount;
      ptr->subscribeFlag = result[i]->subscribeFlag;
      infos.push_back(ptr);
    }
  }
  tc.step("[CardImportData::set Set Cache Done]");
  return true;
}

bool CardImportData::setSubscribeFlag(const ImportInfoNewSeq& values) {
  MCE_INFO("[CardImportData::setSubscribeFlag] values.size() " << values.size());
  MCE_INFO("[CardImportData::setSubscribeFlag] infos.size() " << infos.size());
  TimeCost tc = TimeCost::create("[CardImportData::setNew Before Set Cache]",1);
  IceUtil::Mutex::Lock lock(*this);
  ImportInfoNewSeq::iterator it;
  sort(infos.begin(), infos.end(), CompareID);
  for (int i = 0; i < int(values.size()); ++i) {
    it = lower_bound(infos.begin(), infos.end(), values[i], CompareID2);
    if (it != infos.end()) {
      MCE_INFO("[CardImportData::setSubscribeFlag] find");
      (*it)->subscribeFlag = values[i]->subscribeFlag;
    }
  }
  tc.step("[CardImportData::setSubscribeFlag Set Cache Done]");
  return true;
}

ImportIdImportDetailSeq CardImportData::getIdImportDetail() {
  ImportIdImportDetailSeq results(infos.size());
  IceUtil::Mutex::Lock lock(*this);
  MCE_INFO("[CardImportData::getIdImportDetail] infos.size():" << infos.size() << " result.size():" << results.size());
  for (int i = 0; i < int(infos.size()); ++i) {
    ImportIdImportDetailPtr ptr = new ImportIdImportDetail;
    ptr->id = infos[i]->id;
    ptr->importDetail = infos[i]->importDetail;
    results[i] = ptr;
  }
  return results;
}

bool CardImportData::isContact(const std::string& email, int eType) {
  MCE_INFO("[CardImportData::isContact] email " << email << " eType " << eType);
  MCE_INFO("[CardImportData::isContact] infos.size() " << infos.size());
  IceUtil::Mutex::Lock lock(*this);
  ImportInfoNewSeq::iterator it;
  switch(eType) {
    case 0:
      it = std::find_if(infos.begin(), infos.end(), findDetail(email));
      break;
    case 1:
      it = std::find_if(infos.begin(), infos.end(), findMsn(email));
      break;
    case 2:
      it = std::find_if(infos.begin(), infos.end(), findQq(email));
      break;
    case 3:
      it = std::find_if(infos.begin(), infos.end(), findEmail(email));
      break;
    default:
      it = infos.end();
  }
  if (it != infos.end()) {
    MCE_INFO("[CardImportData::isContact] find");
    return true;
  } else {
    return false;
  }
}

int CardImportData::remove(const std::string& detail, unsigned char way) {
  MCE_INFO("[CardImportData::remove] detail " << detail << " way " << way);
  IceUtil::Mutex::Lock lock(*this);
  ImportInfoNewSeq::iterator it;
// unsigned char way = info->importWay;
//  std::string detail = info->importDetail;
  it = std::find_if(infos.begin(), infos.end(), findImportInfo(way, detail));
  if (it!=infos.end()) {
    infos.erase(it);
  }
  MCE_INFO("[CardImportData::remove] infos.size() " << (int)infos.size());
  return (int)infos.size();
}
