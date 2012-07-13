#ifndef _BUDDYSOA_ICE_
#define _BUDDYSOA_ICE_

#include <Util.ice>

module xce {
module buddy {

  enum Operation {
    AddFriendApply,
    AcceptFriendApply,
    DenyFriendApply,
    DenyAllFriendApply,
    DeleteFriend,
    AddHotFriend,
    DeleteHotFriend,
    DenyFriendRecommend,
    DenyAllFriendRecommend,
    AddBlock,
    RemoveBlock
  };


  struct FriendApply {
    int applicant;
    int accepter;
    string domain;
    string content;
    string from;
    int ip; 
    long timestamp;
  };  

  struct FriendApplyResult {
    int applicant;
    int accepter;
    int result;
    string message;
  };  

  interface BuddySOAManager {
    bool needAuthCode(int userId, Operation op);
    FriendApplyResult addFriendApply(FriendApply fa);
    FriendApplyResult acceptFriendApply(FriendApply fa);
    int deleteFriend(int host, int guest);
    int addBlock(int host, int guest);
    int removeBlock(int host, int guest);
    int denyFriendApply(FriendApply fa);
    int denyAllFriendApply(int id);
    int recommendFriend(int matchmaker, int targetUserId, MyUtil::IntList recommendIdList, bool isOneWay, int type);
    int denyRecommendFriend(int host, int guest);
    int denyAllRecommendFriend(int host);
    int addHotFriend(int host, int friend);
    int updateHotFriendMemo(int host, int friend, string memo);
    int deleletHotFriend(int host, int friend);
    int setHotFriend(int host, MyUtil::IntList friends);
  };  

};
};
#endif
