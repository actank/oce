#include <boost/lexical_cast.hpp>
#include <json/json.h>
#include <sstream>
#include <ctime>
#include "FeedEdmSenderI.h"
#include "MiscEdmSenderCache.h"
#include "MayKnowEdmSenderTask.h"
#include "FriendEdmSenderTask.h"
#include "UserCacheAdapter.h"
#include "AddFriendEdmSenderTask.h"
#include "FocusFriendEdmSenderTask.h"
#include "FeedEdmCacheAdapter.h"
#include "UserBaseAdapter.h"
#include "UserNetworkAdapter.h"
#include "FeedAssistantAdapter.h"
#include "FeedNewsAdapter.h"
#include "AdNotifyAdapter.h"
#include "LogWrapper.h"
#include "ip_area_cache.h"
#include "pugixml.hpp"
#include "Allocator.h"
#include "UserUrlAdapter.h"

void ::MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&xce::edm::FeedEdmSenderI::instance(),
      service.createIdentity("FEDM", ""));
  MCE_INFO("add FeedEdmSender end");
  TaskManager::instance().execute(new xce::edm::MiscEdmSenderCacheLoadTask());
  TaskManager::instance().execute(new xce::edm::FrequencyCacheLoadTask());
  TaskManager::instance().execute(new xce::ad::IpAreaCacheReloadTask());
  MCE_INFO("start sender");

}

namespace xce {
namespace edm {
using namespace xce::ad;
using namespace pugi;
using namespace xce::feed;
using namespace xce::usercache;
using namespace boost;
using namespace com::xiaonei::platform::storage;
using namespace xce::adapter::userurl;
FeedEdmSenderI::FeedEdmSenderI() {
}

void FeedEdmSenderI::ClearFrequencyCacheByUser(int userid, const Ice::Current& current){
	FrequencyCache::instance().ClearCacheByUser(userid);
}


void FeedEdmSenderI::Receive(int user_id, const MyUtil::Str2StrMap& params, const Ice::Current&){
	ProcessReceive(user_id, params);
}

void FeedEdmSenderI::NoCheckReceive(int user_id, const MyUtil::Str2StrMap& params, const Ice::Current& current){
	Str2StrMap::const_iterator it;
	int kind = -1;
	int v = 0;
	it = params.find("edmid");
	if(it != params.end()){
		kind = 1;
		try{
			v = boost::lexical_cast<int>(it->second);
		}catch(...){
		}
	}
	it = params.find("edmtype");
	if(it != params.end()){
		kind = 2;
		try{
			v = boost::lexical_cast<int>(it->second);
		}catch(...){
		}
	}
	if(kind > 0){
   	FeedSeedInfoPtr selected_edm;
		try {
			if(1 == kind){
				selected_edm = FeedEdmCacheAdapter::instance().SelectEdmById(v);
			}
			else if(2 == kind){
				selected_edm = FeedEdmCacheAdapter::instance().SelectEdmByType(user_id, v);
			}
		} catch(Ice::Exception& e) {
			MCE_WARN("call FeedEdmCache server error:" << e);
		}
		if (selected_edm != 0) { 
			FeedSeedPtr feed_seed = _BuildSeed(selected_edm,user_id);        
			MCE_INFO("feed_seed->xml = " << feed_seed->xml);
			xml_document_ptr d = new xml_document();
			if(d->load(feed_seed->xml.c_str())){
				MCE_INFO("OK");
			}
			if(12 == selected_edm->type){
				FeedAssistantAdapter::instance().dispatch(feed_seed); 
				MCE_INFO("call dispatch type = 12");
			}
			else{
				FeedAssistantAdapter::instance().dispatchEDM(feed_seed); 
			}
		} else {
			MCE_DEBUG("no edm selected");
		}
		MCE_INFO("for test userid = " << user_id << " kind = " << kind << " v = " << v);
		return;
	}
	else{
		ProcessReceive(user_id, params, false);
	}
}

void FeedEdmSenderI::ReceiveForPhone(int user_id, const MyUtil::Str2StrMap& params, const Ice::Current& current){
	{
		MyUtil::Str2StrMap::const_iterator ite = params.find("ip");
		if (ite == params.end()) {
			return;
		}
		MCE_INFO("FeedEdmSenderI::ReceiveForPhone --> Phone Login user_id:" << user_id << ",ip:" << ite->second << ",city:" << _GetAreaCode(ite->second) <<" of province:" << _GetAreaCode2P(ite->second));
	}
  if (_ValidateUser(user_id)) {
    int limit_check = 0;
		limit_check = _ValidateFrequencyLimit(user_id);
    MCE_INFO("FeedEdmSenderI::ProcessReceive --> userid = " << user_id << " limit_check:" << limit_check);
    if (limit_check != 0) {
      FeedUserInfoPtr user_info = _GetUserInfo(user_id, limit_check);
      MyUtil::Str2StrMap::const_iterator ite = params.find("ip");
      if (ite != params.end()) {
        user_info->city = _GetAreaCode(ite->second);
        user_info->province = _GetAreaCode2P(ite->second);
        MCE_DEBUG("FeedEdmSenderI::ReceiveForPhone --> get area :" << user_info->city << " of province:" << user_info->province << " from ip:" << ite->second);
      }
      FeedSeedInfoPtr selected_edm = 0;
      try {
        selected_edm = FeedEdmCacheAdapter::instance().SelectEdmForPhone(user_id, params, user_info);
      } catch(Ice::Exception& e) {
        MCE_WARN("FeedEdmSenderI::ReceiveForPhone --> call FeedEdmCache server error:" << e);
      }
      if (selected_edm != 0) { 
        FeedSeedPtr feed_seed = _BuildSeed(selected_edm,user_id);        
        //MCE_INFO("FeedEdmSenderI::ReceiveForPhone --> until get user info spend time:" << (end_cache.tv_sec - start_cache.tv_sec)* 10000000 + end_cache.tv_usec - start_cache.tv_usec);
        //MCE_INFO("FeedEdmSenderI::ReceiveForPhone --> send edm:" << selected_edm->configId << ",user_id:" << user_id << ",limit_type:" << selected_edm->limitType << ",ip:" << ite->second << ",city:" << user_info->city );
        FeedAssistantAdapter::instance().dispatchEDM(feed_seed); 
        //gettimeofday(&end_cache,NULL);
        //MCE_INFO("FeedEdmSenderI::ReceiveForPhone --> send edm spend time:" << (end_cache.tv_sec - start_cache.tv_sec)* 10000000 + end_cache.tv_usec - start_cache.tv_usec);
        if (selected_edm->limitType == 1) { 
          FrequencyCache::instance().Add(user_id, kWeekly);//增加到已发送用户cache
        } else {
          FrequencyCache::instance().Add(user_id, kDaily);
        }
      } else {
        MCE_DEBUG("FeedEdmSenderI::ReceiveForPhone --> no edm selected");
      }
    }
    _RemoveLogInFootPrint(user_id);
  }
  try {
    AdNotifierAdapter::instance().Login(user_id);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedEdmSenderI::ReceiveForPhone --> call AdNotifyAdapter failed, err->" << e);
  }
}

void FeedEdmSenderI::ProcessReceive(int user_id, const MyUtil::Str2StrMap& params, bool ischeck) {
  timeval start_cache,end_cache;
  {
    MyUtil::Str2StrMap::const_iterator ite = params.find("ip");
    if (ite == params.end()) {
      return;
    }
    MCE_INFO("Login user_id:" << user_id << ",ip:" << ite->second << ",city:" << _GetAreaCode(ite->second) <<" of province:" << _GetAreaCode2P(ite->second));
  }
  if(user_id < 0) {
    return;
  }
  srand(time(0));
  int random = rand() % 2;
  if(random == 0) {
    TaskManager::instance().execute(new AddFriendEdmSenderTask(user_id));
  } else {
    TaskManager::instance().execute(new FocusFriendEdmSenderTask(user_id));
  }
  TaskManager::instance().execute(new FriendEdmSenderTask(user_id));
  TaskManager::instance().execute(new MayKnowEdmSenderTask(user_id));
  if (_ValidateUser(user_id)) {
    gettimeofday(&start_cache,NULL);
    int limit_check = 0;
    // limit_check 1: kWeekly 2: kDaily
    /*if (user_id != 987915272 && user_id != 225092473 && user_id != 287727723 && user_id != 287727623) {
      limit_check = _ValidateFrequencyLimit(user_id);
    } else {
      limit_check = 1;
    }*/
		if(ischeck){
    	limit_check = _ValidateFrequencyLimit(user_id);
		}
		else{
			MyUtil::Str2StrMap::const_iterator lit = params.find("limit_check");
			if(params.end() == lit){
				return;
			}
			int v = 0;
			try{
				v = boost::lexical_cast<int>(lit->second);
			}catch(...){
			}
			limit_check = v;
		}
    MCE_INFO("FeedEdmSenderI::ProcessReceive --> userid = " << user_id << " limit_check:" << limit_check);
    if (limit_check != 0) {
      FeedUserInfoPtr user_info = _GetUserInfo(user_id, limit_check);
      gettimeofday(&end_cache,NULL);
      MyUtil::Str2StrMap::const_iterator ite = params.find("ip");
      if (ite != params.end()) {
        user_info->city = _GetAreaCode(ite->second);
        user_info->province = _GetAreaCode2P(ite->second);
        MCE_DEBUG("get area :" << user_info->city << " of province:" << user_info->province << " from ip:" << ite->second);
      }
      FeedSeedInfoPtr selected_edm = 0;
     /* try {
        FeedNewsAdapter::instance().getHotFeedData(user_id, 0, 1);
      } catch (Ice::Exception& e) {
        MCE_WARN("call FeedNewsAdapter failed, err->" << e);
      }*/
      try {
        selected_edm = FeedEdmCacheAdapter::instance().SelectEdm(user_id, params, user_info);
      } catch(Ice::Exception& e) {
        MCE_WARN("call FeedEdmCache server error:" << e);
      }
      if (selected_edm != 0) { 
        FeedSeedPtr feed_seed = _BuildSeed(selected_edm,user_id); 
        //MCE_INFO("until get user info spend time:" << (end_cache.tv_sec - start_cache.tv_sec)* 10000000 + end_cache.tv_usec - start_cache.tv_usec);
				ostringstream logos;
				logos << "FeedEdmSenderI::ProcessReceive --> send edm :" << selected_edm->configId << " user_id:" << user_id << " limit_type:" << selected_edm->limitType << " ip:" << ite->second << " city:" << user_info->city << " age:" << user_info->age << " stage:" << user_info->stage << " gender:" << user_info->gender << " ";
				for(FeedSchoolInfoSeq::iterator it = user_info->universitySeq.begin(); it != user_info->universitySeq.end(); ++it){
					logos << " university:" << (*it).id << "_" << (*it).enrollYear;
				}
				for(FeedSchoolInfoSeq::iterator it = user_info->highschoolSeq.begin(); it != user_info->highschoolSeq.end(); ++it){
					logos << " highschool:" << (*it).id << "_" << (*it).enrollYear;
				}
        MCE_INFO(logos.str());
				if(12 == selected_edm->type){
					FeedAssistantAdapter::instance().dispatch(feed_seed); 
					MCE_INFO("dispatch feed type = 12");
				}
				else{
					FeedAssistantAdapter::instance().dispatchEDM(feed_seed); 
				}
        gettimeofday(&end_cache,NULL);
        MCE_INFO("send edm spend time:" << (end_cache.tv_sec - start_cache.tv_sec)* 10000000 + end_cache.tv_usec - start_cache.tv_usec);
        if (selected_edm->limitType == 1) { 
          FrequencyCache::instance().Add(user_id, kWeekly);//增加到已发送用户cache
        } else {
          FrequencyCache::instance().Add(user_id, kDaily);
        }
      } else {
        MCE_DEBUG("no edm selected");
      }
    }
    _RemoveLogInFootPrint(user_id);
  }
  try {
    AdNotifierAdapter::instance().Login(user_id);
  } catch (Ice::Exception& e) {
    MCE_WARN("call AdNotifyAdapter failed, err->" << e);
  }
}
void FeedEdmSenderI::FlushCache(const Ice::Current& ) {
  FrequencyCache::instance().FlushCache(); 
}


FeedSeedPtr FeedEdmSenderI::_BuildSeed(const FeedSeedInfoPtr& info, int userid) {
  FeedSeedPtr feed_seed = new FeedSeed();
  int feed_type = 8000 + info->type;
  int feed_parent_type = 8000 + info->type;
  int type = feed_type&0xffff;
  type |= 1<<28;
  type |= 1<<30;
  type |= 0<<26;
  feed_seed->miniMerge = info->configId;
  feed_seed->newsMerge = info->configId;
  feed_seed->source = info->configId;
  feed_seed->psource = info->configId;
  feed_seed->actor = userid;
  feed_seed->type = type;
  feed_seed->ptype = feed_parent_type;
  feed_seed->time = (int)time(NULL); 
  feed_seed->baseWeight = 21;
  xml_document doc;
  xml_node f_node = doc.append_child();
  f_node.set_name("f");
  f_node.append_attribute("v") = "1.0";
  switch (feed_type) {
    case 8001:
      f_node.append_attribute("type") = "edmText";
      break;
    case 8002:
      f_node.append_attribute("type") = "edmPic";
      break;
    case 8003:
      f_node.append_attribute("type") = "edmVedio";
      break;
    case 8004:
      f_node.append_attribute("type") = "edmFlash";
      break;
    case 8012:
      f_node.append_attribute("type") = "edmPic";
      break;
    case 8013:
      f_node.append_attribute("type") = "edmPic";
      break;
    default:
      MCE_WARN("error edm type,edm_id:" <<info->configId);
  }
  xml_node time_node = f_node.append_child();
  time_node.set_name("time");
  std::string time_string = boost::lexical_cast<string>(feed_seed->time);
  MCE_DEBUG("time:" << feed_seed->time << ",string _time:" << time_string.c_str());
  time_node.append_child(node_pcdata).set_value(time_string.c_str());
  time_node.append_child(node_pcdata).set_value("000");
  xml_node type_node = f_node.append_child();
  type_node.set_name("type");
  std::string type_string = boost::lexical_cast<string>(feed_seed->type);
  type_node.append_child(node_pcdata).set_value(type_string.c_str());
  xml_node from_node = f_node.append_child();
  from_node.set_name("from");
  xml_node id_node = from_node.append_child();
  id_node.set_name("id");
  std::string id_string = boost::lexical_cast<string>(feed_seed->actor);
  id_node.append_child(node_pcdata).set_value(id_string.c_str());
  xml_node edm_node = f_node.append_child();
  edm_node.set_name("edm");
  xml_node logo_node = edm_node.append_child();
  logo_node.set_name("logo"); 
  logo_node.append_child(node_pcdata).set_value(info->logo.c_str());
  xml_node title_node = edm_node.append_child();
  title_node.set_name("title"); 
  title_node.append_child(node_pcdata).set_value(info->title.c_str());
  xml_node title_link_node = edm_node.append_child();
  title_link_node.set_name("titleLink");
  title_link_node.append_child(node_pcdata).set_value(info->url.c_str());
  xml_node body_node = edm_node.append_child();
  body_node.set_name("body");
  body_node.append_child(node_pcdata).set_value(info->body.c_str());
  xml_node body_link_node = edm_node.append_child();
  body_link_node.set_name("bodyLink");
  body_link_node.append_child(node_pcdata).set_value(info->bodyUrl.c_str());
  xml_node media_node = edm_node.append_child();
  media_node.set_name("media");
  xml_node media_src_node = media_node.append_child();
  media_src_node.set_name("src");
  xml_node media_url_node = media_node.append_child();
  media_url_node.set_name("url");
  Json::Reader reader;
  Json::Value json_value;
  bool parsingSuccessful;
  switch (feed_type) {
    case 8001:
      MCE_DEBUG("edm txt"); 
      break;
    case 8002: 
      {
        std::string media_pic = info->mediaSrc;
        if (media_pic == ""||media_pic == "NULL") {
          break;
        }
        parsingSuccessful = reader.parse(media_pic, json_value);
        if ( !parsingSuccessful ){
          MCE_WARN("Parse mesia info failed,edm_id:" << info->configId << ",mesia:" << media_pic);
        } else {
          media_src_node.append_child(node_pcdata).set_value(json_value["picSrc"].asString().c_str());
        }
      }
      break;
    case 8003:
      {
        std::string media_vedio = info->mediaSrc;
        if (media_vedio == ""||media_vedio == "NULL") {
          break;
        }
        parsingSuccessful = reader.parse(media_vedio, json_value);
        if ( !parsingSuccessful )
        {
          MCE_WARN("Parse mesia info failed,edm_id:" << info->configId << ",mesia:" << media_vedio);
        } else {
          media_src_node.append_child(node_pcdata).set_value(json_value["videoSrc"].asString().c_str());
          media_url_node.append_child(node_pcdata).set_value(json_value["videoPicSrc"].asString().c_str());
        }
      } 
      break;
    case 8004:
      {
        std::string media_flash = info->mediaSrc;
        if (media_flash == ""||media_flash == "NULL") {
          break;
        }
        parsingSuccessful = reader.parse(media_flash, json_value);
        if ( !parsingSuccessful )
        {
          MCE_WARN("Parse mesia info failed,edm_id:" << info->configId << ",mesia:" << media_flash);
        } else {
          media_src_node.append_child(node_pcdata).set_value(json_value["smallFlashSrc"].asString().c_str());
          media_url_node.append_child(node_pcdata).set_value(json_value["bigFlashSrc"].asString().c_str());
        }
      }
      break;
    case 8012: 
      {
        std::string media_pic = info->mediaSrc;
        if (media_pic == ""||media_pic == "NULL") {
          break;
        }
        parsingSuccessful = reader.parse(media_pic, json_value);
        if ( !parsingSuccessful ){
          MCE_WARN("Parse mesia info failed,edm_id:" << info->configId << ",mesia:" << media_pic);
        } else {
          media_src_node.append_child(node_pcdata).set_value(json_value["picSrc"].asString().c_str());
        }
      }
      break;
    case 8013: 
      {
        std::string media_pic = info->mediaSrc;
					MCE_INFO("8013   json =  " << media_pic);
        if (media_pic == ""||media_pic == "NULL") {
          break;
        }
        parsingSuccessful = reader.parse(media_pic, json_value);
        if ( !parsingSuccessful ){
          MCE_WARN("Parse mesia info failed,edm_id:" << info->configId << ",mesia:" << media_pic);
        } else {
					//-----------------------------
					xml_node flash_node = media_node.append_child();
					flash_node.set_name("flash");
					xml_node f_width_node = flash_node.append_child();
					f_width_node.set_name("width");
					xml_node f_height_node = flash_node.append_child();
					f_height_node.set_name("height");
					xml_node f_url_node = flash_node.append_child();
					f_url_node.set_name("url");
					//-----------------------------
          media_src_node.append_child(node_pcdata).set_value(json_value["picSrc"].asString().c_str());
					f_width_node.append_child(node_pcdata).set_value(json_value["flashWeigth"].asString().c_str());
					f_height_node.append_child(node_pcdata).set_value(json_value["flashHeight"].asString().c_str());
					f_url_node.append_child(node_pcdata).set_value(json_value["flashSrc"].asString().c_str());
        }
      }
      break;
    default:
      MCE_WARN("error edm type,edm_id:" <<info->configId);
  } 
  if ( feed_type == 8012) {
      xml_node friend_node = f_node.append_child();
      friend_node.set_name("fl");
      std::map<int, UserCachePtr> firends_info= UserCacheAdapter::instance().GetUserCache(info->friendId);
			MCE_INFO("FeedEdmSenderI::_BuildSeed --> firends_info.size = " << firends_info.size() << " info->friendId.size = " << info->friendId.size());
      for (std::map<int, UserCachePtr>::iterator ite  = firends_info.begin(); ite != firends_info.end(); ++ite) {
				xml_node friend_list_node = friend_node.append_child();
				friend_list_node.set_name("friend");
        xml_node friend_id_node = friend_list_node.append_child();
        friend_id_node.set_name("id");
        friend_id_node.append_child(node_pcdata).set_value(boost::lexical_cast<string>(ite->second->id()).c_str());
        xml_node friend_name_node = friend_list_node.append_child();
        friend_name_node.set_name("name");
        friend_name_node.append_child(node_pcdata).set_value(ite->second->name().c_str());
        if (info->displayFriendType != -1) {
          xml_node friend_src_node = friend_list_node.append_child();
          friend_src_node.set_name("src");
					//UserUrlInfoPtr obj = UserUrlAdapter::instance().getUserUrl(ite->second->id());
					string tinyurl = HeadUrlUtil::instance().getHeadFullUrl(ite->second->tinyurl());
          friend_src_node.append_child(node_pcdata).set_value(tinyurl.c_str());
        }
      }
      xml_node  action_name_node = edm_node.append_child();
      action_name_node.set_name("f_edm_action_name");
      action_name_node.append_child(node_pcdata).set_value(info->socialTitle.c_str());
      xml_node action_url_node = edm_node.append_child();
      action_url_node.set_name("f_edm_action_url");
      action_url_node.append_child(node_pcdata).set_value(info->url.c_str());
  }
  if (info->likePageLink != "") {
    xml_node like_page_link_node = edm_node.append_child();
    like_page_link_node.set_name("page_action_url");
    like_page_link_node.append_child(node_pcdata).set_value(info->likePageLink.c_str());
  }
	
  xml_node like_name_node = edm_node.append_child();
  like_name_node.set_name("page_action_name");
  like_name_node.append_child(node_pcdata).set_value(info->likeName.c_str());
  xml_node outer_url_node = edm_node.append_child();
  outer_url_node.set_name("outerUrl");
  xml_node config_node = edm_node.append_child();
  config_node.set_name("config");
  config_node.append_child(node_pcdata).set_value(boost::lexical_cast<string>(info->configId).c_str());
  xml_node user_node = edm_node.append_child();
  user_node.set_name("user");
  xml_node user_id_node = user_node.append_child();
  user_id_node.set_name("id");
  std::string user_id_string = boost::lexical_cast<string>(feed_seed->actor);
  user_id_node.append_child(node_pcdata).set_value(user_id_string.c_str());
  xml_node user_name_node = user_node.append_child();
  user_name_node.set_name("name");
  xml_node reply_node = edm_node.append_child();
  reply_node.set_name("reply");
  xml_node reply_type_node = reply_node.append_child();
  reply_type_node.set_name("type");
  reply_type_node.append_child(node_pcdata).set_value("edm");
  xml_node interactive_node = edm_node.append_child();
  interactive_node.set_name("interactive");
  interactive_node.append_child(node_pcdata).set_value(boost::lexical_cast<string>(info->interactiveFlag).c_str());
  feed_seed->xml = doc.xml();
  std::ostringstream expression;
  expression << "u(" << feed_seed->actor << ")";
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("base_weight_id",boost::lexical_cast<string>(feed_seed->baseWeight).c_str()));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("expr",expression.str()));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("publish_mini","0"));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("publish_news","1"));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("send_config","0"));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("update_time","1"));
  return feed_seed;
}
std::string FeedEdmSenderI::_GetAreaCode(const std::string& ip) {
  std::string city = IpAreaCache::instance().GetIpArea(ip);
  MCE_DEBUG("ip:" << ip << ",city:" << city);
  return city;
}
std::string FeedEdmSenderI::_GetAreaCode2P(const std::string& ip) {
  std::string province = IpAreaCache::instance().GetIpArea2P(ip);
  MCE_DEBUG("ip:" << ip << ",province:" << province);
  return province;
}

FeedUserInfoPtr FeedEdmSenderI::_GetUserInfo(Ice::Int user_id, int limit_check) {
  FeedUserInfoPtr user = new FeedUserInfo;
  user->userId = user_id;
  user->limitCheck = limit_check;
  ::xce::adapter::userbase::UserBaseFullViewPtr user_base_info_ptr;
  try {
    user_base_info_ptr = ::xce::adapter::userbase::UserBaseAdapter::instance().getUserBaseFullView(user_id);
  } catch (std::exception& e) {
    MCE_WARN("UserBaseAdapter error,can't get user base common view,user id = " + user_id);
  }
  if (user_base_info_ptr == 0) {
    return user;
  }
  if (user_base_info_ptr->gender() == "女生") {
    user->gender = 0;
  } else if (user_base_info_ptr->gender() == "男生") {
    user->gender = 1;
  } else {
    user->gender = -1;
  }
  time_t now = time(NULL);
  tm now_tm;
  localtime_r(&now,&now_tm);
  MCE_DEBUG("now year:" << now_tm.tm_year << ",user birth year:" << user_base_info_ptr->birthyear());
  user->age = now_tm.tm_year + 1900 - user_base_info_ptr->birthyear();
  user->stage = user_base_info_ptr->stage();
  MCE_DEBUG("stage:" << user->stage);
  MCE_DEBUG("userid:" << user_id << ",birth year:" <<  user_base_info_ptr->birthyear());
  mop::hi::svc::model::NetworkBigObjectNPtr big_object_ptr;
  try {
    big_object_ptr = mop::hi::svc::adapter::UserNetworkAdapter::instance().getBigObjectN(user_id);
  } catch (Ice::Exception& e) {
    MCE_WARN("call UserNetworkAdapter server error!");
    return user;
  }
  try {
    ::std::vector<FeedSchoolInfo> university_seq_temp;
    for(mop::hi::svc::model::UniversityInfoSeq::iterator ite = big_object_ptr->universitys.begin(); ite != big_object_ptr->universitys.end(); ++ite) {
      FeedSchoolInfo school_info;
      school_info.id = (*ite)->universityId;
      school_info.enrollYear = (*ite)->enrollYear; 
      university_seq_temp.push_back(school_info);
      MCE_DEBUG("univ_id:" << (*ite)->universityId << ",enroll_year:" << (*ite)->enrollYear);
    }
    user->universitySeq = university_seq_temp;
    std::vector<FeedSchoolInfo> hight_seq_temp;
    for(::mop::hi::svc::model::HighSchoolInfoSeq::iterator ite = big_object_ptr->highschools.begin(); ite != big_object_ptr->highschools.end(); ++ite) {
      FeedSchoolInfo school_info;
      school_info.id = (*ite)->HighSchoolId;
      school_info.enrollYear = (*ite)->enrollYear;
      hight_seq_temp.push_back(school_info);
      MCE_DEBUG("high_id:" << (*ite)->HighSchoolId << ",enroll_year:" << (*ite)->enrollYear);
    }
    user->highschoolSeq = hight_seq_temp;
  } catch (Ice::Exception& e) {
    MCE_WARN("call UserNetworkAdapter return null handle error!");
  }
  return user;
}


void FeedEdmSenderI::_RemoveLogInFootPrint(int user_id) {
  // 发送结束，删除用户登录记录，允许下次访问
    ValidateUserPtr validate_user_ptr = new ValidateUser();
    validate_user_ptr->RemoveFromDuplicateCache(user_id);
}

bool FeedEdmSenderI::_ValidateUser(Ice::Int user_id) {
  if (user_id < 0) {
    return false;
  }
  ValidateUserPtr validate_user_ptr = new ValidateUser();
  bool duplicate = validate_user_ptr->CheckDuplicateAndInsert(user_id);
  if (duplicate) {
    return false;
  }
  bool valid = validate_user_ptr->CheckUserStatus(user_id);
  if (!valid) {
    validate_user_ptr->RemoveFromDuplicateCache(user_id);
  }
  return  valid;
}

int FeedEdmSenderI::_ValidateFrequencyLimit(Ice::Int user_id) {
  if (user_id < 0) {
    return 0;
  }
  return FrequencyCache::instance().CheckFrequencyLimit(user_id);
}


}
}
