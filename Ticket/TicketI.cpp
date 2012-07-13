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
#include "Monitor/client/service/ServiceMonitor.h"
#include "util/cpp/InvokeMonitor.h"
#include "ServiceI.h"
#include "util/cpp/String.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
using std::string;
using std::vector;
using MyUtil::InvokeClient;
using __gnu_cxx::hash_map;

void MyUtil::initialize() {
	MyUtil::ServiceI& service = ServiceI::instance();
	int six_hours = (6 * 60 * 60);
	int duration = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Service." + service.getName() + ".Duration", six_hours);
	string suffix = service.getCommunicator()->getProperties()->getPropertyWithDefault("Service." + service.getName() + ".Suffix", "");
	MCE_INFO("Ticket Duration -> " << duration);
	MCE_INFO("Ticket Suffix -> " << suffix);
	passport::TicketManagerIPtr manager = new passport::TicketManagerI(duration, suffix);
	manager->initialize();
	service.getAdapter()->add(manager, service.createIdentity(passport::MANAGER, ""));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Ticket.Mod");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Ticket.Interval", 5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerTicket", manager, mod, interval,new MyUtil::OceChannel, "30", "1000", "1048576");
  MyUtil::ServiceMonitorManager::instance().start();
}

namespace passport {

void TicketMap::set(int userId, const string& ticket) {
	IceUtil::Mutex::Lock lock(mutex_);
	id2ticket_[userId] =  ticket;
	ticket2id_[ticket] =  userId;
}

string TicketMap::get(int userId) {
	IceUtil::Mutex::Lock lock(mutex_);
	hash_map<int, string>::iterator it = id2ticket_.find(userId);
	if ((it != id2ticket_.end())) {
		return it->second;
	} else {
		return "";
	}
}

int TicketMap::get(const std::string& ticket) {
	IceUtil::Mutex::Lock lock(mutex_);
	hash_map<string, int>::iterator it = ticket2id_.find(ticket);
	if ((it != ticket2id_.end())) {
		return it->second;
	} else {
		return -1;
	}
}

bool TicketMap::del(int userId) {
	IceUtil::Mutex::Lock lock(mutex_);
	hash_map<int, string>::iterator it = id2ticket_.find(userId);
	if (it == id2ticket_.end()) {
		return false;
	}
	hash_map<string, int>::iterator it2 = ticket2id_.find(it->second);
	if (it2 == ticket2id_.end()) {
		// TODO record this is impossible;
	} else {
		ticket2id_.erase(it2);
	}
	id2ticket_.erase(it);
	return true;
}

bool TicketMap::del(const string& ticket) {
	IceUtil::Mutex::Lock lock(mutex_);
	hash_map<string, int>::iterator it = ticket2id_.find(ticket);
	if (it == ticket2id_.end()) {
		return false;
	}
	hash_map<int, string>::iterator it2 = id2ticket_.find(it->second);
	if (it2 == id2ticket_.end()) {
		// TODO record this is impossible;
	} else {
		id2ticket_.erase(it2);
	}
	ticket2id_.erase(it);
	return true;
}

void TicketHolder::initialize() {
	for (int init = 0; init < BUCKET_SIZE; init++) {
		all_tickets_.push_back(new TicketMap);
	}
}
void TicketHolder::set(int userId, const string& ticket) {
	findTicketMap(userId)->set(userId, ticket);
}

string TicketHolder::get(int userId) {
	return findTicketMap(userId)->get(userId);
}

int TicketHolder::get(const string& ticket) {
	return findTicketMap(ticket)->get(ticket);
}

bool TicketHolder::del(int userId) {
	return findTicketMap(userId)->del(userId);
}

bool TicketHolder::del(const string& ticket) {
	return findTicketMap(ticket)->del(ticket);
}

TicketMapPtr& TicketHolder::findTicketMap(const string& ticket) {
	if (ticket.size() != 33) {
		return all_tickets_.at(0);
	}
	int seed = (ticket.at(7) - '0') + (ticket.at(11) - '0') * 10 + (ticket.at(19) - '0') * 100;
	return all_tickets_.at(seed % BUCKET_SIZE);
}
TicketMapPtr& TicketHolder::findTicketMap(int userId) {
	int seed = (userId / 10) % 1000;
	return all_tickets_.at(seed % BUCKET_SIZE);
}

/*******************************************************************************************************/

TicketManagerI::TicketManagerI(Ice::Int duration, string suffix) :
	duration_(duration), suffix_(suffix) {
}

void TicketManagerI::initialize() {
	TicketHolder::instance().initialize();
	TicketCleaner::instance().initialize(duration_);
}

string TicketManagerI::createTicket(Ice::Int userId, const string& infos, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "");
	string ticket = TicketHolder::instance().get(userId);
	if (ticket != "") {
		return ticket;
	}
	ticket = generateTicket(userId, infos);
	TicketHolder::instance().set(userId, ticket);
	TicketCleaner::instance().recycle(userId);
	MCE_INFO("TicketManagerI::createTicket " << userId << " " << infos << " : " << ticket);
	return ticket;
}

bool TicketManagerI::destroyTicket(const string& ticket, const Ice::Current& current) {
  ostringstream oss;
  oss << "ticket: " << ticket;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);
	return TicketHolder::instance().del(ticket);;
}

Ice::Int TicketManagerI::verifyTicket(const string& ticket, const string& infos, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "");
	int id = TicketHolder::instance().get(ticket);
	if (id > 0)
		TicketCleaner::instance().recycle(id);
	MCE_DEBUG("TicketManagerI::verifyTicket " << ticket << " " << infos << " : " << id);
	return id;
}

string TicketManagerI::queryTicket(Ice::Int userId, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "");
	string ticket = TicketHolder::instance().get(userId);
	if( !ticket.empty())
		TicketCleaner::instance().recycle(userId);
	MCE_INFO("TicketManagerI::queryTicket " << userId << " : " << ticket);
	return ticket;
}

string TicketManagerI::generateTicket(Ice::Int id, const string& infos) {
	std::ostringstream txt;
	txt << id << infos << time(NULL);
	string raw_ticket = MyUtil::StrUtil::md5(txt.str());
	int raw_id_pos1 = (id / 10) % 10;
	int raw_id_pos2 = (id / 100) % 10;
	int raw_id_pos3 = (id / 1000) % 10;
	std::ostringstream ticket;
	for (int i = 0; i < 32; ++i) {
		if (i == 7) {
			ticket << raw_id_pos1;
			continue;
		}
		if (i == 11) {
			ticket << raw_id_pos2;
			continue;
		}
		if (i == 19) {
			ticket << raw_id_pos3;
			continue;
		}
		ticket << raw_ticket.at(i);
	}
	ticket << suffix_;
	return ticket.str();
}

/*******************************************************************************************************/

void TicketRecycle::add(int userId) {
	int endTime = time(NULL) + duration_;
	IceUtil::Mutex::Lock lock(mutex_);
	id2Time[userId] = endTime;
}

vector<int> TicketRecycle::get() {
	vector<int> result;
	IceUtil::Mutex::Lock lock(mutex_);
	for(hash_map<int, int>::iterator it = id2Time.begin(); it != id2Time.end();){
		if(it->second < time(NULL)) {
			result.push_back(it->first);
			id2Time.erase(it++);
		}else{
			++it;
		}
	}
	return result;
}

void TicketCleaner::initialize(int duration) {
	duration_ = duration;
	for (int init = 0; init < BUCKET_SIZE; ++init) {
		recycles_.push_back(new TicketRecycle(duration));
	}
	input_ = new InputThread(*this);
	input_->start();
	output_ = new OutputThread(*this);
	output_->start();
}

void TicketCleaner::recycle(int userId) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(unscheduled_mutex_);
	unscheduled_.push_back(userId);
	if (unscheduled_.size() >= 1000) unscheduled_mutex_.notify();
}
void TicketCleaner::swap(vector<int>& empty_vector) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(unscheduled_mutex_);
	if (unscheduled_.size() < 1000) unscheduled_mutex_.timedWait(IceUtil::Time::seconds(1));
	unscheduled_.swap(empty_vector);
}
void TicketCleaner::add(int userId) {
	recycles_.at(userId % BUCKET_SIZE)->add(userId);
}

void TicketCleaner::InputThread::run() {
	while (true) {
		cleaner_.swap(to_be_input_);
		MCE_INFO("TicketCleaner::InputThread::run() " << to_be_input_.size() << " to be clean");
		for (std::vector<int>::iterator it = to_be_input_.begin(); it != to_be_input_.end(); ++it) {
			cleaner_.add(*it);
		}
		to_be_input_.clear();
	}
}

void TicketCleaner::OutputThread::run() {
	while (true) {
		int benchmark_count = 0;
		for (std::vector<TicketRecyclePtr>::iterator it = cleaner_.recycles_.begin(); it != cleaner_.recycles_.end(); ++it) {
			std::vector<int> to_be_remove = (*it)->get();
			for (std::vector<int>::iterator i = to_be_remove.begin(); i != to_be_remove.end(); ++i) {
				TicketHolder::instance().del(*i);
				benchmark_count++;
			}
		}
		MCE_INFO("TicketCleaner::OutputThread::run() " << benchmark_count << " cleaned.");
		if(benchmark_count<100) {
			sleep(60);
		} else {
			sleep(10);
		}
	}
}

}
