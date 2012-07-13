#include "RestMediatorAdapter.h"

using namespace talk::rest;


RestMediatorManagerPrx RestMediatorAdapter::getManagerOneway(int id) {
	return locate<RestMediatorManagerPrx> (_managerOneway, "M", id, ONE_WAY);
}
void RestMediatorAdapter::restRequest(Ice::Long reqId, int userid, const string& path, const map<string, string>& paras){
	getManagerOneway(userid)->restRequest(reqId, userid, path, paras);
}

