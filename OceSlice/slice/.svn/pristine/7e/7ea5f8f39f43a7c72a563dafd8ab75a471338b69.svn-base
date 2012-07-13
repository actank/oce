#ifndef _FRIEND_OF_FRIEND_REBUILD_INDEX_
#define _FRIEND_OF_FRIEND_REBUILD_INDEX_

#include <Util.ice>

module com
{
  module xiaonei
  {
    module search2
    {
      module xce 
      {
        module friendoffriend
        {
          interface FriendOfFriendRebuildIndex {
            /// @brief GetIndexPath 获取指定用户的索引路径，若索引不存在，
            ///     则实时建索引，若索引已经存在，则直接返回相应路径
            /// 
            /// @param id 指定用户id
            /// @param rebuildFlag 每次是否都重建索引
            /// 
            /// @return 索引的路径
            string GetIndexPath(int id, MyUtil::IntSeq excludeIds, bool rebuildFlag);

            bool DeleteIndex(int id);
            //bool RemoveIndex(int id);
          };
        };
      };
    };
  };
};
#endif
