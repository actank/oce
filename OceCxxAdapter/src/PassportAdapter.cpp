#include "PassportAdapter.h"

using namespace mop::hi::svc::adapter;

void PassportAdapter::initialize() {
	//_manager = locateProxy<PassportManagerPrx>("Manager", TWO_WAY);
	//_manager = PassportManagerPrx::uncheckedCast(_channel->getCommunicator()->stringToProxy("Manager@Passport"));
}

string PassportAdapter::createTicket(int userId) {
	return locate<PassportManagerPrx>(_managers, "Manager", userId, TWO_WAY)->createTicket(userId);
}

string PassportAdapter::queryTicket(int userId) {
	return locate<PassportManagerPrx>(_managers, "Manager", userId, TWO_WAY)->queryTicket(userId);
}

int PassportAdapter::verifyTicket(const string& ticket) {
	if(ticket.length() == 33){
		return locate<PassportManagerPrx>(_managers, "Manager", (ticket.at(32) - '0'), TWO_WAY)->verifyTicket(ticket);
	}else{
		return false;
	}
}
