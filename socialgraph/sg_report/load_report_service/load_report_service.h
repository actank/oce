#ifndef _XCE_SOCIALGRAPH_REPORT_SERVICE_H__
#define _XCE_SOCIALGRAPH_REPORT_SERVICE_H__

#include "SocialGraphReport.h"
#include "ServiceI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "third-party/mongo-c-driver/mongo.h"


namespace xce {
namespace socialgraph {

class MongoDbOperator : virtual public MyUtil::Singleton<MongoDbOperator> {
 public:
  // create the job with a job_name
  // @param job_name biz or sth else 
  // @return the unique job_id string for this job
  std::string CreateJob(const std::string& job_name);
  // update the status which can be only increased.
  // we should check the current status value, and decide whether to increase status or not
  // @param job_id a string which stands for a single job
  // @param status int value for next status
  // @param status a string message
  // @return void
  void UpdateToStatus(const std::string& job_id, int status, const std::string& msg = "");

  bool IsConnected() {
    return conn_status_ == mongo_conn_success;
  }
  ~MongoDbOperator();
 private:
  MongoDbOperator();
  void Insert(Ice::Long ts, const std::string& job_id);
  friend class MyUtil::Singleton<MongoDbOperator>;
 private:
  mongo_connection conn_;
  int conn_status_;
};

class LoadReportServiceImpl : virtual public ReportService, virtual public MyUtil::Singleton<LoadReportServiceImpl> {
 public:
  ~LoadReportServiceImpl() { }
  virtual std::string applyJobId(const std::string&, const Ice::Current& = Ice::Current());
  virtual void reportJob(const std::string&, Ice::Int status, const std::string&, const Ice::Current& = Ice::Current());
 private:
  LoadReportServiceImpl() {}
  friend class MyUtil::Singleton<LoadReportServiceImpl>;
};


}}

#endif

