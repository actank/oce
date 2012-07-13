#include "SocialGraphReportServiceAdapter.h"

namespace xce {
namespace socialgraph {

std::string ReportServiceAdapter::applyJobId(const std::string& biz) {
  return getProxy(0)->applyJobId(biz);
}

void ReportServiceAdapter::start(const std::string& biz, const std::string& message) {
  getProxy(0)->reportJob(biz, kStart, message);

}
void ReportServiceAdapter::running(const std::string& biz, const std::string& message) {
  getProxy(0)->reportJob(biz, kRunning, message);
}

void ReportServiceAdapter::finish(const std::string& biz, const std::string& message) {
  getProxy(0)->reportJob(biz, kFinish, message);
}

void ReportServiceAdapter::interrupt(const std::string& biz, const std::string& message) {
  getProxy(0)->reportJob(biz, kInterrupt, message);
}

}}


