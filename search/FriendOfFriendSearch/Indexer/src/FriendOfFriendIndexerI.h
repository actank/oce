#ifndef __FRIEND_OF_FRIEND_INDEXER_H
#define __FRIEND_OF_FRIEND_INDEXER_H

#include "FriendOfFriendIndexer.h"
#include <queue>
#include <vector>
#include "IceUtil/IceUtil.h"
#include <boost/shared_ptr.hpp>
#include "ServiceI.h"
#include "QueryRunner.h"
#include "search/FriendOfFriendSearch/library/include/map.h"
#include "search/FriendOfFriendSearch/library/include/mapdb.h"

namespace com {
  namespace renren {
    namespace search2 {
      namespace friendoffriend {

        /// @brief Object name
        const string kFOFIndexerObjectName = "FOF";
        const int kTaskLevelForFetchName = 5;

        /// @brief 所有用户的id到姓名的一个大映射表，可从文件中读取，也可保存到文件中
        class AllId2Name {
         public:
           /// @brief AllId2Name
           /// 
           /// @param hash_size(in)  由于使用了hash_map, 为减少resize带来的性能问题，采用预分配的方法
           AllId2Name(int hash_size);
           bool Set(int id, string name);
           string Get(int id);
           bool Update(int id, string new_name);
           bool IsValid();
           bool SetValid();
           bool ReadFromFile(const string& file_name);
           int Size();
           bool Save2File(const string& file_name);

         private:
           hash_map<int, string> all_id_2_name_map_;
           IceUtil::RWRecMutex all_id_2_name_map_rw_lock_;
           bool all_id_2_name_map_valid_;
        };

        /// @brief 一级消息队列线程, 用于RebuildAllRelatedIndex中异步的重建核心用户的索引
        ///共有两个地方会调用重建索引的接口(RebuildIndex):一个是用户名字更改，另一处是好友关系变更，后者的调用量比较大，
        ///服务端重建所有相关的索引(RebuildAllRelatedIndex)也比较慢，很容易造成堵塞(无论是server端还是client端)， 为解决此问题， 
        ///在服务端采用异步消息队列机制，同时设定消息队列的最大长度，超过此长度则丢掉一些时间较久远的请求；RebuildAllRelatedIndex服务本身耗时较长，
        ///分两大块，首先是更新核心用户的索引，其次是删除所有相关用户的索引，前者较为重要，应优先完成，故使用了两级异步消息队列
        class FirstLevelMessageQueueThread : virtual public IceUtil::Thread {
         public:
           FirstLevelMessageQueueThread(unsigned int mod) :mod_(mod) {
           }
           void invoke(const Ice::Int user_id);
           void run();
         private:
           IceUtil::Monitor<IceUtil::Mutex> mutex_;
           std::queue<Ice::Int> cache_;
           unsigned int mod_;
        };

        /// @brief 二级消息队列线程，用于RebuildAllRelatedIndex中异步的删除相关用户的索引
        class SecondLevelMessageQueueThread : virtual public IceUtil::Thread {
         public:
           SecondLevelMessageQueueThread(unsigned int mod) :mod_(mod) {
           }

           void invoke(const Ice::Int user_id);
           void run(); 
         private:
           IceUtil::Monitor<IceUtil::Mutex> mutex_;
           std::queue<Ice::Int> cache_;
           unsigned int mod_;
        };


        class FriendOfFriendIndexerManagerI: virtual public FriendOfFriendIndexerManager, 
        public MyUtil::Singleton<FriendOfFriendIndexerManagerI> {
         public:
           FriendOfFriendIndexerManagerI();

           /// @brief BuildIndex 
           /// TODO:建索引要分优先级
           /// 
           /// @param userId 
           /// @param level 运行优先级，分为高中低三级 
           /// @param ic 
           /// 
           /// @return 
           virtual bool BuildIndex(::Ice::Int user_id, ::Ice::Short level, const ::Ice::Current& ic);

           /// @brief RebuildAllRelatedIndex 当用户名字改变或好友关心改变时，需触发重建所有相关的索引
           /// 
           /// @param user_id 
           /// @param level 运行优先级 
           /// @param ic 
           /// 
           /// @return 
           virtual bool RebuildAllRelatedIndex(::Ice::Int user_id, ::Ice::Short level, const ::Ice::Current& ic);
           bool RebuildIndex(int user_id);
           bool RemoveAllRelatedIndex(int user_id);

           void bootstrap();

         private :
           bool BuildIndex(::Ice::Int user_id);
           int cluster_;
           FriendOfFriendMapDB cache_handler_;
           std::vector<FirstLevelMessageQueueThread*> first_level_message_queue_thread_vec_;
           std::vector<SecondLevelMessageQueueThread*> second_level_message_queue_thread_vec_;
           AllId2Name all_id_2_name_;
           //TODO:破坏了封装性，改为内部类？
           friend class FirstLevelMessageQueueThread;
        };



        class BatchResultHandlerI: public com::xiaonei::xce::ResultHandler {
         public:
           BatchResultHandlerI(AllId2Name& id_2_name, int& max_id) : id_2_name_(id_2_name), max_id_(max_id) {};
           virtual bool handle(mysqlpp::Row& row) const;
         private :
           AllId2Name& id_2_name_;
           bool read_from_file_flag_;
           int& max_id_;
        };  

        class LoadAndUpdateId2NameMapTask : public MyUtil::Task {
         public :
           LoadAndUpdateId2NameMapTask(AllId2Name& all_id_2_name, const bool read_from_file_flag, const string& file);
           virtual void handle();
         private:
           AllId2Name& all_id_2_name_;
           bool read_from_file_flag_;
           string file_;
        };

      }
    }
  }
}
#endif
