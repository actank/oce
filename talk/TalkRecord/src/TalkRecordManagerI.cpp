#include "TalkRecordManagerI.h"
#include "ServiceI.h"

using namespace com::xiaonei::talk::record;

void MyUtil::initialize(){
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(new TalkRecordManagerI, service.createIdentity("TRM", ""));
}


void TalkRecordManagerI::record(::Ice::Int from, ::Ice::Int to, const string& msg, const Ice::Current&){
	ostringstream os;
	os<< "from: " << from << " to: " << to << " msg: " << msg;
	MCE_INFO(os.str());
}

