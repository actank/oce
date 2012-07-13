#include "PassportReplicaAdapter.h"
#include "OceCxxAdapter/src/BatchUpdaterReplicaAdapter.h"

using namespace mop::hi::svc::adapter;
using namespace xce::clusterstate;

PassportReplicaAdapter::PassportReplicaAdapter() :
	_clientCS("ControllerPassport", 120, 300){
	
	_clientCS.intialize();
	_cluster = _clientCS.getCluster();
}

string PassportReplicaAdapter::createTicket(int userId) {
//	return locate<PassportManagerPrx>(_managers, "Manager", userId, TWO_WAY)->createTicket(userId);    _clientCS.getProxy(INDEX)
	
	return _clientCS.getProxy(userId)->createTicket(userId);

}

string PassportReplicaAdapter::queryTicket(int userId) {
//	return locate<PassportManagerPrx>(_managers, "Manager", userId, TWO_WAY)->queryTicket(userId);

	return _clientCS.getProxy(userId)->queryTicket(userId);

}

int PassportReplicaAdapter::verifyTicket(const string& ticket) {
	if(ticket.length() == 33){
//		return locate<PassportManagerPrx>(_managers, "Manager", (ticket.at(32) - '0'), TWO_WAY)->verifyTicket(ticket);

		return _clientCS.getProxy(ticket.at(32) - '0')->verifyTicket(ticket);

	}else{
		return false;
	}
}
