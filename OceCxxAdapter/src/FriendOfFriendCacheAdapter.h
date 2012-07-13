/// 
/// @file FriendOfFriendCacheAdapter.h
/// @brief 二度好友缓存服务的适配器，供客户端使用
/// @author zhigang.wu@opi-corp.com
/// @date 2010-05-17
/// 
#ifndef __FRIEND_OF_FRIEND_CACHE_ADAPTER_H__
#define __FRIEND_OF_FRIEND_CACHE_ADAPTER_H__

#include "FriendOfFriendCache.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

//#define USE_SEARCH_TEST_CHANNEL
namespace com {
namespace renren {
namespace search2 {
namespace friendoffriend {

class FriendOfFriendCacheAdapter : public MyUtil::AdapterI, 
#ifdef USE_SEARCH_TEST_CHANNEL
  public MyUtil::AdapterISingleton<MyUtil::SearchFoFChannel, FriendOfFriendCacheAdapter> {
#else 
  public MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, FriendOfFriendCacheAdapter> {
#endif
   public:
     FriendOfFriendCacheAdapter(); 

     /// @brief load 通知FriendOfFriendCache服务加载指定用户的多维好友信息
     /// 
     /// @param user_id 用户id
     /// 
     /// @return 无异常返回true，否则返回false
     bool load(int user_id);

          /// @brief get 获取指定用户的多维好友信息
     /// 
     /// @param user_id 指定用户的id
     /// 
     /// @return 多维好友信息
     FriendInfoSeq get(int user_id);

     /// @brief remove 在缓存中删除指定用户的多维好友信息
     /// 
     /// @param user_id 指定用户的id
     /// 
     /// @return 成功删除返回true，否则返回false
     bool remove(int user_id);

     bool UpdateAllRelatedCache(int user_id);

   private:
     virtual string name() {
       return "FOF";
     }
     virtual string endpoints() {
       return "@FriendOfFriendCache";
     }
     virtual size_t cluster() {
       return 1;
     }

     enum {
       kProxyNum = 100,
     };

     
     vector<FriendOfFriendCacheManagerPrx> _proxies;

     FriendOfFriendCacheManagerPrx GetFriendOfFriendCacheManager(int id);
     vector<FriendOfFriendCacheManagerPrx> _managers;
  };


}
}
}
}



#endif
