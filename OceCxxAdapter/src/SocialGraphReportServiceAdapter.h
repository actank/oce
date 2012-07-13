#ifndef _SG_REPORT_SERVICE_ADAPTER_H_
#define _SG_REPORT_SERVICE_ADAPTER_H_

#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "SocialGraphReport.h"

namespace xce {
namespace socialgraph {

class ReportServiceAdapter : public MyUtil::ReplicatedClusterAdapterI<ReportServicePrx>, 
                             public MyUtil::Singleton<ReportServiceAdapter> {
 public:
  ReportServiceAdapter():
		MyUtil::ReplicatedClusterAdapterI<ReportServicePrx>("ControllerSocialGraphReportService", 120, 1000, new MyUtil::SocialGraphChannel()) {
  }

  std::string applyJobId(const std::string& biz); 
  void start(const std::string& biz, const std::string& message = "");
  void running(const std::string& biz, const std::string& message = "");
  void finish(const std::string& biz, const std::string& message = "");
  void interrupt(const std::string& biz, const std::string& message = "");

  void startAndRunning(const std::string& biz, const std::string& message = "") {
    start(biz, "");
    running(biz, message);
  }

}; 

}}

#endif 

