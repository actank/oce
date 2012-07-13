#include <cmath>
#include "FeedUserRecordAdapter.h"

using namespace MyUtil;
using namespace xce::feed;

FeedUserRecordPrx FeedUserRecordAdapter::getManager(int id) {
  return locate<FeedUserRecordPrx> (_managers, "FeedUserRecord", id, TWO_WAY);
}

FeedUserRecordPrx FeedUserRecordAdapter::getManagerOneway(int id) {
  return locate<FeedUserRecordPrx> (_managers, "FeedUserRecord", id, ONE_WAY);
}

void FeedUserRecordAdapter::Set(::Ice::Int uid, ::Ice::Long feedid, ::Ice::Int type, const Ice::Current&) {
    getManagerOneway(uid)->Set(uid, feedid, type);

}

FeedUserRecordResPtr FeedUserRecordAdapter::Get(int uid, const Ice::Current&) {
  return getManager(uid)->Get(uid);
}


