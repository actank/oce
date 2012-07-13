#ifndef _SOCIALGRAPH_REPORT_H_
#define _SOCIALGRAPH_REPORT_H_

#include <Util.ice>

module xce {
module socialgraph {

  const string kUnknownJobName = "UNKNOWN_JOB_NAME";

  const int kError = -1;
  const int kApply = 0;
  const int kStart = 1;
  const int kRunning = 2;
  const int kFinish = 3;
  const int kInterrupt = 4;

  interface ReportService {
    string applyJobId(string biz);
    void reportJob(string biz, int status, string message);
  };

};
};

#endif
