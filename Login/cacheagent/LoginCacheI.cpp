/*
 * =====================================================================================
 *
 *       Filename:  LoginI.cpp
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
#include "LoginCacheI.h"
#include "ServiceI.h"
#include "IceLogger.h"
#include "util/cpp/String.h"
#include "QueryRunner.h"
#include "util/cpp/String.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "LoginCacheAdapter.h"

using std::string;
using std::vector;
using namespace com::xiaonei::xce;

void MyUtil::initialize() {
	MyUtil::ServiceI& service = ServiceI::instance();
	int six_hours = (6 * 60 * 60);
	int duration = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Service." + service.getName() + ".Duration", six_hours);
	string suffix = service.getCommunicator()->getProperties()->getPropertyWithDefault("Service." + service.getName() + ".Suffix", "");
	MCE_INFO("Login Name -> "<< service.getName());
	MCE_INFO("Login Duration -> " << duration);
	MCE_INFO("Login Suffix -> " << suffix);
	string type;
	string controller;
	if(service.getName().find("Wap")!=string::npos) {
		type = "WAP";
		controller = "ControllerWapLoginCache";
	} else {
		type = "WEB";
		controller = "ControllerLoginCache";
	}

	MCE_INFO("Login CacheType -> "<< type);
	passport::LoginCacheIPtr manager = new passport::LoginCacheI(duration, suffix, type);
	manager->initialize();
	service.getAdapter()->add(manager, service.createIdentity(passport::MANAGER, ""));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("LoginCache.Mod");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("LoginCache.Interval", 5);
	//xce::serverstate::ServerStateSubscriber::instance().initialize(controller, manager, mod, interval, "2000", "2000", "1048576");
}

namespace passport {

void LoginMap::set(int userId, const string& ticket,int second) {
	int endTime;
	if(second < 0)
		endTime = -1;
	else
		endTime = time(NULL) + second;
	IceUtil::Mutex::Lock lock(mutex_);
	id2ticket_[userId] =  make_pair(ticket,endTime);
	ticket2id_[ticket] =  make_pair(userId,endTime);
}

string LoginMap::get(int userId) {
	IceUtil::Mutex::Lock lock(mutex_);
	hash_map<int, pair<string,int> >::iterator it = id2ticket_.find(userId);
	if (it != id2ticket_.end() && (it->second.second< 0 || it->second.second > time(NULL))) {
		return it->second.first;
	} else {
		return "";
	}
}

int LoginMap::get(const std::string& ticket) {
	IceUtil::Mutex::Lock lock(mutex_);
	hash_map<string, pair<int,int> >::iterator it = ticket2id_.find(ticket);
	if ((it != ticket2id_.end()) && (it->second.second< 0 || it->second.second > time(NULL))) {
		return it->second.first;
	} else {
		return -1;
	}
}

bool LoginMap::del(int userId) {
	IceUtil::Mutex::Lock lock(mutex_);
	hash_map<int, pair<string,int> >::iterator it = id2ticket_.find(userId);
	if (it == id2ticket_.end()) {
		return false;
	}
	hash_map<string, pair<int,int> >::iterator it2 = ticket2id_.find(it->second.first);
	if (it2 == ticket2id_.end()) {
		// TODO record this is impossible;
	} else {
		ticket2id_.erase(it2);
	}
	id2ticket_.erase(it);
	return true;
}

bool LoginMap::del(const string& ticket) {
	IceUtil::Mutex::Lock lock(mutex_);
	hash_map<string, pair<int,int> >::iterator it = ticket2id_.find(ticket);
	if (it == ticket2id_.end()) {
		return false;
	}
	hash_map<int, pair<string,int> >::iterator it2 = id2ticket_.find(it->second.first);
	if (it2 == id2ticket_.end()) {
		// TODO record this is impossible;
	} else {
		id2ticket_.erase(it2);
	}
	ticket2id_.erase(it);
	return true;
}

void LoginHolder::initialize() {
	for (int init = 0; init < BUCKET_SIZE; init++) {
		all_tickets_.push_back(new LoginMap);
	}
}
void LoginHolder::set(int userId, const string& ticket,int second) {
	findLoginMap(userId)->set(userId, ticket,second);
}

string LoginHolder::get(int userId) {
	return findLoginMap(userId)->get(userId);
}

int LoginHolder::get(const string& ticket) {
	return findLoginMap(ticket)->get(ticket);
}

bool LoginHolder::del(int userId) {
	return findLoginMap(userId)->del(userId);
}

bool LoginHolder::del(const string& ticket) {
	return findLoginMap(ticket)->del(ticket);
}

LoginMapPtr& LoginHolder::findLoginMap(const string& ticket) {
	if (ticket.size() != 33) {
		return all_tickets_.at(0);
	}
	int seed = (ticket.at(17) - '0') + (ticket.at(23) - '0') * 10 + (ticket.at(29) - '0') * 100;
	return all_tickets_.at(seed % BUCKET_SIZE);
}
LoginMapPtr& LoginHolder::findLoginMap(int userId) {
	int seed = (userId / 10) % 1000;
	return all_tickets_.at(seed % BUCKET_SIZE);
}

/*******************************************************************************************************/

LoginCacheI::LoginCacheI(Ice::Int duration, string suffix, string type) :
	duration_(duration), suffix_(suffix) {
	if(type == "WAP"){
		DB_DESC_PASSPORT = "passport_wap";
		TABLENAME = "passport_ticket_wap";
	}else{
		DB_DESC_PASSPORT = "passport";
		TABLENAME = "passport_ticket";
	}
}

void LoginCacheI::initialize() {
	//LoginHolder::instance().initialize();
}

void LoginCacheI::set(Ice::Int userId, const string& ticket, int second ,const Ice::Current& current) {
	/*if(second < 0)
		savePersistent(ticket, userId);
	if (!(LoginHolder::instance().get(userId)).empty()) {
		LoginHolder::instance().del(userId);
	}
	LoginHolder::instance().set(userId, ticket, second);*/
	try
	{
		WapLoginCacheAdapter::instance().set( userId, ticket, second );
	}catch( ...)
	{
		MCE_WARN("LoginCacheI::setTicket Exception!");
	}
	MCE_INFO("LoginCacheI::setTicket " << userId << " " << ticket <<endl );
}

bool LoginCacheI::destroyByTicket(const string& ticket, const Ice::Current& current) {
	MCE_INFO("LoginCacheI::destroyTicket " << ticket);
	/*removePersistent(ticket);
	return LoginHolder::instance().del(ticket);*/
	try
	{
		if( ticket.length() == 33 )
		{
			return WapLoginCacheAdapter::instance().getProxy( ticket.at(32) - '0')->destroyByTicket( ticket );
		}
		return false;
	}catch( ...)
	{
		MCE_WARN("LoginCacheI::destroyByTicket Exception!");
	}
}

bool LoginCacheI::destroyById(int userId, const Ice::Current& current) {
        MCE_INFO("LoginCacheI::destroyById" << userId);
	/*removePersistent(userId);
	return LoginHolder::instance().del(userId);*/
	try
	{
		return WapLoginCacheAdapter::instance().getProxy( userId )->destroyById( userId );
	}catch( ...)
	{
		MCE_WARN("LoginCacheI::destroyById Exception!");
	}
}

Ice::Int LoginCacheI::ticket2Id(const string& ticket, const Ice::Current& current) {
	/*int id = LoginHolder::instance().get(ticket);
	if (id > 0){
		MCE_INFO("LoginCacheI::verifyTicket " << ticket <<" : " << id);
		return id;
	} else {
		return loadPersistent(ticket);
	}*/
        MCE_INFO("LoginCacheI::ticket2Id" << ticket);
	try
	{
		if( ticket.length() == 33 )
		{
			return WapLoginCacheAdapter::instance().getProxy( ticket.at(32) - '0')->ticket2Id( ticket );
		}
		return -1;
	}catch( ...)
	{
		MCE_WARN("LoginCacheI::ticket2Id Exception!");
	}
}

string LoginCacheI::id2Ticket(Ice::Int userId, const Ice::Current& current) {
	/*string ticket = LoginHolder::instance().get(userId);
	if( !ticket.empty()){
		MCE_INFO("LoginCacheI::queryLogin " << userId << " : " << ticket);
		return ticket;
	} else {
		return loadPersistent(userId);
	}*/
        MCE_INFO("LoginCacheI::id2Ticket" << userId);
	try
	{
		return WapLoginCacheAdapter::instance().id2Ticket( userId );
	}catch( ...)
	{
		MCE_WARN("LoginCacheI::id2Ticket Exception!");
	}
}

bool LoginCacheI::getStatus(int userId, const Ice::Current& current) {
	/*{
		IceUtil::Mutex::Lock lock(_mutex);
		if (userStatus.find(userId) != userStatus.end()) {
			return true;
		}
	}
	if (loadCross(userId)) {
		IceUtil::Mutex::Lock lock(_mutex);
		userStatus.insert(userId);
		return true;
	}
	return false;*/
        MCE_INFO("LoginCacheI::getStatus" << userId);
	try
	{
		return WapLoginCacheAdapter::instance().getProxy( userId )->getStatus( userId );
	}catch( ...)
	{
		MCE_WARN("LoginCacheI::getStatus Exception!");
	}
}
void LoginCacheI::setStatus(int userId, const Ice::Current& current) {
	/*{
		IceUtil::Mutex::Lock lock(_mutex);
		userStatus.insert(userId);
	}
	saveCross(userId);*/
        MCE_INFO("LoginCacheI::setStatus" << userId);
	try
	{
		return WapLoginCacheAdapter::instance().getProxy( userId )->setStatus( userId );
	}catch( ...)
	{
		MCE_WARN("LoginCacheI::setStatus Exception!");
	}
}


/*******************************************************************************************************/
void LoginCacheI::removePersistent(const string& ticket) {
	Statement sql;
	sql << "DELETE FROM " <<TABLENAME<<" WHERE ticket='" << ticket << "'";
	QueryRunner(DB_DESC_PASSPORT, CDbWServer).schedule(sql);
}
void LoginCacheI::removePersistent(int userId) {
	Statement sql;
	sql << "DELETE FROM "<<TABLENAME<<" WHERE userid =" << userId ;
	QueryRunner(DB_DESC_PASSPORT, CDbWServer).schedule(sql);
}
void LoginCacheI::savePersistent(const string& ticket, int userId) {
	Statement sql;
	sql << "REPLACE INTO "<<TABLENAME<<" (userid, ticket) VALUES (" << userId
			<< ",'" << ticket << "')";
	QueryRunner(DB_DESC_PASSPORT, CDbWServer).schedule(sql);
	return;
}

Ice::Int LoginCacheI::loadPersistent(const string& ticket) {
	MCE_DEBUG("Loading persistent Login " << ticket);
	Statement sql;
	sql << "SELECT userid FROM "<<TABLENAME<<" WHERE ticket = '" << ticket
			<< "' AND time > DATE_SUB(now(), INTERVAL 30 day) LIMIT 1";
	int userId = -1;
	size_t qsize = QueryRunner(DB_DESC_PASSPORT, CDbRServer) .query(sql,
			LoadPersistentByTicketHandler(userId));
	if (qsize != 1 || userId < 0) {
		return -1;
	}
	LoginHolder::instance().set(userId,ticket,-1);
	return userId;
}

string LoginCacheI::loadPersistent(int userId) {
	MCE_DEBUG("Loading persistent UserId: " << userId);
	Statement sql;
	sql << "SELECT ticket FROM "<<TABLENAME<<" WHERE userid = " << userId
			<< " AND time > DATE_SUB(now(), INTERVAL 30 day) LIMIT 1";
	string ticket;
	size_t qsize = QueryRunner(DB_DESC_PASSPORT, CDbRServer) .query(sql,
			LoadPersistentByIdHandler(ticket));
	if (qsize != 1 || ticket.size()!=33) {
		return "";
	}
	LoginHolder::instance().set(userId,ticket,-1);
	return ticket;
}

void LoginCacheI::saveCross(int userId) {
	Statement sql;
	sql << "INSERT IGNORE INTO passport_cross_domain (userid) VALUES ('"
			<< userId << "')";
	QueryRunner("passport", CDbWServer).schedule(sql);
}

bool LoginCacheI::loadCross(int userId) {
	MCE_DEBUG("Loading cross domain " << userId);
	Statement sql;
	sql << "SELECT userid FROM passport_cross_domain WHERE userid = '"
			<< userId << "'";
	size_t qsize = QueryRunner("passport", CDbRServer) .query(sql,
			LoadCrossHandler());
	return qsize == 1;
}


}
