/*
 * =====================================================================================
 *
 *       Filename:  TicketI.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2009年09月03日 16时53分13秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tanbokan (), bokan.tan@opi-corp.com
 *        Company:  opi-corp.com
 *
 * =====================================================================================
 */
#include "TicketI.h"
#include "ServiceI.h"
#include "util/cpp/String.h"

void MyUtil::initialize() {
	MyUtil::ServiceI& service = ServiceI::instance();
	int six_hours = (6* 60* 60 ) ;
	int
			duration =
					service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(
							"Service." + service.getName() + ".Duration",
							six_hours);
	string suffix =
			service.getCommunicator()->getProperties()->getPropertyWithDefault(
					"Service." + service.getName() + ".Suffix", "");
	MCE_INFO("Ticket Duration -> " << duration);
	MCE_INFO("Ticket Suffix -> " << suffix);
	passport::TicketManagerIPtr manager = new passport::TicketManagerI(
			duration, suffix);
	service.getAdapter()->add(manager, service.createIdentity(
			passport::MANAGER, ""));
}

namespace passport {

TicketManagerI::TicketManagerI(Ice::Int duration, string suffix) :
	_duration(duration), _suffix(suffix) {
	_userValueEvictors.reserve(SEGMENT_SIZE);
	_ticketValueEvictors.reserve(SEGMENT_SIZE);
	for (size_t i = 0; i < SEGMENT_SIZE; ++i) {
		_userValueEvictors.push_back(new UserValueEvictor(_duration));
		_ticketValueEvictors.push_back(new TicketValueEvictor(_duration));
	}
}

string TicketManagerI::createTicket(Ice::Int userId, const string& infos,
		const Ice::Current& current) {
	TicketValueEvictorPtr& tvp = findTicketValueEvictor(userId);
	string ticket = tvp->locate(userId, "");
	if (!ticket.empty()) {
		findUserValueEvictor(ticket)->remove(ticket);
	}
	ostringstream txt;
	txt << infos << time(NULL);
	ticket = MyUtil::StrUtil::md5(txt.str()) + _suffix;
	MCE_DEBUG("Create Ticket for"<<userId<<" ticket is:"<<ticket<<" info is :"<<txt.str());

	findUserValueEvictor(ticket)->add(userId, ticket);
	tvp->add(ticket, userId);

	return ticket;
}

bool TicketManagerI::destroyTicket(const string& ticket,
		const Ice::Current& current) {
	UserValueEvictorPtr& uvp = findUserValueEvictor(ticket);
	Ice::Int userId = uvp->locate(ticket, -1);
	if (userId <= 0) {
		MCE_DEBUG("Ticke not exist: "<<ticket);
		return false;
	}

	TicketValueEvictorPtr& tvp = findTicketValueEvictor(userId);
	string ticket_in_mem = tvp->locate(userId, "");
	if (ticket_in_mem.empty() || ticket != ticket_in_mem) {
		MCE_DEBUG("Ticke has conflict for uid:"<<userId<<" ticket:"<<ticket);
		return false;
	}
	tvp->remove(userId);
	uvp->remove(ticket);

	return true;
}

Ice::Int TicketManagerI::verifyTicket(const string& ticket,
		const string& infos, const Ice::Current& current) {
	return findUserValueEvictor(ticket)->locate(ticket, -1);
}

string TicketManagerI::queryTicket(Ice::Int userId, const Ice::Current& current) {
	return findTicketValueEvictor(userId)->locate(userId, "");
}

UserValueEvictorPtr& TicketManagerI::findUserValueEvictor(const string& ticket) {
	if (ticket.empty()) {
		return _userValueEvictors.at(0);
	} else {
		return _userValueEvictors.at(abs(abs(MyUtil::StrUtil::gen_md5_seed(
				ticket, _userValueEvictors.size()))));
	}

}
TicketValueEvictorPtr& TicketManagerI::findTicketValueEvictor(Ice::Int userId) {
	return _ticketValueEvictors.at(abs(userId) % _ticketValueEvictors.size());
}

}
AddTask::AddTask(int _table,vector<TicketInfo>& _queue) 
		: Task(TASK_LEVEL_ADD),table(_table){
			queue.swap(_queue);
}
void AddTask::handle() {

}
