
#include "FeedFocusAdapter.h"

using namespace MyUtil;
using namespace xce::feed;


FocusManagerPrx FeedFocusAdapter::getManager(int id) {
  return locate<FocusManagerPrx> (_managers, "M", id, TWO_WAY);
}

FocusManagerPrx FeedFocusAdapter::getManagerOneway(int id) {
  return locate<FocusManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}


FeedDataSeq FeedFocusAdapter::GetFocusFeedData(int user, int begin,int limit){
  return getManager(user)->GetFocusFeedData(user,begin,limit);
}

FeedDataSeq FeedFocusAdapter::GetFocusFeedDataByStype(int user, int begin,int limit,const vector<int> & stypes){
  return getManager(user)->GetFocusFeedDataByStype(user,begin,limit,stypes);
}

IntSeq FeedFocusAdapter::GetFocuses(int user){
  return getManager(user)->GetFocuses(user);
}

int FeedFocusAdapter::GetFocusFeedIncCount(int user) {
  return getManager(user)-> GetFocusFeedIncCount(user);
}
/*
void FeedFocusAdapter::AddFocuses(int user, const ::MyUtil::IntSeq& focuses){
  getManagerOneway(user)->AddFocuses(user,focuses);
}

void FeedFocusAdapter::DelFocuses(int user, const ::MyUtil::IntSeq& focuses){
  getManagerOneway(user)->DelFocuses(user,focuses);
}
*/



