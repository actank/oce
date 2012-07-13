#include "FriendClusterCacheAdapter.h"

namespace xce {
namespace socialgraph {


FriendClusterData FriendClusterCacheAdapter::Get(MyUtil::IntList userIdList) {
  if (!userIdList.empty()) {
    int id = userIdList[0];
    return getProxy(id)->GetFriendClusterData(userIdList);
  }
  else {
    return FriendClusterData();
  }
}
}}



