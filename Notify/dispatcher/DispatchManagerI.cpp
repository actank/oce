/*
 * DispatchManagerI.cpp
 *
 *  Created on: Dec 9, 2009
 *      Author: antonio
 */

#include "DispatchManagerI.h"
#include "QueryRunner.h"

#include "Notify/util/schema_manager.h"
#include "NotifyDispatcherNAdapter.h"
#include "UserPassportAdapter.h"
#include "Notify/util/time_stat.h"

using namespace xce::notify;

using namespace com::xiaonei::xce;
using namespace xce::adapter::userpassport;
using namespace MyUtil;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();

  service.getAdapter()->add(&DispatchManagerI::instance(),
      service.createIdentity("M", ""));
}

Ice::Long DispatchManagerI::GetServreIndex(Ice::Long nid, const MyUtil::Str2StrMap& props) {
  Ice::Long server_index = nid;
  MyUtil::Str2StrMap::const_iterator it = props.find("feed_source");
  if (it != props.end()) {
    try {
      server_index = boost::lexical_cast<Ice::Long>(it->second);
    } catch (boost::bad_lexical_cast & e) {}
  }

  return server_index;
}

void DispatchManagerI::deliverWebpager(const MyUtil::Str2StrMap& props,
    const Ice::Current&) {

  if (!SchemaManager::instance().checkDispatch(props)) {
    MCE_WARN("DispatchManagerI::deliverWebpager illegal input");
    return;
  }

  Ice::Long nid = createNotifyId();
  MCE_INFO("DispatchManagerI::deliverWebpager nid=" << nid << " to=" << props.find("to")->second);

  Ice::Long server_index = GetServreIndex(nid, props);

  try {
    NotifyDispatcherNAdapter::instance().dispatchN(server_index, nid, false, props);
  } catch(Ice::Exception& e) {
    MCE_WARN("NotifyDispatcherNAdapter error : " << e);
  }
}

//string GetIp(const Ice::Current& current){
//	string ip = current.con->toString();
//	size_t found = ip.find('\n');
//	if (found != string::npos) {
//		ip[found] = ' ';
//	}
//	return ip;
//}
string GetIp(const Ice::Current& current){
	if(!current.con){
		return "";
	}
	string ip = current.con->toString();
	size_t found = ip.find('\n');
	if (found == string::npos) {
		return ip;
	}
	string res ( ip.begin() + found + 1,ip.end());
	return res;
}

void DispatchManagerI::dispatch(const MyUtil::Str2StrMap& props,
    const Ice::Current& cur) {
	TimeStat ts;
  if (!SchemaManager::instance().checkDispatch(props)) {
    MCE_WARN("DispatchManagerI::dispatch illegal input");
    return;
  }
  if(props.find("type")->second == "213"){
  	MCE_WARN("type = 213. not send.  " << GetIp(cur));
  	return ;
  }
  Ice::Int from_uid = 0;
  try {
    from_uid = boost::lexical_cast<Ice::Int>(props.find("from")->second);
  } catch (boost::bad_lexical_cast & e) {
    return;
  }
  // TODO 取消注释
//  UserPassportDataPtr data = UserPassportAdapter::instance().getUserPassport(from_uid);
//  if (data && (data->safeStatus & 0x07C0)) {
//    MCE_INFO("user blocked : " << from_uid);
//    return;
//  }


	UserPassportInfoPtr data; 
	try {
		data = UserPassportAdapter::instance().getUserPassport(from_uid);
	} catch(Ice::Exception& e) {
		MCE_WARN("NotifyDispatcherNAdapter call UserPassportAdapter from id " << from_uid  <<" error : " << e);
	}
  if (data && (data->safestatus() & 0x07C0)) {
    MCE_WARN("user blocked : " << from_uid);
    return;
  }

  Ice::Long nid = createNotifyId();
  Ice::Long server_index = GetServreIndex(nid, props);
  try {
    NotifyDispatcherNAdapter::instance().dispatchN(server_index, nid, true, props);
  } catch(Ice::Exception& e) {
    MCE_WARN("NotifyDispatcherNAdapter error : " << e);
  }

  MyUtil::Str2StrMap::const_iterator it = props.find("tominigroup");
  string groups = "NULL";
  if(it != props.end()){
  	groups = it->second;
  }

  MCE_INFO("DispatchManagerI::dispatch nid:" << nid 
      << " to:" << props.find("to")->second
      << " schema:" << props.find("schema_id")->second
      << " type:" << props.find("type")->second
      << " from:" << props.find("from")->second
      << " idx:" << server_index
			<< " tog:" << groups
			<< " cost:" << ts.getTime()
			<< " " << GetIp(cur)
			);
}

bool DispatchManagerI::reloadSchema(const Ice::Current&) {
  MCE_INFO("DispatchManagerI::reloadSchema");
  return SchemaManager::instance().reload();
}

Ice::Long DispatchManagerI::createNotifyId() {
  {
    IceUtil::Mutex::Lock lock(_mutex);
    if (++_count > 5000) {
      MCE_WARN("DbHelper::createId --> IdSeqDb is blocked");
      --_count;
      return -1;
    }
  }
  {
    IceUtil::Mutex::Lock lock(_mutex);
    while (1) {
      MCE_DEBUG("_curIndex:" << _curIndex << ", _topIndex:" << _topIndex
          << ", count:" << _count);
      if (_curIndex != -1 && _curIndex < _topIndex) {
        --_count;
        return _curIndex++;
      }
      {
        Statement sql;
        sql << "update " << TAB_NOTIFY_ID_SEQ << " SET id = id +"
            << ID_SEQ_STEP;
        QueryRunner(DB_INSTANCE, CDbWServer, TAB_NOTIFY_ID_SEQ).store(
            sql);
      }
      {
        Statement sql;
        sql << "SELECT * FROM " << TAB_NOTIFY_ID_SEQ;
        mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE,
            CDbWServer, TAB_NOTIFY_ID_SEQ).store(sql);
        if (!res) {
          --_count;
          return -1;
        }
        for (size_t i = 0; i < res.num_rows(); ++i) {
          mysqlpp::Row row = res.at(i);
          _topIndex = (Ice::Long) row["ID"];
          _curIndex = _topIndex - ID_SEQ_STEP;
          MCE_DEBUG("topIndex:" << _topIndex);
          break;
        }
      }

    }
  }
}

