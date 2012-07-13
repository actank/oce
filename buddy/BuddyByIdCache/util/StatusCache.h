#ifndef STATUSCACHE_H_
#define STATUSCACHE_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <bitset>
#include <Singleton.h>
#include <AtomicSTL.h>
#include <ext/hash_map>
#include "StatusCacheAdapter.h"
#include <exception>

namespace xce {
  namespace buddy {

    const int STATUS_CACHE_SIZE = 2147483647;

    class StatusCacheResultHandler : virtual public com::xiaonei::xce::ResultHandler
    {
      public:
        StatusCacheResultHandler(int& large_id, map<int, int>& result) :
          _large_id(large_id), _result(result) {
          }
      protected:
        virtual bool handle(mysqlpp::Row& res) const{
          int uid = res["id"];
          _large_id = uid > _large_id ? uid : _large_id;
          int status = res["status"];
          _result.insert(make_pair(uid, status));
          return true;
        }

      private:
        int& _large_id;
        map<int,int>& _result;
    };

    //-------------------------------------------------------------

    class StatusCache: public MyUtil::Singleton<StatusCache> {
      private:
        friend class MyUtil::Singleton<StatusCache>;
        StatusCache(): _inited_flag(false), maxId_(0) {
          MCE_INFO("StatusCache::StatusCache");
          //Maybe we can do some optimization to the following lines
          for (int i=0;i<STATUS_CACHE_SIZE;++i) {
            _all_status.reset(i);
          }
          MCE_INFO("StatusCache::StatusCache -> done");
        }

        int getMaxId(){
          IceUtil::Monitor<IceUtil::Mutex>::Lock lock(maxmutex_);
          return maxId_;
        }
        void setMaxId(int id){
          IceUtil::Monitor<IceUtil::Mutex>::Lock lock(maxmutex_);
          maxId_ = id;
          maxmutex_.notify();
        }
        bool isValid(){
          IceUtil::RWRecMutex::RLock lock(_inited_flag_mutex);
          MCE_INFO("StatusCache::isValid -> "<<_inited_flag);
          return _inited_flag;
        }

      public:
        virtual void reload(std::vector<int> ids){}
        virtual void reload(int id){}

        void initializeFromSvc(){
          MCE_INFO("[StatusCache::initializeFromSvc] start ...");
          bool isReady = false;
          while( !isReady ){
            try{
              xce::statuscache::StatusCacheAdapter::instance().getAllStatus( _all_status );
              isReady = true;
            } catch( Ice::Exception e ){
              MCE_WARN("[StatusCacheAdapter::getAllStatus()] found Ice::Exception : " << e.what() );
            } catch( ... ){
              MCE_WARN("[StatusCacheAdapter::getAllStatus()] found unkown exception : " );
            }
            sleep(1);
          }
          setMaxId(STATUS_CACHE_SIZE - 1);

          MCE_INFO("[StatusCache::initializeFromSvc] thread loading done.");
          {
            IceUtil::RWRecMutex::WLock lock(_inited_flag_mutex);
            _inited_flag = true;
          }
          MCE_INFO("[StatusCache::initializeFromSvc] thread set flag done.");
          try {
            vector<int> pendding;
            {
              IceUtil::Mutex::Lock lock(_pendding_filter_mutex);
              pendding.swap(_pendding_filter_userids);
            }
            //Maybe something wrong happens
            for( vector<int>::iterator it = pendding.begin(); it
                != pendding.end(); ++it) {
              reload(*it);
            }
          } catch (...) {
            MCE_WARN("[StatusCache::initializeFromSvc] reload pendding exit with unknown exception.");
          }
          MCE_INFO("[StatusCache::initializeFromSvc] thread all done.");
        }

        void initializeFromDB(){
          MCE_INFO("[StatusCache::initializeFromDB] start ...");
          //Maybe we can do some optimization to the following lines
          for (int i=0;i<STATUS_CACHE_SIZE;++i) {
            _all_status.set(i);
          }
          try {
            int large_id = 0;
            do {
              com::xiaonei::xce::Statement sql;
              sql << "SELECT id, status FROM user_passport WHERE id > " << getMaxId() << " LIMIT 10000";
              map<int, int> db_data;
              com::xiaonei::xce::QueryRunner("user_passport_status",com::xiaonei::xce::CDbRServer ).query(
                  sql, StatusCacheResultHandler( large_id, db_data )
                  );
              {
                for (map<int, int>::const_iterator it = db_data.begin(); it
                    != db_data.end(); ++it) {
                  if (it->second == 0 || it->second == 3) {
                    _all_status.reset(it->first);
                  }
                }
              }
              if (large_id > getMaxId()) {
                setMaxId(large_id);
              } else {
                setMaxId(STATUS_CACHE_SIZE - 1);
                break;
              }
              MCE_INFO("StatusCache::initializeFromDB -> initializing to " << large_id);
            } while (true);
          } catch (...) {
            MCE_WARN("StatusCache::initializeFromDB -> loading exit with unknown exception.");
          }
          MCE_INFO("[StatusCache::initializeFromDB] thread loading done.");
          {
            IceUtil::RWRecMutex::WLock lock(_inited_flag_mutex);
            _inited_flag = true;
          }
          MCE_INFO("[StatusCache::initializeFromDB] thread set flag done.");
          try {
            vector<int> pendding;
            {
              IceUtil::Mutex::Lock lock(_pendding_filter_mutex);
              pendding.swap(_pendding_filter_userids);
            }
            //Maybe there will be something wrong happens
            for( vector<int>::iterator it = pendding.begin(); it
                != pendding.end(); ++it) {
              reload(*it);
            }
          } catch (...) {
            MCE_WARN("[StatusCache::initializeFromDB] reload pendding exit with unknown exception.");
          }
          MCE_INFO("[StatusCache::initializeFromDB] thread all done.");
        }

        void set(int userid, int status){
          try {
            MCE_DEBUG("StatusCache::run -> valid userid: "<<userid << " status: "<<status);
            if (status != 0 && status != 3) {
              _all_status.set(userid);
            } else {
              _all_status.reset(userid);
            }
          } catch( std::exception e){
            MCE_WARN("StatusCache::run -> std::exception" << e.what() );
          } catch(...) {
            MCE_WARN("StatusCache::run -> unknown exception");
          }
        }
        MyUtil::IntSeq filter(int userId,std::map<int, int>& orderFriend){
          MyUtil::IntSeq result;
          ostringstream filter_str;
          bool filted = false;
          bool inited = false;
          {
            IceUtil::RWRecMutex::RLock lock(_inited_flag_mutex);
            inited = _inited_flag;
          }
          if( inited ){
            for (std::map<int,int>::const_iterator it = orderFriend.begin(); it
                != orderFriend.end(); ++it) {
              if( it->first < 0 ){
                continue;
              }

              if( !_all_status[it->first] ){
                result.push_back( it->first );
              } else{
                filted = true;
                filter_str << it->first << " ";
              }

            }
          } else{
            for (std::map<int,int>::const_iterator it = orderFriend.begin(); it
                != orderFriend.end(); ++it) {
              result.push_back( it->first );
            }
            IceUtil::Mutex::Lock lock(_pendding_filter_mutex);
            _pendding_filter_userids.push_back( userId );
          }

          if( filted ){
            MCE_INFO("[StatusCache::filter] userId=" << userId << " friends.size()=" << result.size() << " + filterd: " << filter_str.str() );
          } else{
            MCE_INFO("[StatusCache::filter] userId=" << userId << " friends.size()=" << result.size() );
          }
          return result;
        }

        bool isGoodUser(int userid){
          return !_all_status[userid];
        }
        MyUtil::ByteSeq getStatusMinMax(int minId, int maxId){
          if(maxId <= minId) {
            MCE_INFO("StatusCache::getStatusMinMax maxId must be larger than minId");
            return MyUtil::ByteSeq();
          }
          bool inited;
          {
            IceUtil::RWRecMutex::RLock lock(_inited_flag_mutex);
            inited = _inited_flag;
          }
          if(inited && getMaxId() >= maxId - 1) {
            MyUtil::ByteSeq result;
            int n = (maxId - minId) / 8;
            MCE_DEBUG("StatusCache::getStatusMinMax -> n: " << n);
            for(int i = 0; i < n; ++i) {
              result.push_back(Ice::Byte());
            }
            for(int i = minId; i < maxId; ++i) {
              if (_all_status[i]) {
                result[(i - minId) / 8] |= (1 << (i - minId) % 8);
              }
            }
            MCE_DEBUG("StatusCache::getStatusMinMax -> size: " << result.size());
            return result;
          } else {
            MCE_INFO("StatusCache::getStatusMinMax inited = " << inited << " maxId = " << getMaxId());
            return MyUtil::ByteSeq();
          }
        }

      private:
        bool _inited_flag;
        IceUtil::RWRecMutex _inited_flag_mutex;

        vector<int> _pendding_filter_userids;
        IceUtil::Mutex _pendding_filter_mutex;

        int maxId_;
        IceUtil::Monitor<IceUtil::Mutex> maxmutex_;

        std::bitset<STATUS_CACHE_SIZE> _all_status;
    };

    //-------------------------------------------------------------
   /* class StatusCacheInitThread: public IceUtil::Thread {
      public:
        virtual void run(){
          bool isReady = false;
          int tryLimit = 10;
          while( !isReady && (--tryLimit)>0 ){
            MCE_INFO("[StatusCacheInitThread::run()] trying to get status from StatusCache:" << tryLimit );
            try{
              isReady = xce::statuscache::StatusCacheAdapter::instance().isValid();
            } catch( Ice::Exception e ){
              MCE_WARN("[StatusCacheAdapter::isValid()] found Ice::Exception : " << e.what() );
              continue;
            } catch( ... ){
              MCE_WARN("[StatusCacheAdapter::isValid()] found unkown exception : " );
              continue;
            }
            sleep(1);
          }
          if( isReady ){
            StatusCache::instance().initializeFromSvc();
          } else{
            StatusCache::instance().initializeFromDB();
          }

        }
    };
    typedef IceUtil::Handle<StatusCacheInitThread> StatusCacheInitThreadPtr;*/
    //-------------------------------------------------------------

    /*
       const int STATUS_TRUNKS = 10111;

       class StatusCache: public MyUtil::Singleton<StatusCache> {
       private:
       class STHandler: public com::xiaonei::xce::ResultHandler {
       public:
       STHandler(int& large_id, std::map<int, int>& result);
       protected:
       virtual bool handle(mysqlpp::Row& res) const;
       private:
       int& _large_id;
       map<int, int>& _result;
       };
       friend class MyUtil::Singleton<StatusCache>;
       StatusCache(): _inited_flag(false){
       for ( int x=0;x<STATUS_TRUNKS;x++ ) _all_status.push_back(new _StatusDataTrunk);
       }
       public:
       void initialize();
       bool isValid();
       void set(int userid, int status);
       MyUtil::IntSeq filter(int userId,std::map<int, int>& orderFriend);
       private:
       bool _inited_flag;
       IceUtil::RWRecMutex _inited_flag_mutex;

       vector<int> _pendding_filter_userids;
       IceUtil::Mutex _pendding_filter_mutex;

       struct _StatusDataTrunk {
       __gnu_cxx::hash_map<int, int> data;
       IceUtil::RWRecMutex mutex;
       };
       vector<_StatusDataTrunk*> _all_status;
       };

       class StatusCacheInitThread: public IceUtil::Thread {
       public:
       virtual void run(){
       StatusCache::instance().initialize();
       };
       };
       typedef IceUtil::Handle<StatusCacheInitThread> StatusCacheInitThreadPtr;
     */

  }
}

#endif /* STATUSCACHE_H_ */
