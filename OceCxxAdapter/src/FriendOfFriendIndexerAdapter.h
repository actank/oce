/// 
/// @file FriendOfFriendIndexerAdapter.h
/// @brief 二度好友索引服务的适配器，供客户端使用
/// @author zhigang.wu@opi-corp.com
/// @date 2010-05-17
/// 
#ifndef __FRIEND_OF_FRIEND_INDEXER_ADAPTER_H__
#define __FRIEND_OF_FRIEND_INDEXER_ADAPTER_H__

#include "FriendOfFriendIndexer.h"
#include <queue>
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

//#define USE_SEARCH_TEST_CHANNEL
namespace com {
  namespace renren {
    namespace search2 {
      namespace friendoffriend {

        using std::queue;

        class FriendOfFriendIndexerAdapter : public MyUtil::AdapterI, 
#ifdef USE_SEARCH_TEST_CHANNEL
        public MyUtil::AdapterISingleton<MyUtil::SearchFoFChannel, FriendOfFriendIndexerAdapter> {
#else 
          public MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, FriendOfFriendIndexerAdapter> {
#endif
           public:
             FriendOfFriendIndexerAdapter(); 

             /// @brief BuildIndex 建立用户索引
             /// 
             /// @param user_id  用户ID
             /// @param level 任务的优先级
             /// 
             /// @return 
             bool BuildIndex(const int user_id, const Ice::Short level = HighLevel);
             bool RebuildIndex(const int user_id, const Ice::Short level = HighLevel);

             bool BuildIndexAsync(const int user_id, const Ice::Short level=HighLevel);

             /// @brief RebuildIndex_async 用户姓名改变，重建相关索引，异步调用
             /// 
             /// @param user_id 
             /// @param level 任务优先级 
             /// 
             /// @return 
             bool RebuildIndexAsync(const int user_id, const Ice::Short level=HighLevel);

           private:
                          
             virtual string name() {
               return "FOF";
             }
             virtual string endpoints() {
               return "@FriendOfFriendIndexer";
             }
             virtual size_t cluster() {
               return 1;
             }

             FriendOfFriendIndexerManagerPrx GetFriendOfFriendIndexerManager(int id);

             struct Pair {
               int id_;//用户id
               Ice::Short level_; //执行优先级
               Pair(const int id, const Ice::Short level):id_(id), level_(level) {
               }
             };

             class BuildIndexThread : virtual public IceUtil::Thread {
              public:
                bool invoke(const int user_id, Ice::Short level); 
              protected:
                virtual void run(); 
              private:
                IceUtil::Monitor<IceUtil::Mutex> mutex_;
                queue<Pair> cache_;
             };

             class BuildIndexTask : public MyUtil::Task {
              public:
                BuildIndexTask(Pair& pair);
                virtual ~BuildIndexTask();
                virtual void handle();
              private:
                Pair pair_;
             };

             class RebuildIndexThread : virtual public IceUtil::Thread {
              public:
                bool invoke(const int user_id, Ice::Short level); 
              protected:
                virtual void run(); 
              private:
                IceUtil::Monitor<IceUtil::Mutex> mutex_;
                queue<Pair> cache_;
             };

             class RebuildIndexTask : public MyUtil::Task {
              public:
                RebuildIndexTask(Pair& pair);
                virtual ~RebuildIndexTask();
                virtual void handle();
              private:
                Pair pair_;
             };

             RebuildIndexThread invoke_rebuild_index_;
             BuildIndexThread invoke_build_index_;

             enum {
               kProxyNum = 50,
             };

             vector<FriendOfFriendIndexerManagerPrx> proxies_;

             vector<FriendOfFriendIndexerManagerPrx> managers_;

          };

        }
      }
    }
  }
#endif
