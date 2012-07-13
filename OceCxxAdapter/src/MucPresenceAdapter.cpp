#include "MucPresenceAdapter.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace talk::adapter;


PresenceHandlerPrx MucPresenceAdapter::getManager(int id) {
	return locate<PresenceHandlerPrx>(_managers, "M", id, TWO_WAY);
}

PresenceHandlerPrx MucPresenceAdapter::getManagerOneway(int id) {
	return locate<PresenceHandlerPrx>(_managersOneway, "M", id, ONE_WAY);
}

//---------------------------------------------

void MucPresenceAdapter::message(const JidPtr& host, const string& msg){
	getManagerOneway(host->userId)->message(host,msg);
}

void MucPresenceAdapter::mucOfflineNotify(const JidSeq& jids){
	if(jids.empty()){
		return;
	}
	getManagerOneway(jids.at(0)->userId)->mucOfflineNotify(jids);
}
