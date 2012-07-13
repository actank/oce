#ifndef LOGIN_LOGIC_ADAPTER_H
#define LOGIN_LOGIC_ADAPTER_H

#include <AdapterI.h>
#include <Login.h>
#include <ServiceI.h>
#include <IceUtil/IceUtil.h>

namespace login {

class Account : public Ice::Object {
public:
  Account(int id, int success, const std::string& account, const std::string& domain, int status, const std::string& ticket) : id_(id), success_(success), account_(account), domain_(domain), status_(status), ticket_(ticket) {}
  Account(const passport::AccountInfoPtr& ai) : id_(ai->userId), success_(ai->success), account_(ai->account), domain_(ai->domain), status_(ai->status), ticket_(ai->ticket){}
  int id() const {return id_;}
  int success() const {return success_;}
  std::string account() const {return account_;}
  std::string domain() const {return domain_;}
  int status() const {return status_;}
  std::string ticket() const {return ticket_;}
  std::string str() const;
private:
  int id_;
  int success_;
  std::string account_;
  std::string domain_;
  int status_;
  std::string ticket_;
};

typedef IceUtil::Handle<Account> AccountPtr;

class AbstractLoginLogicAdapter : virtual public Ice::Object {
public:
  ~AbstractLoginLogicAdapter() {}
  virtual AccountPtr login(const std::vector<passport::AccountType>& types, const std::string& user, const std::string& password, const std::string& site, int expirePeriod, bool reuseTicket) = 0;
  virtual std::vector<AccountPtr> get(const std::string& account, const std::vector<passport::AccountType>& types) = 0;
  virtual std::vector<AccountPtr> get(const std::string& account, passport::AccountType type) = 0;
  virtual std::vector<AccountPtr> getAccount(const std::string& account) = 0;
  virtual std::vector<AccountPtr> getPhone(const std::string& account) = 0;
  virtual std::vector<AccountPtr> getId(const std::string& account) = 0;
  virtual std::vector<AccountPtr> getBindedId(const std::string& account) = 0;
};

class WebLoginLogicAdapter : virtual public AbstractLoginLogicAdapter, public MyUtil::ReplicatedClusterAdapterI<passport::LoginLogicPrx>,
  public MyUtil::Singleton<WebLoginLogicAdapter> {
public:
  WebLoginLogicAdapter() : MyUtil::ReplicatedClusterAdapterI<passport::LoginLogicPrx> ("ControllerLoginLogic",120,300) {}
  virtual AccountPtr login(const std::vector<passport::AccountType>& types, const std::string& user, const std::string& password, const std::string& site, int expirePeriod, bool reuseTicket);
  virtual std::vector<AccountPtr> get(const std::string& account, const std::vector<passport::AccountType>& types);
  virtual std::vector<AccountPtr> get(const std::string& account, passport::AccountType type);
  virtual std::vector<AccountPtr> getAccount(const std::string& account);
  virtual std::vector<AccountPtr> getPhone(const std::string& account);
  virtual std::vector<AccountPtr> getId(const std::string& account);
  virtual std::vector<AccountPtr> getBindedId(const std::string& account);
private:
  passport::LoginLogicPrx getLoginLogic(int id);
};

class WapLoginLogicAdapter : virtual public AbstractLoginLogicAdapter, public MyUtil::ReplicatedClusterAdapterI<passport::LoginLogicPrx>,
  public MyUtil::Singleton<WapLoginLogicAdapter> {
public:
  WapLoginLogicAdapter() : MyUtil::ReplicatedClusterAdapterI<passport::LoginLogicPrx> ("ControllerWapLoginLogic",120,300) {}
  virtual AccountPtr login(const std::vector<passport::AccountType>& types, const std::string& user, const std::string& password, const std::string& site, int expirePeriod, bool reuseTicket);
  virtual std::vector<AccountPtr> get(const std::string& account, const std::vector<passport::AccountType>& types);
  virtual std::vector<AccountPtr> get(const std::string& account, passport::AccountType type);
  virtual std::vector<AccountPtr> getAccount(const std::string& account);
  virtual std::vector<AccountPtr> getPhone(const std::string& account);
  virtual std::vector<AccountPtr> getId(const std::string& account);
  virtual std::vector<AccountPtr> getBindedId(const std::string& account);
private:
  passport::LoginLogicPrx getLoginLogic(int id);
};

}

#endif
