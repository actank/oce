#ifndef __LoginCacheI_h__
#define __LoginCacheI_h__

#include <Login.h>
#include "DbCxxPool/src/QueryRunner.h"
#include <boost/shared_ptr.hpp>
#include <queue>
#include <map>
#include <set>
#include <string>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceUtil/Mutex.h>
#include "OceCxxAdapter/src/LoginCacheAdapter.h"
#include "OceCxxAdapter/src/UserPassportAdapter.h"
#include "OceCxxAdapter/src/XceLoggerAdapter.h"
#include <util/cpp/InvokeMonitor.h>

namespace passport {
int CACHE_WEB = 1;
int CACHE_WAP = 2;
int CACHE_MOBILE_CLIENT = 3;
const string MANAGER = "M";

class LoginLogicI;

class DoLoginVirtualTask : public virtual MyUtil::Task {
public:
	DoLoginVirtualTask(LoginLogicI& p_manager, const ::passport::AMD_LoginLogic_loginVirtualPtr& p_callback,
                const ::passport::AccountTypeSeq& p_ats,
                const ::std::string& p_account,
                const ::std::string& p_pwd,
                const ::std::string& p_site,
                ::Ice::Int p_second,
                bool p_reuse_ticket,
                const MyUtil::InvokeClient& client)
	: manager(p_manager), callback(p_callback), ats(p_ats), account(p_account),
	 pwd(p_pwd), site(p_site), second(p_second), 
	 reuse_ticket(p_reuse_ticket), _client(client) {
	}
	virtual void handle();
private:
	void changeIfVirtual(const ::passport::AccountInfoPtr& accountInfo);
	bool getVirtualIdIfLastLogin(const int userId, long& switchTime, int& virtualId);
private:
	LoginLogicI& manager;
	::passport::AMD_LoginLogic_loginVirtualPtr callback;
	::passport::AccountTypeSeq ats;
	::std::string account;
	::std::string pwd;
	::std::string site;
	::Ice::Int second;
	bool reuse_ticket;
  MyUtil::InvokeClient _client;
};

class DoLoginDefaultAccountTask : public virtual MyUtil::Task {
public:
	DoLoginDefaultAccountTask(LoginLogicI& p_manager, const ::passport::AMD_LoginLogic_loginDefaultAccountPtr& p_callback,
                const ::passport::AccountTypeSeq& p_ats,
                const ::std::string& p_account,
                const ::std::string& p_pwd,
                const ::std::string& p_site,
                ::Ice::Int p_second,
                bool p_reuse_ticket,
                const MyUtil::InvokeClient& client)
	: manager(p_manager), callback(p_callback), ats(p_ats), account(p_account),
	 pwd(p_pwd), site(p_site), second(p_second), 
	 reuse_ticket(p_reuse_ticket), _client(client) {
	}
	virtual void handle();

private:
	LoginLogicI& manager;
	::passport::AMD_LoginLogic_loginDefaultAccountPtr callback;
	::passport::AccountTypeSeq ats;
	::std::string account;
	::std::string pwd;
	::std::string site;
	::Ice::Int second;
	bool reuse_ticket;
  MyUtil::InvokeClient _client;
};

class DoLoginTask : public virtual MyUtil::Task {
public:
	DoLoginTask(LoginLogicI& p_manager, const ::passport::AMD_LoginLogic_loginPtr& p_callback,
                const ::passport::AccountTypeSeq& p_ats,
                const ::std::string& p_account,
                const ::std::string& p_pwd,
                const ::std::string& p_site,
                ::Ice::Int p_second,
                bool p_reuse_ticket,
                const MyUtil::InvokeClient& client)
	: manager(p_manager), callback(p_callback), ats(p_ats), account(p_account),
	 pwd(p_pwd), site(p_site), second(p_second), 
	 reuse_ticket(p_reuse_ticket), _client(client) {
	}
	virtual void handle();

private:
	LoginLogicI& manager;
	::passport::AMD_LoginLogic_loginPtr callback;
	::passport::AccountTypeSeq ats;
	::std::string account;
	::std::string pwd;
	::std::string site;
	::Ice::Int second;
	bool reuse_ticket;
  MyUtil::InvokeClient _client;
};

class LoginLogicI: public LoginLogic {
friend class DoLoginTask;
friend class DoLoginDefaultAccountTask;
friend class DoLoginVirtualTask;
public:
	LoginLogicI();
	void initialize(int cache_type, std::string db_passport);
	/*
	 virtual AccountInfoSeq login(const AccountTypeSeq&, const string& account,
			const string& pwd, const string& site, int second, bool,
			const Ice::Current& = Ice::Current());
	*/
	virtual void login_async(const ::passport::AMD_LoginLogic_loginPtr&,
		const ::passport::AccountTypeSeq&,
		const ::std::string&,
		const ::std::string&,
		const ::std::string&,
		::Ice::Int,
		bool, const ::Ice::Current& = ::Ice::Current());

	virtual void loginDefaultAccount_async(const ::passport::AMD_LoginLogic_loginDefaultAccountPtr&,
		const ::passport::AccountTypeSeq&,
		const ::std::string&,
		const ::std::string&,
		const ::std::string&,
		::Ice::Int,
		bool, const ::Ice::Current& = ::Ice::Current());
	
	virtual void loginVirtual_async(const ::passport::AMD_LoginLogic_loginVirtualPtr&,
		const ::passport::AccountTypeSeq&,
		const ::std::string&,
		const ::std::string&,
		const ::std::string&,
		::Ice::Int,
		bool, const ::Ice::Current& = ::Ice::Current());

	virtual std::string createTicket(::Ice::Int id, ::Ice::Int expire, bool reuse, const Ice::Current&);
	virtual bool has(::passport::AccountType, const ::std::string&,
			const Ice::Current&);
	virtual bool hasWithTypes(const ::passport::AccountTypeSeq&, const ::std::string&, const Ice::Current&);
	virtual AccountInfoSeq get(::passport::AccountType, const ::std::string&,
			const Ice::Current&);
	virtual AccountInfoSeq getWithTypes(const ::passport::AccountTypeSeq&, const ::std::string&, const Ice::Current&);
	virtual AccountInfoSeq getById(::Ice::Int id, const Ice::Current&);
	virtual AccountInfoPtr id2Account(::Ice::Int id, const Ice::Current& =
			Ice::Current());
  virtual AccountInfoPtr switchAccount(::Ice::Int id, const string& pwd, bool reuse, const Ice::Current& = Ice::Current());
  virtual AccountInfoPtr switchAccountIncVirtualId(::Ice::Int souceId, ::Ice::Int desId, const string& desPwd, bool reuse, const Ice::Current& = Ice::Current());

  virtual bool bind(int id, const std::string& account, const Ice::Current& = Ice::Current());
  virtual bool unbind(int id, const std::string& account, const Ice::Current& = Ice::Current());
  virtual bool exist(const std::string& name, const Ice::Current& = Ice::Current());
private:
	string generateTicket(int, const string&, const string&, const string&);
	int loadUserIdByAccount(const string&, const string&, AccountInfoSeq&);
	int loadUserIdByDefaultAccount(const string&, const string&, AccountInfoSeq&);
	int loadUserIdById(const string&, const string&, AccountInfoSeq&);
	int loadUserIdById(int, const string&, AccountInfoSeq&);
	int loadUserIdByPhone(const string&, const string&, AccountInfoSeq&);
	int loadUserIdByNickName(const string&, const string&, AccountInfoSeq&);
	bool only_num(const string&) const;
	bool runExecSeq(const string&, const string&, const AccountTypeSeq&,
			AccountInfoSeq&, bool);
	bool runExecSeqWithDefaultAccount(const string&, const string&, const AccountTypeSeq&,
			AccountInfoSeq&, bool);
	void addLog(const string& account, const string& pwd, const string& site,
                int second, bool reuse_ticket, const AccountInfoSeq& ai) const;
  void updateSwitchTime(int userid) const;
  void updateVirtualIdSwitchTime(int virtualId, bool isLastLogin) const;
	//int loadUserId(const string&,const string&);
	std::string _suffix;
	IceUtil::Handle<passport::AbstractLoginCacheAdapter> _cache_adapter;
	std::string _db_passport;

	com::xiaonei::service::XceLoggerPtr logger_;
	std::vector<std::string> category_;

};
typedef IceUtil::Handle<LoginLogicI> LoginLogicIPtr;

class LoadUserIdByAccountHandler: virtual public com::xiaonei::xce::ResultHandler {
public:
	LoadUserIdByAccountHandler(AccountInfoSeq& ais, const std::string& pwd) :
		_ais(ais), _pwd(pwd) {
	}
protected:
	virtual bool handle(mysqlpp::Row& res) const;
private:
	AccountInfoSeq& _ais;
	const std::string& _pwd;
};

class LoadUserIdByAliasHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
  LoadUserIdByAliasHandler() {}
protected:
  virtual bool handle(mysqlpp::Row& res) const {return true;}
};

class LoadUserIdByDefaultAccountHandler: virtual public com::xiaonei::xce::ResultHandler {
public:
	LoadUserIdByDefaultAccountHandler(map<int, long>& accounts) :
		_accounts(accounts) {
	}
protected:
	virtual bool handle(mysqlpp::Row& res) const;
private:
	map<int, long>& _accounts;
};

class LoadUserIdByIdHandler: virtual public com::xiaonei::xce::ResultHandler {
public:
	LoadUserIdByIdHandler(AccountInfoSeq& ais, const std::string& pwd) :
		_ais(ais), _pwd(pwd) {
	}
protected:
	virtual bool handle(mysqlpp::Row& res) const;
private:
	AccountInfoSeq& _ais;
	const std::string& _pwd;
};

class LoadUserIdByPhoneHandler: virtual public com::xiaonei::xce::ResultHandler {
public:
	LoadUserIdByPhoneHandler(int& userId, bool& exist) :
		_userId(userId), _exist(exist) {
	}
protected:
	virtual bool handle(mysqlpp::Row& res) const {
		if ((int) res["status"] == 1){
			_exist = true;
			_userId = (int) res["userId"];
		}
		return true;
	}
private:
	int& _userId;
	bool& _exist;
};
class LoadUserIdByNickNameHandler: virtual public com::xiaonei::xce::ResultHandler {
public:
	LoadUserIdByNickNameHandler(int& userId) :
		_userId(userId) {
	}
protected:
	virtual bool handle(mysqlpp::Row& res) const {
		_userId = (int) res["user_id"];
		return true;
	}
private:
	int& _userId;
};

class LoadPersistentByTicketHandler: virtual public com::xiaonei::xce::ResultHandler {
public:
	LoadPersistentByTicketHandler(int& id) :
		_id(id) {
	}
	;
protected:
	virtual bool handle(mysqlpp::Row& res) const {
		_id = (int) res["userid"];
		return true;
	}
private:
	int& _id;
};

class LoadPersistentByIdHandler: virtual public com::xiaonei::xce::ResultHandler {
public:
	LoadPersistentByIdHandler(std::string& ticket) :
		_ticket(ticket) {
	}
protected:
	virtual bool handle(mysqlpp::Row& res) const {
		_ticket = res["ticket"].c_str();
		return true;
	}
private:
	string& _ticket;
};

class LoadVirtualIdHandler: virtual public com::xiaonei::xce::ResultHandler {
public:
	LoadVirtualIdHandler(int& virtualId, long& switchTime, bool& isLastLogin) :
		_virtualId(virtualId), _switchTime(switchTime), _isLastLogin(isLastLogin) {
	}
protected:
	virtual bool handle(mysqlpp::Row& res) const {
		long switchTime = (time_t)mysqlpp::DateTime(res["switch_time"]);
		if(switchTime > _switchTime){
			_switchTime = switchTime;
			_virtualId = (int)res["virtual_id"];
			_isLastLogin = (bool)res["is_last_login"];
		}else if(switchTime == _switchTime){
			bool isLastLogin = (bool)res["is_last_login"];
			if(isLastLogin){
				_isLastLogin = isLastLogin;
				_virtualId = (int)res["virtual_id"];
			}
		}
		return true;
	}
private:
	int& _virtualId;
	long& _switchTime;
	bool& _isLastLogin;
};

}
#endif
