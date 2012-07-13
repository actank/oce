#include "ExceptionRecorderI.h"
#include "ServiceI.h"
#include "IceLogger.h"

using namespace xce::server;
using namespace MyUtil;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&ExceptionRecorderI::instance(),service.createIdentity("M", ""));
}


bool ExceptionRecorderI::RecordException(const string& serverName,
    const string& type,
    const string& describe,
    const Ice::Current&) {
  MCE_DEBUG("ExceptionRecorderI::RecordException() --> serverName: " << serverName 
    << " type: " << type << " describe: "<<describe);
  return true;
}


ExceptionContent ExceptionRecorderI::getException(Ice::Int time, const Ice::Current&) {
  MCE_DEBUG("ExceptionRecorderI::getException() --> time: " << time; 
  ExceptionContent ec;
  ec.serverName="my server name.";
  return ec;
}


