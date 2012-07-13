
#ifndef XCE_OLDSLICE_SOCIALGRAPHLOGIC_ADAPTER_H__
#define XCE_OLDSLICE_SOCIALGRAPHLOGIC_ADAPTER_H__

#include "arch/adapter2.h"
#include "OceSlice/slice/SocialGraphLogic.h"

namespace xce {

inline xce::socialgraph::SocialGraphLogicPrx CachedSocialGraphLogicAdapter(int uid) {
  return Adapter::oce::Cached<xce::socialgraph::SocialGraphLogicPrx>("ControllerSocialGraphLogic", uid % 10);
}
}
#endif // XCE_OLDSLICE_SOCIALGRAPHLOGIC_ADAPTER_H__
