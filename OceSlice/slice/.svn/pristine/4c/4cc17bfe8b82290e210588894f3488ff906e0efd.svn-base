#ifndef _MINIGROUP_ICE
#define _MINIGROUP_ICE

#include <Util.ice>
#include <ObjectCache.ice>

module xce {
module mngp {

  class MiniGroup {
    long id;
    string name;
    string icon;
    MyUtil::Str2StrMap extraInfo;
  };

  class User2MiniGroup {
    int userId;
    MyUtil::LongSeq miniGroupIds;
  };
  
  sequence<MiniGroup> MiniGroupSeq;

  class MiniGroupCache {
    void setData(MyUtil::ObjectResult miniGroupData);
    void setValid(bool valid);
  };

  class User2MiniGroupCache {
    void setData(MyUtil::ObjectResult user2MiniGroupData);
    void setValid(bool valid);
  };

  class MiniGroupManager {
    //获取用户一共参加了那几个MiniGroup
    MiniGroupSeq getMiniGroups(int userId);

    MiniGroupSeq getMiniGroupsWithIds(MyUtil::LongSeq miniGroupIds);
    //添加多个MiniGroup，如果系统中原来有相同id的MiniGroup，那么进行覆盖
    void addMiniGroups(MiniGroupSeq miniGroups);

    //设置用户加入了那些MiniGroups
    void setJoinedMiniGroups(int userId, MyUtil::LongSeq miniGroupIds);

    //将用户加入miniGroupIds中，miniGroupIds会追加到用户已经加入的MiniGroup的后面
    void addJoinedMiniGroups(int userId, MyUtil::LongSeq miniGroupIds);

    //删除一个用户加入的miniGroupIds, 删除后剩余的MiniGroup保持原来的顺序
    void removeJoinedMiniGroups(int userId, MyUtil::LongSeq miniGroupIds);

    //删除一个用户加入的所有miniGroupIds
    void removeAllJoinedMiniGroups(int userId);
    
    void setValid(bool flag);
  };

  class MiniGroupWriter {
    void addMiniGroups(MiniGroupSeq miniGroups); 
    void reloadMiniGroups(MyUtil::LongSeq miniGroupIds);
    void addJoinedMiniGroups(int userId, MyUtil::LongSeq miniGroupIds);
    void setJoinedMiniGroups(int userId, MyUtil::LongSeq miniGroupIds);
    void removeJoinedMiniGroups (int userId, MyUtil::LongSeq miniGroupIds);
    void reloadUsers(MyUtil::IntSeq userIds);
  };

};
};

#endif
