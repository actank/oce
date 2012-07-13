
#include "FeedFocusInvertAdapter.h"

using namespace MyUtil;
using namespace xce::feed;


FeedFocusInvertPrx FeedFocusInvertAdapter::getManager(int id) {
  return locate<FeedFocusInvertPrx> (_managers, "M", id, TWO_WAY);
}

FeedFocusInvertPrx FeedFocusInvertAdapter::getManagerOneway(int id) {
  return locate<FeedFocusInvertPrx> (_managersOneway, "M", id, ONE_WAY);
}



/*
void FeedFocusInvertAdapter::AddFocuses(int user, const ::MyUtil::IntSeq& focuses){
  getManagerOneway(user)->AddFocuses(user,focuses);
}

void FeedFocusInvertAdapter::DelFocuses(int user, const ::MyUtil::IntSeq& focuses){
  getManagerOneway(user)->DelFocuses(user,focuses);
}
*/



