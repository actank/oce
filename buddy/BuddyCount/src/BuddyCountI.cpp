#include <algorithm>
#include "BuddyCountI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"

using namespace MyUtil;
using namespace xce::buddy;
using namespace com::xiaonei::xce;
using namespace std;
using namespace ::xce::serverstate;

void MyUtil::initialize()  {

	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(BC, "BC",new BuddyCountFactory, false);
	service.getAdapter()->add(&BuddyCountManagerI::instance(), service.createIdentity("M", ""));
	TaskManager::instance().config(TASK_LEVEL_CREATE, MyUtil::ThreadPoolConfig(1, 1));
        int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyCount.Mod",0);
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyCount.Interval",5);
        ServerStateSubscriber::instance().initialize("ControllerBuddyCount",&BuddyCountManagerI::instance(),mod,interval);
	{ 
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyCount.Status",0);
		ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status;
		MCE_INFO( os.str() );
	}

}



// ================= BuddyCounterI ===================================
bool BuddyCounterI::find(int type, int& pos)	{

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

bool BuddyCounterI::insert(int type, int step, int pos)	{

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

void BuddyCounterI::inc(int type, int step)	{

	IceUtil::Mutex::Lock lock(*this);
	int pos = -1;
	if ( find(type, pos) )	{
		values.at(pos) += step;
	}
	else	{
		insert(type, step, pos);
	}
}

void BuddyCounterI::dec(int type, int step)	{

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

void BuddyCounterI::set(int type, int value)	{

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

int BuddyCounterI::get(int type)	{

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


void BuddyCounterI::sort() {
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

void BuddyCountManagerI::inc(Ice::Int id, Ice::Int type, Ice::Int step,
		const Ice::Current& current)	{
	ostringstream inMsg;
	inMsg << "BuddyCountManagerI::inc -> id=" << id << " step=" << step << " type=" << type << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	MCE_INFO(inMsg.str());
	locateCount(id)->inc(type, step);
}

void BuddyCountManagerI::dec(Ice::Int id, Ice::Int type, Ice::Int step,
		const Ice::Current& current)	{
	ostringstream inMsg;
	inMsg << "BuddyCountManagerI::dec -> id=" << id << " step=" << step << " type=" << type << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	MCE_INFO(inMsg.str());
	locateCount(id)->dec(type, step);
}

void BuddyCountManagerI::set(Ice::Int id, Ice::Int type, Ice::Int value,
		const Ice::Current& current)	{
	ostringstream inMsg;
	inMsg << "BuddyCountManagerI::set -> id=" << id << " value=" << value << " type=" << type << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	MCE_INFO(inMsg.str());
	locateCount(id)->set(type, value);
}


int BuddyCountManagerI::get(Ice::Int id, Ice::Int type, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "BuddyCountManagerI::get -> id=" << id << " type=" << type << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	BuddyCounterIPtr counter = locateCount(id);
	if(counter->timestamp - time(NULL) > DURATION){
		ServiceI::instance().removeObject(BC, id);
		return 0;
	}else{	
		return counter->get(type);
	}
}

BuddyCounterPtr BuddyCountManagerI::getAll(Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"BuddyCountManagerI::getAll -> id=" << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	BuddyCounterPtr counter = locateCount(id);
        if(counter->timestamp - time(NULL) > DURATION){
                ServiceI::instance().removeObject(BC, id);
                return new BuddyCounterI();
        }else{
                return counter;
        }
}

void BuddyCountManagerI::reload(Ice::Int id, const Ice::Current& current)	{
	ostringstream inMsg;
	inMsg<<"BuddyCountManagerI::reload -> id=" << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	MCE_INFO(inMsg.str());
	ServiceI::instance().removeObject(BC, id);
}

BuddyCounterIPtr BuddyCountManagerI::locateCount(Ice::Int id)	{
	return ServiceI::instance().locateObject<BuddyCounterIPtr>(BC, id);
}

BuddyCounterIPtr BuddyCountManagerI::findCount(Ice::Int id)	{
	return ServiceI::instance().findObject<BuddyCounterIPtr>(BC, id);
}

// ============== BuddyCountTask ================================

CreateTask::CreateTask(Ice::Int id,int level):Task(level), _id(id){
}

void CreateTask::handle(){
	MCE_DEBUG("CreateTask::handle -> " << _id);
	BuddyCounterIPtr result = new BuddyCounterI;
	result->timestamp = time(NULL);
        ServiceI::instance().getObjectCache()->addObject(BC,_id, result);
}

// ============== BuddyCountFactory ================================

Ice::ObjectPtr BuddyCountFactory::create(Ice::Int id)	{
	BuddyCounterIPtr result = new BuddyCounterI;
	TaskManager::instance().execute(new CreateTask(id));
        return result;
}

