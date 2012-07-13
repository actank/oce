#include "StatusCacheI.h"
#include <ServiceI.h>
#include <exception>
#include <IceUtil/IceUtil.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "util/cpp/TimeCost.h"

using namespace xce::statuscache;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace xce::clusterstate;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&StatusCacheManagerI::instance(),service.createIdentity("M", ""));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("StatusCache.Mod", 0);
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("StatusCache.Interval", 5);
        ServerStateSubscriber::instance().initialize("ControllerStatusCache", &StatusCacheManagerI::instance(), mod, interval);
        TaskManager::instance().execute(new FillTask());
}

namespace xce{
namespace statuscache {
void FillTask::handle() {
	StatusCacheManagerI::instance().initialize();
}

StatusCacheManagerI::StatusCacheManagerI() : _inited_flag(false), maxId_(0) {
	MCE_INFO("StatusCacheManagerI::StatusCacheManagerI");
	for (int i=0;i<STATUS_CACHE_SIZE;++i) {
		_status.set(i);
	}
	MCE_INFO("StatusCacheManagerI::StatusCacheManagerI -> done");
}

void StatusCacheManagerI::initialize() {
	MCE_INFO("StatusCacheManagerI::initialize -> start ...");
	//queue_.setProps(10000);
	//start();
	try {
		int large_id = 0;
		do {
			Statement sql;
			sql << "SELECT id, status FROM user_passport WHERE id > " << getMaxId() << " LIMIT 10000";
			map<int, int> db_data;
			com::xiaonei::xce::QueryRunner("user_passport_status",com::xiaonei::xce::CDbRServer ).query(
					sql, StatusCacheResultHandler( large_id, db_data )
			);
			{
				for (map<int, int>::const_iterator it = db_data.begin(); it
						!= db_data.end(); ++it) {
					if (it->second == 0 || it->second == 3) {
						_status.reset(it->first);
					}
				}
			}
			if (large_id > getMaxId()) {
				setMaxId(large_id);
			} else {
				setMaxId(STATUS_CACHE_SIZE);
				break;
			}
			MCE_INFO("StatusCacheManagerI::initialize -> initializing to " << large_id);
		} while (true);
	} catch (...) {
		MCE_WARN("StatusCacheManagerI::initialize -> loading exit with unknown exception.");
	}
	MCE_INFO("StatusCacheManagerI::initialize -> thread loading done.");
	{
		IceUtil::RWRecMutex::WLock lock(_inited_flag_mutex);
		_inited_flag = true;
	}
	MCE_INFO("StatusCacheManagerI::initialize -> thread all done.");
}

ByteSeq StatusCacheManagerI::getStatusMinMax(int min, int max, const Ice::Current& current){

	ostringstream inMsg;
	inMsg<<"StatusCacheManagerI::getStatusMinMax min: " <<min << " max: "<< max << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str(), TimeCost::LOG_LEVEL_INFO);
	
	ByteSeq result;
	if (max <= min) {
		MCE_WARN("StatusCacheManagerI::getStatusMinMax -> min >= max");
		return result;
	}
	int n=(max-min)/8;
	MCE_DEBUG("StatusCacheManagerI::getStatusMinMax -> n: "<<n);
	for (int i=0;i<n;i++){
		result.push_back(Ice::Byte());
	}
	for(int i = min; i < max; ++i) {
		if (_status[i]) {
			result[(i-min)/8]|=(1<<(i-min)%8);
		}
	}
	MCE_DEBUG("StatusCacheManagerI::getStatusMinMax -> size: "<<result.size());
	return result;
}

Int2BoolMap StatusCacheManagerI::getStatusBatch(const MyUtil::IntSeq& ids, const Ice::Current& current){
	ostringstream op;
	Int2BoolMap res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = _status[*it];
		op << *it << "(" << res[*it] << ") ";
	}
	MCE_INFO("StatusCacheManagerI::getStatusBatch -> " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

MyUtil::IntSeq StatusCacheManagerI::getStatusIdsFrom(const MyUtil::IntSeq& ids, const Ice::Current& current){
	ostringstream op;
	IntSeq res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		if( !_status[*it] ){
			res.push_back( *it );
			op << *it << "(0) ";
		}
	}
	MCE_INFO("StatusCacheManagerI::getStatusIdsFrom -> " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

bool StatusCacheManagerI::isStatusTrue(int id, const Ice::Current& current){

	ostringstream inMsg;
	inMsg<<"StatusCacheManagerI::isStatusTrue id: " << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str(), TimeCost::LOG_LEVEL_INFO);
	
	return _status[id];
}

bool StatusCacheManagerI::isGoodUser(int id, const Ice::Current& current) {

	ostringstream inMsg;
	inMsg<<"StatusCacheManagerI::isGoodUser id: " << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str(), TimeCost::LOG_LEVEL_INFO);
	
	return !_status[id];
}

void StatusCacheManagerI::updateStatus(int userid,int status, const Ice::Current& current){
	ostringstream inMsg;
	inMsg<<"StatusCacheManagerI::updateStatus userid: " << userid << " status: "<< status << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str(), TimeCost::LOG_LEVEL_INFO);
	
	if (status !=0 && status != 3) {
		_status.set(userid);
	}  else {
		_status.reset(userid);
	}
}

void StatusCacheManagerI::updateStatusBatch(const ::MyUtil::Int2IntMap& statusmap, const Ice::Current& current){
	ostringstream op;
        for (Int2IntMap::const_iterator it = statusmap.begin(); it != statusmap.end(); ++it) {
		if ( (it->second!=0)&&(it->second!=3) ) {
			_status.set(it->first);
		}  else {
			_status.reset(it->first);
		}
		op << it->first << "(" << it->second << ") ";
        }
	MCE_INFO("StatusCacheManagerI::updateStatusBatch -> " << op.str() );
}

bool StatusCacheManagerI::isValid(const Ice::Current& current){
	IceUtil::RWRecMutex::RLock lock(_inited_flag_mutex);
	
	ostringstream inMsg;
	inMsg<<"StatusCacheManagerI::isValid "<< current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str(), TimeCost::LOG_LEVEL_INFO);

	return _inited_flag;
}

int StatusCacheManagerI::getMaxId() {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(maxmutex_);
	return maxId_;
}

void StatusCacheManagerI::setMaxId(int id) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(maxmutex_);
	maxId_ = id;
	maxmutex_.notify();
}

bool StatusCacheResultHandler::handle(mysqlpp::Row& res) const {
	int uid = res["id"];
	_large_id = uid > _large_id ? uid : _large_id;
	int status = res["status"];
	_result.insert(make_pair(uid, status));
	return true;
}
}
}
