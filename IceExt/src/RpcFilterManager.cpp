#include "RpcFilterManager.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "ServiceI.h"
#include <XceStormAdapter.h>

using MyUtil::RpcFilterI;
using MyUtil::PassportControlTypeHandler;
using MyUtil::PassportControlHandler;
using MyUtil::RpcFilterManager;
using MyUtil::RpcFilterObserver;
using MyUtil::RpcFilterReloadTask;
using Ice::Current;
using com::xiaonei::xce::Statement;
using com::xiaonei::xce::CDbRServer;
using com::xiaonei::xce::QueryRunner;
using xce::storm::XceStormAdapter;

void RpcFilterReloadTask::handle() {
  filter_->reload();
}

bool RpcFilterObserver::update(const MyUtil::Str2StrMap& context, const Ice::Current& e) {
  MCE_INFO("RpcFilterObserver::update type: " << filter_->type());
  TaskManager::instance().execute(new RpcFilterReloadTask(filter_));
  return true;
}

void RpcFilterI::reload(const Ice::Current& current) {
  {
    bool flag = false;
    PassportControlTypeHandler handler(flag, type_);
    Statement sql;
    sql << "SELECT type FROM passport_control_type WHERE type=" << mysqlpp::quote << type_;
    QueryRunner("admin_passport_control", CDbRServer).query(sql, handler);
    flag_ = flag;
  }

  {
    vector<string> ips;
    vector<string> fields;
    __gnu_cxx::hash_map<string, vector<string> > ip_field_list;
    PassportControlHandler handler(ips, fields);
    Statement sql;
    sql << "SELECT inet_ntoa(ip), visit_field FROM passport_control";
    QueryRunner("admin_passport_control", CDbRServer).query(sql, handler);
    for(vector<string>::iterator pi = ips.begin(), fi = fields.begin(); pi != ips.end() && fi != fields.end(); ++pi, ++fi) {
      vector<string> parts;
      boost::algorithm::split(parts, (*fi), boost::algorithm::is_any_of(":") );
      for (vector<string>::iterator si = parts.begin(); si != parts.end(); ++si) {
        ip_field_list[*pi].push_back((*si));
      }
      sort(ip_field_list[*pi].begin(), ip_field_list[*pi].end());
    }
    {
      IceUtil::RWRecMutex::WLock lock(mutex_);
      ip_field_list.swap(ip_field_list_);
    }
  }
}

void RpcFilterI::run() {
  while (true) {
    try {
      reload();
    } catch (const Ice::Exception& e) {
      MCE_WARN("RpcFilterI::run " << e);
    } catch (const std::exception& e) {
      MCE_WARN("RpcFilterI::run " << e.what());
    } catch (...) {
      MCE_WARN("RpcFilterI::run unknown");
    }
    try {
      sleep(600);
    } catch (const Ice::Exception& e) {
      MCE_WARN("RpcFilterI::run " << e);
    } catch (const std::exception& e) {
      MCE_WARN("RpcFilterI::run " << e.what());
    } catch (...) {
      MCE_WARN("RpcFilterI::run unknown");
    }
  }
}

bool RpcFilterI::isValid(const Ice::Current& c) {
  return flag_;
}

bool RpcFilterI::filter(const string& ip, const string& type) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  __gnu_cxx::hash_map<std::string, vector<string> >::iterator i = ip_field_list_.find(ip);
  if (i == ip_field_list_.end()) {
    MCE_WARN("RpcFilterI::filter ip: " << ip << " type: " << type << " ip not in list");
    return false;
  }
  if (binary_search(i->second.begin(), i->second.end(), type)) {
    return true;
  }
  MCE_WARN("RpcFilterI::filter ip: " << ip << " type: " << type << " type not in list");
  return false;
}

bool PassportControlTypeHandler::handle(mysqlpp::Row& res) const {
    flag_ = true;
    return false;
}

bool PassportControlHandler::handle(mysqlpp::Row& res) const {
  ips_.push_back((string)res["inet_ntoa(ip)"]);
  fields_.push_back((string)res["visit_field"]);
  return true;
}

void RpcFilterManager::initialize(const string& type) {
  filter_ = new RpcFilterI(type);
  ServiceI::instance().getAdapter()->add(filter_, ServiceI::instance().getCommunicator()->stringToIdentity("RF"));
  XceStormAdapter::instance().subscribe("PASSPORT_CONTROL", new RpcFilterObserver(filter_));
  TaskManager::instance().config(TASK_LEVEL_PASSPORT_CONTROL, MyUtil::ThreadPoolConfig(1, 10));
}

bool RpcFilterManager::filter(const Ice::Current& current, const std::string& type) {
  if (!filter_->isValid()) {
    return true;
  }
  string ip2port = current.con->toString().substr(current.con->toString().find("remote",25) + 17);
  vector<string> parts;
  boost::algorithm::split(parts, ip2port, boost::algorithm::is_any_of(":") );
  return filter_->filter(parts[0], type);
}
