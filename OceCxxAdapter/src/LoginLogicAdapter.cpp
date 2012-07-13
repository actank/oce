#include "LoginLogicAdapter.h"

using login::AccountPtr;
using login::WebLoginLogicAdapter;
using login::WapLoginLogicAdapter;
using passport::AccountType;
using passport::AccountInfoPtr;
using passport::LoginLogicPrx;
using passport::BindedId;
using passport::Account;
using passport::Id;
using passport::Phone;

string login::Account::str() const {
  ostringstream oss;
  oss<<"id="<<id_<<";";
  oss<<"account="<<account_<<";";
  oss<<"success="<<success_<<";";
  oss<<"domain="<<domain_<<";";
  oss<<"status="<<status_<<";";
  oss<<"ticket="<<ticket_<<".";
  return oss.str();
}

LoginLogicPrx WebLoginLogicAdapter::getLoginLogic(int id) {
  return getProxy(id);
}

vector<AccountPtr> WebLoginLogicAdapter::get(const std::string& account, const std::vector<passport::AccountType>& types) {
  map<int, AccountPtr> result;
  for(vector<passport::AccountType>::const_iterator i = types.begin(); i != types.end(); ++i) {
    vector<AccountPtr> tempaccounts = get(account, *i);
    for (vector<AccountPtr>::iterator j = tempaccounts.begin(); j != tempaccounts.end(); ++j) {
      map<int, AccountPtr>::iterator aa = result.find((*j)->id());
      if (aa == result.end()) {
        result.insert(make_pair<int, AccountPtr>((*j)->id(), (*j)));
      }
    }
  }
  if (result.size() > 1) {
    result.erase(-1);
  }
  vector<AccountPtr> res;
  for (map<int, AccountPtr>::iterator ii = result.begin(); ii != result.end(); ++ii) {
    res.push_back(ii->second);
  }
  return res;
}

vector<AccountPtr> WebLoginLogicAdapter::get(const string& account, passport::AccountType type) {
  switch (type) {
    case passport::Account:
      return getAccount(account);
      break;
    case Phone:
      return getPhone(account);
      break;
    case Id:
      return getId(account);
      break;
    case BindedId:
      return getBindedId(account);
      break;
    default:
      return vector<AccountPtr>();
  }
}

std::vector<AccountPtr> WebLoginLogicAdapter::getBindedId(const std::string& account) {
  vector<AccountInfoPtr> raw;
  LoginLogicPrx prx = NULL;
  try {
    prx = getLoginLogic(rand() % 10);
    raw = prx->get(BindedId, account);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  vector<AccountPtr> result;
  for (vector<AccountInfoPtr>::iterator i = raw.begin(); i != raw.end(); ++i) {
    result.push_back(new Account(*i));
  }
  return result;
}

std::vector<AccountPtr> WebLoginLogicAdapter::getId(const std::string& account) {
  vector<AccountInfoPtr> raw;
  LoginLogicPrx prx = NULL;
  try {
    prx = getLoginLogic(rand() % 10);
    raw = prx->get(Id, account);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  vector<AccountPtr> result;
  for (vector<AccountInfoPtr>::iterator i = raw.begin(); i != raw.end(); ++i) {
    result.push_back(new Account(*i));
  }
  return result;
}


std::vector<AccountPtr> WebLoginLogicAdapter::getPhone(const std::string& account) {
  vector<AccountInfoPtr> raw;
  LoginLogicPrx prx = NULL;
  try {
    prx = getLoginLogic(rand() % 10);
    raw = prx->get(Phone, account);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  vector<AccountPtr> result;
  for (vector<AccountInfoPtr>::iterator i = raw.begin(); i != raw.end(); ++i) {
    result.push_back(new Account(*i));
  }
  return result;
}

std::vector<AccountPtr> WebLoginLogicAdapter::getAccount(const std::string& account) {
  vector<AccountInfoPtr> raw;
  LoginLogicPrx prx = NULL;
  try {
    prx = getLoginLogic(rand() % 10);
    raw = prx->get(passport::Account, account);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  vector<AccountPtr> result;
  for (vector<AccountInfoPtr>::iterator i = raw.begin(); i != raw.end(); ++i) {
    result.push_back(new Account(*i));
  }
  return result;
}

AccountPtr WebLoginLogicAdapter::login(const std::vector<AccountType>& types, const std::string& user, const std::string& password, const std::string& site, int expirePeriod, bool reuseTicket) {
  AccountInfoPtr buf = NULL;
  vector<AccountInfoPtr> accs;
  LoginLogicPrx prx = NULL;
  try {
    prx = getLoginLogic(rand()%10);
    accs = prx->loginDefaultAccount(types, user, password, site, expirePeriod, reuseTicket);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  for (vector<AccountInfoPtr>::iterator i = accs.begin(); i != accs.end(); ++i) {
    if ((*i)->success == 0) {
      if ((*i)->domain == site) {
        return new Account(*i);
      } else {
        buf = *i;
      }
    }
  }
  if (buf != 0) {
    return new Account(buf);
  } else {
    return new Account(accs[accs.size() - 1]);
  }
}

vector<AccountPtr> WapLoginLogicAdapter::get(const std::string& account, const std::vector<passport::AccountType>& types) {
  map<int, AccountPtr> result;
  for(vector<passport::AccountType>::const_iterator i = types.begin(); i != types.end(); ++i) {
    vector<AccountPtr> tempaccounts = get(account, *i);
    for (vector<AccountPtr>::iterator j = tempaccounts.begin(); j != tempaccounts.end(); ++j) {
      map<int, AccountPtr>::iterator aa = result.find((*j)->id());
      if (aa == result.end()) {
        result.insert(make_pair<int, AccountPtr>((*j)->id(), (*j)));
      }
    }
  }
  if (result.size() > 1) {
    result.erase(-1);
  }
  vector<AccountPtr> res;
  for (map<int, AccountPtr>::iterator ii = result.begin(); ii != result.end(); ++ii) {
    res.push_back(ii->second);
  }
  return res;
}

vector<AccountPtr> WapLoginLogicAdapter::get(const string& account, passport::AccountType type) {
  switch (type) {
    case passport::Account:
      return getAccount(account);
      break;
    case Phone:
      return getPhone(account);
      break;
    case Id:
      return getId(account);
      break;
    case BindedId:
      return getBindedId(account);
      break;
    default:
      return vector<AccountPtr>();
  }
}

std::vector<AccountPtr> WapLoginLogicAdapter::getBindedId(const std::string& account) {
  vector<AccountInfoPtr> raw;
  LoginLogicPrx prx = NULL;
  try {
    prx = getLoginLogic(rand() % 10);
    raw = prx->get(BindedId, account);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  vector<AccountPtr> result;
  for (vector<AccountInfoPtr>::iterator i = raw.begin(); i != raw.end(); ++i) {
    result.push_back(new Account(*i));
  }
  return result;
}

std::vector<AccountPtr> WapLoginLogicAdapter::getId(const std::string& account) {
  vector<AccountInfoPtr> raw;
  LoginLogicPrx prx = NULL;
  try {
    prx = getLoginLogic(rand() % 10);
    raw = prx->get(Id, account);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  vector<AccountPtr> result;
  for (vector<AccountInfoPtr>::iterator i = raw.begin(); i != raw.end(); ++i) {
    result.push_back(new Account(*i));
  }
  return result;
}


std::vector<AccountPtr> WapLoginLogicAdapter::getPhone(const std::string& account) {
  vector<AccountInfoPtr> raw;
  LoginLogicPrx prx = NULL;
  try {
    prx = getLoginLogic(rand() % 10);
    raw = prx->get(Phone, account);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  vector<AccountPtr> result;
  for (vector<AccountInfoPtr>::iterator i = raw.begin(); i != raw.end(); ++i) {
    result.push_back(new Account(*i));
  }
  return result;
}

std::vector<AccountPtr> WapLoginLogicAdapter::getAccount(const std::string& account) {
  vector<AccountInfoPtr> raw;
  LoginLogicPrx prx = NULL;
  try {
    prx = getLoginLogic(rand() % 10);
    raw = prx->get(passport::Account, account);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  vector<AccountPtr> result;
  for (vector<AccountInfoPtr>::iterator i = raw.begin(); i != raw.end(); ++i) {
    result.push_back(new Account(*i));
  }
  return result;
}

AccountPtr WapLoginLogicAdapter::login(const std::vector<AccountType>& types, const std::string& user, const std::string& password, const std::string& site, int expirePeriod, bool reuseTicket) {
  AccountInfoPtr buf = NULL;
  vector<AccountInfoPtr> accs;
  LoginLogicPrx prx = NULL;
  try {
    prx = getLoginLogic(rand() % 10);
    accs = prx->loginDefaultAccount(types, user, password, site, expirePeriod, true);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  for (vector<AccountInfoPtr>::iterator i = accs.begin(); i != accs.end(); ++i) {
    if ((*i)->success == 0) {
      if ((*i)->domain == site) {
        return new Account(*i);
      } else {
        buf = (*i);
      }
    }
  }
  if (buf != 0) {
    return new Account(buf);
  } else {
    return new Account(accs[accs.size() - 1]);
  }
}

LoginLogicPrx WapLoginLogicAdapter::getLoginLogic(int id) {
  return getProxy(id);
}
