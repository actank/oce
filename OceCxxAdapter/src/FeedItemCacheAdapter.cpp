
#include "FeedItemCacheAdapter.h"

using namespace MyUtil;
using namespace xce::feed;


UserFeedsPrx FeedItemCacheAdapter::getManager(int id) {
  return locate<UserFeedsPrx> (_managers, "M", id, TWO_WAY);
}

UserFeedsPrx FeedItemCacheAdapter::getManagerOneway(int id) {
  return locate<UserFeedsPrx> (_managersOneway, "M", id, ONE_WAY);
}

void FeedItemCacheAdapter::put(const FeedMeta& d){
	UserFeedsPrx::uncheckedCast(getManagerOneway(0)->ice_datagram())->put(d);
  //getManagerOneway(0)->put(d);
}

FeedMetaSeq FeedItemCacheAdapter::get(const IntSeq& uids, int begin, int limit){
  return getManager(0)->get(uids, begin, limit);
}

FeedMetaSeq FeedItemCacheAdapter::getByTypes(const IntSeq& uids, int begin, int limit, const IntSeq& types){
  return getManager(0)->getByTypes(uids, begin, limit, types);
}

FeedMetaSeq FeedItemCacheAdapter::getWithFilter(const IntSeq& uids, int begin, int limit, const StrSeq& filter){
  return getManager(0)->getWithFilter(uids, begin, limit, filter);
}

FeedMetaSeq FeedItemCacheAdapter::getByTypesWithFilter(const IntSeq& uids, int begin, int limit, const IntSeq& types, const StrSeq& filter){
  return getManager(0)->getByTypesWithFilter(uids, begin, limit, types, filter);
}

/*
void FeedItemCacheAdapter::AddFocuses(int user, const ::MyUtil::IntSeq& focuses){
  getManagerOneway(user)->AddFocuses(user,focuses);
}

void FeedItemCacheAdapter::DelFocuses(int user, const ::MyUtil::IntSeq& focuses){
  getManagerOneway(user)->DelFocuses(user,focuses);
}
*/



