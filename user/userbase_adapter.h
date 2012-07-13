#ifndef XCE_OLDSLICE_USERBASE_ADAPTER_H__
#define XCE_OLDSLICE_USERBASE_ADAPTER_H__

#include "arch/adapter2.h"
#include "OceSlice/slice/UserBase.h"

namespace xce {

inline userbase::UserBaseViewManagerPrx CachedUserBaseViewAdapter(int uid) {
	return Adapter::userbase2::Cached<userbase::UserBaseViewManagerPrx>("ControllerUserBase", uid);
}

inline userbase::UserBaseViewManagerPrx CreateUserBaseViewAdapter(int uid) {
	return CachedUserBaseViewAdapter(uid);
}

inline userbase::UserBasicManagerPrx CachedUserBasicAdapter(int uid) {
  return Adapter::userbase2::Cached<userbase::UserBasicManagerPrx>("ControllerUserBase", uid);
}

inline userbase::UserBasicManagerPrx CreateUserBasicAdapter(int uid) {
  return CachedUserBasicAdapter(uid);
}


inline userbase::UserUrlManagerPrx CachedUserUrlAdapter(int uid) {
  return Adapter::userbase2::Cached<userbase::UserUrlManagerPrx>("ControllerUserBase", uid);
}

inline userbase::UserUrlManagerPrx CreateUserUrlAdapter(int uid) {
	return CachedUserUrlAdapter(uid);
}

}
#endif // XCE_OLDSLICE_USERBASE_ADAPTER_H__
