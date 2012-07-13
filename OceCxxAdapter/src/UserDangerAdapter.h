#ifndef _USERDANGERADAPTER_H_
#define _USERDANGERADAPTER_H_

#include <IceUtil/RWRecMutex.h>
#include <UserDanger.h>
#include <AdapterI.h>
#include <TaskManager.h>

namespace xce {
  namespace userdanger {

      class UserDangerAdapter : public MyUtil::ReplicatedClusterAdapterI<UserDangerManagerPrx>, public MyUtil::Singleton<UserDangerAdapter> {
        public:
          UserDangerAdapter();
          bool isNotFeed(int userid);
          bool isNotAcceptBuddyApplication(int userid);
          bool isOnlySeenByFriend(int userid);
          bool isOnlySeenBySelf(int userid);
          bool isUGCOnlySeenBySelf(int userid);
          bool isUGCAllCheck(int userid);
          bool isSearchShielding(int userid);
          map<int, bool> isSearchShielding(const std::vector<int>& ids);
          bool isMailSubscriptionShielding(int userid);
          bool isUgcAuditFirst(int userid);
          bool isMiniGroupHide(int userid);
          bool isMiniSiteHide(int userid);
      };

  }
}
#endif
