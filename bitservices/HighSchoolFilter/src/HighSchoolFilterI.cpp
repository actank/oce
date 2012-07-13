#include "HighSchoolFilterI.h"
#include <exception>
#include <IceUtil/IceUtil.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::highschoolfilter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace xce::clusterstate;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&HighSchoolFilterManagerI::instance(),service.createIdentity("M", ""));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("HighSchoolFilter.Mod", 0);
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("HighSchoolFilter.Interval", 5);
        ServerStateSubscriber::instance().initialize("ControllerHighSchoolFilter", &HighSchoolFilterManagerI::instance(), mod, interval);
        TaskManager::instance().config(TASK_LEVEL_INIT, ThreadPoolConfig(1, 100));
	HighSchoolFilterManagerI::instance().initialize();
}

namespace xce{
namespace highschoolfilter {

HighSchoolFilterManagerI::HighSchoolFilterManagerI() {
}

void HighSchoolFilterManagerI::initialize() {
	MCE_INFO("HighSchoolFilterManagerI::initialize -> start ...");

	//TODO: read highschool for all users from local file

	MCE_INFO("HighSchoolFilterManagerI::initialize -> set every bit");
	for (int i=0;i<HIGHSCHOOL_FILTER_SIZE;++i) {
		_user_stage_highschool.reset(i);
	}
	MCE_INFO("HighSchoolFilterManagerI::initialize -> all bit setted!");

	TaskManager::instance().execute( new InitTask(_user_stage_highschool) );
}

void InitTask::handle(){
	MCE_INFO("InitTask -> start ...");
	int _max_id = 0;
	int large_id = 0;
//compare the status with DB and the default status
	try {
		do {
			Statement sql;
			sql << "SELECT id, stage FROM user_stage WHERE id > " << large_id << " LIMIT 10000";

			map<int, bool> db_stage_highschool;
			com::xiaonei::xce::QueryRunner("user_stage_preloader",com::xiaonei::xce::CDbRServer ).query(
					sql, HighSchoolResultHandler( large_id, db_stage_highschool )
					);
			{
				for (map<int, bool>::const_iterator it = db_stage_highschool.begin(); it
						!= db_stage_highschool.end(); ++it) {
					if( it->second){
						_user_stage_highschool.set(it->first);
					}
				}
			}

                        if (large_id > _max_id) {
                                _max_id = large_id;
                        } else {
                                break;
                        }

			MCE_INFO("InitTask -> initializing to " << large_id);
		} while (true);
	} catch (...) {
		MCE_WARN("InitTask -> loading exit with unknown exception.");
	}
	MCE_INFO("InitTask -> done");
}

//
bool HighSchoolResultHandler::handle(mysqlpp::Row& res) const {
        int uid = res["id"];
	_large_id = uid > _large_id ? uid : _large_id;
	int stage = (int) res["stage"];

        if (stage==10) {
		_stage_highschool_result[uid] = true;
        } else {
		_stage_highschool_result[uid] = false;
        }
        return true;
}
//

ByteSeq HighSchoolFilterManagerI::getStageHighSchoolMinMax(int min, int max, const Ice::Current& current){
	ByteSeq result;
	if (max <= min) {
		MCE_WARN("HighSchoolFilterManagerI::getStageHighSchoolMinMax -> min >= max"
                	<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
		return result;
	}
	int n=(max-min)/8;
	for (int i=0;i<n;i++){
		result.push_back(Ice::Byte());
	}
	for(int i = min; i < max; ++i) {
		if (_user_stage_highschool[i]) {
			result[(i-min)/8]|=(1<<(i-min)%8);
		}
	}
	MCE_INFO("HighSchoolFilterManagerI::getStageHighSchoolMinMax -> min: "<<min << " max: "<<max<<" size:"<<result.size()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return result;
}

Int2BoolMap HighSchoolFilterManagerI::getStageHighSchoolBatch(const MyUtil::IntSeq& ids, const Ice::Current& current){
	ostringstream op;
	Int2BoolMap res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = _user_stage_highschool[*it];
		op << *it << "(" << res[*it] << ") ";
	}
	MCE_INFO("HighSchoolFilterManagerI::getStageHighSchoolBatch -> " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

MyUtil::IntSeq HighSchoolFilterManagerI::getStageHighSchoolIdsFrom(const MyUtil::IntSeq& ids, const Ice::Current& current){
	ostringstream op;
	IntSeq res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		if( _user_stage_highschool[*it] ){
			res.push_back( *it );
		} else{
			op << *it << " ";
		}
	}
	MCE_INFO("HighSchoolFilterManagerI::getStarIdsFrom " << ids.size() << " ids -> res: " << res.size() << " ids. filter: " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

bool HighSchoolFilterManagerI::isStageHighSchoolTrue(int id, const Ice::Current& current){
	MCE_INFO("HighSchoolFilterManagerI::isStageHighSchoolTrue -> id: "<<id<<" res: "<<_user_stage_highschool[id]
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return _user_stage_highschool[id];
}

void HighSchoolFilterManagerI::updateStageHighSchool(int userId,int stage, const Ice::Current& current){
	ostringstream op;
	op << " " << userId;
        if (stage==10) {
		_user_stage_highschool.set(userId);
		op << "(" << stage << ":true)";
        } else {
		_user_stage_highschool.reset(userId);
		op << "(" << stage << ":false)";
        }
	MCE_INFO("HighSchoolFilterManagerI::updateStageHighSchool -> " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
}

void HighSchoolFilterManagerI::updateStageHighSchoolBatch(const ::MyUtil::Int2IntMap& stagemap, const Ice::Current& current){
	ostringstream op;
        for (Int2IntMap::const_iterator it = stagemap.begin(); it != stagemap.end(); ++it) {
		op << " " << it->first;
		if (it->second==10) {
			_user_stage_highschool.set(it->first);
			op << "(" << it->second << ":true)";
		} else {
			_user_stage_highschool.reset(it->first);
			op << "(" << it->second << ":false)";
		}
        }
	MCE_INFO("HighSchoolFilterManagerI::updateStageHighSchoolBatch -> " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
}

}
}
