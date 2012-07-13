#include <boost/lexical_cast.hpp>
#include "TemplateDataCollector.h"
#include "ServiceI.h"
#include "TicketAdapter.h"
#include "FeedMemcProxy/client/talk_cache_client.h"
#include "PhotoUrlTools.h"
#include "ScoreCacheAdapter.h"
#include "Date.h"
#include <iostream>
#include "UserUrlAdapter.h"

using namespace xce::adapter::userurl;
using namespace std;
using namespace xce::scorecache;
using namespace xce::scorecache::adapter;
using namespace com::xiaonei::talk::util;
using namespace xce::talk;
using namespace passport;
using namespace MyUtil;
using namespace talk::window;
using ctemplate::TemplateDictionary;

static bool checkTicket(int userid, const string& ticket) {
	int id = 0;
	try {
		id = TicketAdapter::instance().verifyTicket(ticket, "");
		MCE_DEBUG("get passport -> id: " << id << " " << ticket);
		if (id == userid) {
			return true;
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("ActiveUser::checkTicket-->TicketAdapter::verifyTicket-->" << userid << " " << e);
	}
	MCE_DEBUG("get passport -> err: userid:" << userid << " uid:" << id << " " << ticket);
	return false;
}



static int GetUserId(const Str2StrMap& cookies, const Str2StrMap& parameter) {
  int uid = 0;
  Str2StrMap::const_iterator it = cookies.find("id");
  if (it != cookies.end() && (!it->second.empty())) {
    try {
      uid = boost::lexical_cast<int>(it->second);
      return uid;
    } catch (...) {
      MCE_WARN("OnlineBuddyListCollector::fillData --> cast err, " << it->second);
    }
  }

  it = parameter.find("uid");
  if (it != parameter.end()) {
    try {
      uid = boost::lexical_cast<int>(it->second);
    } catch (std::exception& e) {
      MCE_WARN("OnlineBuddyListCollector::fillData --> cast wap uid err:"<<e.what());
    }
  }
  return uid;
}

static string GetTicket(const Str2StrMap& cookies, const Str2StrMap& parameter) {
  Str2StrMap::const_iterator it = cookies.find("t");
  if (it != cookies.end()) {
    return it->second;
  }
  it = parameter.find("t");
  if (it != parameter.end()) {
    return it->second;
  }
  return string();
}

static int GetTargetUserId(const Str2StrMap& parameter){
	int targetuser = -1;
	Str2StrMap::const_iterator it = parameter.find("targetuser");
	if(it != parameter.end()){
  	try {
      targetuser = boost::lexical_cast<int>(it->second);
    } catch (std::exception& e) {
      MCE_WARN("OnlineBuddyListCollector::fillData GetTargetUserId cast targetuser = " << it->second << "--> ERROR  err:"<<e.what());
			return -1;
    }
	}
	return targetuser;
}

bool StaticAdInfoCollector::fillData(const std::string& path, const Str2StrMap& cookies, const Str2StrMap& parameter, TemplateDictionary& dict) {
	return true;
}

bool UserInfoCollector::fillData(const std::string& path, const Str2StrMap& cookies, const Str2StrMap& parameter, TemplateDictionary& dict) {
	
	int uid = GetUserId(cookies, parameter);
	if (uid <= 0) {
		MCE_WARN("UserInfoCollector::fillData --> uid:" << uid << " path:" << path);
		return false;
	}
	string ticket = GetTicket(cookies, parameter);
  if (checkTicket(uid, ticket)) {
		dict.SetValue("userid", boost::lexical_cast<string>(uid));
		TalkUserPtr uPtr;
		try {
			uPtr = TalkCacheClient::instance().GetUserByIdWithLoad(uid);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserInfoCollector::fillData -->TalkCacheClient::GetUserByIdWithLoad-->err : " <<e);
		}
		if(!uPtr){
			return false;
		}
		UserUrlInfoPtr urlinfo;
		try{
			urlinfo = UserUrlAdapter::instance().getUserUrl(uid);
		}catch(Ice::Exception& e){
			MCE_WARN("UserInfoCollector::fillData-->UserUrlAdapter::getUserUrl-->uid:" << uid << " error:" << e);
		}
		if(!urlinfo){
			return false;
		}
		if(uPtr && urlinfo){
			string username = uPtr->name;
			string url = urlinfo->headUrl();
			string tinyurl = PhotoUrlHelper::instance().GetFullUrl(url);
			dict.SetValue("tinyurl", tinyurl);
			dict.SetValue("username", username);
		}
		try{
			ClientScoreDataN sd = ScoreCacheNAdapter::instance().getClientScoreDataN(uid);
			dict.SetValue("historyLoginDays", boost::lexical_cast<string>(sd.historyLoginDays));
			dict.SetValue("continueLoginDays", boost::lexical_cast<string>(sd.continueLoginDays));
			Date datetime((sd.lastLoginTime+0.0) / 1000.0);
			MCE_INFO("UserInfoCollector::fillData --> userid:" << uid << " lastLoginTime:" << (sd.lastLoginTime +0.0) / 1000.0 << " datetime:" << datetime.toDateTime());
			ostringstream os;
			if(datetime.month() <= 9){
				os << "0" << datetime.month() << "-";
			}else{
				os << datetime.month() << "-";
			}
			if(datetime.day() <= 9){
				os << "0" << datetime.day();
			}else{
				os << datetime.day();
			}os << " ";
			if(datetime.hour() <= 9){
				os << "0" << datetime.hour() << ":";
			}else{
				os << datetime.hour() << ":";
			}
			if(datetime.minute() <= 9){
				os << "0" << datetime.minute();
			}else{
				os << datetime.minute();
			}
			dict.SetValue("lastLoginTime", boost::lexical_cast<string>(os.str()));
			dict.SetValue("loginType", boost::lexical_cast<string>(sd.loginType));
			dict.SetValue("score", boost::lexical_cast<string>(sd.score));
			dict.SetValue("level", boost::lexical_cast<string>(sd.level));
			dict.SetValue("nextLevelScore", boost::lexical_cast<string>(sd.nextLevelScore));
			dict.SetValue("awardCount", boost::lexical_cast<string>(sd.awardCount));
			int vip = sd.vip;
			if(vip > 0){
				MCE_INFO("UserInfoCollector::fillData --> userid:" << uid << " vip:" << vip);
				dict.ShowSection("VIP_SEC");
				dict.SetValue("vip", boost::lexical_cast<string>(vip));
			}
			ostringstream oss;
			for(vector<string>::iterator it = sd.icons.begin(); it != sd.icons.end(); ++it){
				string level = (*it);
				size_t pos = level.find("-");
				if(string::npos ==pos){
					continue;
				}
				level.erase(pos,1);
				dict.AddSectionDictionary(level);
				oss << level << "(" << (*it) << ")  ,";
			}
			MCE_INFO("UserInfoCollector::fillData --> icons:" << oss.str());
		}catch(Ice::Exception& e) {
			MCE_WARN("UserInfoCollector::fillData --> call ScoreCache err : " << e);
		}
  } else {
    return false;
  }
}

