#ifndef _COMMON_FRIEND_CACHE_I_H_
#define _COMMON_FRIEND_CACHE_I_H_
#include "CommonFriendCache.h"
#include <map>
#include <set>
#include <boost/thread/shared_mutex.hpp>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "IceExt/src/ObjectCacheI.h"
#include "IceExt/src/ServiceI.h"
#include <ConnectionPoolManager.h>
#include <QueryRunner.h>
#include "UtilCxx/src/Singleton.h"
#include "UtilCxx/src/GlobalId.h"
#include "UtilCxx/src/AtomicSTL.h"
#include "base/threadpool.h"
#include "base/rwlock.h"
#include "FriendRank.h"
#include "commondata.pb.h"

namespace xce {
  namespace buddy {

    const int CommonFriendCacheDataCategory = 0;
    const int RelationCacheDataCategory = 1;
   
    const int TASK_LEVEL_GID_LOAD = 0;
    const int TASK_LEVEL_GID_RELOAD = 1;
 
    class RelationData: virtual public Ice::Object,
    virtual public IceUtil::Mutex {
      public:

        RelationData( MyUtil::IntSeq& ids){
          _ids.swap(ids);
          _timestamp =  time(0);
        }

        virtual ~RelationData(){
          ;
        }

        MyUtil::IntSeq get(int begin, int limit){
          IceUtil::Mutex::Lock lock(*this);
          if( limit < 0 ){
            return _ids;
          }
          if( ((unsigned int)begin >= _ids.size()) || (begin < 0)){
            return MyUtil::IntSeq();
          }
          if( (unsigned int)(begin+limit) >= _ids.size()){
            return MyUtil::IntSeq( _ids.begin()+begin, _ids.end());
          }else{
            return MyUtil::IntSeq( _ids.begin()+begin, _ids.begin()+begin+limit );
          }
        }

        bool find(int id){

          IceUtil::Mutex::Lock lock(*this);
          MyUtil::IntSeq::iterator it = lower_bound(_ids.begin(), _ids.end(), id);
          if (it != _ids.end() && *it == id) {
            return true;
          } else {
            return false;
          }
        }      

        void add(int id){
          IceUtil::Mutex::Lock lock(*this);
          MyUtil::IntSeq::iterator it = lower_bound(_ids.begin(), _ids.end(), id);
          if (it != _ids.end() && *it == id) {
          } else {
            _ids.insert(it, id);
          }
        }

        void remove(int id){
          IceUtil::Mutex::Lock lock(*this);
          MyUtil::IntSeq::iterator it = lower_bound( _ids.begin(), _ids.end(), id );
          if( it!=_ids.end() && *it==id ){
            _ids.erase( it );
          }
        }
        
        void clear(){
          IceUtil::Mutex::Lock lock(*this);
          _ids.clear();
        }

        int getCount(){
          IceUtil::Mutex::Lock lock(*this);
          return _ids.size();
        }

        time_t getTimestamp(){
          IceUtil::Mutex::Lock lock(*this);
          return _timestamp;
        }

        void setTimestamp(time_t timestamp){
          IceUtil::Mutex::Lock lock(*this);
          _timestamp = timestamp;
        }

      private:
        MyUtil::IntSeq _ids;
        time_t _timestamp;
    };
    typedef IceUtil::Handle<RelationData> RelationDataPtr;

    struct CommonFriendData : virtual public Ice::Object {
      CommonFriendData() { }
      virtual ~CommonFriendData() { }
      std::string data_;
      IceUtil::Mutex mutex_;
    };

    typedef IceUtil::Handle<CommonFriendData> CommonFriendDataPtr;

    int CompareFriendRank(const xce::socialgraph::RankData& rd1, const xce::socialgraph::RankData& rd2);

    class ComputeMessage : public xce::Message {
      public:
        ComputeMessage(int userId, MyUtil::GlobalIdPtr globalId) : userId_(userId), globalId_(MyUtil::GlobalIdTransfer::get(globalId)),type_(globalId->type) {
        }
        ComputeMessage(int userId, MyUtil::GlobalIdPtr globalId, std::string data) : userId_(userId), globalId_(MyUtil::GlobalIdTransfer::get(globalId)), data_(data),type_(globalId->type) { 
        }
        virtual void Run();
        virtual ~ComputeMessage() { }
        std::string data() {
          return data_;
        }
      private:
        int userId_;
        long globalId_;
        std::string data_;
        int type_;
    };

    class CommonFriendDAO {
      public:
        CommonFriendDAO() {}
        virtual ~CommonFriendDAO() {}
        bool FetchFriend(long globalId, MyUtil::IntSeq& targetIdSeq, MyUtil::IntSeq& resultIdSeq);
    };

    class FetchFriendResultHandler : public com::xiaonei::xce::ResultHandler {
      public:
        FetchFriendResultHandler(MyUtil::IntSeq& resultIntSeq) : resultIntSeq_(resultIntSeq) {

        }
        virtual bool handle(mysqlpp::Row& row) const;
        virtual ~FetchFriendResultHandler() {}
      private:
        MyUtil::IntSeq& resultIntSeq_;
    };



    //  manager
    class CommonFriendCacheManagerI : virtual public CommonFriendCacheManager, 
    virtual public MyUtil::Singleton<CommonFriendCacheManagerI> {
      public:
        virtual ~CommonFriendCacheManagerI() {}

        virtual MyUtil::IntSeq GetCommon(Ice::Int uid, 
            const MyUtil::GlobalIdPtr& gid, 
            Ice::Int begin = 0,  
            Ice::Int limit = 0, 
            const Ice::Current& current = Ice::Current());

        virtual MyUtil::IntSeq GetUnCommon(Ice::Int uid, 
            const MyUtil::GlobalIdPtr& gid, 
            Ice::Int begin = 0, 
            Ice::Int limit = 0, 
            const Ice::Current& current = Ice::Current());
        virtual bool SetData(const xce::buddy::CommonFriendObject& cfo, const Ice::Current& current = Ice::Current());

        virtual CommonFriendEntry GetCommonAndUnCommon(const xce::buddy::CommonFriendParam&, const Ice::Current& current = Ice::Current());

        virtual void add(const MyUtil::GlobalIdPtr& gid, Ice::Int uid, const Ice::Current& current = Ice::Current());

        virtual void remove(const MyUtil::GlobalIdPtr& gid, Ice::Int uid, const Ice::Current& current = Ice::Current());
      
        virtual int getCount(const MyUtil::GlobalIdPtr& gid, const Ice::Current& current = Ice::Current());

        virtual bool hasRelation(const MyUtil::GlobalIdPtr& gid, Ice::Int uid,const Ice::Current& current = Ice::Current());

        xce::atomicstl::AtomicMap<Ice::Long, Ice::Long>* getMap(){
          return &reloadMap_;
        }
      private:
        friend class MyUtil::Singleton<CommonFriendCacheManagerI>;
        CommonFriendCacheManagerI() : threadPool_(8, 40) { 

        }

        bool CheckParam(const xce::buddy::CommonFriendParam& param) {
          if (CheckParam(param.cbegin, param.climit) && CheckParam(param.ubegin, param.ulimit)) {
            return true;
          }
          return false;
        }

        bool CheckParam(Ice::Int begin, Ice::Int limit) {
          if (begin < 0)
            return false;
          if (limit < -1 || limit == 0)
            return false;
          return true;
        }

      private:
        xce::ThreadPool threadPool_;
        xce::atomicstl::AtomicMap<Ice::Long, Ice::Long> reloadMap_;
    };


    class ServiceSchemaI: public MyUtil::Descriptor {
      public:
        virtual void configure(const Ice::PropertiesPtr& props);
    };

    class CommonFriendDataFactory : virtual public MyUtil::ServantFactory {
      public:
        virtual Ice::ObjectPtr create(Ice::Int id) {
          CommonFriendDataPtr object = new CommonFriendData();
          return object;
        }
    };
    
    class OrderByIdAscHandler : virtual public com::xiaonei::xce::ResultHandler{
      public:
        OrderByIdAscHandler(std::vector<int>& data,
            const std::string& keyField) :
          _data(data), _keyField(keyField) {
          }
      protected:
        virtual bool handle(mysqlpp::Row& res) const{
          _data.push_back((int) res[_keyField.c_str()]);
          return true;
        }
      private:
        std::vector<int>& _data;
        std::string _keyField;
    };

    class RelationDataFactory : virtual public MyUtil::ServantFactory {
      public:
        virtual Ice::ObjectPtr create(Ice::Long id) {
          std::vector<int> ids;
          com::xiaonei::xce::Statement sql;
          sql     << "SELECT userid"
            << " FROM member_" << (id % 100)
            << " WHERE object_id = " << id;
          std::ostringstream pattern;
          pattern << "member_" << (id % 100);
          com::xiaonei::xce::QueryRunner("relation_center", com::xiaonei::xce::CDbRServer, pattern.str()).query(
              sql, OrderByIdAscHandler( ids, "userid" ));
          MCE_INFO("RelationDataFactory::create id" << id << " size:" << ids.size());
          return new RelationData(ids);
        }
    };
    
    class LoadTask: virtual public MyUtil::Task {
      public:
        LoadTask(Ice::Long gid , Ice::Int level=TASK_LEVEL_GID_LOAD):
          MyUtil::Task(level), _gid(gid)  {
          }
        virtual void handle();
      private:
        Ice::Long _gid;

    };

    class ReloadTask: virtual public MyUtil::Task {
      public:
        ReloadTask(Ice::Long gid , Ice::Int level=TASK_LEVEL_GID_RELOAD):
          MyUtil::Task(level), _gid(gid)  {
          }
        virtual void handle();
      private:
        Ice::Long _gid;

    };
  }
}
#endif


