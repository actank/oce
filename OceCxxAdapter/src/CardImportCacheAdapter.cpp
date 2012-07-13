#include "OceCxxAdapter/src/CardImportCacheAdapter.h"

using namespace xce::cardcache;

CardImportCacheAdapter::CardImportCacheAdapter():
		clientCS_("ControllerCardImportCache", 120, 300) {
	clientCS_.intialize();
}

ImportInfoSeq CardImportCacheAdapter::get(int userId, int limit) {

	try {
		return clientCS_.getProxy(0)->get(userId,limit);			
	} catch (...) {
		MCE_WARN("get userIds batch exception");
	}
	return ImportInfoSeq();
}

ImportInfoNewSeq CardImportCacheAdapter::getNew(int userId, int limit) {

	try {
		return clientCS_.getProxy(0)->getNew(userId,limit);			
	} catch (...) {
		MCE_WARN("get userIds batch exception");
	}
	return ImportInfoNewSeq();
}
