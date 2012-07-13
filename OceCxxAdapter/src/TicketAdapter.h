#ifndef TICKETADAPTER_H_
#define TICKETADAPTER_H_

#include "AdapterI.h"
#include "Singleton.h"
#include "Ticket.h"
namespace passport {
using namespace MyUtil;

class TicketAdapter : public MyUtil::ReplicatedClusterAdapterI<TicketManagerPrx>,
	public MyUtil::Singleton<TicketAdapter> {
public:
	TicketAdapter() : MyUtil::ReplicatedClusterAdapterI<TicketManagerPrx> ("ControllerTicket",120,300) {
	}
	string createTicket(int userId,const string& infos);
	int verifyTicket(const string& ticket,const string& infos);
	string queryTicket(int userId);
	bool destroyTicket(const string&);
protected:
	virtual void initialize();

};
};

#endif /*TICKETADAPTER_H_*/
