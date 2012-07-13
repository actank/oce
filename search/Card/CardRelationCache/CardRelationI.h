#ifndef __CardCacheI_h__
#define __CardCacheI_h__

#include <CardCache.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <time.h>
#include "Ice/Ice.h"
#include "IceUtil/IceUtil.h"
#include "ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "search/Card/share/CardDBAdapter.h"
#include "OceCxxAdapter/src/UserCountAdapter.h"

namespace xce
{

  namespace cardcache
  {

    std::string FIELD_UID = "user_id";
    std::string FIELD_FID = "friend_id";
    std::string FIELD_FLAG = "request_flag";
    std::string FIELD_TIME = "UNIX_TIMESTAMP(record_time) as record_time";
    std::string FIELD_TIME1 = "record_time";

    std::string DESC_ADDRESSBOOK = "addressbook";
    int CARD_RELATION_DATA = 0;
    int TYPE = 43;
    int CLUSTER = 100;

    static bool CompareTime(const RelationInfoNewPtr& a, const RelationInfoNewPtr& b) {
      if (a->recordTime >= b->recordTime) {
        return true;
      } else {
        return false;
      }
    }

    class CardRelationFactoryI : public MyUtil::ServantFactory {
      public:
        /// @brief create 当向Cache中取不到对象时调用create添加一个对象 
        /// 
        /// @param id
        /// 
        /// @return 
        virtual Ice::ObjectPtr create(long id);
    };
    typedef IceUtil::Handle<CardRelationFactoryI> CardRelationFactoryIPtr;

    class CardRelationCacheI;
    typedef IceUtil::Handle<CardRelationCacheI> CardRelationCacheIPtr;
    class CardRelationCacheI : virtual public CardRelationCache,
    public MyUtil::Singleton<CardRelationCacheI>
    {
      public:
        CardRelationCacheI();
        void initialize();
        /// @brief getTableName 根据用户ID确定DB中用户好友名片交换关系表的表名
        /// 
        /// @param int userId 用户ID
        /// 
        /// @return 返回表名
        std::string getTableName(int);

        /// @brief get 根据名片交换状态从Cache中取当前状态下用户的好友信息
        /// 
        /// @param ::Ice::Int userId 用户ID
        /// @param ::Ice::Int begin 开始的好友ID
        /// @param ::Ice::Int limit 取的数量
        /// @param ::Ice::Int requestFlag 对应的名片交换状态
        /// @param Ice::Current
        /// 
        /// @return 返回好友ID
        virtual ::MyUtil::IntSeq get(::Ice::Int, ::Ice::Int, ::Ice::Int, ::Ice::Int, const Ice::Current&);

        /// @brief getSize 从Cache中取某个用户某种状态的好友个数
        /// 
        /// @param ::Ice::Int userId 用户ID
        /// @param ::Ice::Int requestFlag 对应的名片交换状态
        /// @param Ice::Current
        /// 
        /// @return 返回某个用户某种状态的好友个数
        virtual ::Ice::Int getSize(::Ice::Int, ::Ice::Int, const Ice::Current&);

        /// @brief set 向Cache中插入或修改用户好友名片交换关系信息 
        /// 
        /// @param ::xce::cardcache::RelationInfoPtr 用户好友名片交换信息
        /// @param Ice::Current
        /// 
        /// @return 返回状态信息
        virtual Ice::Int set(const ::xce::cardcache::RelationInfoPtr&, const Ice::Current&);

        /// @brief setBatch 向Cache中批量插入或修改用户好友名片交换信息
        /// 
        /// @param ::xce::cardcache::RelationInfoSeq relationInfos 用户好友名片交换信息
        /// @param Ice::Current
        virtual void setBatch(const ::xce::cardcache::RelationInfoSeq&, const Ice::Current&);

        /// @brief getRelation 从Cache中取两个用户之间的名片交换信息
        /// 
        /// @param Ice::Int userId 用户ID
        /// @param Ice::Int friendId 用户好友ID
        /// @param Ice::Current
        /// 
        /// @return 返回两个用户的名片交换信息
        virtual int getRelation(Ice::Int, Ice::Int, const Ice::Current&);

        /// @brief getAllRelation 获取用户名片交换状态的所有好友ID
        /// 
        /// @param Ice::Int userId 用户ID
        /// @param Ice::Current
        /// 
        /// @return 返回好友ID的向量，分为三个
        virtual AllRelationPtr getAllRelation(Ice::Int, const Ice::Current&);

        /// @brief adjust 调用CardDBAdapter中的方法改变用户的关系，同时改变Cache中的关系
        /// 
        /// @param int userId 用户ID
        /// @param int friendId 用户好友ID
        /// @param int state 同requestFlag，名片交换状态
        /// 
        /// @return 返回状态
        int adjust(int, int, int);

        /// @brief remove 移除两个用户的名片交换信息
        /// 
        /// @param ::Ice::Int userId 用户ID
        /// @param ::Ice::Int friendId 用户好友ID
        /// @param Ice::Current
        virtual void remove(::Ice::Int,::Ice::Int,const Ice::Current&);

        virtual ::MyUtil::IntSeq getReceive(::Ice::Int, ::Ice::Int, ::Ice::Int, const ::Ice::Current&);

        virtual bool ignoreReceive(::Ice::Int, const ::MyUtil::IntSeq&, const ::Ice::Current&);

        virtual bool ignoreAll(::Ice::Int, const ::Ice::Current&);

        virtual ::MyUtil::Int2IntSeqMap getAccept(::Ice::Int, ::Ice::Int, ::Ice::Int, const ::Ice::Current&);
        void setAcceptUser(time_t, int , int );//设置用户接受名片交换请求的信息
        std::map<std::string, std::map<int, std::set<int> > > acceptUsers;

      private:
        class Dispatcher: virtual public IceUtil::Thread {
          public:
            Dispatcher(CardRelationCacheIPtr facade, int mod):_lastInvokeTime(time(NULL)),_facade(facade),_mod(mod){}
            virtual void run();
            void setDbInfo(int, int, int, time_t);
          private:
            void invoke();
            bool shouldInvoke();
            void invokeAndClearCache();

            std::vector<dbInfo> _toInvokeCache;
            time_t _lastInvokeTime;
            CardRelationCacheIPtr _facade;
            int _mod;
            IceUtil::Monitor<IceUtil::Mutex> dbInfoMutex;
        };

        typedef IceUtil::Handle<Dispatcher> DispatcherPtr;
        std::vector<DispatcherPtr> _dispatchThreads;

        IceUtil::Mutex acceptUserMutex;
        CardRelationFactoryIPtr factory;
        std::string tableName;
        int tableCount;
    };

    class NotifyCardDbTask : virtual public MyUtil::Task {
      public:
        NotifyCardDbTask(std::vector<dbInfo>& toInvokeCache, int mod);
        virtual void handle();
      private:
        std::vector<dbInfo> _toInvokeCache;
        int _mod;
    };

    /// @brief 定义用户名片交换信息的Cache操作方法和Cache中存储的数据结构
    class CardRelationData : virtual public Ice::Object,
    virtual public IceUtil::Mutex {
      public:
        /// @brief CardRelationData 构造函数，设置stateList的大小为3并初始化
        CardRelationData(){
          stateList.resize(3);
        }

        /// @brief get 提供接口给CardRelationCacheI::get调用
        /// 
        /// @param begin 开始的friendId
        /// @param limit 要取的friendId个数
        /// @param requestFlag 名片交换状态
        /// 
        /// @return 返回friendId的向量
        MyUtil::IntSeq get(int begin,int limit,int requestFlag);    

        /// @brief getSize 提供接口给CardRelationCacheI::getSize调用
        /// 
        /// @param requestFlag 名片交换的状态
        /// 
        /// @return 返回某种状态下的friendId数量
        int getSize(int requestFlag);

        /// @brief add 向Cache中的添加操作
        /// 
        /// @param friendId 用户好友ID
        /// @param requestFlag 名片交换状态
        /// 
        /// @return 添加成功返回true
        bool add(int friendId,int requestFlag,int time);

        /// @brief remove 从Cache中移除某种状态下用户的好友Id
        /// 
        /// @param friendId 好友ID
        /// @param requestFlag 名片交换状态
        /// 
        /// @return 成功返回true
        bool remove(int friendId,int requestFlag);

        /// @brief has 判断两个好友用户是否存在某种状态的关系
        /// 
        /// @param friendId 好友ID
        /// @param requestFlag 名片交换状态
        /// 
        /// @return 存在则返回true
        bool has(int friendId,int requestFlag);
        MyUtil::IntSeq getReceive(int userId,int begin,int limit,int requestFlag);
      private:
//        std::vector<std::vector<int> > stateList;
        std::vector<std::map<int, int> > stateList; 
    };

    typedef IceUtil::Handle<CardRelationData> CardRelationDataPtr;

    /// @brief 处理从数据库中读出的数据
    class CardRelationResultHandlerI : public com::xiaonei::xce::ResultHandler {
      public:
        CardRelationResultHandlerI(CardRelationDataPtr& result):result_(result){};
        virtual bool handle(mysqlpp::Row&) const;
      private:
        CardRelationDataPtr& result_;
    };

/*    class LoadUserResultHandler : virtual public com::xiaonei::xce::ResultHandler {
      public:
        LoadUserResultHandler(std::string& maxtime, std::vector<acceptInfo>& infos) : maxtime_(maxtime), acceptInfos_(infos) {}
      protected:
        virtual bool handle(mysqlpp::Row& res) const {
          acceptInfo info;
          int userId = (int)res[FIELD_UID.c_str()];
          int friendId = (int)res[FIELD_FID.c_str()];
          time_t time = (time_t)res[FIELD_TIME1.c_str()];
          info.userId = userId;
          info.friendId = friendId;
          info.time = time;
          acceptInfos_.push_back(info);
          char time_str[12];
          strftime( time_str, sizeof(time_str), "%Y-%m-%d",localtime(&time) );
          if (acceptUsers_[time_str] == NULL) {
            std::set<int> a = new set<int>();
            a.insert(friendId);
            std::map<int, std::set<int> > b = new std::map<int, std::set<int> >();
            b[userId] = a;
            acceptUsers_[time_str] = b;
          } else {
            if (acceptUsers_[time_str][userId] == NULL) {
              std::set<int> a = new set<int>();
              a.insert(friendId);
              acceptUsers_[time_str][userId] = a;
            } else {
              acceptUsers_[time_str][userId].insert(friendId);
            }
          }
          return true;
        }
      private:
        std::vector<acceptInfo>& acceptInfos_;
        std::string& maxtime_;
    };

    class LoadTask : public MyUtil::Task {
      public :
        LoadTask(std::string tableName) : tableName_(tableName) {}
        virtual void handle();
      private:
        void loadAcceptUsers();
        std::vector<acceptInfo> acceptInfos_;
        std::string tableName_;
    };
*/
  }

}

#endif
