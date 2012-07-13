
#include "FeedItemWrapperAdapter.h"

using namespace MyUtil;
using namespace xce::feed;


FeedItemWrapperPrx FeedItemWrapperAdapter::getManager(int id) {
  return locate<FeedItemWrapperPrx> (_managers, "M", id, TWO_WAY);
}

FeedItemWrapperPrx FeedItemWrapperAdapter::getManagerOneway(int id) {
  return locate<FeedItemWrapperPrx> (_managersOneway, "M", id, ONE_WAY);
}

FeedMetaSeq FeedItemWrapperAdapter::get(int userid, int begin, int limit){
  return getManager(0)->get(userid, begin, limit);
}

FeedMetaSeq FeedItemWrapperAdapter::GetFeeds(int userid, int begin, int limit, const MyUtil::IntSeq& types, const MyUtil::IntSeq& ntypes) {
  return getManager(0)->GetFeeds(userid, begin, limit, types, ntypes);
}

FeedMetaSeq FeedItemWrapperAdapter::GetFeedsByGroupName(int userid, const string& name, int begin, int limit, const MyUtil::IntSeq& types, const MyUtil::IntSeq& ntypes) {
  return getManager(0)->GetFeedsByGroupName(userid, name, begin, limit, types, ntypes);
}

FeedMetaSeq FeedItemWrapperAdapter::GetFeedsByFocus(int userid, int begin, int limit, const MyUtil::IntSeq& types, const MyUtil::IntSeq& ntypes) { 
  return getManager(0)->GetFeedsByFocus(userid, begin, limit, types, ntypes);
}
