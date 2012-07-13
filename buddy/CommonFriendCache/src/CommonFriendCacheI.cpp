#include "CommonFriendCacheI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "BuddyByIdReplicaAdapter.h"
#include "FriendRankCacheAdapter.h"
#include <algorithm>
#include "util/cpp/TimeCost.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "util/cpp/InvokeMonitor.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::serverstate;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace xce::atomicstl;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  const static std::string alias("CFC");
  service.registerObjectCache(CommonFriendCacheDataCategory, alias, new SyncServantFactory(new CommonFriendDataFactory));
  service.registerObjectCache(RelationCacheDataCategory, "CFR", new SyncServantFactory(new RelationDataFactory));
  
  service.getAdapter()->add(&CommonFriendCacheManagerI::instance(), service.createIdentity("CFCID", ""));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CommonFriendCache.Mod",0);
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CommonFriendCache.Interval", 5);

  //----------------------------Monitor-----------------------------------------
  MyUtil::ServiceMonitorManager::instance().start();

  TaskManager::instance().config(TASK_LEVEL_GID_LOAD, ThreadPoolConfig(1,5));
  TaskManager::instance().config(TASK_LEVEL_GID_RELOAD, ThreadPoolConfig(1,5));

  service.registerXceObserver(new ServiceSchemaI);
  // register to Controller Service
  ServerStateSubscriber::instance().initialize("ControllerCommonFriendCache",  &CommonFriendCacheManagerI::instance(), mod, interval);
}

//=========================================================

namespace xce {
  namespace buddy {

    int CompareFriendRank(const xce::socialgraph::RankData& rd1, const xce::socialgraph::RankData& rd2) {
      return rd1.weight > rd2.weight;
    }

    void ComputeMessage::Run() {
      std::ostringstream oss;
      oss << "ComputeMessage::Run() globalId:" << globalId_ << "  userId:" << userId_;
      TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
      /*
         xce::socialgraph::RankSeq rankSeq;
         try {
         rankSeq = xce::socialgraph::FriendRankCacheAdapter::instance().GetRank(userId_);
         } catch (...) {
         MCE_WARN("FriendRankCacheAdapter::GetRank()... excpetion  userId_:" << userId_);
         }
         std::sort(rankSeq.begin(), rankSeq.end(), CompareFriendRank);
         MyUtil::IntSeq userIdSeq;
         for (size_t i = 0; i < rankSeq.size(); ++i) {
         userIdSeq.push_back(rankSeq[i].userid);
         }
       */
      MyUtil::IntSeq userIdSeq;
      try {
        userIdSeq = BuddyByIdCacheAdapter::instance().getFriendListAsc(userId_, 2000);
      } catch (...) {
        MCE_WARN("BuddyByIdCacheAdapter::instance().getFriendListAsc(" << userId_ << ", 2000) exception");
      }
/*
      MyUtil::IntSeq friendIdSeq;
      CommonFriendDAO dao;
      if (!dao.FetchFriend(globalId_, userIdSeq, friendIdSeq)) {
        return;
      } 
      if (friendIdSeq.empty())
        return;    
*/
      MyUtil::IntSeq friendIdSeq = ServiceI::instance().locateObject<RelationDataPtr> (RelationCacheDataCategory, globalId_)->get(0,-1);
 
      MyUtil::IntSeq commonFriendSeq(userIdSeq.size());
      MyUtil::IntSeq::iterator end= set_intersection(userIdSeq.begin(),userIdSeq.end(),friendIdSeq.begin(),
        friendIdSeq.end(), commonFriendSeq.begin());   
      commonFriendSeq.resize(end-commonFriendSeq.begin());
      if (commonFriendSeq.empty())
        return;
 
      xce::buddy::Items items;
      if(!data_.empty()){
        items.ParseFromString(data_);
      }
      xce::buddy::Item* item = items.add_items();
      item->set_gid(globalId_);
      for (size_t i = 0; i < commonFriendSeq.size(); ++i) {
        item->add_common(commonFriendSeq[i]);
      }
      CommonFriendDataPtr commonFriendData = new CommonFriendData();
      items.SerializeToString(&(commonFriendData->data_));

      GlobalIdPtr guid = new GlobalId(type_,userId_);	
      ServiceI::instance().addObject(commonFriendData, CommonFriendCacheDataCategory, guid);
      data_ = commonFriendData->data_;

      MCE_INFO("SetData ObjectCache globalid:" << globalId_ << " userId:" << userId_ << " commons:" << commonFriendSeq.size());
    }

    bool CommonFriendDAO::FetchFriend(long globalId, MyUtil::IntSeq& targetIdSeq, MyUtil::IntSeq& resultIdSeq) {
      std::ostringstream oss;
      oss << "FetchFriend() globalId:" << globalId << "  targetIdSeq.size():" << targetIdSeq.size();
      TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);

      if (targetIdSeq.empty())
        return false;

      std::ostringstream pattern;
      pattern << "member_" << globalId % 100;

      Statement sql;

      sql << "SELECT userid FROM member_" << globalId % 100
        << " WHERE object_id = " << globalId << " and userid in (";
      for (size_t i = 0; i < targetIdSeq.size(); ++i) {
        if (i != targetIdSeq.size() - 1) {
          sql << targetIdSeq[i] << ",";
        }
        else {
          sql << targetIdSeq[i] << ")";
        }
      }

      try {
        QueryRunner("relation_center", CDbRServer, pattern.str()).query(sql, FetchFriendResultHandler(resultIdSeq));
        return true;
      } catch (...) {
        MCE_WARN("FetchFriend ... exception  gId:" << globalId);
      }  

      return false;
    }


    bool FetchFriendResultHandler::handle(mysqlpp::Row& row) const {
      if (!row.empty()) {
        int fan_id = (int) row["userid"];
        resultIntSeq_.push_back(fan_id);
      }
      return true;
    }


    MyUtil::IntSeq CommonFriendCacheManagerI::GetCommon(Ice::Int uid, 
        const MyUtil::GlobalIdPtr& gid, 
        Ice::Int begin, 
        Ice::Int limit, 
        const Ice::Current& current) {
      using namespace MyUtil;
      Ice::Long globalId = GlobalIdTransfer::get(gid);
      ostringstream msg;
      msg << " uid:" << uid << " gid:" << globalId << " begin:" << begin << " limit:" << limit; 
      InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
      MyUtil::IntSeq seq;
      if (!CheckParam(begin, limit))
        return seq;
      
      RelationDataPtr rdata = ServiceI::instance().findObject<RelationDataPtr> (RelationCacheDataCategory, globalId); 
      if(!rdata){
        TaskManager::instance().execute(new LoadTask(globalId)); 
        MCE_INFO("Relation object miss, gid:" << globalId );
        return seq;
      }
      time_t now = time(0);
      if(now - rdata->getTimestamp() > 7200){
        //if(reloadMap_.has(globalId)){
        //  MCE_INFO("Relation object reload repeatly, gid:" << globalId << " exist time:" << now - rdata->getTimestamp());
        //} else{
        //  reloadMap_[globalId] = globalId;
        MCE_INFO("Relation object reload, gid:" << globalId << " exist time:" << now - rdata->getTimestamp());
        rdata->setTimestamp(now);
        TaskManager::instance().execute(new ReloadTask(globalId));
        //}
        return seq;
      }
  
      GlobalIdPtr guid = new GlobalId(gid->type,uid); 
      CommonFriendDataPtr object = ServiceI::instance().findObject<CommonFriendDataPtr>(CommonFriendCacheDataCategory, 
          GlobalIdTransfer::get(guid));
      std::string byte_data;
      if (object) { //找到了
        IceUtil::Mutex::Lock lock(object->mutex_);
        xce::buddy::Items items;
        byte_data = object->data_;
        items.ParseFromString(byte_data);
        bool miss = true;
        for (int i = 0; i < items.items_size(); ++i) {
          const xce::buddy::Item& item = items.items(i);
          if (globalId == item.gid()) {
            miss = false;
            break;  
          }
        }
        if (miss) {
          ComputeMessage message(uid, gid, object->data_);
          message.Run();
          byte_data = message.data();
        }
        MCE_INFO("object hit and miss:" << miss << " uid:" << uid << " gid:" << globalId << " items.items_size():" << items.items_size());

      }
      else {
        ComputeMessage message(uid, gid);
        message.Run();
        byte_data = message.data();
      } 

      xce::buddy::Items items;
      items.ParseFromString(byte_data);
      for (int i = 0; i < items.items_size(); ++i) {
        const xce::buddy::Item& item = items.items(i);
        if (globalId == item.gid()) {
          int size = item.common_size();
          if (limit == -1) {
            for (int i = begin; i < size; ++i) {
              seq.push_back(item.common(i));
            }
            break;
          }
          int end = begin + limit;
          for (int i = begin; i < size && i < end; ++i) {
            seq.push_back(item.common(i));
          }
          break;  
        }
      }
      MCE_INFO("CommonFriendCacheManagerI::GetCommon" << " uid:" << uid << " gid:" << globalId << " commons:" << seq.size());
      return seq;
    }

    MyUtil::IntSeq CommonFriendCacheManagerI::GetUnCommon(Ice::Int uid, 
        const MyUtil::GlobalIdPtr& gid, 
        Ice::Int begin, 
        Ice::Int limit, 
        const Ice::Current& current) {
      using namespace MyUtil;
      ostringstream msg;
      msg << " uid:" << uid << " gid:" << GlobalIdTransfer::get(gid) << " begin:" << begin << " limit:" << limit;
      InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);  
      MyUtil::IntSeq seq;
      return seq;


    }

    CommonFriendEntry CommonFriendCacheManagerI::GetCommonAndUnCommon(const xce::buddy::CommonFriendParam& param, const Ice::Current& current) {
      using namespace MyUtil;
      MCE_INFO(" [CommonFriendCacheManagerI::GetCommonAndUnCommon] uid:" << param.uid << " gid:" << GlobalIdTransfer::get(param.gid) << " cbegin:" << param.cbegin << " climit:" << param.climit << " ubegin:" << param.ubegin << " ulimit:" << param.ulimit);
      ostringstream msg;
      msg << " uid:" << param.uid << " gid:" << GlobalIdTransfer::get(param.gid) << " cbegin:" << param.cbegin << " climit:" << param.climit << " ubegin:" << param.ubegin << " ulimit:" << param.ulimit;
      InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
      CommonFriendEntry et;
      return et;
    }

    bool CommonFriendCacheManagerI::SetData(const CommonFriendObject& cfo, const Ice::Current& current) {
      return false;
    }

    void CommonFriendCacheManagerI::add(const MyUtil::GlobalIdPtr& gid, Ice::Int uid, const Ice::Current& current){
      long globalId = GlobalIdTransfer::get(gid);
      MCE_INFO("add gid:" << globalId << "uid:" << uid);
      ostringstream msg;
      msg << " gid:" << globalId << "uid:" << uid;
      InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
      RelationDataPtr rdata = ServiceI::instance().findObject<RelationDataPtr> (RelationCacheDataCategory, globalId);
      if(!rdata){
        MCE_INFO(" [CommonFriendCacheManagerI::add] Relation object miss, gid:" << gid );
        return;
      }    
      ServiceI::instance().locateObject<RelationDataPtr> (RelationCacheDataCategory, globalId)->add(uid);    
    }

    void CommonFriendCacheManagerI::remove(const MyUtil::GlobalIdPtr& gid, Ice::Int uid, const Ice::Current& current){
      long globalId = GlobalIdTransfer::get(gid);
      ostringstream msg;
      msg << " gid:" << globalId << "uid:" << uid;
      InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

      RelationDataPtr rdata = ServiceI::instance().findObject<RelationDataPtr> (RelationCacheDataCategory, globalId);
      if(!rdata){
        MCE_INFO(" [CommonFriendCacheManagerI::remove] Relation object miss, gid:" << gid );
        return;
      }
      rdata->remove(uid);
    }

    int CommonFriendCacheManagerI::getCount(const MyUtil::GlobalIdPtr& gid, const Ice::Current& current){
      long globalId = GlobalIdTransfer::get(gid);

      RelationDataPtr rdata = ServiceI::instance().findObject<RelationDataPtr> (RelationCacheDataCategory, globalId);
      if(!rdata){
        MCE_INFO(" [CommonFriendCacheManagerI::getCount] Relation object miss, gid:" << globalId );
        TaskManager::instance().execute(new LoadTask(globalId));
        return 0;
      }
      int count = rdata->getCount();
      ostringstream msg;
      msg << " gid:" << globalId << " count:" << count;
      InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
      return count;      
    }

    bool CommonFriendCacheManagerI::hasRelation(const MyUtil::GlobalIdPtr& gid, Ice::Int uid,const Ice::Current& current){
      long globalId = GlobalIdTransfer::get(gid);
      RelationDataPtr rdata = ServiceI::instance().findObject<RelationDataPtr> (RelationCacheDataCategory, globalId);
      if(!rdata){
        MCE_INFO(" [CommonFriendCacheManagerI::hasRelation] Relation object miss, gid:" << globalId );
        TaskManager::instance().execute(new LoadTask(globalId));
        return false;
      }
      bool isFound = rdata->find(uid);
      ostringstream msg;
      msg << " gid:" << globalId << " uid:" << uid << " result:" << isFound;
      InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
      return isFound; 
    }

    void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
      //...
      TaskManager::instance().config(TASK_LEVEL_GID_LOAD, ThreadPoolConfig(1,5));
      TaskManager::instance().config(TASK_LEVEL_GID_RELOAD, ThreadPoolConfig(1,5));
    }

    void LoadTask::handle(){
      ServiceI::instance().locateObject<RelationDataPtr> (RelationCacheDataCategory, _gid);
    }

    void ReloadTask::handle(){
      ServiceI::instance().reloadObject (RelationCacheDataCategory, _gid);
      CommonFriendCacheManagerI::instance().getMap()->erase(_gid); 
    }
  }
} // end xce::buddy

