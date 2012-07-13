#include <algorithm>
#include "UserApiCountI.h"
#include "BatchUpdaterUserApiCountReplicaAdapter.h"
#include "ConnectionQuery.h"
#include "ConnectionHolder.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "util/cpp/TimeCost.h"
#include <util/cpp/InvokeMonitor.h>
#include <Monitor/client/service/ServiceMonitor.h>


using namespace MyUtil;
using namespace xce::userapicount;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::xce;
using namespace mop::hi::svc::batchtraits;
using namespace std;
using namespace ::xce::serverstate;

void MyUtil::initialize()  {

	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(UAC, "UAC",new UserApiCountFactory);
	service.getAdapter()->add(&UserApiCountManagerI::instance(), service.createIdentity("M", ""));
	TaskManager::instance().config(TASK_LEVEL_CREATE, MyUtil::ThreadPoolConfig(1, 1));
	
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserApiCount.Mod",0);
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserApiCount.Interval",5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerUserApiCount",&UserApiCountManagerI::instance(),mod,interval);
	{ 
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserApiCount.Status",0);
		xce::serverstate::ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status<<" mod : "<<mod<<" interval : "<<interval;
		MCE_WARN( os.str() );
	}
  ServiceMonitorManager::instance().start();
}



// ================= UserApiCounterI ===================================
bool UserApiCounterI::find(int type, int& pos)	{

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

bool UserApiCounterI::insert(int type, int step, int pos)	{

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

void UserApiCounterI::inc(int type, int step)	{

	IceUtil::Mutex::Lock lock(*this);
	int pos = -1;
	if ( find(type, pos) )	{
		values.at(pos) += step;
	}
	else	{
		insert(type, step, pos);
	}
}

void UserApiCounterI::dec(int type, int step)	{

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

void UserApiCounterI::set(int type, int value)	{

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

int UserApiCounterI::get(int type)	{

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


// ================= UserApiCountManagerI ====================================

void UserApiCountManagerI::inc(Ice::Int id, Ice::Int type, Ice::Int step,
		const Ice::Current& current)	{
	
	ostringstream inMsg;
	inMsg<<"id =" << id << " step =" << step << "type = " << type;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	UserApiCounterIPtr counter = findCount(id);
	if ( counter )	{
		counter->inc(type, step);
	}
	BatchUpdaterUserApiCountReplicaAdapter::instance().inc(id, type, step);
}

void UserApiCountManagerI::dec(Ice::Int id, Ice::Int type, Ice::Int step,
		const Ice::Current& current)	{
	
	ostringstream inMsg;
	inMsg<<"id =" << id << " step =" << step << "type = " << type;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	UserApiCounterIPtr counter= findCount(id);
	if ( counter )	{
		counter->dec(type, step);
	}
	BatchUpdaterUserApiCountReplicaAdapter::instance().dec(id, type, step);
}

void UserApiCountManagerI::set(Ice::Int id, Ice::Int type, Ice::Int value,
		const Ice::Current& current)	{
	
	ostringstream inMsg;
	inMsg<<"id =" << id << " value =" << value << "type = " << type;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	UserApiCounterIPtr counter = findCount(id);
	if ( counter )	{
		counter->set(type, value);
	}
	BatchUpdaterUserApiCountReplicaAdapter::instance().set(id, type, value);
}


int UserApiCountManagerI::get(Ice::Int id, Ice::Int type, const Ice::Current& current)	{
	
	ostringstream inMsg;
	inMsg<<"id =" << id << "type = " << type;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return locateCount(id)->get(type);
}

UserApiCounterPtr UserApiCountManagerI::getAll(Ice::Int id, const Ice::Current& current)	{
	
	ostringstream inMsg;
	inMsg<<"id =" << id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	return locateCount(id);
}

void UserApiCountManagerI::reload(Ice::Int id, const Ice::Current& current)	{
	ostringstream inMsg;
	inMsg << "id = "<<id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	ServiceI::instance().removeObject(UAC, id);
}

UserApiCounterIPtr UserApiCountManagerI::locateCount(Ice::Int id)	{
	return ServiceI::instance().locateObject<UserApiCounterIPtr>(UAC, id);
}

UserApiCounterIPtr UserApiCountManagerI::findCount(Ice::Int id)	{
	return ServiceI::instance().findObject<UserApiCounterIPtr>(UAC, id);
}

// ============== UserApiCountTask ================================

CreateTask::CreateTask(Ice::Int id,int level):Task(level), _id(id){
}

void CreateTask::handle(){
	MCE_DEBUG("CreateTask::handle -> " << _id);
	ostringstream pattern;
	
	pattern << "user_visit_count_" << (_id % 100);
	Statement newsql;
	newsql << "SELECT type, count FROM " << pattern.str() << " WHERE id = " << _id;

	UserApiCounterIPtr result = new UserApiCounterI;
	QueryRunner("user_visit_count", CDbRServer, pattern.str())
		.query(newsql, UserApiCountResultHandler(result));

	ServiceI::instance().getObjectCache()->addObject(UAC,_id, result);
}

// ============== UserApiCountFactory ================================

Ice::ObjectPtr UserApiCountFactory::create(Ice::Int id)	{
	UserApiCounterIPtr result = new UserApiCounterI;
	TaskManager::instance().execute(new CreateTask(id));
        return result;
}

// ============= UserApiCountResultHandler ==========================

UserApiCountResultHandler::UserApiCountResultHandler(UserApiCounterIPtr& counter) :
	_counter(counter)	{
}

bool UserApiCountResultHandler::handle(mysqlpp::Row& row) const	{
	int type = (int) row["type"];
	int count = (int) row["count"];
	_counter->types.push_back(type);
	_counter->values.push_back(count);
	return true;
}
