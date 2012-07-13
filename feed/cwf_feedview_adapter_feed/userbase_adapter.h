#ifndef XCE_OLDSLICE_USERBASE_ADAPTER_H__
#define XCE_OLDSLICE_USERBASE_ADAPTER_H__

#include "arch/adapter2.h"
#include "UserBase.h"

namespace xce {

inline userbase::UserBaseViewManagerPrx CreateUserBaseViewAdapter(int uid) {
  return Adapter::userbase::Create<userbase::UserBaseViewManagerPrx>("UBVM@UserBase", uid % 100);
}

inline userbase::UserBaseViewManagerPrx CachedUserBaseViewAdapter(int uid) {
  return Adapter::userbase::Cached<userbase::UserBaseViewManagerPrx>("UBVM@UserBase", uid % 100, 100);
}

inline userbase::UserBasicManagerPrx CreateUserBasicAdapter(int uid) {
  return Adapter::userbase::Create<userbase::UserBasicManagerPrx>("UBM@UserBase", uid % 100);
}

inline userbase::UserBasicManagerPrx CachedUserBasicAdapter(int uid) {
  return Adapter::userbase::Cached<userbase::UserBasicManagerPrx>("UBM@UserBase", uid % 100, 100);
}

inline userbase::UserUrlManagerPrx CreateUserUrlAdapter(int uid) {
  return Adapter::userbase::Create<userbase::UserUrlManagerPrx>("U@UserBase", uid % 100);
}

inline userbase::UserUrlManagerPrx CachedUserUrlAdapter(int uid) {
  return Adapter::userbase::Cached<userbase::UserUrlManagerPrx>("U@UserBase", uid % 100, 100);
}

}
#endif // XCE_OLDSLICE_USERBASE_ADAPTER_H__
