#ifndef _FRIENDCLUSTERCACHE_ICE_
#define _FRIENDCLUSTERCACHE_ICE_

#include <Util.ice>

module xce {
module socialgraph {

  dictionary<int, MyUtil::StrList> Int2StrListMap;
  dictionary<string, MyUtil::IntList> Str2IntListMap;

  struct FriendClusterData {
     Int2StrListMap data1; // userid to clustername seq
     Str2IntListMap data2; // clustername to userid seq
  };

  interface FriendClusterCache {
    FriendClusterData GetFriendClusterData(MyUtil::IntList userIdList);
    MyUtil::StrSeq GetC(int id);
    MyUtil::IntSeq GetU(string cname);
  };

};
};

#endif
