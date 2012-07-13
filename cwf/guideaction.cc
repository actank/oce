#include "cwf/guideaction.h"

#include "boost/thread.hpp"

#include "base/globalinit.h"
#include "base/startuplist.h"
#include "base/ptime.h"

#include "cwf/frame.h"

namespace cwf {

int GuideAction::counter_ = 0;

bool GuideAction::Match(const std::string& url) const {
  return url.compare("/guideaction") == 0;
}
HttpStatusCode GuideAction::Process(Request* req, Response* res) {
  std::ostringstream oss;
  oss << "GuideAction::Process() counter:" << (++counter_);
//  PTIME(pt, oss.str(), true, 0);
  SetHeader(res);
  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
  res->WriteRaw("guideaction");
  return cwf::HC_OK;
}

void GuideAction::SetHeader(Response* res) {
  static const std::string kDefaultContentType("text/html; charset=utf-8");
  res->header().set_status_code(cwf::HC_OK, "OK");
  res->header().Add(cwf::HH_CONTENT_TYPE, kDefaultContentType);
  res->OutputHeader();
}

}

namespace xce {

static void Init() {
  cwf::FrameWork::RegisterAction(new cwf::GuideAction);
}

GLOBAL_INIT(CWF_SDK_ACTION, {
  AddStartup(&Init);
});

}


