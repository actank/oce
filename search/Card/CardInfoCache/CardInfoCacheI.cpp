#include "../share/CardDBAdapter.h"
#include "CardInfoCacheI.h"
#include <ServiceI.h>
#include "QueryRunner.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include "OceCxxAdapter/src/UserContactAdapter.h"

using namespace MyUtil;
using namespace xce::cardcache;
using namespace xce::serverstate;
using namespace com::xiaonei::xce;
using namespace xce::serverstate;
using namespace xce::adapter::usercontact;

//===============MyUtil::Initialize()==============
//初始化
void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.registerObjectCache(CARD_INFO_CACHE, "CIC", new CardInfoCacheFactory, false);
  service.getAdapter()->add(&CardInfoCacheI::instance(),service.createIdentity(CARD_INFO_CACHE_STR, ""));
  std::string controllerEndpoint = service.getCommunicator()->getProperties()->getPropertyWithDefault("CardInfoCache.Endpoint","ControllerCardInfoCache");
  int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CardInfoCache.Mod",1);
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CardInfoCache.Interval",5);
  ServerStateSubscriber::instance().initialize(controllerEndpoint,&CardInfoCacheI::instance(),mod,interval);
}

//===============CardInfoCacheI===================
//从CardInfoCache中根据userId取名片信息
::xce::cardcache::CardInfoSeq CardInfoCacheI::get(const ::MyUtil::IntList& userIds, const Ice::Current& current) {
  MCE_INFO("[CardInfoCacheI::get] UserIdSize:" << userIds.size());
  CardInfoSeq results;
  MyUtil::LongSeq long_ids;
  MyUtil::ObjectResultPtr objPtr;
  CardInfoCacheEntryPtr entry;
  
  if (userIds.empty()) {
    return CardInfoSeq();
  }

  for (MyUtil::IntList::const_iterator itr = userIds.begin(); itr != userIds.end(); ++itr) {
    long_ids.push_back(*itr);
  }
  //从objectCache中取出用户名相应的对象，遍历得到的map把结果插入到CardInfo向量中
  objPtr = ServiceI::instance().getObjectCache()->locateObjects(CARD_INFO_CACHE, long_ids);
  for (MyUtil::ObjectMap::iterator it = objPtr->data.begin(); it != objPtr->data.end(); ++it) {
  	CardInfoPtr info = new CardInfo;
    info->userId = it->first;
    entry = CardInfoCacheEntryPtr::dynamicCast(it->second);
    info->address = entry->get_address();
    info->qq = entry->get_qq();
    info->mobile = entry->get_mobile();
    info->email = entry->get_email();
    info->msn = entry->get_msn();
    info->extra = entry->get_extra();
    info->privacy = entry->get_privacy();

    results.push_back(info);
  }
  MCE_INFO("[CardInfoCache::get] ResultsCount:"<<results.size());
  return results;
}

//从CardInfoCache中取名片的qq、手机、msn和隐私
::xce::cardcache::PartOfCardInfoSeq CardInfoCacheI::getPart(const ::MyUtil::IntList& userIds, const ::Ice::Current& current) {
  MCE_INFO("[CardInfoCacheI::getPart] UserIdSize:" << userIds.size());
  PartOfCardInfoSeq results;
  if (userIds.empty()) {
    return PartOfCardInfoSeq();
  }
  MyUtil::ObjectResultPtr objPtr;
  MyUtil::LongSeq long_ids;

  if (userIds.empty()) {
    return results;
  }

  for (MyUtil::IntList::const_iterator itr = userIds.begin(); itr != userIds.end(); ++itr) {
    long_ids.push_back(*itr);
  }

  //从objectCache中取出用户名相应的对象，遍历得到的map把结果(用户名、qq、手机、隐私)插入到PartOfCardInfo向量中
  objPtr = ServiceI::instance().getObjectCache()->locateObjects(CARD_INFO_CACHE, long_ids);
  for (MyUtil::ObjectMap::iterator it = objPtr->data.begin(); it != objPtr->data.end(); ++it) {
	CardInfoCacheEntryPtr entry = CardInfoCacheEntryPtr::dynamicCast(it->second);
	PartOfCardInfoPtr partOfCardInfo = new PartOfCardInfo;
    partOfCardInfo->userId = it->first;
    partOfCardInfo->qq = entry->get_qq();
    partOfCardInfo->mobile = entry->get_mobile();
    partOfCardInfo->privacy = entry->get_privacy();
    results.push_back(partOfCardInfo);
  }
  MCE_INFO("[CardInfoCache::getPart] ResultsCount:"<<results.size());
  return results;
}

bool CardInfoCacheI::isValid(const Ice::Current&) {
    IceUtil::RWRecMutex::RLock lock(_validMutex);
    return _isValid;
}

void CardInfoCacheI::setValid(bool newState, const Ice::Current&) {
    IceUtil::RWRecMutex::WLock lock(_validMutex);
    _isValid = newState;
    if (_isValid) {
                ServerStateSubscriber::instance().setStatus(1);
        } else {
                ServerStateSubscriber::instance().setStatus(0);
        }
}   

//给UserLogic提供修改Contact相关信息和个人主页隐私的接口，方便数据同步
void CardInfoCacheI::setProperty(int userId, const Str2StrMap& props, const Ice::Current& current) {	
	CardInfoCacheEntryPtr p = ServiceI::instance().locateObject<CardInfoCacheEntryPtr>(CARD_INFO_CACHE, userId);
	if(!p)
		return;
	// 输入的字段：PROFILE_PRIVACY MOBILEPHONE MSN QQ
	CardInfoCacheEntryPtr newdata = new CardInfoCacheEntry(p);
	ostringstream oss;	
	oss<<"[CardInfoCacheI::SetProperty] userId:"<<userId<<" ";
	for(Str2StrMap::const_iterator it=props.begin(); it!=props.end(); ++it){
		oss<<"["<<it->first<<" "<<it->second<<"]";
		if(it->first == "MOBILEPHONE"){
			newdata->set_mobile(it->second);
		}else if(it->first == "MSN"){
			newdata->set_msn(it->second);
		}else if(it->first == "QQ"){
			newdata->set_qq(it->second);
		}else if(it->first == "PROFILE_PRIVACY"){
			std::string privacy = parse.getJsonStr(it->second);
			newdata->set_privacy(privacy);
		}
	}
	MCE_INFO(oss.str());
	ServiceI::instance().addObject(newdata, CARD_INFO_CACHE, userId);	
}

//从Cache中取出一个用户的对象，并返回他对应的隐私
string CardInfoCacheI::getPrivacy(int userId, const Ice::Current& current){
	CardInfoCacheEntryPtr p = ServiceI::instance().locateObject<CardInfoCacheEntryPtr>(CARD_INFO_CACHE, userId);
	if(p){
		return p->get_privacy();
	}else
		return "";
}

//向Cache中更新用户隐私
bool CardInfoCacheI::setPrivacy(int userId, const string& privacy, const Ice::Current& current){
	CardInfoCacheEntryPtr p = ServiceI::instance().locateObject<CardInfoCacheEntryPtr>(CARD_INFO_CACHE, userId);//取出用户对象
	if(!p)
		return false;
	MCE_INFO("[CardInfoCacheI::SetPrivacy] userId:"<<userId<<" privacy:"<<privacy);
	CardInfoCacheEntryPtr newdata = new CardInfoCacheEntry(p);//调用set_privacy方法修改隐私
	newdata->set_privacy(privacy);
	ServiceI::instance().addObject(newdata, CARD_INFO_CACHE, userId);	//写入Cache
	return true;
}

//当有新增用户或用户修改名片时向DB和CardInfoCache中写入这些信息
bool CardInfoCacheI::set(const ::xce::cardcache::CardInfoSeq& CardDatas, const Ice::Current& current) {
  MCE_INFO("CardInfoCacheI::set -> CardDatas.size(): "<<CardDatas.size());
  if (CardDatas.empty()) {
    return false;
  }

  if (!CardDBAdapter::AddInfo(CardDatas)) {//调用写DB操作
    return false;
  }
  try {
    for(CardInfoSeq::const_iterator it=CardDatas.begin();it!=CardDatas.end();++it){//往UserContact同步数据
      UserContactInfoPtr contactPtr = new UserContactInfo;
      contactPtr->set_id((*it)->userId);
      contactPtr->set_msn((*it)->msn);
      contactPtr->set_mobilephone((*it)->mobile);
      if ((*it)->qq == "") {
        contactPtr->set_qq(0);
      } else {
        long qq_long = boost::lexical_cast<long>((*it)->qq);
        if (qq_long > INT_MAX) {
          contactPtr->set_qq(INT_MAX);
          (*it)->qq = "2147483647";
        } else {
          contactPtr->set_qq(boost::lexical_cast<int>((*it)->qq));
        }
      }
      MCE_INFO("UserContactAdapter userId:" << (*it)->userId << "; msn:" << (*it)->msn << "; mobile:" << (*it)->mobile << "; qq:" << (*it)->qq);
      UserContactAdapter::instance().setUserContact((*it)->userId, contactPtr);
    }
  } catch (exception& ex) {
    MCE_ERROR("CardInfoCacheI::set " << ex.what());
  }
  for (CardInfoSeq::const_iterator it = CardDatas.begin(); it != CardDatas.end(); ++it) {
	  TaskManager::instance().execute(new UpdateTask(*it, current));//调用更新Cache操作
  }

  return true;
}

//删除名片信息，预留出的接口
void CardInfoCacheI::remove(const ::MyUtil::IntSeq&, const Ice::Current&) {
  //Reserve Interface
}

/*
Int2Str2StrMap CardInfoCacheI::transform(const CardSetInfoSeq& CardDatas) {
  Int2Str2StrMap res;
  MyUtil::Str2StrMap str_res;
  Byte2StrMap byte_res;

  for (CardSetInfoSeq::const_iterator it = CardDatas.begin(); it != CardDatas.end(); ++it) {
    byte_res = (*it)->data;
    for (Byte2StrMap::const_iterator byte_it = byte_res.begin(); byte_it != byte_res.end(); ++byte_it) {
      Ice::Byte field = byte_it->first;    
      switch (field) {
        case FIELDADDRESS:
          str_res[ADDRESS] = byte_it->second;
          break;
        case FIELDQQ:
          str_res[QQ] = byte_it->second;
          break;
        case FIELDMOBILE:
          str_res[MOBILE] = byte_it->second;
          break;
        case FIELDEMAIL:
          str_res[EMAIL] = byte_it->second;
          break;
        case FIELDMSN:
          str_res[MSN] = byte_it->second;
          break;
        case FIELDEXTRA:
          str_res[EXTRA] = byte_it->second;
          break;
        case FIELDPRIVACY:
          str_res[PRIVACY] = boost::lexical_cast<unsigned char>(byte_it->second);
          break;
        default:
          MCE_WARN("UpdateTask::CopyData -> Invalid field: " << field);
      }
    }
    res[(*it)->userId] = str_res;
  }
  return res;
}
*/

//============CardInfoCacheFactory===============
Ice::ObjectPtr CardInfoCacheFactory::create(Ice::Long userId) {

  MCE_INFO("[CardInfoCacheFactory::create] userId:" << userId);

  CardInfoPtr info = new CardInfo;
  readCardInfo(userId,info);
  readUserContactInfo(userId,info);
  readPrivacy(userId,info);

  CardInfoCacheEntryPtr entry = new CardInfoCacheEntry(info);
  return entry;
}
/*
MyUtil::ObjectResultPtr CardInfoCacheFactory::create(const MyUtil::LongSeq& userIds) {

  MCE_INFO("CardInfoCacheFactory::create -> userIds.size(): " << userIds.size());

  ObjectResultPtr res = new ObjectResult();
  for (size_t i = 0; i < userIds.size(); ++i) {
    Ice::ObjectPtr obj = create(userIds.at(i));
    if (obj) {
      res->data[userIds.at(i)] = obj;
    }
  }
  return res;
}
*/
//从DB中读取名片信息
bool CardInfoCacheFactory::readCardInfo(int userId, CardInfoPtr& info) {

  MCE_INFO("[CardInfoCacheFactory::readCardInfo] userId:" << userId);

  Statement sql;
  sql << "SELECT address, email, extra FROM " << CARD_TABLE_NAME << " WHERE user_id = " << userId;
  size_t qsize = QueryRunner(DB_DESC_ADDRESSBOOK, CDbRServer).query(sql,LoadPersistentByCardInfoHandler(info));
  if (qsize != 1) {
	return false;
  }
  return true;
}

//从user_config中读取用户隐私
bool CardInfoCacheFactory::readPrivacy(int userId, CardInfoPtr& info){
		Statement sql;
		sql << "select profile_privacy from user_config where id = "<<userId;
		size_t qsize = QueryRunner("user_config_preloader", CDbRServer).query(sql, LoadProfilePrivacyHandler(info,privacy_));
		if(qsize != 1)
				return false;	
		return true;
}

//从DB中读取用户联系资料
bool CardInfoCacheFactory::readUserContactInfo(int userId, CardInfoPtr& info) {

  MCE_INFO("[CardInfoCacheFactory::readUserContactInfo] userId:" << userId);
  Statement sql;
  sql << "SELECT msn, mobilephone, qq FROM " << CONTACT_TABLE_NAME << " WHERE id = " << userId;
  size_t qsize = QueryRunner("user_contact_preloader", CDbRServer).query(sql, LoadPersistentByUserContactInfoHandler(info));
  if (qsize != 1) {
      return false;
  }
  return true;
}

//============CardInfoCacheEntry=================
CardInfoCacheEntry::CardInfoCacheEntry(const CardInfoCacheEntryPtr& entry) : 
  address_(entry->address_),  qq_(entry->qq_),
  mobile_(entry->mobile_), email_(entry->email_), msn_(entry->msn_),
  extra_(entry->extra_), privacy_(entry->privacy_) {
}

CardInfoCacheEntry::CardInfoCacheEntry(const CardInfoPtr& info) : 
  address_(info->address),  qq_(info->qq),
  mobile_(info->mobile), email_(info->email), msn_(info->msn),
  extra_(info->extra), privacy_(info->privacy) {
  } 

//================UpdateTask======================
//更新Cache操作
void UpdateTask::handle(){ 
//  CardInfoCacheEntryPtr olddata = ServiceI::instance().locateObject<CardInfoCacheEntryPtr>(CARD_INFO_CACHE ,data_->userId);
    MCE_INFO("UpdateTask::handle -> Need Reload ID: "<<data_->userId);
    CardInfoCacheEntryPtr newdata = new CardInfoCacheEntry(data_);
	CardInfoCacheEntryPtr olddata = ServiceI::instance().locateObject<CardInfoCacheEntryPtr>(CARD_INFO_CACHE, data_->userId);
	if(olddata){
		newdata->set_privacy(olddata->get_privacy());
    	ServiceI::instance().addObject(newdata, CARD_INFO_CACHE, data_->userId);
	}
}

void UpdateTask::UpdateData(CardInfoCacheEntryPtr& data, const CardInfoPtr& updateddata) {
  /*
  for (Byte2StrMap::const_iterator itr = updateddata.begin(); itr != updateddata.end(); ++itr) {                                
    Ice::Byte field = itr->first;
    switch (field) {
      case FIELDADDRESS:
        data->set_address(itr->second);
        break;
      case FIELDQQ:
        data->set_qq(itr->second);
        break;
      case FIELDMOBILE:
        data->set_mobile(itr->second);
        break;
      case FIELDEMAIL:
        data->set_email(itr->second);
        break;
      case FIELDMSN:
        data->set_msn(itr->second);
        break;
      case FIELDEXTRA:
        data->set_extra(itr->second);
        break;
      case FIELDPRIVACY:
        data->set_privacy(boost::lexical_cast<unsigned char>(itr->second));
        break;
      default:
        MCE_WARN("UpdateTask::CopyData -> Invalid field: "<<field);
    }
  }
  */
}

