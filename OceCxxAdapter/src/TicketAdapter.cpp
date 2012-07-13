#include "TicketAdapter.h"

using namespace passport;

void TicketAdapter::initialize() {
}

string TicketAdapter::createTicket(int userId,const string& infos) {
	return getProxy(userId)->createTicket(userId,infos);
}

string TicketAdapter::queryTicket(int userId) {
	return getProxy(userId)->queryTicket(userId);
}

int TicketAdapter::verifyTicket(const string& ticket,const string& infos) {
	if(ticket.length()==33){
		return getProxy(ticket.at(32) - '0')->verifyTicket(ticket,infos);
	}else{
		return -1;
	}
}
bool TicketAdapter::destroyTicket(const string& ticket){
	if(ticket.length()==33){
		return getProxy(ticket.at(32) - '0')->destroyTicket(ticket);
	}else{
		return false;
	}
}
