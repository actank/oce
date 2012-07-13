/*
 * FeedQueueI.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#include "FeedQueueI.h"
#include "LogWrapper.h"
#include "QueryRunner.h"
#include "ServiceI.h"
#include "MyQueue.h"
#include "Restore.h"

using namespace xce::feed;
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedQueueI::instance(), service.createIdentity(
      "M", ""));
  FeedQueueI::instance().Init();
  IdGenerator::instance().Init();
  Restorer::instance().Init();
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  int index = props->getPropertyAsIntWithDefault("Service.ServerIndex", 0);
  Restorer::instance().SetServiceIndex(index);
//  TaskManager::instance().execute(new RestoreTask("", "",1));//启动后回复上个小时和这个小时的
  TaskManager::instance().scheduleRepeated(&QueueStatTimer::instance());
}
void FeedQueueI::Init() {
  qm_ = new QueueManager();
  qm_->Init(5, 10000000);
}

int FeedQueueI::Push(const SqlInfoPtr& sql, const Ice::Current&) {
//  MCE_INFO("FeedQueueI::Push. " << sql->instance << " " << sql->wrflag << " " << sql->pattern << " " << sql->statement);
  return qm_->Push(sql);
//  for(int i = 0; i != 100; ++i){
//    if(!qm_->Push(sql)){
//      return 0;
//    }
//  }//TODO 一定要删除
//  return 1;
}

void FeedQueueI::TestPush(const SqlInfoPtr& sql, const Ice::Current&) {
  FeedQueueI::Push(sql);
}

void FeedQueueI::Restore(const string & begintime, const string & endtime,int thishour,
    const Ice::Current&) {
  MCE_INFO("FeedQueueI::Restore");
  //   Restorer::instance().Restore(begintime,endtime);
  TaskManager::instance().execute(new RestoreTask(begintime, endtime,thishour));
}

string FeedQueueI::DumpToString() {
  return qm_->DumpToString();
}
int FeedQueueI::Size() {
  return qm_->Size();
}

void QueueStatTimer::handle() {
  MCE_INFO("##QUEUESTAT##  FeedQueue " << FeedQueueI::instance().DumpToString()
      << "  Restorer " << Restorer::instance().DumpToString());
}

//void FeedQueueI::Restore(){
//  vector<string> inlogs,outlogs;
//  MakeLogsName(inlogs,outlogs);
//  if(!Restorer::instance().AddLogs(inlogs,outlogs)){
//
//    return;
//  }
//  TaskManager::instance().execute(&Restorer::instance());
//}


