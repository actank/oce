#ifndef OCE_USER_ADAPTER_H__
#define OCE_USER_ADAPTER_H__

#include "arch/adapter.h"
#include "user/buddycore.h"

namespace xce {

using com::xiaonei::service::buddy::BuddyDesc;
using com::xiaonei::service::buddy::OpState;
using com::xiaonei::service::buddy::ApplyInfo;
using com::xiaonei::service::buddy::Applyship;
using com::xiaonei::service::buddy::Blockship;
using com::xiaonei::service::buddy::Friendship;
using com::xiaonei::service::buddy::Relationship;

typedef ClientPtr<com::xiaonei::service::buddy::BuddyCoreManagerPrx> BuddyCoreAdapter;

inline BuddyCoreAdapter CachedBuddyCore(int id) {
  static BuddyCoreAdapter x_(
    RegistryLocator::xiaonei_instance()     // 指明 Registry
    , ModuloPolicy("M@BuddyCore", 10), id); // 指明散的策略, (proxy_name, cluster_count)
  return x_;
}

}
#endif // OCE_USER_ADAPTER_H__
