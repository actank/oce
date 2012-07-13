
#include "FeedClassAdapter.h"

using namespace MyUtil;
using namespace xce::feed;


FeedClassPrx FeedClassAdapter::getManager(int id) {
  return locate<FeedClassPrx> (_managers, "M", id, TWO_WAY);
}

FeedClassPrx FeedClassAdapter::getManagerOneway(int id) {
  return locate<FeedClassPrx> (_managersOneway, "M", id, ONE_WAY);
}


//void FeedClassAdapter::AddMember(int memberid,int groupid){
//  return getManager(memberid)->AddMember(memberid,groupid);
//}
//void FeedClassAdapter::DelMember(int memberid,int groupid){
//  return getManagerOneway(memberid)->DelMember(memberid,groupid);
//}
//void FeedClassAdapter::DelGroup(int groupid){
//  for(unsigned i = 0; i != cluster(); ++i){
//    getManagerOneway(i)->DelGroup(groupid);
//  }
//}
FeedDataSeq FeedClassAdapter::GetFeedDataOfFriendsInGroups(int uid, MyUtil::IntSeq groupids,int begin, int limit) {
  return getManager(uid)->GetFeedDataOfFriendsInGroups(uid,groupids,begin,limit);
}




