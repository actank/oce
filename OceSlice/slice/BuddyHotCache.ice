#ifndef _BUDDY_HOT_CACHE_ICE_
#define _BUDDY_HOT_CACHE_ICE_

#include <Util.ice>
#include <ObjectCache.ice>

module xce {
module buddy {

class HotData{
  int hotfriend;
  int timestamp;
  string memo;
  int position;
};
sequence<HotData> HotDataSeq;

class BuddyHotData {
  HotDataSeq hotdatas;
};

interface BuddyHotCacheManager{
  
  //write
  void addHotFriend(int host, HotData data);
  void deleteHotFriend(int host, HotData data);
  void setHotFriend(int host,HotDataSeq data);
  
  //read
  MyUtil::IntSeq getHotFriendList(int host);
  HotDataSeq getHotFriendListData(int host);
  int getHotFriendCount(int host);

  //generate
  void reload(int host);  

  //HA
  bool isValid();
  void setValid(bool valid);
  
  //preloader support
  void setData(MyUtil::ObjectResult objs);

};

};
};
#endif
