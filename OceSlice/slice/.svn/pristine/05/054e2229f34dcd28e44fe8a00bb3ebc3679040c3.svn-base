#ifndef _FRIEND_OF_FRIEND_DEMO_
#define _FRIEND_OF_FRIEND_DEMO_
#include <SearchModel.ice>
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
          interface FriendOfFriendDemo { 
            /// @brief search 
            /// 
            /// @param id 用户的id
            /// @param query 用户输入的搜索串
            /// @param begin 
            /// @param limit 
            /// 
            /// @return 
            com::xiaonei::search2::model::IndexResult search(int id, string query, int begin, int limit, MyUtil::IntSeq excludeIds, bool rebuildFlag);
          };
        };
      };
    };
  };
};

#endif
