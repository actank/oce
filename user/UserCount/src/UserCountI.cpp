#include <algorithm>
#include "UserCountI.h"
#include "BatchUpdaterUserCountReplicaAdapter.h"
#include "BatchUpdaterNotifyCountAdapter.h"
#include "ConnectionQuery.h"
#include "ConnectionHolder.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include <util/cpp/InvokeMonitor.h>
#include <Monitor/client/service/ServiceMonitor.h>

using namespace MyUtil;
using namespace xce::usercount;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::xce;
using namespace mop::hi::svc::batchtraits;
using namespace std;
using namespace ::xce::serverstate;

void MyUtil::initialize()  {

	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(UC, "UC",new UserCountFactory, false);
	service.getAdapter()->add(&UserCountManagerI::instance(), service.createIdentity("M", ""));
	TaskManager::instance().config(TASK_LEVEL_CREATE, MyUtil::ThreadPoolConfig(1, 1));
        int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserCount.Mod",0);
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserCount.Interval",5);
        ServerStateSubscriber::instance().initialize("ControllerUserCount",&UserCountManagerI::instance(),mod,interval);
	{ 
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserCount.Status",0);
		ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status;
		MCE_WARN( os.str() );
	}
  ServiceMonitorManager::instance().start();
}



// ================= UserCounterI ===================================
bool UserCounterI::find(int type, int& pos)	{

	vector<int>::iterator itPos = lower_bound(types.begin(), types.end(), type);

	if ( itPos == types.end() )	{

		pos = -1;
		return false;
	}

	if ( (*itPos) == type )	{
		pos = distance(types.begin(), itPos);
		return true;
	}
	else  {
		pos = distance(types.begin(), itPos);
		return false;
	}
}

bool UserCounterI::insert(int type, int step, int pos)	{

	// 先检查types和values是否正确对应
	size_t nKeysSize = types.size(), nValuesSize = values.size();
	if ( nKeysSize != nValuesSize )	{
		/* MCE_WARN("Error: size of Keys and Values are not equal!\n");
		return false;*/

		// 如果不完全对应，则进行修复(按短取齐)
		MCE_WARN("Error: size of types and values are not equel!\nTrying to fix it\n");
		if ( nKeysSize > nValuesSize ) {
			types.resize(nValuesSize);
		}
		else	{
			values.resize(nValuesSize);
		}
		MCE_WARN("Fix complete\n");
	}

	// 插入信息
	if ( ( pos < 0 ) || ( pos >= (int)types.size() ) )	{
		types.push_back(type);
		values.push_back(step);
		return true;
	}
	else	{
		vector<int>::iterator itTypesIn = types.begin() + pos;
		vector<int>::iterator itValuesIn = values.begin() + pos;

		types.insert(itTypesIn, type);
		values.insert(itValuesIn, step);

		return true;
	}
}

void UserCounterI::inc(int type, int step)	{

	IceUtil::Mutex::Lock lock(*this);
	int pos = -1;
	if ( find(type, pos) )	{
		values.at(pos) += step;
	}
	else	{
		insert(type, step, pos);
	}
}

void UserCounterI::dec(int type, int step)	{

	IceUtil::Mutex::Lock lock(*this);
	int pos = -1;
	if ( find(type, pos) )	{
		values.at(pos) -= step;
		if ( values.at(pos) <= 0 )	{

			values.at(pos) = 0;
		}
	}
	else	{
		MCE_DEBUG("No any data of this type [" << type << "]\n");
	}
}

void UserCounterI::set(int type, int value)	{

	IceUtil::Mutex::Lock lock(*this);
	int pos = -1;
	if ( find(type, pos) )	{
		values.at(pos) = value;
	}
	else	{
		MCE_DEBUG("No any data of this type [" << type << "]. Now create it\n")
		insert(type, value, pos);
	}
}

int UserCounterI::get(int type)	{

	IceUtil::Mutex::Lock lock(*this);
	int pos = -1;
	if ( find(type, pos) )	{
		return values.at(pos);
	}
	else	{

		// 如果没有该类信息，就返回0(因为是count信息)
		return 0;
	}
}


void UserCounterI::sort() {
	map<int, int> m;
	for (unsigned int i = 0; i < types.size(); ++i) {
		m.insert(make_pair(types[i], values[i]));
	}
	MyUtil::IntSeq t;
	MyUtil::IntSeq v;
	for (map<int, int>::iterator i = m.begin(); i != m.end(); ++i) {
		t.push_back(i->first);
		v.push_back(i->second);
	}
	types = t;
	values = v;
}



// ================= CountManagerI ====================================

void UserCountManagerI::inc(Ice::Int id, Ice::Int type, Ice::Int step,
		const Ice::Current& current)	{
	ostringstream inMsg;
	inMsg << "id=" << id << " step=" << step << " type=" << type;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	UserCounterIPtr counter = findCount(id);
	if ( counter )	{
		counter->inc(type, step);
	}
	if (type == 12) {
		BatchUpdaterNotifyCountAdapter::instance().inc(id, step);
	} else {
		BatchUpdaterUserCountReplicaAdapter::instance().inc(id, type, step);
	}
}

void UserCountManagerI::dec(Ice::Int id, Ice::Int type, Ice::Int step,
		const Ice::Current& current)	{
	ostringstream inMsg;
	inMsg << "id=" << id << " step=" << step << " type=" << type;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	UserCounterIPtr counter= findCount(id);
	if ( counter )	{
		counter->dec(type, step);
	}
	if (type == 12) {
		BatchUpdaterNotifyCountAdapter::instance().dec(id, step);
	} else {
		BatchUpdaterUserCountReplicaAdapter::instance().dec(id, type, step);
	}
}

void UserCountManagerI::set(Ice::Int id, Ice::Int type, Ice::Int value,
		const Ice::Current& current)	{
	ostringstream inMsg;
	inMsg << "id=" << id << " value=" << value << " type=" << type;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	UserCounterIPtr counter = findCount(id);
	if ( counter )	{
		counter->set(type, value);
	}
	if (type == 12) {
		BatchUpdaterNotifyCountAdapter::instance().set(id, value);
	} else {
		BatchUpdaterUserCountReplicaAdapter::instance().set(id, type, value);
	}
}


int UserCountManagerI::get(Ice::Int id, Ice::Int type, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << id << " type=" << type;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	return locateCount(id)->get(type);
}

UserCounterPtr UserCountManagerI::getAll(Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id=" << id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	return locateCount(id);
}

void UserCountManagerI::reload(Ice::Int id, const Ice::Current& current)	{
	ostringstream inMsg;
	inMsg<<"id=" << id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	ServiceI::instance().removeObject(UC, id);
}

UserCounterIPtr UserCountManagerI::locateCount(Ice::Int id)	{
	return ServiceI::instance().locateObject<UserCounterIPtr>(UC, id);
}

UserCounterIPtr UserCountManagerI::findCount(Ice::Int id)	{
	return ServiceI::instance().findObject<UserCounterIPtr>(UC, id);
}

// ============== UserCountTask ================================

CreateTask::CreateTask(Ice::Int id,int level):Task(level), _id(id){
}

void CreateTask::handle(){
	MCE_DEBUG("CreateTask::handle -> " << _id);
	ostringstream pattern;
	
	pattern << "user_count_" << (_id % 100);
	Statement newsql;
	newsql << "SELECT type, count FROM " << pattern.str() << " WHERE id = " << _id;

	UserCounterIPtr result = new UserCounterI;
	QueryRunner("user_count", CDbRServer, pattern.str())
		.query(newsql, UserCountResultHandler(result));

	ostringstream notifypattren;
	notifypattren << "notify_count_" << (_id % 100);
	Statement notifysql;
	notifysql << "SELECT count FROM " << notifypattren.str() << " WHERE id = " << _id;
	QueryRunner("notify_count", CDbRServer, notifypattren.str()).query(notifysql, NotifyCountResultHandler(result));
	result->sort();
	ServiceI::instance().getObjectCache()->addObject(UC,_id, result);
}

// ============== UserCountFactory ================================

Ice::ObjectPtr UserCountFactory::create(Ice::Int id)	{
	UserCounterIPtr result = new UserCounterI;
	TaskManager::instance().execute(new CreateTask(id));
        return result;
}

// ============= UserCountResultHandler ==========================

UserCountResultHandler::UserCountResultHandler(UserCounterIPtr& counter) :
	_counter(counter)	{
}

bool UserCountResultHandler::handle(mysqlpp::Row& row) const	{
	int type = (int) row["type"];
	if (type == 12) {
		return true;
	}
	int count = (int) row["count"];
	_counter->types.push_back(type);
	_counter->values.push_back(count);
	return true;
}


NotifyCountResultHandler::NotifyCountResultHandler(UserCounterIPtr& counter) :
	_counter(counter)	{
}

bool NotifyCountResultHandler::handle(mysqlpp::Row& row) const	{
	int count = (int) row["count"];
	_counter->types.push_back(12);
	_counter->values.push_back(count);
	return true;
}
