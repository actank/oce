/*
 * TransferI.h
 *
 *  Created on: Sep 19, 2009
 *      Author: antonio
 */

#ifndef TRANSFERI_H_
#define TRANSFERI_H_

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "Singleton.h"
#include "Transfer.h"
#include "TaskManager.h"


namespace passport {

using boost::multi_index_container;
using namespace boost::multi_index;
using namespace std;

const int POOL_SIZE = 10;

struct Ticket{
	int uid;
	time_t time;
	string ticket;
};

//struct UID {};
////struct TICKET {};
//struct TIME{};

typedef multi_index_container<
    Ticket,
    indexed_by<
		ordered_unique< member<Ticket,int,&Ticket::uid> >,
		ordered_non_unique<member<Ticket,time_t,&Ticket::time> >,
		hashed_unique< member<Ticket,std::string,&Ticket::ticket> >
    >
> TicketSet;



class TransferManagerI: public TransferManager, public MyUtil::Singleton<TransferManagerI> {
public:
	virtual string createTicket(int userId, int time, const Ice::Current& = Ice::Current());
	virtual int verifyTicket(const string& ticket, const Ice::Current& = Ice::Current());


	void removeTimeOutTicket();
private:
	string generateTicket(Ticket& t);
	IceUtil::Mutex _mutex[POOL_SIZE];
	TicketSet _ticketSetPool[POOL_SIZE];
};

class RemoveTimer : public MyUtil::Timer{
public:
	RemoveTimer():Timer(1000){
	};
	virtual void handle();
};

};
#endif /* TRANSFERI_H_ */
