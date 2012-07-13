#ifndef __FEEDCLASS_ICE__
#define __FEEDCLASS_ICE__

#include <Util.ice>
#include <RFeed.ice>
module xce
{
module feed
{

class FeedClass{
 // MyUtil::IntSeq GetMembers(int groupid);
 // void AddMember(int memberid,int groupid);
 // void DelMember(int memberid,int groupid);
 // void DelGroup(int groupid);
  FeedDataSeq GetFeedDataOfFriendsInGroups(int uid, MyUtil::IntSeq groupids,int begin, int limit) ;
};


};
};

#endif