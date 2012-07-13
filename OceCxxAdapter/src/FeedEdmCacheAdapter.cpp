#include "FeedEdmCacheAdapter.h"
namespace xce {
namespace edm {

FeedEdmCachePrx FeedEdmCacheAdapter::getManager(int id) {
  return locate<FeedEdmCachePrx> (_managers, "FEC", id, MyUtil::TWO_WAY);
}

FeedEdmCachePrx FeedEdmCacheAdapter::getManagerOneway(int id) {
  return locate<FeedEdmCachePrx> (_managersOneway, "FEC", id, MyUtil::ONE_WAY);
}
FeedSeedInfoPtr FeedEdmCacheAdapter::SelectEdm(Ice::Int user_id, const MyUtil::Str2StrMap& params, const FeedUserInfoPtr& user) {
  return getManager(user_id)->SelectEdm(user_id,params,user);
}
FeedSeedInfoPtr FeedEdmCacheAdapter::SelectEdmForPhone(Ice::Int user_id, const MyUtil::Str2StrMap& params, const FeedUserInfoPtr& user) {
  return getManager(user_id)->SelectEdmForPhone(user_id,params,user);
}
FeedSeedInfoPtr FeedEdmCacheAdapter::SelectEdmById(int edmid) {
  return getManager(0)->SelectEdmById(edmid);
}
FeedSeedInfoPtr FeedEdmCacheAdapter::SelectEdmByType(int userid, int type) {
  return getManager(0)->SelectEdmByType(userid, type);
}
std::string FeedEdmCacheAdapter::LookUpEdm(Ice::Int edm_id) {
  return getManager(edm_id)->LookUpEdm(edm_id);
}
void FeedEdmCacheAdapter::SetSendingFlag(Ice::Int edm_id) {
  getManagerOneway(edm_id)->SetSendingFlag(edm_id);
}
void FeedEdmCacheAdapter::SetSuspendFlag(Ice::Int edm_id) {
  getManagerOneway(edm_id)->SetSuspendFlag(edm_id);
}
void FeedEdmCacheAdapter::UpdateEdmInfo(Ice::Int edm_id) {
  getManagerOneway(edm_id)->UpdateEdmInfo(edm_id);
}
void FeedEdmCacheAdapter::SetForComplete() {
  getManagerOneway(0)->SetForComplete();
}
}
}

