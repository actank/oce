#ifndef __FRIENDOFFRIENDCACHEI_H__
#define __FRIENDOFFRIENDCACHEI_H__

#include <queue>
#include <ext/hash_map>
#include <ext/hash_set>
#include <IceUtil/IceUtil.h>
#include "FriendOfFriendCache.h"
#include "Id2FriendInfoSeqMapDB.h"
#include "ServiceI.h"
#include "QueryRunner.h"

namespace com {
  namespace renren {
  namespace search2 {
  namespace friendoffriend {

    typedef __gnu_cxx::hash_map<int, std::vector<FriendInfo> > Int2FriendInfoSeqMap;
    using __gnu_cxx::hash_set;

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

    class FriendOfFriendCacheManagerI: virtual public FriendOfFriendCacheManager, 
    public MyUtil::Singleton<FriendOfFriendCacheManagerI> {
     public:
       FriendOfFriendCacheManagerI();

       /// @brief load在缓存中加载指定用户的多维好友信息
       /// 
       /// @param user_id 
       /// @param Ice::Current& 
       /// 
       /// @return 
       virtual bool load(const int user_id, const Ice::Current&);

       /// @brief remove 删除缓存中指定用户的多维好友信息
       /// 
       /// @param user_id 
       /// @param Ice::Current& 
       /// 
       /// @return 
       virtual bool remove(const int user_id, const Ice::Current&);

      /// @brief get 获取指定用户的多维好友信息
       /// 
       /// @param user_id 
       /// @param Ice::Current& 
       /// 
       /// @return 
       virtual FriendInfoSeq get(const int user_id, const int begin, const int end, bool& is_finished, const Ice::Current&);

       bool UpdateAllRelatedCache(const int user_id, const Ice::Current& ic);
       bool UpdateAllRelatedCache(const int user_id);
       void bootstrap();
       bool UpdateSpecialUserIdSet(hash_set<int>& new_set);
     private :
       /// @brief 缓存， Key value 形式，缓存指定用户所有二度好友(包括一度好友和自己)映射。 
       Id2FriendInfoSeqMapDB mem_cache_handler_;

       std::vector<FirstLevelMessageQueueThread*> first_level_message_queue_thread_vec_;

       /// @brief 好友特别多(大于2000)的用户id集，从数据库中加载得到。对于极端情况下用户二度好友数较多的情况，直接过滤掉，不进行计算
       hash_set<int> special_user_id_set_;
       /// @brief 读写set_rw_rec_lock_用到的锁
       IceUtil::RWRecMutex set_rw_rec_lock_;
       /// @brief 
       int cluster_;
       /// @brief 
       int mod_;
    };

    class UpdateSpecialUserIdSetTask : public MyUtil::Task {
     public:
      UpdateSpecialUserIdSetTask();
      void handle();
    };

    class BatchResultHandlerI : public com::xiaonei::xce::ResultHandler {
     public:
       BatchResultHandlerI(hash_set<int>& special_user_id_set);
       virtual bool handle(mysqlpp::Row&) const;
     private:
       hash_set<int>& special_user_id_set_;
    };

  }
}
}
}
#endif
