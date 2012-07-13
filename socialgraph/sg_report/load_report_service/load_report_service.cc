#include "load_report_service.h"

#include <sstream>

#include "third-party/mongo-c-driver/mongo.h"
#include "third-party/mongo-c-driver/bson.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"


void MyUtil::initialize() {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  using namespace xce::serverstate;
  using namespace xce::socialgraph;
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&LoadReportServiceImpl::instance(), service.createIdentity("SGRS", ""));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("SocialGraphReportService.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("SocialGraphReportService.Interval", 5);

  static const std::string kControllerName = "ControllerSocialGraphReportService";

  ConnectionPoolManager::instance().initialize();

  // register to Controller Service
  ServerStateSubscriber::instance().initialize(kControllerName,  &LoadReportServiceImpl::instance(), mod, interval, new MyUtil::SocialGraphChannel());
  ServerStateSubscriber::instance().setStatus(1);
}

namespace xce {
namespace socialgraph {

const static char* kMongoDbServer = "sg8.xce.d.xiaonei.com";
const static char* kMongoDbNamespace = "sg.taskreport";
const static int kMongoDbPort = 27017;

MongoDbOperator::MongoDbOperator() {
  MCE_INFO("create a mongodb connection");
  conn_status_ = mongo_connect(&conn_, kMongoDbServer, kMongoDbPort);
  if (IsConnected()) {
    MCE_INFO("mongodb connection is established");
  }
  else {
    MCE_WARN("create mongodb connection failed!");
  }
}

MongoDbOperator::~MongoDbOperator() {
  MCE_INFO("destroy mongodb connection");
  mongo_destroy(&conn_);
}

std::string MongoDbOperator::CreateJob(const std::string& job_name) {
  Ice::Long ts = IceUtil::Time::now().toMilliSeconds();
  std::ostringstream otem;
  otem << job_name << "_" << ts;
  // insert db
  Insert(ts, otem.str());
  return otem.str();
}

void MongoDbOperator::Insert(Ice::Long ts, const std::string& job_id) {
  if (!IsConnected()) {
    MCE_WARN("MongoDbOperator::Insert() db connection is not available!!!");
    return;
  }
  bson b[1];
	bson_buffer buf[1];
	bson_buffer_init(buf);
	bson_append_string(buf, "job_id", job_id.c_str());
	bson_append_long(buf, "apply", ts);
	bson_append_long(buf, "start", ts);
	bson_append_long(buf, "running", ts);
	bson_append_long(buf, "finish", ts);
	bson_append_long(buf, "interrupt", ts);
	bson_append_int(buf, "status", 0);
	{
		bson_buffer* sa = bson_append_start_array(buf, "message");
		bson_append_finish_object(sa);
	}
	bson_from_buffer(b, buf);
	mongo_insert(&conn_, kMongoDbNamespace, b);
  bson_destroy(b);
}

void MongoDbOperator::UpdateToStatus(const std::string& job_id, int status, const std::string& msg) {
  if (!IsConnected()) {
    MCE_WARN("MongoDbOperator::Insert() db connection is not available!!!");
    return;
  }
  bson cond[1];
	bson_buffer cond_buf[1];
	bson_buffer_init(cond_buf);
  bson_append_string(cond_buf, "job_id", job_id.c_str());
  {
    bson_buffer * sub = bson_append_start_object(cond_buf, "status");
    bson_append_int(sub, "$lt", status); 
    bson_append_finish_object(sub);
  }
  bson_from_buffer(cond, cond_buf);

  Ice::Long ts = IceUtil::Time::now().toMilliSeconds();
  bson op[1];
	bson_buffer op_buf[1];
	bson_buffer_init(op_buf);
  {
    bson_buffer * sub = bson_append_start_object(op_buf, "$set");
    bson_append_int(sub, "status", status);
    switch (status) {
      case kStart:
        bson_append_long(sub, "start", ts);
        break;
      case kRunning:
        bson_append_long(sub, "running", ts);
        break;
      case kFinish:
        bson_append_long(sub, "finish", ts);
        break;
      case kInterrupt:
        bson_append_long(sub, "interrupt", ts);
        break;
      default:
        MCE_WARN("ERROR status:" << status);
        break;
    }
    bson_append_finish_object(sub);
  }
  {
    if (!msg.empty()) {
      bson_buffer * sub = bson_append_start_object(op_buf, "$push");
      bson_append_string(sub, "message", msg.c_str());
      bson_append_finish_object(sub);
    }
  }
  bson_from_buffer(op, op_buf);
  mongo_update(&conn_, kMongoDbNamespace, cond, op, 0);
  bson_destroy(op);
  bson_destroy(cond);
}



std::string LoadReportServiceImpl::applyJobId(const std::string& biz_name, const Ice::Current& current) {
  MCE_INFO("LoadReportServiceImpl::applyJobId()");
  return MongoDbOperator::instance().CreateJob(biz_name);
}

void LoadReportServiceImpl::reportJob(const std::string& biz_name, Ice::Int status , const std::string& message, const Ice::Current& current) {
  MCE_INFO("LoadReportServiceImpl::reportJob()");
  return MongoDbOperator::instance().UpdateToStatus(biz_name, status, message);
}

}}



