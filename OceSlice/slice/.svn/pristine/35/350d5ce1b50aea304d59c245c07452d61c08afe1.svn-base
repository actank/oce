#ifndef _FRIEND_OF_FRIEND_INDEXER_ICE
#define _FRIEND_OF_FRIEND_INDEXER_ICE

#include <Util.ice>
#include <FriendOfFriendCache.ice>

module com 
{
  module renren
  {
    module search2
    {
      module friendoffriend
      {  
        const short LowLevel = 0;
        const short MiddleLevel = 1;
        const short HighLevel = 2;


        interface FriendOfFriendIndexerManager 
        {

          /// @brief BuildIndex
          /// 
          /// @param userId 
          /// @param level 运行级别(异步调用，可以设置运行的优先级)
          /// 
          /// @return 
          ["ami"]
            bool BuildIndex(int userId, short level);

          /// 1.好友关系变更  2. 姓名更改 3.账号注销或封禁
          /// @brief RebuildIndexForNameChange 姓名更改，通知重建相关索引
          /// 
          /// @param userId 
          /// @param level 运行级别(异步调用，可以设置运行的优先级)
          /// 
          /// @return 
          ["ami"]
            bool RebuildAllRelatedIndex(int userId, short level);
        };

      };
    };
  };
};
#endif
