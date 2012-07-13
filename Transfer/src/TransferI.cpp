/*
 * TransferI.cpp
 *
 *  Created on: Sep 19, 2009
 *      Author: antonio
 */

#include "ServiceI.h"
#include <time.h>
#include "TransferI.h"
#include "util/cpp/String.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace passport;
using namespace MyUtil;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.getAdapter()->add(&TransferManagerI::instance(),
			service.createIdentity("M", ""));
	//	--------------------------------HA--------------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Transfer.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Transfer.Interval",5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerTransfer",&TransferManagerI::instance(),mod,interval);
	{ 
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Transfer.Status",0);
		xce::serverstate::ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status<<" mod : "<<mod<<" interval : "<<interval;
		MCE_WARN( os.str() );
	}
	
	TaskManager::instance().scheduleRepeated(new RemoveTimer());
}

string TransferManagerI::createTicket(int userId, int second,
		const Ice::Current&) {
	MCE_DEBUG("TransferManagerI::createTicket");
	Ticket t;
	t.uid = userId;
	t.time = time(NULL) + second;
	t.ticket = generateTicket(t);

	IceUtil::Mutex::Lock lock(_mutex[userId % POOL_SIZE]);

	pair<TicketSet::iterator, bool> p =
			_ticketSetPool[userId % POOL_SIZE].insert(t);
	if (p.second) {
		return t.ticket;
	} else {
		t.ticket = p.first->ticket;
		_ticketSetPool[userId % POOL_SIZE].replace(p.first, t);
		return t.ticket;
	}
}
int TransferManagerI::verifyTicket(const string& ticket, const Ice::Current&) {
	MCE_DEBUG("TransferManagerI::verifyTicket");
	//	ostringstream os;
	//	for(size_t i = 0; i < ticket.length();++i){
	//		if(ticket[i] >= '0' && ticket[i]<='9'){
	//			os << ticket[i];
	//		}else{
	//			break;
	//		}
	//	}
	//	string tmp = os.str();
	if (ticket.empty()) {
		return -1;
	}
	int mod = ticket[0] - '0';
	if (mod >= 0 && mod < 10) {

		IceUtil::Mutex::Lock lock(_mutex[mod]);

		TicketSet::nth_index<2>::type::iterator it = _ticketSetPool[mod].get<2>().find(ticket);
		if (it != _ticketSetPool[mod].get<2> ().end()) {
			return it->uid;
		}

	}
	return -1;
}

void TransferManagerI::removeTimeOutTicket() {
	time_t time = ::time(NULL);

	for (size_t i = 0; i < POOL_SIZE; ++i) {
		IceUtil::Mutex::Lock lock(_mutex[i]);

		TicketSet::nth_index<1>::type& index = _ticketSetPool[i].get<1> ();
		for (TicketSet::nth_index<1>::type::iterator it = index.begin(); it
				!= index.end();) {

			MCE_DEBUG("time:"<<time <<"  it->time:"<<time << " it->ticket"<< " id:"<<it->uid);
			if (it->time < time) {
				MCE_DEBUG("remove timeout ticket:"<<it->ticket);
				it = index.erase(it);
			} else {
				return;
			}
		}
	}
}
string TransferManagerI::generateTicket(Ticket& t) {
	ostringstream os;
	os << t.uid << "_" << t.time;
	string ticket = StrUtil::md5(os.str());
	os.str("");
	os << t.uid % 10 << ticket;
	return os.str();
}

void RemoveTimer::handle() {
	MCE_DEBUG("RemoveTimer::handle");
	TransferManagerI::instance().removeTimeOutTicket();
}
