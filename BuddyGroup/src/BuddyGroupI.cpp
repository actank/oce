#include "BuddyGroupI.h"
#include "QueryRunner.h"
//#include "RelationAdapter.h"
#include "BuddyCoreAdapter.h"
//#include "TalkIqAdapter.h"
#include "IqMsgHandlerAdapter.h"
#include "util/cpp/TimeCost.h"
#include <boost/lexical_cast.hpp>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "util/cpp/InvokeMonitor.h"

using namespace com::xiaonei::model;
using namespace com::xiaonei::xce;
using namespace talk::adapter;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::service::buddy;
using namespace xce::serverstate;

bool isUtf8(const string& str) {
  //MCE_DEBUG("check:"<<str);
  int c = 0;
  for (size_t i = str.length() - 1; i >=0; --i) {
    unsigned char ch = (unsigned char)str[i];
    //MCE_DEBUG("ch:"<<(int)ch);
    //MCE_DEBUG("& = "<< (int)(ch & (unsigned char) 0x80) << "  0x80:"<<0x80);
    if ((!(((int)ch & 0x80) == 0x80)) && (c==0)) {
      //MCE_DEBUG("return:"<< (int)(((int)ch & 0x80) == 0x80));
      return true;
    }
    //MCE_DEBUG("ch:"<<(int)ch << " 0xc0:"<<0xc0<< "  &:"<<(int)((int)ch & (unsigned char) 0xc0) << "  0x80:"<<0x80);
    if (((int)ch & 0xc0) == 0x80) {
      c ++;
      //MCE_DEBUG("C="<<c);
    } else {
      switch (c) {
        case 1:
          //MCE_DEBUG("case 1");
          if (((int)ch & 0xe0) == 0xc0) {
            return true;
          } else {
            return false;
          }
        case 2:
          //MCE_DEBUG("case 2");
          if (((int)ch & 0xf0) == 0xe0) {
            return true;
          } else {
            return false;
          }
        case 3:
          //MCE_DEBUG("case 3");
          if (((int)ch & 0xf8) == 0xf0) {
            return true;
          } else {
            return false;
          }
        default:
          //MCE_DEBUG("return false")
          return false;
      }
    }
  }
}

void MyUtil::initialize() {

  ServiceI& service = ServiceI::instance();
  service.registerObjectCache(0, "BG", new BuddyGroupFactoryI);
  service.getAdapter()->add(&BuddyGroupManagerI::instance(), service.createIdentity("M",
        ""));
  //BuddyCoreAdapter::instance().initializeWith(service.getAdapter());
  bool notifyIm = (bool) service.getCommunicator()->getProperties()->
    getPropertyAsIntWithDefault("Service."+ServiceI::instance().getName()+".NotifyIM", 1);
  BuddyGroupManagerI::instance().notifyIm(notifyIm);

  //----------------------------Monitor-----------------------------------------
  MyUtil::ServiceMonitorManager::instance().start();

  //----------------------------HA----------------------------------------------
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyGroup.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyGroup.Interval", 5);

  // �~P~QController注�~F~L
  ServerStateSubscriber::instance().initialize("ControllerBuddyGroup",
      &BuddyGroupManagerI::instance(), mod, interval);
}

Ice::ObjectPtr BuddyGroupFactoryI::create(Ice::Int id) {

  GroupMapWrapperPtr res = new GroupMapWrapper();

  QueryRunner conn("buddygroup", CDbRServer,
      BuddyGroupManagerI::getTableName("relation_group", id));
  Statement sql;
  sql
    << "select a.groupid gid, a.name gname, if(b.guest is null, 0, b.guest) guest"
    << " from "<<BuddyGroupManagerI::getTableName("relation_group", id)
    <<" a left join "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", id)<<" b "
    << "on (a.host=b.host and a.groupid = b.groupid) where a.host ="
    << boost::lexical_cast<string>(id);
  //      DefResSetPtr drs = DefResSetPtr::dynamicCast(conn.doSelect(sql.str()));
  //      mysqlpp::StoreQueryResult res = conn.store(Statement(sql.str()));

#ifndef NEWARCH
  mysqlpp::Result dataRes = conn.store(Statement(sql.str()));
#else
  mysqlpp::StoreQueryResult dataRes = conn.store(sql);
#endif
  map<int,string> illegalGroups;

  if (dataRes) {

#ifndef NEWARCH
    for (size_t i = 0; i<dataRes.size(); ++i)
#else
      for (size_t i = 0; i<dataRes.num_rows(); ++i) {
#endif
        mysqlpp::Row row = dataRes.at(i);
        int groupId = (int)row["gid"];
#ifndef NEWARCH

        string groupName = row["gname"].get_string();
#else

        string groupName = row["gname"].c_str();
#endif

        int guest = (int)row["guest"];

        if (!isUtf8(groupName) || "" == groupName) {
          illegalGroups[groupId] = groupName;
          continue;
        }

        BuddyGroupMap::iterator it = res->value.find(groupName);
        if (it == res->value.end()) {
          BuddyGroupPtr ptr = new BuddyGroup();
          ptr->groupId = groupId;
          if (guest)
            ptr->buddyIds.insert(guest);
          res->value[groupName] = ptr;
          res->idmap[groupId] = groupName;
        } else {
          if (guest)
            it->second->buddyIds.insert(guest);
        }
      }
  }

  if (!illegalGroups.empty()) {
    TaskManager::instance().execute(new CleanIllegalDataTask(id,illegalGroups));
  }

  ::xce::binstring::BinStringPtr bsptr = res->serialize();
  BuddyGroupDataPtr data = new BuddyGroupData(bsptr);	

  MCE_INFO("Create "<<id << " done");
  return data;
}

void BuddyGroupManagerI::addGroup(Ice::Int host, const string& groupName,
    const Ice::Current& current) {

  ostringstream msg;
  msg << " groupName:" << groupName<< " host:" <<host;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  if (!isUtf8(groupName) || "" == groupName) {
    MCE_WARN("BuddyGroupManagerI::addGroup --> Illegal GroupName:"
        <<groupName<<"  host:"<<host);
    return;
  }

  //	QueryRunner conn("buddygroup", CDbWServer,
  //			BuddyGroupManagerI::getTableName("relation_group", host));
  int groupId = 0;
  {
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    //	ostringstream sql;
    query<<"insert into "<<BuddyGroupManagerI::getTableName("relation_group",
        host)<<" (host, name) values ("<<host<<","<<mysqlpp::quote
      <<groupName<<")";
    //	int groupId = conn.execute(Statement(sql.str())).insert_id();
    groupId = query.execute().insert_id();
  }

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  {
    IceUtil::Mutex::Lock lock(*bgdptr);
    ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
    GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);

    BuddyGroupPtr gptr = new BuddyGroup();
    gptr->groupId = groupId; 
    wptr->value[groupName] = gptr;
    wptr->idmap[groupId] = groupName;

    ::xce::binstring::BinStringPtr destbp = wptr->serialize();
    bgdptr->setBP(destbp);
  }
  MCE_DEBUG("BuddyGroupManagerI::addGroup " <<groupName<<"  host:"<<host << " done");
}

void BuddyGroupManagerI::renameGroup(Ice::Int host, const string& oldName,
    const string& newName, const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " oldName:" << oldName << " newName:" << newName;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO); 
  //if newName exist
  if (!(isUtf8(oldName) && isUtf8(newName))) {
    MCE_WARN("BuddyGroupManagerI::renameGroup --> Illegal OldGroupName:"
        <<oldName<<"  NewGroupName:"<<newName<<"  host:"<<host);
    return;
  }

  if( oldName == newName){
    MCE_WARN("BuddyGroupManagerI::renameGroup host:"<< host << " old name don't changed, do nothing");
    return;
  }

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);

  BuddyGroupMap::iterator nit = wptr->value.find(newName);

  if (nit != wptr->value.end()) {

    BuddyGroupMap::iterator oit = wptr->value.find(oldName);
    MCE_DEBUG(__LINE__ << "  -->");
    if (oit != wptr->value.end()) {

      for (set<int>::iterator bit = oit->second->buddyIds.begin(); bit
          != oit->second->buddyIds.end(); ++bit) {
        nit->second->buddyIds.insert(*bit);
      }

      _removeGroup(host, oldName);

      if(!oit->second->buddyIds.empty()){
        ConnectionHolder conn("buddygroup", CDbWServer,
            BuddyGroupManagerI::getTableName("relation_group", host));
        mysqlpp::Query query = conn.query();
        query <<"insert into  "<<BuddyGroupManagerI::getTableName(
            "relation_group_item", host) <<" (host,guest,groupid) values ";
        for (set<int>::const_iterator sit = oit->second->buddyIds.begin(); sit != oit->second->buddyIds.end(); ++sit) {
          if ( sit != oit->second->buddyIds.begin()) {
            query <<",";
          }
          query <<"("<<host<<","<< *sit<<","<<nit->second->groupId
            <<")";

        }
        query.execute();
      }
      oit->second->buddyIds.clear();
      wptr->value.erase(oit);
    }
  } else {

    //		QueryRunner conn("buddygroup", CDbWServer,
    //				BuddyGroupManagerI::getTableName("relation_group", host));
    {
      ConnectionHolder conn("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn.query();

      //		ostringstream sql;
      query<<"update " <<BuddyGroupManagerI::getTableName("relation_group",
          host) <<" set name="<<mysqlpp::quote<<newName<<" where host="
        <<host <<" and name="<<mysqlpp::quote<<oldName;
      //		conn.execute(sql.str());
      query.execute();
    }

    //if (ServiceI::instance().hasObject(0, host)) {
    //GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
    if (wptr) {
      //GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);
      BuddyGroupMap::iterator it = wptr->value.find(oldName);

      if (it != wptr->value.end()) {
        wptr->value[newName] = it->second;
        wptr->idmap[it->second->groupId] = newName;
        wptr->value.erase(it);
      }

    }
    //}
  }
  //notify im
  //GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);

  if (_notifyIm) {
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;

    if (wptr) {
      BuddyGroupMap::iterator it = wptr->value.find(newName);
      if (it != wptr->value.end()) {
        for (set<int>::iterator bit = it->second->buddyIds.begin(); bit
            != it->second->buddyIds.end(); ++bit) {
          bi->items[*bit] = _getBuddyGroups(host, *bit);
        }
      }
    }
    if (bi->items.empty()) {
      ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
      bgdptr->setBP(bsptr);
      return;
    }
    MCE_DEBUG("BuddyGroupManagerI::renameGroup 3-->"<< bi->items.size());
    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }

  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
  MCE_DEBUG("BuddyGroupManagerI::renameGroup host:"<< host <<" oldName:" << oldName << " newName:" << newName )
}

void BuddyGroupManagerI::renameGroupWithGid(Ice::Int host, Ice::Int groupId,
    const string& newName, const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupId:" << groupId << " newName:" << newName;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  //if newName exist
  if (!isUtf8(newName)) {
    MCE_WARN("BuddyGroupManagerI::renameGroup --> Illegal groupId:"
        <<groupId<<"  NewGroupName:"<<newName<<"  host:"<<host);
    return;
  }

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);

  string oldName = "";
  BuddyGroupIdMap::iterator iit = wptr->idmap.find(groupId);
  if(iit != wptr->idmap.end()){
    oldName = iit->second;
  }

  if( oldName == newName){
    MCE_WARN("BuddyGroupManagerI::renameGroup host:"<< host << " old name don't changed, do nothing");
    return;
  }

  BuddyGroupMap::iterator nit = wptr->value.find(newName);

  if (nit != wptr->value.end()) {

    BuddyGroupMap::iterator oit = wptr->value.find(oldName);
    MCE_DEBUG(__LINE__ << "  -->");
    if (oit != wptr->value.end()) {

      for (set<int>::iterator bit = oit->second->buddyIds.begin(); bit
          != oit->second->buddyIds.end(); ++bit) {
        nit->second->buddyIds.insert(*bit);
      }

      _removeGroup(host, oldName);

      if(!oit->second->buddyIds.empty()){
        ConnectionHolder conn("buddygroup", CDbWServer,
            BuddyGroupManagerI::getTableName("relation_group", host));
        mysqlpp::Query query = conn.query();
        query <<"insert into  "<<BuddyGroupManagerI::getTableName(
            "relation_group_item", host) <<" (host,guest,groupid) values ";
        for (set<int>::const_iterator sit = oit->second->buddyIds.begin(); sit != oit->second->buddyIds.end(); ++sit) {
          if ( sit != oit->second->buddyIds.begin()) {
            query <<",";
          }
          query <<"("<<host<<","<< *sit<<","<<nit->second->groupId
            <<")";

        }
        query.execute();
      }
      oit->second->buddyIds.clear();
      wptr->value.erase(oit);
    }
  } else {

    //		QueryRunner conn("buddygroup", CDbWServer,
    //				BuddyGroupManagerI::getTableName("relation_group", host));
    {
      ConnectionHolder conn("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn.query();

      //		ostringstream sql;
      query<<"update " <<BuddyGroupManagerI::getTableName("relation_group",
          host) <<" set name="<<mysqlpp::quote<<newName<<" where host="
        <<host <<" and name="<<mysqlpp::quote<<oldName;
      //		conn.execute(sql.str());
      query.execute();
    }

    //if (ServiceI::instance().hasObject(0, host)) {
    //GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
    if (wptr) {
      //GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);
      BuddyGroupMap::iterator it = wptr->value.find(oldName);

      if (it != wptr->value.end()) {
        wptr->value[newName] = it->second;
        wptr->idmap[it->second->groupId] = newName;
        wptr->value.erase(it);
      }

    }
    //}
  }
  //notify im
  //GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);

  if (_notifyIm) {
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;

    if (wptr) {
      BuddyGroupMap::iterator it = wptr->value.find(newName);
      if (it != wptr->value.end()) {
        for (set<int>::iterator bit = it->second->buddyIds.begin(); bit
            != it->second->buddyIds.end(); ++bit) {
          bi->items[*bit] = _getBuddyGroups(host, *bit);
        }
      }
    }
    if (bi->items.empty()) {
      ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
      bgdptr->setBP(bsptr);
      return;
    }
    MCE_DEBUG("BuddyGroupManagerI::renameGroup 3-->"<< bi->items.size());
    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }

  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
  MCE_DEBUG("BuddyGroupManagerI::renameGroup host:"<< host <<" oldName:" << oldName << " newName:" << newName )
}

void BuddyGroupManagerI::removeGroup(Ice::Int host, const string& groupName,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupName:" <<groupName;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  if (!isUtf8(groupName)) {
    MCE_WARN("BuddyGroupManagerI::removeGroup --> Illegal GroupName:"
        <<groupName<<"  host:"<<host);
    return;
  }

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);

  int groupId = -1;
  set<int> buddyIds;

  BuddyGroupMap::iterator it;
  BuddyGroupIdMap::iterator iit;

  if (wptr) {
    //	GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);
    it = wptr->value.find(groupName);

    if (it != wptr->value.end()) {
      buddyIds = it->second->buddyIds;
      groupId = it->second->groupId;
    }

    iit = wptr->idmap.find(groupId);
    if (iit == wptr->idmap.end()){

      //bug712中的缓存与数据库数据不一致，具体原因未明，以下代码用于消极修复
      MCE_WARN("BuddyGroupManagerI::removeGroup groupId "<< groupId << " not found in " << host);
      lock.release();
      removeGroupForce(host, groupName, current);

      return;
    }
  }
  //	QueryRunner conn("buddygroup", CDbWServer,
  //			BuddyGroupManagerI::getTableName("relation_group", host));
  //	ostringstream sql;
  {
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();

    query<<"delete from "<<BuddyGroupManagerI::getTableName("relation_group",
        host)<<"  where host="<<host <<" and name="<<mysqlpp::quote
      <<groupName;
    query.execute();
  }
  if (it != wptr->value.end()) {
    wptr->value.erase(it);
  }
  if (iit != wptr->idmap.end() ){
    wptr->idmap.erase(iit);
  }

  MCE_DEBUG("BuddyGroupManagerI::removeGroup1 -->"<< buddyIds.size());

  if (groupId == -1) {
    ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
    bgdptr->setBP(bsptr);
    return;
  }

  MCE_DEBUG("BuddyGroupManagerI::removeGroup2 -->"<< buddyIds.size());
  {
    ConnectionHolder conn1("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn1.query();
    //	sql.str("");
    query<<"delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where groupid="<<groupId
      << " and host="<< host;
    query.execute();
  }
  //notify im
  if (_notifyIm) {
    if (buddyIds.empty()) {
      ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
      bgdptr->setBP(bsptr);
      return;
    }
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;
    if (wptr) {
      for (set<int>::iterator it = buddyIds.begin(); it != buddyIds.end(); ++it) {
        bi->items[*it] = _getBuddyGroups(host, *it);
      }
    }
    if (bi->items.empty()) {
      ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
      bgdptr->setBP(bsptr);
      return;
    }

    MCE_DEBUG("BuddyGroupManagerI::removeGroup 3-->"<< buddyIds.size());
    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }
  //---------
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
}

//先删除缓存，再从数据库中读取，达到强制删除的目的
void BuddyGroupManagerI::removeGroupForce(Ice::Int host, const string& groupName,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupName:" <<groupName;
  MCE_WARN("BuddyGroupManagerI::removeGroupForce" << msg.str());

  if (!isUtf8(groupName)) {
    MCE_WARN("BuddyGroupManagerI::removeGroupForce --> Illegal GroupName:"
        <<groupName<<"  host:"<<host);
    return;
  }

  //先删除缓存，强制从数据库同步
  ServiceI::instance().removeObject(0, host);
  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);

  int groupId = -1;
  set<int> buddyIds;

  BuddyGroupMap::iterator it;
  BuddyGroupIdMap::iterator iit;

  if (wptr) {
    //	GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);
    it = wptr->value.find(groupName);

    if (it != wptr->value.end()) {
      buddyIds = it->second->buddyIds;
      groupId = it->second->groupId;
    }

    iit = wptr->idmap.find(groupId);
    if (iit == wptr->idmap.end()){
      MCE_WARN("BuddyGroupManagerI::removeGroupForce groupId "<< groupId << " not found in " << host);
      return;
    }
  }
  //	QueryRunner conn("buddygroup", CDbWServer,
  //			BuddyGroupManagerI::getTableName("relation_group", host));
  //	ostringstream sql;
  {
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();

    query<<"delete from "<<BuddyGroupManagerI::getTableName("relation_group",
        host)<<"  where host="<<host <<" and name="<<mysqlpp::quote
      <<groupName;
    query.execute();
  }
  if (it != wptr->value.end()) {
    wptr->value.erase(it);
  }
  if (iit != wptr->idmap.end() ){
    wptr->idmap.erase(iit);
  }

  MCE_DEBUG("BuddyGroupManagerI::removeGroupForce1 -->"<< buddyIds.size());

  if (groupId == -1) {
    ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
    bgdptr->setBP(bsptr);
    return;
  }

  MCE_DEBUG("BuddyGroupManagerI::removeGroupForce2 -->"<< buddyIds.size());
  {
    ConnectionHolder conn1("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn1.query();
    //	sql.str("");
    query<<"delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where groupid="<<groupId
      << " and host="<< host;
    query.execute();
  }
  //notify im
  if (_notifyIm) {
    if (buddyIds.empty()) {
      ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
      bgdptr->setBP(bsptr);
      return;
    }
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;
    if (wptr) {
      for (set<int>::iterator it = buddyIds.begin(); it != buddyIds.end(); ++it) {
        bi->items[*it] = _getBuddyGroups(host, *it);
      }
    }
    if (bi->items.empty()) {
      ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
      bgdptr->setBP(bsptr);
      return;
    }

    MCE_DEBUG("BuddyGroupManagerI::removeGroupForce 3-->"<< buddyIds.size());
    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }
  //---------
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
}

void BuddyGroupManagerI::removeGroupWithGid(Ice::Int host, Ice::Int groupId,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupId:" <<groupId;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  string groupName = "";

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);

  set<int> buddyIds;

  BuddyGroupMap::iterator it;
  BuddyGroupIdMap::iterator	iit;

  if (wptr) {
    //	GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);

    iit = wptr->idmap.find(groupId);
    if (iit == wptr->idmap.end()){
      MCE_WARN("BuddyGroupManagerI::removeGroupWithGid groupId "<< groupId << " not found in " << host);
      return;
    }
    groupName = iit->second;
    it = wptr->value.find(groupName);
    if (it != wptr->value.end()) {
      buddyIds = it->second->buddyIds;
    }
  }
  //	QueryRunner conn("buddygroup", CDbWServer,
  //			BuddyGroupManagerI::getTableName("relation_group", host));
  //	ostringstream sql;
  {
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();

    query<<"delete from "<<BuddyGroupManagerI::getTableName("relation_group",
        host)<<"  where host="<<host <<" and name="<<mysqlpp::quote
      <<groupName;
    query.execute();
  }
  if (iit != wptr->idmap.end() ){
    wptr->idmap.erase(iit);
  }
  if (it != wptr->value.end()) {
    wptr->value.erase(it);
  }

  MCE_DEBUG("BuddyGroupManagerI::removeGroup1 -->"<< buddyIds.size());

  MCE_DEBUG("BuddyGroupManagerI::removeGroup2 -->"<< buddyIds.size());
  {
    ConnectionHolder conn1("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn1.query();
    //	sql.str("");
    query<<"delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where groupid="<<groupId
      << " and host="<< host;
    query.execute();
  }
  //notify im
  if (_notifyIm) {
    if (buddyIds.empty()) {
      ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
      bgdptr->setBP(bsptr);
      return;
    }
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;
    if (wptr) {
      for (set<int>::iterator it = buddyIds.begin(); it != buddyIds.end(); ++it) {
        bi->items[*it] = _getBuddyGroups(host, *it);
      }
    }
    if (bi->items.empty()) {
      ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
      bgdptr->setBP(bsptr);
      return;
    }

    MCE_DEBUG("BuddyGroupManagerI::removeGroup 3-->"<< buddyIds.size());
    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }
  //---------
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
}

void BuddyGroupManagerI::_removeGroup(Ice::Int host, const string& groupName) {

  MCE_DEBUG("BuddyGroupManagerI::_removeGroup");

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);

  int groupId = -1;
  set<int> buddyIds;

  BuddyGroupMap::iterator it;
  if (wptr) {
    //	GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);


    it = wptr->value.find(groupName);

    if (it != wptr->value.end()) {
      buddyIds = it->second->buddyIds;
      if (!it->second->buddyIds.empty()) {
        groupId = it->second->groupId;
      }
    }
  }
  //	QueryRunner conn("buddygroup", CDbWServer,
  //			BuddyGroupManagerI::getTableName("relation_group", host));
  //	ostringstream sql;
  {
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query<<"delete from "<<BuddyGroupManagerI::getTableName("relation_group",
        host)<<"  where host="<<host <<" and name="<<mysqlpp::quote
      <<groupName;
    //	conn.execute(sql.str());
    query.execute();
  }
  if (it != wptr->value.end()) {
    wptr->value.erase(it);
  }
  BuddyGroupIdMap::iterator iit;
  iit = wptr->idmap.find(groupId);
  if(iit != wptr->idmap.end()){
    wptr->idmap.erase(iit);
  }

  MCE_DEBUG("BuddyGroupManagerI::_removeGroup1 -->"<< buddyIds.size());

  if (groupId == -1) {
    return;
  }

  MCE_DEBUG("BuddyGroupManagerI::_removeGroup2 -->"<< buddyIds.size());

  //	sql.str("");
  //	sql<<"delete from "<<BuddyGroupManagerI::getTableName(
  //			"relation_group_item", host)<<" where groupid="<<groupId
  //			<< " and host="<< host;
  //	conn.execute(sql.str());
  {
    ConnectionHolder conn1("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn1.query();
    query <<"delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where groupid="<<groupId
      << " and host="<< host;
    query.execute();
  }
  //notify im
  /*
     if (_notifyIm) {
     if (buddyIds.empty()) {
     return;
     }
     BuddyItemPtr bi = new BuddyItem();
     bi->host = host;
     if (wptr) {
     for (set<int>::iterator it = buddyIds.begin(); it != buddyIds.end(); ++it) {
     bi->items[*it] = _getBuddyGroups(host, *it);
     }
     }
     if (bi->items.empty()) {
     return;
     }

     MCE_DEBUG("BuddyGroupManagerI::_removeGroup 3-->"<< buddyIds.size());
     IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
     }*/
  //---------
}

void BuddyGroupManagerI::cleanGroup(Ice::Int host, const string& groupName,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupName:" <<groupName;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  if (!isUtf8(groupName)) {
    MCE_WARN("BuddyGroupManagerI::cleanGroup --> Illegal GroupName:"
        <<groupName<<"  host:"<<host);
    return;
  }

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);

  int groupId = -1;
  set<int> buddyIds;

  BuddyGroupMap::iterator it;
  if (wptr) {
    it = wptr->value.find(groupName);
    if (it != wptr->value.end()) {
      if (it->second->buddyIds.empty()) {
        //groupId = it->second->groupId;
        return;
      }
      buddyIds = it->second->buddyIds;
      groupId = it->second->groupId;
      it->second->buddyIds.clear();
    }
  }
  if (groupId >= 0) {
    //		QueryRunner conn("buddygroup", CDbWServer,
    //				BuddyGroupManagerI::getTableName("relation_group", host));
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    //		ostringstream sql;
    query<<"delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where groupid="<<groupId
      <<" and host="<<host;
    query.execute();
  }

  //notify im
  if (_notifyIm) {
    if (buddyIds.empty()) {
      ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
      bgdptr->setBP(bsptr);

      return;
    }
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;
    if (wptr) {
      for (set<int>::iterator it = buddyIds.begin(); it != buddyIds.end(); ++it) {
        bi->items[*it] = _getBuddyGroups(host, *it);
      }
    }
    if (bi->items.empty()) {
      ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
      bgdptr->setBP(bsptr);

      return;
    }
    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }
  //---------
  MCE_DEBUG("BuddyGroupManagerI::cleanGroup "
      <<groupName<<"  host:"<<host << "END");
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
}

void BuddyGroupManagerI::cleanGroupWithGid(Ice::Int host, Ice::Int groupId,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupId:" <<groupId;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);

  set<int> buddyIds;
  BuddyGroupIdMap::iterator iit;
  BuddyGroupMap::iterator it;
  string groupName;
  if (wptr) {
    iit = wptr->idmap.find(groupId);
    if(iit == wptr->idmap.end()){
      MCE_WARN("BuddyGroupManagerI::cleanGroupWithGid groupId " << groupId << " not found in " << host);
      return;
    }
    groupName = iit->second;
    it = wptr->value.find(groupName);
    if (it != wptr->value.end()) {
      if (it->second->buddyIds.empty()) {
        //groupId = it->second->groupId;
        return;
      }
      buddyIds = it->second->buddyIds;
      //groupId = it->second->groupId;
      it->second->buddyIds.clear();
    }
  }
  if (groupId >= 0) {
    //		QueryRunner conn("buddygroup", CDbWServer,
    //				BuddyGroupManagerI::getTableName("relation_group", host));
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    //		ostringstream sql;
    query<<"delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where groupid="<<groupId
      <<" and host="<<host;
    query.execute();
  }

  //notify im
  if (_notifyIm) {
    if (buddyIds.empty()) {
      ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
      bgdptr->setBP(bsptr);

      return;
    }
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;
    if (wptr) {
      for (set<int>::iterator it = buddyIds.begin(); it != buddyIds.end(); ++it) {
        bi->items[*it] = _getBuddyGroups(host, *it);
      }
    }
    if (bi->items.empty()) {
      ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
      bgdptr->setBP(bsptr);

      return;
    }
    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }
  //---------
  MCE_DEBUG("BuddyGroupManagerI::cleanGroup "
      <<groupName<<"  host:"<<host << "END");
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
}

void BuddyGroupManagerI::_cleanGroup(Ice::Int host, const string& groupName
    ) { 
  MCE_DEBUG("BuddyGroupManagerI::_cleanGroup "
      <<groupName<<"  host:"<<host);
  if (!isUtf8(groupName)) {
    MCE_WARN("BuddyGroupManagerI::cleanGroup --> Illegal GroupName:"
        <<groupName<<"  host:"<<host);
    return;
  }

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);

  int groupId = -1;
  set<int> buddyIds;

  BuddyGroupMap::iterator it;
  if (wptr) {
    it = wptr->value.find(groupName);
    if (it != wptr->value.end()) {
      if (it->second->buddyIds.empty()) {
        //groupId = it->second->groupId;
        return;
      }
      buddyIds = it->second->buddyIds;
      groupId = it->second->groupId;
      it->second->buddyIds.clear();
    }
  }
  if (groupId >= 0) {
    //              QueryRunner conn("buddygroup", CDbWServer,
    //                              BuddyGroupManagerI::getTableName("relation_group", host));
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    //              ostringstream sql;
    query<<"delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where groupid="<<groupId
      <<" and host="<<host;
    query.execute();
  }

  //---------
  MCE_DEBUG("BuddyGroupManagerI::_cleanGroup "
      <<groupName<<"  host:"<<host << "END");
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
}

void BuddyGroupManagerI::addBuddyToGroup(Ice::Int host,
    const string& groupName, Ice::Int buddy, const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupName:" <<groupName << " buddy:" << buddy;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  if (!isUtf8(groupName)) {
    MCE_WARN("BuddyGroupManagerI::addBuddyToGroup --> Illegal GroupName:"
        <<groupName<<"  host:"<<host);
    return;
  }

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  //GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);


  BuddyGroupMap::iterator it = wptr->value.find(groupName);
  if (it != wptr->value.end()) {
    if (it->second->buddyIds.find(buddy) != it->second->buddyIds.end()) {
      return;
    }
    it->second->buddyIds.insert(buddy);
    //		QueryRunner conn("buddygroup", CDbWServer,
    //				BuddyGroupManagerI::getTableName("relation_group", host));
    //		ostringstream sql;
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query <<"insert into  "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host) <<" (host,guest,groupid) values ("
      <<host<<","<<buddy<<"," <<it->second->groupId<<")";
    query.execute();

  } else {

    //		QueryRunner conn("buddygroup", CDbWServer,
    //				BuddyGroupManagerI::getTableName("relation_group", host));
    //		ostringstream sql;
    int groupId =0;
    {
      ConnectionHolder conn("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn.query();

      query << "insert into "<<BuddyGroupManagerI::getTableName(
          "relation_group", host)<<" (host,name) values ("<< host <<","
        <<mysqlpp::quote <<groupName<<")";
      groupId = query.execute().insert_id();
    }
    //		sql.str("");
    {
      ConnectionHolder conn1("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn1.query();
      query << "insert into  "<<BuddyGroupManagerI::getTableName(
          "relation_group_item", host) <<" (host,guest,groupid) values ("
        <<host<<","<<buddy<<"," <<groupId<<")";
      query.execute();
    }
    //yuanfei
    BuddyGroupPtr bgPtr = new BuddyGroup();
    bgPtr->groupId = groupId;
    bgPtr->buddyIds.insert(buddy);
    wptr->value[groupName] = bgPtr;
    //
  }

  //notify im
  if (_notifyIm) {
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;
    bi->items[buddy] = _getBuddyGroups(host, buddy);

    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }
  //---------

  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
}

void BuddyGroupManagerI::addBuddyToGroupWithGid(Ice::Int host,
    Ice::Int groupId, Ice::Int buddy, const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupId:" <<groupId << " buddy:" << buddy;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  //GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);

  BuddyGroupIdMap::iterator iit = wptr->idmap.find(groupId);
  if(iit == wptr->idmap.end()){
    MCE_WARN("BuddyGroupManagerI::addBuddyToGroupWithGid Illegal groupId: " 
        << groupId <<" host: "<< host << " buddy: " << buddy);
    return;
  }
  string groupName = iit->second;
  BuddyGroupMap::iterator it = wptr->value.find(groupName);
  if (it != wptr->value.end()) {
    if (it->second->buddyIds.find(buddy) != it->second->buddyIds.end()) {
      return;
    }
    it->second->buddyIds.insert(buddy);
    //		QueryRunner conn("buddygroup", CDbWServer,
    //				BuddyGroupManagerI::getTableName("relation_group", host));
    //		ostringstream sql;
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query <<"insert into  "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host) <<" (host,guest,groupid) values ("
      <<host<<","<<buddy<<"," <<it->second->groupId<<")";
    query.execute();

  } else {

    //		QueryRunner conn("buddygroup", CDbWServer,
    //				BuddyGroupManagerI::getTableName("relation_group", host));
    //		ostringstream sql;
    int groupId =0;
    {
      ConnectionHolder conn("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn.query();

      query << "insert into "<<BuddyGroupManagerI::getTableName(
          "relation_group", host)<<" (host,name) values ("<< host <<","
        <<mysqlpp::quote <<groupName<<")";
      groupId = query.execute().insert_id();
    }
    //		sql.str("");
    {
      ConnectionHolder conn1("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn1.query();
      query << "insert into  "<<BuddyGroupManagerI::getTableName(
          "relation_group_item", host) <<" (host,guest,groupid) values ("
        <<host<<","<<buddy<<"," <<groupId<<")";
      query.execute();
    }
    //yuanfei
    BuddyGroupPtr bgPtr = new BuddyGroup();
    bgPtr->groupId = groupId;
    bgPtr->buddyIds.insert(buddy);
    wptr->value[groupName] = bgPtr;
    //
  }

  //notify im
  if (_notifyIm) {
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;
    bi->items[buddy] = _getBuddyGroups(host, buddy);

    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }
  //---------

  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
}

void BuddyGroupManagerI::addBuddiesToGroup(Ice::Int host,
    const string& groupName, const MyUtil::IntSeq & buddies,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupName:" << groupName << " buddies.size:" << buddies.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  if (!isUtf8(groupName)) {
    MCE_WARN("BuddyGroupManagerI::addBuddiesToGroup --> Illegal GroupName:"
        <<groupName<<"  host:"<<host);
    return;
  }

  if (buddies.empty()) {
    return;
  }

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);
  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  //GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);


  BuddyGroupMap::iterator it = wptr->value.find(groupName);
  if (it != wptr->value.end()) {
    MyUtil::IntSeq insertIds;
    for (size_t i = 0; i < buddies.size(); ++i) {
      //it->second->buddyIds.insert(buddies.at(i));
      if (it->second->buddyIds.find(buddies.at(i))
          != it->second->buddyIds.end()) {
        continue;
      }
      insertIds.push_back(buddies.at(i));
      it->second->buddyIds.insert(buddies.at(i));
    }
    //    QueryRunner conn("buddygroup", CDbWServer,
    //        BuddyGroupManagerI::getTableName("relation_group", host));
    //    ostringstream sql;
    if(!insertIds.empty()){
      ConnectionHolder conn("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn.query();
      query <<"insert into  "<<BuddyGroupManagerI::getTableName(
          "relation_group_item", host) <<" (host,guest,groupid) values ";
      for (size_t i = 0; i < insertIds.size(); ++i) {
        if (i) {
          query <<",";
        }
        query <<"("<<host<<","<<insertIds.at(i)<<","<<it->second->groupId
          <<")";

      }
      query.execute();
    }
  } else {

    //    QueryRunner conn("buddygroup", CDbWServer,
    //        BuddyGroupManagerI::getTableName("relation_group", host));
    //    ostringstream sql;
    int groupId = 0;
    {
      ConnectionHolder conn("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn.query();
      query << "insert into "<<BuddyGroupManagerI::getTableName(
          "relation_group", host)<<" (host,name) values ("<< host <<","
        <<mysqlpp::quote <<groupName<<")";
      groupId = query.execute().insert_id();
    }
    //    sql.str("");
    {
      ConnectionHolder conn1("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn1.query();
      query << "insert into  "<<BuddyGroupManagerI::getTableName(
          "relation_group_item", host) <<" (host,guest,groupid) values ";
      for (size_t i = 0; i < buddies.size(); ++i) {
        if (i) {
          query <<",";
        }

        query <<"("<<host<<","<<buddies.at(i)<<","<<groupId<<")";
      };
      query.execute();
    }
    //yuanfei
    BuddyGroupPtr bgPtr = new BuddyGroup();
    bgPtr->groupId = groupId;
    for (size_t i = 0; i < buddies.size(); ++i) {
      bgPtr->buddyIds.insert(buddies.at(i));
    }
    wptr->value[groupName] = bgPtr;
    //
  }
  //notify im
  if (_notifyIm) {
    if (buddies.empty()) {
      return;
    }
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;

    for (size_t i = 0; i < buddies.size(); ++i) {
      bi->items[buddies.at(i)] = _getBuddyGroups(host, buddies.at(i));
    }
    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }
  //---------
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
}
void BuddyGroupManagerI::addBuddiesToGroupWithGid(Ice::Int host,
    Ice::Int groupId, const MyUtil::IntSeq & buddies,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupId:" << groupId << " buddies.size:" << buddies.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  if (buddies.empty()) {
    return;
  }

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);
  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  //GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);
  string groupName = "";
  BuddyGroupIdMap::iterator iit = wptr->idmap.find(groupId); 
  if (iit != wptr->idmap.end()){
    groupName = wptr->idmap[groupId];

    BuddyGroupMap::iterator it = wptr->value.find(groupName);
    if (it != wptr->value.end()) {
      MyUtil::IntSeq insertIds;
      for (size_t i = 0; i < buddies.size(); ++i) {
        //it->second->buddyIds.insert(buddies.at(i));
        if (it->second->buddyIds.find(buddies.at(i))
            != it->second->buddyIds.end()) {
          continue;
        }
        insertIds.push_back(buddies.at(i));
        it->second->buddyIds.insert(buddies.at(i));
      }
      //		QueryRunner conn("buddygroup", CDbWServer,
      //				BuddyGroupManagerI::getTableName("relation_group", host));
      //		ostringstream sql;
      if(!insertIds.empty()){
        ConnectionHolder conn("buddygroup", CDbWServer,
            BuddyGroupManagerI::getTableName("relation_group", host));
        mysqlpp::Query query = conn.query();
        query <<"insert into  "<<BuddyGroupManagerI::getTableName(
            "relation_group_item", host) <<" (host,guest,groupid) values ";
        for (size_t i = 0; i < insertIds.size(); ++i) {
          if (i) {
            query <<",";
          }
          query <<"("<<host<<","<<insertIds.at(i)<<","<<it->second->groupId
            <<")";

        }
        query.execute();
      }
    }
  } else {
  }
  if (_notifyIm) {
    if (buddies.empty()) {
      return;
    }
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;

    for (size_t i = 0; i < buddies.size(); ++i) {
      bi->items[buddies.at(i)] = _getBuddyGroups(host, buddies.at(i));
    }
    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }
  //---------
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);	
}

void BuddyGroupManagerI::setBuddiesToGroup(Ice::Int host,
    const string& groupName, const MyUtil::IntSeq & buddies,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupName:" << groupName << " buddies.size:" << buddies.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  if (!isUtf8(groupName)) {
    MCE_WARN("BuddyGroupManagerI::setBuddiesToGroup --> Illegal GroupName:"
        <<groupName<<"  host:"<<host);
    return;
  }


  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);
  IceUtil::Mutex::Lock lock(*bgdptr);

  _cleanGroup(host,groupName);
  if (buddies.empty()) {
    return;
  }

  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  //GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);

  BuddyGroupMap::iterator it = wptr->value.find(groupName);
  if (it != wptr->value.end()) {
    MCE_DEBUG("BuddyGroupManagerI::setBuddiesToGroup " <<groupName<<"  host:"<<host << " group found");
    it->second->buddyIds.insert(buddies.begin(),buddies.end());

    if(!buddies.empty()){
      ConnectionHolder conn("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn.query();
      query <<"insert into  "<<BuddyGroupManagerI::getTableName(
          "relation_group_item", host) <<" (host,guest,groupid) values ";
      for (size_t i = 0; i < buddies.size(); ++i) {
        if (i) {
          query <<",";
        }
        query <<"("<<host<<","<<buddies.at(i)<<","<<it->second->groupId
          <<")";

      }
      query.execute();
    }
    MCE_DEBUG("BuddyGroupManagerI::setBuddiesToGroup " <<groupName<<"  host:"<<host << " group found done.");
  } else {
    //              QueryRunner conn("buddygroup", CDbWServer,
    //                              BuddyGroupManagerI::getTableName("relation_group", host));
    //              ostringstream sql;
    MCE_DEBUG("BuddyGroupManagerI::setBuddiesToGroup " <<groupName<<"  host:"<<host << " group not found");
    int groupId = 0;
    {
      ConnectionHolder conn("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn.query();
      query << "insert into "<<BuddyGroupManagerI::getTableName(
          "relation_group", host)<<" (host,name) values ("<< host <<","
        <<mysqlpp::quote <<groupName<<")";
      groupId = query.execute().insert_id();
    }
    //              sql.str("");
    {
      ConnectionHolder conn1("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn1.query();
      query << "insert into  "<<BuddyGroupManagerI::getTableName(
          "relation_group_item", host) <<" (host,guest,groupid) values ";
      for (size_t i = 0; i < buddies.size(); ++i) {
        if (i) {
          query <<",";
        }

        query <<"("<<host<<","<<buddies.at(i)<<","<<groupId<<")";
      };
      query.execute();
    }
    //yuanfei
    BuddyGroupPtr bgPtr = new BuddyGroup();
    bgPtr->groupId = groupId;
    for (size_t i = 0; i < buddies.size(); ++i) {
      bgPtr->buddyIds.insert(buddies.at(i));
    }
    wptr->value[groupName] = bgPtr;
    MCE_DEBUG("BuddyGroupManagerI::setBuddiesToGroup " <<groupName<<"  host:"<<host << " group not found done.");
    //
  }
  //notify im
  if (_notifyIm) {
    if (buddies.empty()) {
      return;
    }
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;

    for (size_t i = 0; i < buddies.size(); ++i) {
      bi->items[buddies.at(i)] = _getBuddyGroups(host, buddies.at(i));
    }
    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }
  //---------
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
  MCE_DEBUG("BuddyGroupManagerI::setBuddiesToGroup " <<groupName<<"  host:"<<host << " END.");
}

void BuddyGroupManagerI::setBuddiesToGroupWithGid(Ice::Int host,
    Ice::Int groupId, const MyUtil::IntSeq & buddies,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupId:" <<groupId << " buddies.size:" << buddies.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);
  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);

  string groupName = "";
  BuddyGroupIdMap::iterator iit = wptr->idmap.find(groupId);
  if ( iit != wptr->idmap.end()){
    groupName = iit->second;
  }
  _cleanGroup(host,groupName);
  if (buddies.empty()) {
    return;
  }

  BuddyGroupMap::iterator it = wptr->value.find(groupName);
  if (it != wptr->value.end()) {
    MCE_DEBUG("BuddyGroupManagerI::setBuddiesToGroupWithGid " <<groupName<<"  host:"<<host << " group found");
    it->second->buddyIds.insert(buddies.begin(),buddies.end());

    if(!buddies.empty()){
      ConnectionHolder conn("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn.query();
      query <<"insert into  "<<BuddyGroupManagerI::getTableName(
          "relation_group_item", host) <<" (host,guest,groupid) values ";
      for (size_t i = 0; i < buddies.size(); ++i) {
        if (i) {
          query <<",";
        }
        query <<"("<<host<<","<<buddies.at(i)<<","<<it->second->groupId
          <<")";

      }
      query.execute();
    }
    MCE_DEBUG("BuddyGroupManagerI::setBuddiesToGroupWithGid " <<groupName<<"  host:"<<host << " group found done.");
  } 
  else {
  }
  //notify im
  if (_notifyIm) {
    if (buddies.empty()) {
      return;
    }
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;

    for (size_t i = 0; i < buddies.size(); ++i) {
      bi->items[buddies.at(i)] = _getBuddyGroups(host, buddies.at(i));
    }
    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }
  //---------
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
  MCE_DEBUG("BuddyGroupManagerI::setBuddiesToGroup " <<groupName<<"  host:"<<host << " END.");
}

void BuddyGroupManagerI::removeBuddyFromGroup(Ice::Int host,
    const string& groupName, Ice::Int buddy, const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupName:" <<groupName << " buddy:" << buddy;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  if (!isUtf8(groupName)) {
    MCE_WARN("BuddyGroupManagerI::removeBuddyFromGroup --> Illegal GroupName:"
        <<groupName<<"  host:"<<host);
    return;
  }

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  //GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);


  BuddyGroupMap::iterator it = wptr->value.find(groupName);
  if (it != wptr->value.end()) {
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query << "delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where host = " << host
      << " and guest = "<<buddy <<" and  groupid = "
      << it->second->groupId;
    query.execute();
    it->second->buddyIds.erase(buddy);
  }

  //notify im
  if (_notifyIm) {
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;
    bi->items[buddy] = _getBuddyGroups(host, buddy);

    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }
  //---------
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
}

void BuddyGroupManagerI::removeBuddyFromGroupWithGid(Ice::Int host,
    Ice::Int groupId, Ice::Int buddy, const Ice::Current& current) {
  MCE_INFO("BuddyGroupManagerI::removeBuddyFromGroupWithGid host: " <<host << 
      " groupId: " << groupId << " buddy: " << buddy);
  ostringstream msg;
  msg << " host:" << host << " groupId:" <<groupId << " buddy:" << buddy;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  //GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);

  string groupName = "";
  BuddyGroupIdMap::iterator iit = wptr->idmap.find(groupId);
  if (iit != wptr->idmap.end() ){
    groupName = iit->second;
  }
  BuddyGroupMap::iterator it = wptr->value.find(groupName);
  if (it != wptr->value.end()) {
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query << "delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where host = " << host
      << " and guest = "<<buddy <<" and  groupid = "
      << it->second->groupId;
    query.execute();
    it->second->buddyIds.erase(buddy);
  }

  //notify im
  if (_notifyIm) {
    BuddyItemPtr bi = new BuddyItem();
    bi->host = host;
    bi->items[buddy] = _getBuddyGroups(host, buddy);

    IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  }
  //---------
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
}

void BuddyGroupManagerI::removeBuddy(Ice::Int host, Ice::Int buddy,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " buddy:" <<buddy;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);
  if (!bgdptr) {
    return;
  }
  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  for (BuddyGroupMap::iterator it = wptr->value.begin(); it
      != wptr->value.end(); ++it) {
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query << "delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where host = " << host
      << " and guest = "<<buddy << " and groupid = "
      << it->second->groupId;
    query.execute();
    it->second->buddyIds.erase(buddy);
  }

  //notify im

  //	BuddyItemPtr bi = new BuddyItem();
  //	bi->host = host;
  //	bi->items[buddy] = StrSeq();
  //
  //	IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
  //---------
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
}

void BuddyGroupManagerI::changeBuddyGroupsForWeb(Ice::Int host, Ice::Int buddy,
    const StrSeq& nameSeq, const Ice::Current& current) {
  //changeBuddyGroups(host, buddy, nameSeq, current);
  ostringstream msg;
  msg << " host:" << host << " buddy:" << buddy << " nameSeq.size:" << nameSeq.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  for (size_t i = 0; i < nameSeq.size(); ++i) {
    if (!isUtf8(nameSeq.at(i))) {

      MCE_WARN("BuddyGroupManagerI::changeBuddyGroupsForWeb --> Illegal GroupName:"
          <<nameSeq.at(i)<<"  host:"<<host);
      return;
    }
  }

  //	IceUtil::Mutex::Lock lock(_changeGroupLock);
  MCE_DEBUG("call BuddyGroupManagerI::changeBuddyGroupsForWeb  ->  host:" <<host
      <<"   buddy:"<<buddy);

  Relationship r;
  r.from = host;
  r.to = buddy;

  /*	BuddyDesc desc = BuddyCoreAdapter::instance().getRelation(r);

      MCE_DEBUG("desc:"<<desc);
      if (desc != Friend) {
      MCE_DEBUG("BuddyGroupManagerI::changeBuddyGroups --> host:"<<host
      << "   desc:"<<desc<<"  not firend");
      return desc;
      }
   */
  set<string> nameSet;
  for (size_t i = 0; i<nameSeq.size(); ++i) {
    nameSet.insert(nameSeq.at(i));
  }
  MyUtil::IntSeq addToGroupItem;
  MyUtil::IntSeq deleteGroupId;
  MyUtil::StrSeq addGroup;

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  //GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);

  set<string>::iterator it1 = nameSet.begin();
  BuddyGroupMap::iterator it2 = wptr->value.begin();

  vector<string> removeGroup;

  while ((it1 != nameSet.end()) && (it2 != wptr->value.end())) {
    if (*it1 < it2->first) {
      MCE_DEBUG("it1:"<<*it1<<"    it2:"<<it2->first);
      addGroup.push_back(*it1);
      it1++;
    } else if (*it1 > it2->first) {
      if (it2->second->buddyIds.find(buddy)
          != it2->second->buddyIds.end()) {
        deleteGroupId.push_back(it2->second->groupId);
        it2->second->buddyIds.erase(buddy);
        if (it2->second->buddyIds.empty()) {
          removeGroup.push_back(it2->first);
        }
      }
      it2++;
    } else {
      if (it2->second->buddyIds.insert(buddy).second) {
        addToGroupItem.push_back(it2->second->groupId);
      }
      it1++;
      it2++;
    }
  }
  while (it1 != nameSet.end()) {
    if (wptr->value.find(*it1) == wptr->value.end()) {
      addGroup.push_back(*it1);
    }
    it1++;
  }
  while (it2 != wptr->value.end()) {
    if (it2->second->buddyIds.find(buddy) != it2->second->buddyIds.end()) {
      deleteGroupId.push_back(it2->second->groupId);
      it2->second->buddyIds.erase(buddy);
      if (it2->second->buddyIds.empty()) {
        removeGroup.push_back(it2->first);
      }
    }
    it2++;
  }

  MCE_DEBUG("host: "<<host<<"  addToGroupItem.size:"<<addToGroupItem.size()
      <<"    deleteGroupId.size:"<<deleteGroupId.size()
      <<"   addGroup.size:"<<addGroup.size());
  if (addToGroupItem.empty() && deleteGroupId.empty() && addGroup.empty()) {
    MCE_DEBUG("host:"<<host<<"   return -1");

    return;
  }

  //	QueryRunner conn("buddygroup", CDbWServer,
  //			BuddyGroupManagerI::getTableName("relation_group", host));

  if (!addToGroupItem.empty()) {
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    //		ostringstream sql;
    query <<"insert into  "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" (host,guest,groupid) values ";
    for (size_t i = 0; i < addToGroupItem.size(); ++i) {
      if (i) {
        query<<",";
      }
      query<<"("<<host<<","<<buddy<<"," <<addToGroupItem.at(i)<<")";
    }
    query.execute();
  }

  if (!deleteGroupId.empty()) {
    //QueryRunner conn("buddygroup", CDbWServer);

    //		ostringstream sql;
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query << "delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where host = " << host
      << " and guest = "<<buddy <<" and  groupid in (";
    for (size_t i = 0; i < deleteGroupId.size(); ++i) {
      if (i) {
        query<<",";
      }
      query<<deleteGroupId.at(i);
    }
    query << ")";
    query.execute();
  }

  if (!addGroup.empty()) {
    //QueryRunner conn("buddygroup", CDbWServer);
    //		ostringstream sql;
    int groupId = -1;
    {
      ConnectionHolder conn("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn.query();
      query<<"insert into "<<BuddyGroupManagerI::getTableName(
          "relation_group", host)<<" (host,name) values ";

      for (size_t i = 0; i < addGroup.size(); ++i) {
        if (i) {
          query<<",";
        }
        query<<"("<<host<<","<<mysqlpp::quote<<addGroup.at(i)<<")";
      }
      groupId = query.execute().insert_id();
    }
    if (groupId != -1) {
      {
        ConnectionHolder conn("buddygroup", CDbWServer,
            BuddyGroupManagerI::getTableName("relation_group", host));
        mysqlpp::Query query = conn.query();
        //			sql.str("");
        query <<"insert into "<<BuddyGroupManagerI::getTableName(
            "relation_group_item", host)
          <<" (host,guest,groupid) values ";
        for (size_t i = 0; i < addGroup.size(); ++i) {
          if (i) {
            query<<",";
          }
          query<<"("<<host<<","<<buddy<<","<<groupId+i<<")";
        }
        query.execute();
      }
      //------------------------------------
      for (size_t i = 0; i < addGroup.size(); ++i) {
        BuddyGroupPtr bgPtr = new BuddyGroup();
        bgPtr->groupId = groupId + i;
        bgPtr->buddyIds.insert(buddy);
        wptr->value[addGroup.at(i)] = bgPtr;
      }
    }
  }

  //delete empty group
  if (!removeGroup.empty()) {
    //		ostringstream sql;
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query<<"delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group", host)<<"  where host="<<host
      <<" and name in (";

    for (size_t i = 0; i < removeGroup.size(); ++i) {
      BuddyGroupMap::iterator it = wptr->value.find(removeGroup.at(i));
      if (it != wptr->value.end()) {
        wptr->value.erase(it);
      }
      if (i) {
        query <<",";
      }
      query <<mysqlpp::quote<< removeGroup.at(i);
    }
    query << ")";
    //		QueryRunner conn("buddygroup", CDbWServer,
    //				BuddyGroupManagerI::getTableName("relation_group", host));
    query.execute();
  }
  //notify im
  if (_notifyIm) {
    if (current.ctx.find("FROM") != current.ctx.end()) {
      BuddyItemPtr bi = new BuddyItem();
      bi->host = host;
      bi->items[buddy] = nameSeq;

      IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
    }
  }
  //---------
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
  //MCE_DEBUG("host:"<<host<<"  return desc:"<<desc);
}

void BuddyGroupManagerI::changeBuddyGroupsForWebWithGid(Ice::Int host, Ice::Int buddy,
    const IntSeq& groupIdSeq, const Ice::Current& current) {
  //changeBuddyGroups(host, buddy, nameSeq, current);
  ostringstream msg;
  msg << " host:" << host << " buddy:" << buddy << " groupIdSeq.size:" << groupIdSeq.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);
  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);

  StrSeq nameSeq;
  BuddyGroupIdMap::iterator iit;
  for(size_t i = 0; i < groupIdSeq.size(); ++i){
    iit = wptr->idmap.find(groupIdSeq.at(i));
    if(iit == wptr->idmap.end()){
      MCE_WARN("BuddyGroupManagerI::changeBuddyGroupsForWebWithGid --> Illegal GroupId:"
          << groupIdSeq.at(i)<<"  host:"<<host);
      return;
    }
  } 

  for (size_t i = 0; i < nameSeq.size(); ++i) {
    if (!isUtf8(nameSeq.at(i))) {

      MCE_WARN("BuddyGroupManagerI::changeBuddyGroupsForWebWithGid --> Illegal GroupName:"
          <<nameSeq.at(i)<<"  host:"<<host);
      return;
    }
  }

  Relationship r;
  r.from = host;
  r.to = buddy;

  /*	BuddyDesc desc = BuddyCoreAdapter::instance().getRelation(r);

      MCE_DEBUG("desc:"<<desc);
      if (desc != Friend) {
      MCE_DEBUG("BuddyGroupManagerI::changeBuddyGroups --> host:"<<host
      << "   desc:"<<desc<<"  not firend");
      return desc;
      }
   */
  set<string> nameSet;
  for (size_t i = 0; i<nameSeq.size(); ++i) {
    nameSet.insert(nameSeq.at(i));
  }
  MyUtil::IntSeq addToGroupItem;
  MyUtil::IntSeq deleteGroupId;
  MyUtil::StrSeq addGroup;

  set<string>::iterator it1 = nameSet.begin();
  BuddyGroupMap::iterator it2 = wptr->value.begin();

  vector<string> removeGroup;

  while ((it1 != nameSet.end()) && (it2 != wptr->value.end())) {
    if (*it1 < it2->first) {
      MCE_DEBUG("it1:"<<*it1<<"    it2:"<<it2->first);
      addGroup.push_back(*it1);
      it1++;
    } else if (*it1 > it2->first) {
      if (it2->second->buddyIds.find(buddy)
          != it2->second->buddyIds.end()) {
        deleteGroupId.push_back(it2->second->groupId);
        it2->second->buddyIds.erase(buddy);
        if (it2->second->buddyIds.empty()) {
          removeGroup.push_back(it2->first);
        }
      }
      it2++;
    } else {
      if (it2->second->buddyIds.insert(buddy).second) {
        addToGroupItem.push_back(it2->second->groupId);
      }
      it1++;
      it2++;
    }
  }
  while (it1 != nameSet.end()) {
    if (wptr->value.find(*it1) == wptr->value.end()) {
      addGroup.push_back(*it1);
    }
    it1++;
  }
  while (it2 != wptr->value.end()) {
    if (it2->second->buddyIds.find(buddy) != it2->second->buddyIds.end()) {
      deleteGroupId.push_back(it2->second->groupId);
      it2->second->buddyIds.erase(buddy);
      if (it2->second->buddyIds.empty()) {
        removeGroup.push_back(it2->first);
      }
    }
    it2++;
  }

  MCE_DEBUG("host: "<<host<<"  addToGroupItem.size:"<<addToGroupItem.size()
      <<"    deleteGroupId.size:"<<deleteGroupId.size()
      <<"   addGroup.size:"<<addGroup.size());
  if (addToGroupItem.empty() && deleteGroupId.empty() && addGroup.empty()) {
    MCE_DEBUG("host:"<<host<<"   return -1");

    return;
  }

  //	QueryRunner conn("buddygroup", CDbWServer,
  //			BuddyGroupManagerI::getTableName("relation_group", host));

  if (!addToGroupItem.empty()) {
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    //		ostringstream sql;
    query <<"insert into  "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" (host,guest,groupid) values ";
    for (size_t i = 0; i < addToGroupItem.size(); ++i) {
      if (i) {
        query<<",";
      }
      query<<"("<<host<<","<<buddy<<"," <<addToGroupItem.at(i)<<")";
    }
    query.execute();
  }

  if (!deleteGroupId.empty()) {
    //QueryRunner conn("buddygroup", CDbWServer);

    //		ostringstream sql;
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query << "delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where host = " << host
      << " and guest = "<<buddy <<" and  groupid in (";
    for (size_t i = 0; i < deleteGroupId.size(); ++i) {
      if (i) {
        query<<",";
      }
      query<<deleteGroupId.at(i);
    }
    query << ")";
    query.execute();
  }

  if (!addGroup.empty()) {
    //QueryRunner conn("buddygroup", CDbWServer);
    //		ostringstream sql;
    int groupId = -1;
    {
      ConnectionHolder conn("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn.query();
      query<<"insert into "<<BuddyGroupManagerI::getTableName(
          "relation_group", host)<<" (host,name) values ";

      for (size_t i = 0; i < addGroup.size(); ++i) {
        if (i) {
          query<<",";
        }
        query<<"("<<host<<","<<mysqlpp::quote<<addGroup.at(i)<<")";
      }
      groupId = query.execute().insert_id();
    }
    if (groupId != -1) {
      {
        ConnectionHolder conn("buddygroup", CDbWServer,
            BuddyGroupManagerI::getTableName("relation_group", host));
        mysqlpp::Query query = conn.query();
        //			sql.str("");
        query <<"insert into "<<BuddyGroupManagerI::getTableName(
            "relation_group_item", host)
          <<" (host,guest,groupid) values ";
        for (size_t i = 0; i < addGroup.size(); ++i) {
          if (i) {
            query<<",";
          }
          query<<"("<<host<<","<<buddy<<","<<groupId+i<<")";
        }
        query.execute();
      }
      //------------------------------------
      for (size_t i = 0; i < addGroup.size(); ++i) {
        BuddyGroupPtr bgPtr = new BuddyGroup();
        bgPtr->groupId = groupId + i;
        bgPtr->buddyIds.insert(buddy);
        wptr->value[addGroup.at(i)] = bgPtr;
      }
    }
  }

  //delete empty group
  if (!removeGroup.empty()) {
    //		ostringstream sql;
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query<<"delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group", host)<<"  where host="<<host
      <<" and name in (";

    for (size_t i = 0; i < removeGroup.size(); ++i) {
      BuddyGroupMap::iterator it = wptr->value.find(removeGroup.at(i));
      if (it != wptr->value.end()) {
        wptr->value.erase(it);
      }
      if (i) {
        query <<",";
      }
      query <<mysqlpp::quote<< removeGroup.at(i);
    }
    query << ")";
    //		QueryRunner conn("buddygroup", CDbWServer,
    //				BuddyGroupManagerI::getTableName("relation_group", host));
    query.execute();
  }
  //notify im
  if (_notifyIm) {
    if (current.ctx.find("FROM") != current.ctx.end()) {
      BuddyItemPtr bi = new BuddyItem();
      bi->host = host;
      bi->items[buddy] = nameSeq;

      IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
    }
  }
  //---------
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
  //MCE_DEBUG("host:"<<host<<"  return desc:"<<desc);
}

int BuddyGroupManagerI::changeBuddyGroups(Ice::Int host, Ice::Int buddy,
    const MyUtil::StrSeq& nameSeq, const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " buddy:" <<  buddy <<  " nameSeq.size:" << nameSeq.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  //delete illegal groups

  for (size_t i = 0; i < nameSeq.size(); ++i) {
    if (!isUtf8(nameSeq.at(i))) {

      MCE_WARN("BuddyGroupManagerI::changeBuddyGroups --> Illegal GroupName:"
          <<nameSeq.at(i)<<"  host:"<<host);
      return -1;
    }
  }

  //	IceUtil::Mutex::Lock lock(_changeGroupLock);
  MCE_DEBUG("call BuddyGroupManagerI::changeBuddyGroups  ->  host:" <<host
      <<"   buddy:"<<buddy);

  Relationship r;
  r.from = host;
  r.to = buddy;

  BuddyDesc desc = BuddyCoreAdapter::instance().getRelation(r);

  MCE_DEBUG("desc:"<<desc);
  if (desc != Friend) {
    MCE_DEBUG("BuddyGroupManagerI::changeBuddyGroups --> host:"<<host
        << "   desc:"<<desc<<"  not firend");
    return desc;
  }

  set<string> nameSet;
  for (size_t i = 0; i<nameSeq.size(); ++i) {
    nameSet.insert(nameSeq.at(i));
  }
  MyUtil::IntSeq addToGroupItem;
  MyUtil::IntSeq deleteGroupId;
  MyUtil::StrSeq addGroup;

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  //GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);

  set<string>::iterator it1 = nameSet.begin();
  BuddyGroupMap::iterator it2 = wptr->value.begin();

  vector<string> removeGroup;

  while ((it1 != nameSet.end()) && (it2 != wptr->value.end())) {
    if (*it1 < it2->first) {
      MCE_DEBUG("it1:"<<*it1<<"    it2:"<<it2->first);
      addGroup.push_back(*it1);
      it1++;
    } else if (*it1 > it2->first) {
      if (it2->second->buddyIds.find(buddy)
          != it2->second->buddyIds.end()) {
        deleteGroupId.push_back(it2->second->groupId);
        it2->second->buddyIds.erase(buddy);
        if (it2->second->buddyIds.empty()) {
          removeGroup.push_back(it2->first);
        }
      }
      it2++;
    } else {
      if (it2->second->buddyIds.insert(buddy).second) {
        addToGroupItem.push_back(it2->second->groupId);
      }
      it1++;
      it2++;
    }
  }
  while (it1 != nameSet.end()) {
    if (wptr->value.find(*it1) == wptr->value.end()) {
      addGroup.push_back(*it1);
    }
    it1++;
  }
  while (it2 != wptr->value.end()) {
    if (it2->second->buddyIds.find(buddy) != it2->second->buddyIds.end()) {
      deleteGroupId.push_back(it2->second->groupId);
      it2->second->buddyIds.erase(buddy);
      if (it2->second->buddyIds.empty()) {
        removeGroup.push_back(it2->first);
      }
    }
    it2++;
  }

  MCE_DEBUG("host: "<<host<<"  addToGroupItem.size:"<<addToGroupItem.size()
      <<"    deleteGroupId.size:"<<deleteGroupId.size()
      <<"   addGroup.size:"<<addGroup.size());
  if (addToGroupItem.empty() && deleteGroupId.empty() && addGroup.empty()) {
    MCE_DEBUG("host:"<<host<<"   return -1");
    return -1;
  }

  //	QueryRunner conn("buddygroup", CDbWServer,
  //			BuddyGroupManagerI::getTableName("relation_group", host));

  if (!addToGroupItem.empty()) {
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    //		ostringstream sql;
    query <<"insert into  "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" (host,guest,groupid) values ";
    for (size_t i = 0; i < addToGroupItem.size(); ++i) {
      if (i) {
        query<<",";
      }
      query<<"("<<host<<","<<buddy<<"," <<addToGroupItem.at(i)<<")";
    }
    query.execute();
  }

  if (!deleteGroupId.empty()) {
    //QueryRunner conn("buddygroup", CDbWServer);

    //		ostringstream sql;
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query << "delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where host = " << host
      << " and guest = "<<buddy <<" and  groupid in (";
    for (size_t i = 0; i < deleteGroupId.size(); ++i) {
      if (i) {
        query<<",";
      }
      query<<deleteGroupId.at(i);
    }
    query << ")";
    query.execute();
  }

  if (!addGroup.empty()) {
    //QueryRunner conn("buddygroup", CDbWServer);
    //		ostringstream sql;
    int groupId = -1;
    {
      ConnectionHolder conn("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn.query();
      query<<"insert into "<<BuddyGroupManagerI::getTableName(
          "relation_group", host)<<" (host,name) values ";

      for (size_t i = 0; i < addGroup.size(); ++i) {
        if (i) {
          query<<",";
        }
        query<<"("<<host<<","<<mysqlpp::quote<<addGroup.at(i)<<")";
      }
      groupId = query.execute().insert_id();
    }
    if (groupId != -1) {
      {
        ConnectionHolder conn("buddygroup", CDbWServer,
            BuddyGroupManagerI::getTableName("relation_group", host));
        mysqlpp::Query query = conn.query();
        //			sql.str("");
        query <<"insert into "<<BuddyGroupManagerI::getTableName(
            "relation_group_item", host)
          <<" (host,guest,groupid) values ";
        for (size_t i = 0; i < addGroup.size(); ++i) {
          if (i) {
            query<<",";
          }
          query<<"("<<host<<","<<buddy<<","<<groupId+i<<")";
        }
        query.execute();
      }
      //------------------------------------
      for (size_t i = 0; i < addGroup.size(); ++i) {
        BuddyGroupPtr bgPtr = new BuddyGroup();
        bgPtr->groupId = groupId + i;
        bgPtr->buddyIds.insert(buddy);
        wptr->value[addGroup.at(i)] = bgPtr;
      }
    }
  }

  //delete empty group
  if (!removeGroup.empty()) {
    //		ostringstream sql;
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query<<"delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group", host)<<"  where host="<<host
      <<" and name in (";

    for (size_t i = 0; i < removeGroup.size(); ++i) {
      BuddyGroupMap::iterator it = wptr->value.find(removeGroup.at(i));
      if (it != wptr->value.end()) {
        wptr->value.erase(it);
      }
      if (i) {
        query <<",";
      }
      query <<mysqlpp::quote<< removeGroup.at(i);
    }
    query << ")";
    //		QueryRunner conn("buddygroup", CDbWServer,
    //				BuddyGroupManagerI::getTableName("relation_group", host));
    query.execute();
  }
  //notify im
  if (_notifyIm) {
    if (current.ctx.find("FROM") != current.ctx.end()) {
      BuddyItemPtr bi = new BuddyItem();
      bi->host = host;
      bi->items[buddy] = nameSeq;

      IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
    }
  }
  //---------
  MCE_DEBUG("host:"<<host<<"  return desc:"<<desc);
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
  return desc;
}

int BuddyGroupManagerI::changeBuddyGroupsWithGid(Ice::Int host, Ice::Int buddy,
    const MyUtil::IntSeq& groupIdSeq, const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " buddy:" << buddy << " groupIdSeq.size:" <<groupIdSeq.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  //delete illegal groups
  MCE_DEBUG("BuddyGroupManagerI::changeBuddyGroupsWithGid  ->  host:" <<host
      <<"   buddy:"<<buddy);
  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);
  IceUtil::Mutex::Lock lock(*bgdptr);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  MyUtil::StrSeq nameSeq;
  BuddyGroupIdMap::iterator iit;
  for (size_t i = 0; i < groupIdSeq.size(); ++i) {
    iit = wptr->idmap.find(groupIdSeq.at(i));
    if (iit == wptr->idmap.end()){
      MCE_WARN("BuddyGroupManagerI::changeBuddyGroups --> Illegal GroupId:"
          <<groupIdSeq.at(i)<<"  host:"<<host);
      return -1;
    }
  }
  for (size_t i = 0; i < nameSeq.size(); ++i) {
    if (!isUtf8(nameSeq.at(i))) {

      MCE_WARN("BuddyGroupManagerI::changeBuddyGroups --> Illegal GroupName:"
          <<nameSeq.at(i)<<"  host:"<<host);
      return -1;
    }
  }

  Relationship r;
  r.from = host;
  r.to = buddy;

  BuddyDesc desc = BuddyCoreAdapter::instance().getRelation(r);

  MCE_DEBUG("desc:"<<desc);
  if (desc != Friend) {
    MCE_DEBUG("BuddyGroupManagerI::changeBuddyGroups --> host:"<<host
        << "   desc:"<<desc<<"  not firend");
    return desc;
  }

  set<string> nameSet;
  for (size_t i = 0; i<nameSeq.size(); ++i) {
    nameSet.insert(nameSeq.at(i));
  }
  MyUtil::IntSeq addToGroupItem;
  MyUtil::IntSeq deleteGroupId;
  MyUtil::StrSeq addGroup;

  set<string>::iterator it1 = nameSet.begin();
  BuddyGroupMap::iterator it2 = wptr->value.begin();

  vector<string> removeGroup;

  while ((it1 != nameSet.end()) && (it2 != wptr->value.end())) {
    if (*it1 < it2->first) {
      MCE_DEBUG("it1:"<<*it1<<"    it2:"<<it2->first);
      addGroup.push_back(*it1);
      it1++;
    } else if (*it1 > it2->first) {
      if (it2->second->buddyIds.find(buddy)
          != it2->second->buddyIds.end()) {
        deleteGroupId.push_back(it2->second->groupId);
        it2->second->buddyIds.erase(buddy);
        if (it2->second->buddyIds.empty()) {
          removeGroup.push_back(it2->first);
        }
      }
      it2++;
    } else {
      if (it2->second->buddyIds.insert(buddy).second) {
        addToGroupItem.push_back(it2->second->groupId);
      }
      it1++;
      it2++;
    }
  }
  while (it1 != nameSet.end()) {
    if (wptr->value.find(*it1) == wptr->value.end()) {
      addGroup.push_back(*it1);
    }
    it1++;
  }
  while (it2 != wptr->value.end()) {
    if (it2->second->buddyIds.find(buddy) != it2->second->buddyIds.end()) {
      deleteGroupId.push_back(it2->second->groupId);
      it2->second->buddyIds.erase(buddy);
      if (it2->second->buddyIds.empty()) {
        removeGroup.push_back(it2->first);
      }
    }
    it2++;
  }

  MCE_DEBUG("host: "<<host<<"  addToGroupItem.size:"<<addToGroupItem.size()
      <<"    deleteGroupId.size:"<<deleteGroupId.size()
      <<"   addGroup.size:"<<addGroup.size());
  if (addToGroupItem.empty() && deleteGroupId.empty() && addGroup.empty()) {
    MCE_DEBUG("host:"<<host<<"   return -1");
    return -1;
  }

  //	QueryRunner conn("buddygroup", CDbWServer,
  //			BuddyGroupManagerI::getTableName("relation_group", host));

  if (!addToGroupItem.empty()) {
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    //		ostringstream sql;
    query <<"insert into  "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" (host,guest,groupid) values ";
    for (size_t i = 0; i < addToGroupItem.size(); ++i) {
      if (i) {
        query<<",";
      }
      query<<"("<<host<<","<<buddy<<"," <<addToGroupItem.at(i)<<")";
    }
    query.execute();
  }

  if (!deleteGroupId.empty()) {
    //QueryRunner conn("buddygroup", CDbWServer);

    //		ostringstream sql;
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query << "delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", host)<<" where host = " << host
      << " and guest = "<<buddy <<" and  groupid in (";
    for (size_t i = 0; i < deleteGroupId.size(); ++i) {
      if (i) {
        query<<",";
      }
      query<<deleteGroupId.at(i);
    }
    query << ")";
    query.execute();
  }

  if (!addGroup.empty()) {
    //QueryRunner conn("buddygroup", CDbWServer);
    //		ostringstream sql;
    int groupId = -1;
    {
      ConnectionHolder conn("buddygroup", CDbWServer,
          BuddyGroupManagerI::getTableName("relation_group", host));
      mysqlpp::Query query = conn.query();
      query<<"insert into "<<BuddyGroupManagerI::getTableName(
          "relation_group", host)<<" (host,name) values ";

      for (size_t i = 0; i < addGroup.size(); ++i) {
        if (i) {
          query<<",";
        }
        query<<"("<<host<<","<<mysqlpp::quote<<addGroup.at(i)<<")";
      }
      groupId = query.execute().insert_id();
    }
    if (groupId != -1) {
      {
        ConnectionHolder conn("buddygroup", CDbWServer,
            BuddyGroupManagerI::getTableName("relation_group", host));
        mysqlpp::Query query = conn.query();
        //			sql.str("");
        query <<"insert into "<<BuddyGroupManagerI::getTableName(
            "relation_group_item", host)
          <<" (host,guest,groupid) values ";
        for (size_t i = 0; i < addGroup.size(); ++i) {
          if (i) {
            query<<",";
          }
          query<<"("<<host<<","<<buddy<<","<<groupId+i<<")";
        }
        query.execute();
      }
      //------------------------------------
      for (size_t i = 0; i < addGroup.size(); ++i) {
        BuddyGroupPtr bgPtr = new BuddyGroup();
        bgPtr->groupId = groupId + i;
        bgPtr->buddyIds.insert(buddy);
        wptr->value[addGroup.at(i)] = bgPtr;
      }
    }
  }

  //delete empty group
  if (!removeGroup.empty()) {
    //		ostringstream sql;
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", host));
    mysqlpp::Query query = conn.query();
    query<<"delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group", host)<<"  where host="<<host
      <<" and name in (";

    for (size_t i = 0; i < removeGroup.size(); ++i) {
      BuddyGroupMap::iterator it = wptr->value.find(removeGroup.at(i));
      if (it != wptr->value.end()) {
        wptr->value.erase(it);
      }
      if (i) {
        query <<",";
      }
      query <<mysqlpp::quote<< removeGroup.at(i);
    }
    query << ")";
    //		QueryRunner conn("buddygroup", CDbWServer,
    //				BuddyGroupManagerI::getTableName("relation_group", host));
    query.execute();
  }
  //notify im
  if (_notifyIm) {
    if (current.ctx.find("FROM") != current.ctx.end()) {
      BuddyItemPtr bi = new BuddyItem();
      bi->host = host;
      bi->items[buddy] = nameSeq;

      IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
    }
  }
  //---------
  MCE_DEBUG("host:"<<host<<"  return desc:"<<desc);
  ::xce::binstring::BinStringPtr bsptr = wptr->serialize();
  bgdptr->setBP(bsptr);
  return desc;
}

//int BuddyGroupManagerI::buddyChangeGroups(Ice::Int host, Ice::Int buddy,
//		const MyUtil::StrSeq& nameSeq, const Ice::Current&) {
//	return changeBuddyGroups(host, buddy, nameSeq);
//}


//---------------------------------

BuddiedGroupsMap BuddyGroupManagerI::getBuddiedGroupsMap(Ice::Int host,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host ;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->saveGetBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  BuddiedGroupsMap res;
  if (wptr) {
    //GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);
    for (BuddyGroupMap::iterator it = wptr->value.begin(); it
        != wptr->value.end(); ++it) {
      for (set<int>::iterator iit = it->second->buddyIds.begin(); iit
          != it->second->buddyIds.end() ; ++iit) {
        res[*iit].push_back(it->first);
      }
    }

  }
  return res;
}

BuddyGroupsMap BuddyGroupManagerI::getBuddyGroupsMap(Ice::Int host,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host ;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->saveGetBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  BuddyGroupsMap res;
  if (wptr) {
    //GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);
    for (BuddyGroupMap::iterator it = wptr->value.begin(); it
        != wptr->value.end(); ++it) {
      for (set<int>::iterator iit = it->second->buddyIds.begin(); iit
          != it->second->buddyIds.end() ; ++iit) {
        res[*iit].insert(make_pair<int,string>(it->second->groupId,it->first));
      }
    }

  }
  return res;
}

MyUtil::StrSeq BuddyGroupManagerI::getGroupList(Ice::Int host,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host ;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  ::xce::binstring::BinStringPtr sourcebp = bgdptr->saveGetBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  MyUtil::StrSeq res;
  if (wptr) {
    for (BuddyGroupMap::iterator it = wptr->value.begin(); it
        != wptr->value.end(); ++it) {
      res.push_back(it->first);
    }
  }

  return res;
}

MyUtil::Int2StrMap BuddyGroupManagerI::getGroupListData(Ice::Int host,
    const Ice::Current& current){
  ostringstream msg;
  msg << " host:" << host ;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  ::xce::binstring::BinStringPtr sourcebp = bgdptr->saveGetBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  MyUtil::Int2StrMap res;
  if (wptr) {
    for (BuddyGroupMap::iterator it = wptr->value.begin(); it
        != wptr->value.end(); ++it) {
      res.insert(make_pair<int,string>(it->second->groupId,it->first));
    }
  }

  return res;
}

MyUtil::IntSeq BuddyGroupManagerI::getBuddyListWithGid(Ice::Int host,
    const Ice::Int groupId, Ice::Int begin, Ice::Int limit,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupId:" << groupId << " begin:" << begin << " limit:" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);
  ::xce::binstring::BinStringPtr sourcebp = bgdptr->saveGetBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  MyUtil::IntSeq ids;
  if (wptr) {
    BuddyGroupIdMap::iterator iit;
    iit = wptr->idmap.find(groupId);
    if (iit == wptr->idmap.end()){
      MCE_WARN("BuddyGroupManagerI::getBuddyListWithGid --> Illegal GroupId:"
          <<groupId<<"  host:"<<host);
      return ids;
    }
    string groupName =  iit->second;
    map<string,BuddyGroupPtr>::iterator it = wptr->value.find(groupName);
    if (it != wptr->value.end()) {
      if (begin < 0) {
        begin = 0;
      }
      int index = 0;
      for (set<int>::iterator bit = it->second->buddyIds.begin(); bit
          != it->second->buddyIds.end(); ++bit) {
        if (index < begin) {
          index ++;
          continue;
        }
        if (index >= begin+limit && limit >= 0) {
          break;
        }
        ids.push_back(*bit);
        index ++;
      }
    }

  }

  return ids;
}

MyUtil::IntSeq BuddyGroupManagerI::getBuddyList(Ice::Int host, const string& groupName,
    Ice::Int begin, Ice::Int limit, const Ice::Current& current){
  ostringstream msg;
  msg << " host:" << host << " groupName:" << groupName << " begin:" << begin << " limit:" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  ::xce::binstring::BinStringPtr sourcebp = bgdptr->saveGetBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  MyUtil::IntSeq ids;
  if (wptr) {
    map<string,BuddyGroupPtr>::iterator it = wptr->value.find(groupName);
    if (it != wptr->value.end()) {
      if (begin < 0) {
        begin = 0;
      }
      int index = 0;
      for (set<int>::iterator bit = it->second->buddyIds.begin(); bit
          != it->second->buddyIds.end(); ++bit) {
        if (index < begin) {
          index ++;
          continue;
        }
        if (index >= begin+limit && limit >= 0) {
          break;
        }
        ids.push_back(*bit);
        index ++;
      }
    }

  }

  return ids;
}

MyUtil::StrSeq BuddyGroupManagerI::getBuddyGroups(Ice::Int host,
    Ice::Int guest, const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " guest:" << guest;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  ::xce::binstring::BinStringPtr sourcebp = bgdptr->saveGetBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  if (!wptr) {
    return MyUtil::StrSeq();
  }
  MyUtil::StrSeq res;
  for (map<string,BuddyGroupPtr>::iterator it = wptr->value.begin(); it
      != wptr->value.end(); ++it) {
    if (it->second->buddyIds.find(guest) != it->second->buddyIds.end()) {
      res.push_back(it->first);
    }
  }
  return res;
}

MyUtil::Int2StrMap BuddyGroupManagerI::getBuddyGroupsData(Ice::Int host,
    Ice::Int guest, const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " guest:" << guest;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  ::xce::binstring::BinStringPtr sourcebp = bgdptr->saveGetBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  if (!wptr) {
    return MyUtil::Int2StrMap();
  }
  MyUtil::Int2StrMap res;
  for (map<string,BuddyGroupPtr>::iterator it = wptr->value.begin(); it
      != wptr->value.end(); ++it) {
    if (it->second->buddyIds.find(guest) != it->second->buddyIds.end()) {
      res.insert(make_pair<int,string>(it->second->groupId,it->first));
    }
  }
  return res;
}

MyUtil::StrSeq BuddyGroupManagerI::_getBuddyGroups(Ice::Int host, Ice::Int guest) {
  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  ::xce::binstring::BinStringPtr sourcebp = bgdptr->getBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  if (!wptr) {
    return MyUtil::StrSeq();
  }

  MyUtil::StrSeq res;
  for (map<string,BuddyGroupPtr>::iterator it = wptr->value.begin(); it
      != wptr->value.end(); ++it) {
    if (it->second->buddyIds.find(guest) != it->second->buddyIds.end()) {
      res.push_back(it->first);
    }
  }
  return res;
}

Ice::Int BuddyGroupManagerI::getCountByGroup(Ice::Int host,
    const string& groupName, const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupName:" <<groupName;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  ::xce::binstring::BinStringPtr sourcebp = bgdptr->saveGetBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  if (wptr) {
    map<string,BuddyGroupPtr>::iterator it = wptr->value.find(groupName);
    if (it != wptr->value.end()) {
      return it->second->buddyIds.size();
    }
  }
  return 0;
}

Ice::Int BuddyGroupManagerI::getCountByGroupId(Ice::Int host,
    const Ice::Int groupId, const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host << " groupId:" << groupId;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  ::xce::binstring::BinStringPtr sourcebp = bgdptr->saveGetBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  if (wptr) {
    BuddyGroupIdMap::iterator iit;
    iit = wptr->idmap.find(groupId);
    if (iit == wptr->idmap.end()){
      MCE_WARN("BuddyGroupManagerI::getBuddyListWithGid --> Illegal GroupId:"
          <<groupId<<"  host:"<<host);
      return -1;
    }
    string groupName =  iit->second;
    map<string,BuddyGroupPtr>::iterator it = wptr->value.find(groupName);
    if (it != wptr->value.end()) {
      return it->second->buddyIds.size();
    }
  }
  return 0;
}

MyUtil::Str2IntMap BuddyGroupManagerI::getAllGroupsBuddyCount(int host,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  ::xce::binstring::BinStringPtr sourcebp = bgdptr->saveGetBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  MyUtil::Str2IntMap counts;
  if (wptr) {
    map<string,BuddyGroupPtr>::iterator it = wptr->value.begin();
    for (; it!=wptr->value.end(); ++it) {
      counts[it->first] = it->second->buddyIds.size();
    }
  }
  return counts;
}

MyUtil::Int2IntMap BuddyGroupManagerI::getAllGroupsBuddyCountData(int host,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " host:" << host ;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);

  ::xce::binstring::BinStringPtr sourcebp = bgdptr->saveGetBP();
  GroupMapWrapperPtr wptr = new GroupMapWrapper(sourcebp);
  MyUtil::Int2IntMap counts; 
  if (wptr) {
    map<string,BuddyGroupPtr>::iterator it = wptr->value.begin();
    for (; it!=wptr->value.end(); ++it) {
      counts[it->second->groupId] = it->second->buddyIds.size();
    }
  } 
  return counts;
}

void BuddyGroupManagerI::load(Ice::Int host, const Ice::Current& current){
  ostringstream msg;
  msg << " host:" << host;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  BuddyGroupFactoryIPtr factory = new BuddyGroupFactoryI();
  BuddyGroupDataPtr data = BuddyGroupDataPtr::dynamicCast(factory->create(host));
  ServiceI::instance().addObject(data, 0, host);
}

void BuddyGroupManagerI::saveLoad(::Ice::Int host, const Ice::Current& current){
  ostringstream msg;
  msg << " host:" << host;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  BuddyGroupDataPtr bgdptr = ServiceI::instance().locateObject<BuddyGroupDataPtr>(0, host);
  IceUtil::Mutex::Lock lock(*bgdptr);
  BuddyGroupFactoryIPtr factory = new BuddyGroupFactoryI();
  BuddyGroupDataPtr data = BuddyGroupDataPtr::dynamicCast(factory->create(host));
  ServiceI::instance().addObject(data, 0, host);
}
//----------------------------
void CleanIllegalDataTask::handle() {

  if (_groups.empty()) {
    return;
  }
  //	QueryRunner conn("buddygroup", CDbWServer,
  //			BuddyGroupManagerI::getTableName("relation_group", _host));
  {
    ConnectionHolder conn("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", _host));
    mysqlpp::Query query = conn.query();
    //	ostringstream sql;
    query<<"delete from "<<BuddyGroupManagerI::getTableName("relation_group",
        _host)<<"  where host="<<_host <<" and name in (";

    for (map<int,string>::iterator it = _groups.begin(); it != _groups.end(); ++it) {
      MCE_INFO("Illegal GroupName:"<< it->second<< " GroupId:"<<it->first
          << " host:"<<_host);

      if (it != _groups.begin()) {
        query<<",";
      }
      query<<mysqlpp::quote <<it->second;
    }
    query<<")";

    query.execute();
  }
  //	sql.str("");
  {
    ConnectionHolder conn1("buddygroup", CDbWServer,
        BuddyGroupManagerI::getTableName("relation_group", _host));
    mysqlpp::Query query = conn1.query();
    query<<"delete from "<<BuddyGroupManagerI::getTableName(
        "relation_group_item", _host)<<" where groupid in (";

    for (map<int,string>::iterator it = _groups.begin(); it != _groups.end(); ++it) {
      if (it != _groups.begin()) {
        query<<",";
      }
      query<<it->first;
    }

    query<< ") and host="<< _host;
    query.execute();
  }
}

