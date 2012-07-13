
#include <boost/lexical_cast.hpp>

#include "FeedGroupCountI.h"
#include "FeedGroupAdapter.h"
#include "ServiceI.h"

using namespace MyUtil;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  using namespace xce::feed;
  service.getAdapter()->add(&FeedGroupCountI::instance(), service.createIdentity("M", ""));
}

namespace xce {
namespace feed {

FeedGroupCountI::FeedGroupCountI() {
  cache_.duration(1 * 60); // 1 分钟cache
}

MyUtil::Int2IntMap FeedGroupCountI::GetUnread(Ice::Int uid, const MyUtil::IntSeq & groups, const Ice::Current&) {
  MyUtil::Int2IntMap res = cache_.find(uid, MyUtil::Int2IntMap());
  if (res.empty()) {
    res = FeedGroupAdapter::instance().GetGroupsUnreadCount(uid, groups);
    if (!res.empty()) {
      // cache_.add(res, uid);
    }
  }
  MCE_INFO("uid " << uid << " get groups unread count : " << res.size() << "/" << groups.size());
  return res;
}

}
}

