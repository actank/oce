#ifndef _FRIEND_OF_FRIEND_ICE
#define _FRIEND_OF_FRIEND_ICE

#include <Util.ice>

module com 
{
  module renren 
  {
    module search2 
    {
      module friendoffriend 
      {
        //零度好友(即自己)的类型值
        const short ZeroDegree = 0;
        //一度好友类型值
        const short FirstDegree = 1;
        //二度好友类型值
        const short SecondDegree = 2;

        //广义的好友信息结构体
        struct FriendInfo 
        {
          //好友的id
          int id;
          //好友的类型(零度好友表示自己，一度好友，二度好友，三度好友...)
          short type;
          //共同好友数 
          short commonFriendsNum;
        };
        sequence<FriendInfo> FriendInfoSeq;
        dictionary<string, FriendInfoSeq> Name2FriendInfoSeqMap;

        //计算二度好友的服务接口
        interface FriendOfFriendCacheManager {
          
          ///获得给定用户的所有好友信息列表(包括一度好友和二度好友,按照id从小到大排序), 当return的好友数小于指定的limit数值时，表示已经取到头了
          ///@param userid: 用户id
          /// @param begin 起始第几个
          /// @param limit 本次取几个
          /// 
          //@return：好友信息列表.
          FriendInfoSeq get(int userid, int begin, int limit, out bool isFinished);
          //加载指定用户ID的二度好友数据
          //
          //@param userid: 需要预加载用户id列表
          //@return: 返回true，表示加载成功，
          //         返回false，表示加载不成功
          bool load(int userid);

          //删除指定用户的二度好友数据
          //
          //@param userid: 需要删除的用户id
          //@return: 返回true，表示删除成功，
          //         返回false，表示删除不成功
          bool remove(int userid);

          /// @brief UpdateAllRelatedCache 更新于指定用户相关的所有用户的Cache，核心用户重新计算并缓存，其它用户直接删除
          /// 
          /// @param userid 
          /// 
          /// @return 
          bool UpdateAllRelatedCache(int userid);
        };
      };
    };
  };
};
#endif

