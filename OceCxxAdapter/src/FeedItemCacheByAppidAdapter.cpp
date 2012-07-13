
#include "FeedItemCacheByAppidAdapter.h"

using namespace MyUtil;
using namespace xce::feed;


UserFeedsPrx FeedItemCacheByAppidAdapter::getManager(int id) {
  return locate<UserFeedsPrx> (_managers, "M", id, TWO_WAY);
}

UserFeedsPrx FeedItemCacheByAppidAdapter::getManagerOneway(int id) {
  return locate<UserFeedsPrx> (_managersOneway, "M", id, ONE_WAY);
}

void FeedItemCacheByAppidAdapter::put(const FeedMeta& d){
	UserFeedsPrx::uncheckedCast(getManagerOneway(0)->ice_datagram())->put(d);
  //getManagerOneway(0)->put(d);
}

FeedMetaSeq FeedItemCacheByAppidAdapter::get(const IntSeq& uids, int begin, int limit){
  return getManager(0)->get(uids, begin, limit);
}

FeedMetaSeq FeedItemCacheByAppidAdapter::getByTypes(const IntSeq& uids, int begin, int limit, const IntSeq& types){
  return getManager(0)->getByTypes(uids, begin, limit, types);
}

FeedMetaSeq FeedItemCacheByAppidAdapter::getWithFilter(const IntSeq& uids, int begin, int limit, const StrSeq& filter){
  return getManager(0)->getWithFilter(uids, begin, limit, filter);
}

FeedMetaSeq FeedItemCacheByAppidAdapter::getByTypesWithFilter(const IntSeq& uids, int begin, int limit, const IntSeq& types, const StrSeq& filter){
  return getManager(0)->getByTypesWithFilter(uids, begin, limit, types, filter);
}

/*
void FeedItemCacheByAppidAdapter::AddFocuses(int user, const ::MyUtil::IntSeq& focuses){
  getManagerOneway(user)->AddFocuses(user,focuses);
}

void FeedItemCacheByAppidAdapter::DelFocuses(int user, const ::MyUtil::IntSeq& focuses){
  getManagerOneway(user)->DelFocuses(user,focuses);
}
*/



