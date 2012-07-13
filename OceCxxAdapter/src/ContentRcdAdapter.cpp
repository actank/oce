#include "ContentRcdAdapter.h"

namespace xce {
namespace socialgraph {

ContentRcdSeq ContentRcdAdapter::GetVideoRcd(int host, ContentRcd rcd) {
  return getProxy(0)->GetVideoRcd(host, rcd);
}

}}

