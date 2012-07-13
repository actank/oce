// **********************************************************************
//
// Copyright (c) 2010-2011 OPI, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Refer CMake File 
//ADD_SERVICE_EXEC("buddy/tools" "Tools"  ReplicaCluster OceSliceReplicaCluster 
//                 OceSliceSubjectObserver OceSlice OceSliceXceStorm DbCxxPool IceExt UtilCxx UtilSlice)

#ifndef __BUDDYCACHE_H__
#define __BUDDYCACHE_H__
//system
#include <sys/sysinfo.h>
//Standard C++
#include <iostream>
#include <set>
//Ice
#include <Ice/Ice.h>
//IceExt
#include "ObjectCacheI.h"
//Db
#include "QueryRunner.h"
//Slice 
#include "BuddyByIdCache.h"

namespace xce{
namespace buddy{
namespace tools{

const static int PRODUCE_BATCH_SIZE = 3000;

/**
     * 批量生产UserId数据实现类，继承自ResultHandler <br>
     */
class BatchUsersIdProducerI : public com::xiaonei::xce::ResultHandler {
public:
  BatchUsersIdProducerI (MyUtil::LongSeq& results) : _results(results){};
  virtual bool handle(mysqlpp::Row& row) const {
    long id = (int) row["id"];
    _results.push_back(id);
    return true;
  }
private:
  MyUtil::LongSeq& _results;
};

/**
     * 批量生产FriendId数据实现类，继承自ResultHandler <br>
     */
class OrderByIdAscHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
  OrderByIdAscHandler(MyUtil::IntSeq& data) : _data(data){};
  virtual bool handle(mysqlpp::Row& res) const{
    _data.push_back(res["guest"]);
    return true;
  }
private:
  MyUtil::IntSeq& _data;
};

/**
     * 生产好友数据Factory类，继承自ServantFactory <br>
     */
class BuddyCacheFactory: virtual public MyUtil::ServantFactory {
public:
  Ice::ObjectPtr create(Ice::Int id){
    MCE_DEBUG("BuddyCacheFactory create " << id);
    xce::buddy::BuddyByIdDataPtr obj = new xce::buddy::BuddyByIdData;
    MyUtil::IntSeq orderFriend;
    com::xiaonei::xce::Statement sql;
    sql<< "SELECT guest"
      << " FROM relation_" << (id % 100)
      << " WHERE host = " << id;
    ostringstream pattern;
    pattern << "relation_" << (id % 100);
    com::xiaonei::xce::QueryRunner("relation_buddycache", com::xiaonei::xce::CDbRServer, 
      pattern.str()).query(sql, OrderByIdAscHandler( orderFriend));
    orderFriend.swap(obj->friends);
    MCE_DEBUG("BuddyCacheFactory create " << id << " done");
    return obj; 
  }
};

/**
     * 好友Cache类 <br>
     * 缓存所有好友id列表，是本地Cache，不支持rpc调用
     * 目前数据从好友从库进行加载，从TT加载暂未支持
     * 默认会有10个线程从DB进行加载
     */
class BuddyCache : public MyUtil::Singleton<BuddyCache>{
public:
  virtual ~BuddyCache(){
    ;
  }

  virtual void initialize(const string& logpath = "./buddycache.log" ,
                          const string& loglevel = "INFO" ,
                          const string& cachealias = "BC",
                          const int cachecategory = 0,
                          const int cachesize = 200000000,
                          const int preloadthreads = 10 ){
    //init log
    MCE_INIT(logpath, loglevel);
    MCE_INFO("initialize begin...");
    struct sysinfo *info = new sysinfo();
    if(-1 == sysinfo(info)){

    }
    //init cache
    _cache = new MyUtil::ObjectRetention(cachealias, cachecategory , cachesize, 
      new MyUtil::SyncServantFactory(new BuddyCacheFactory()));
    //init connectionpool to connect to db
    com::xiaonei::xce::ConnectionPoolManager::instance().initialize();
    //start preload
    _cluster = preloadthreads;
    _remain = preloadthreads;
    for(int i=0; i<preloadthreads; i++){
      _preloadvec.push_back(new PreloadThread(*this, i));
      _preloadvec[i]->start(1024*1024).detach();
    }

    while(true){
      IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _mutex );
      if(_remain>0){
        _mutex.wait(); 
      }else{
        break;
      }
    }
    MCE_INFO("preload finished.");
  }

  MyUtil::ObjectRetentionPtr& getCache(){
    return _cache;
  } 
  
  int getCluster() const{
    return _cluster; 
  }

  void decreaseRemain(){
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock( _mutex );
    _remain--;
    _mutex.notify();
  }

private:
  BuddyCache(){
    ;
  }
  friend class MyUtil::Singleton<BuddyCache>;

  class PreloadThread: virtual public IceUtil::Thread {
  public:
    PreloadThread(BuddyCache& bc, int mod) : bc_(bc),mod_(mod){
    }
    virtual void run(){
      MCE_INFO("Thread" << mod_ << " start.");
      int minId = 0;
      while ( true ){
        try {
          MyUtil::LongSeq ids;
          BatchUsersIdProducerI handler(ids);
          com::xiaonei::xce::Statement sql;
          sql << "SELECT id FROM user_passport WHERE id > " << minId
            << " and id%" << bc_.getCluster() << "=" << mod_ 
            << " ORDER BY id LIMIT " << PRODUCE_BATCH_SIZE;
          com::xiaonei::xce::QueryRunner( "usercache_passport",
            com::xiaonei::xce::CDbRServer ).query( sql, handler );

          if( ids.size() <=0 ){
            MCE_INFO("producer finished");
            break;
          }
          minId = ids.at(ids.size()-1);
          MCE_INFO("produce to " << minId);
          bc_.getCache()->create(ids);
        } catch (const Ice::Exception& e) {
        } catch (const std::exception& e) {
        } catch (...) {
      }
    }
    MCE_INFO("Thread" << mod_ << " end , will notify!");
    bc_.decreaseRemain();
  }
  private:
    BuddyCache& bc_;
    int mod_;
  };
  typedef IceUtil::Handle<PreloadThread> PreloadThreadPtr;

private: 
  MyUtil::ObjectRetentionPtr _cache;
  std::vector<IceUtil::ThreadPtr> _preloadvec; 
  int _cluster;
  int _remain;
  IceUtil::Monitor<IceUtil::Mutex> _mutex;
};

};
};
};

#endif
