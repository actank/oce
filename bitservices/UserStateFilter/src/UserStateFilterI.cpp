#include "UserStateFilterI.h"
#include <exception>
#include <IceUtil/IceUtil.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::userstatefilter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace xce::clusterstate;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserStateFilterManagerI::instance(),service.createIdentity("M", ""));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserStateFilter.Mod", 0);
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserStateFilter.Interval", 5);
        ServerStateSubscriber::instance().initialize("ControllerUserStateFilter", &UserStateFilterManagerI::instance(), mod, interval);
        TaskManager::instance().config(TASK_LEVEL_INIT, ThreadPoolConfig(1, 100));
	UserStateFilterManagerI::instance().initialize();
}

namespace xce{
namespace userstatefilter {

UserStateFilterManagerI::UserStateFilterManagerI() {
}

void UserStateFilterManagerI::initialize() {
	MCE_INFO("UserStateFilterManagerI::initialize -> start ...");

	//TODO: read userstate for all users from local file

	MCE_INFO("UserStateFilterManagerI::initialize -> set every bit");
	for (int i=0;i<USERSTATE_FILTER_SIZE;++i) {
		_user_state_star.reset(i);
		_user_state_guide.reset(i);
	}
	MCE_INFO("UserStateFilterManagerI::initialize -> all bit setted!");

	TaskManager::instance().execute( new InitTask(_user_state_star,_user_state_guide) );
}

void InitTask::handle(){
	MCE_INFO("InitTask -> start ...");
	int _max_id = 0;
	int large_id = 0;
//compare the status with DB and the default status
	try {
		do {
			Statement sql;
			sql << "SELECT id, state FROM user_state WHERE id > " << large_id << " LIMIT 10000";

			map<int, bool> db_state_star;
			map<int, bool> db_state_guide;
			com::xiaonei::xce::QueryRunner("user_state_preloader",com::xiaonei::xce::CDbRServer ).query(
					sql, UserStateResultHandler( large_id, db_state_star, db_state_guide )
					);
			{
				for (map<int, bool>::const_iterator it = db_state_star.begin(); it
						!= db_state_star.end(); ++it) {
					if( (it->second) ){
						_user_state_star.set(it->first);
					}
				}
				for (map<int, bool>::const_iterator it = db_state_guide.begin(); it
						!= db_state_guide.end(); ++it) {
					if( it->second ){
						_user_state_guide.set(it->first);
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
bool UserStateResultHandler::handle(mysqlpp::Row& res) const {
        int uid = res["id"];
	_large_id = uid > _large_id ? uid : _large_id;
	int state = (int) res["state"];

        if (state&1) {
		_star_result[uid] = true;
        } else {
		_star_result[uid] = false;
        }

        if (state&5) {
		_guide_result[uid] = false;
        } else {
		_guide_result[uid] = true;
        }

        return true;
}
//

ByteSeq UserStateFilterManagerI::getStateStarMinMax(int min, int max, const Ice::Current& current){
	ByteSeq result;
	if (max <= min) {
		MCE_WARN("UserStateFilterManagerI::getStateStarMinMax -> min >= max"
                	<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
		return result;
	}
	int n=(max-min)/8;
	for (int i=0;i<n;i++){
		result.push_back(Ice::Byte());
	}
	for(int i = min; i < max; ++i) {
		if (_user_state_star[i]) {
			result[(i-min)/8]|=(1<<(i-min)%8);
		}
	}
	MCE_INFO("UserStateFilterManagerI::getStateStarMinMax -> min: "<<min << " max: "<<max<<" size:"<<result.size()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return result;
}

Int2BoolMap UserStateFilterManagerI::getStateStarBatch(const MyUtil::IntSeq& ids, const Ice::Current& current){
	ostringstream op;
	Int2BoolMap res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = _user_state_star[*it];
		op << *it << "(" << res[*it] << ") ";
	}
	MCE_INFO("UserStateFilterManagerI::getStateStarBatch -> " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

MyUtil::IntSeq UserStateFilterManagerI::getStarIdsFrom(const MyUtil::IntSeq& ids, const Ice::Current& current){
	ostringstream op;
	IntSeq res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		if( _user_state_star[*it] ){
			res.push_back( *it );
		} else{
			op << *it << " ";
		}
	}
	MCE_INFO("UserStateFilterManagerI::getStarIdsFrom " << ids.size() << " ids -> res: " << res.size() << " ids. filter: " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

bool UserStateFilterManagerI::isStateStarTrue(int id, const Ice::Current& current){
	MCE_INFO("UserStateFilterManagerI::isStateStarTrue -> id: "<<id<<" res: "<<_user_state_star[id]
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return _user_state_star[id];
}

ByteSeq UserStateFilterManagerI::getStateGuideMinMax(int min, int max, const Ice::Current& current){
	ByteSeq result;
	if (max <= min) {
		MCE_WARN("UserStateFilterManagerI::getStateGuideMinMax -> min >= max"
                	<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
		return result;
	}
	int n=(max-min)/8;
	for (int i=0;i<n;i++){
		result.push_back(Ice::Byte());
	}
	for(int i = min; i < max; ++i) {
		if (_user_state_guide[i]) {
			result[(i-min)/8]|=(1<<(i-min)%8);
		}
	}
	MCE_INFO("UserStateFilterManagerI::getStateGuideMinMax -> min: "<<min << " max: "<<max<<" size:"<<result.size()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return result;
}

Int2BoolMap UserStateFilterManagerI::getStateGuideBatch(const MyUtil::IntSeq& ids, const Ice::Current& current){
	ostringstream op;
	Int2BoolMap res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = _user_state_guide[*it];
		op << *it << "(" << res[*it] << ") ";
	}
	MCE_INFO("UserStateFilterManagerI::getStateGuideBatch -> " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

MyUtil::IntSeq UserStateFilterManagerI::getGuideIdsFrom(const MyUtil::IntSeq& ids, const Ice::Current& current){
	ostringstream op;
	IntSeq res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		if( _user_state_guide[*it] ){
			res.push_back( *it );
		} else{
			op << *it << " ";
		}
	}
	MCE_INFO("UserStateFilterManagerI::getGuideIdsFrom " << ids.size() << " ids -> res: " << res.size() << " ids. filter: " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

bool UserStateFilterManagerI::isStateGuideTrue(int id, const Ice::Current& current){
	MCE_INFO("UserStateFilterManagerI::isStateGuideTrue -> id: "<<id<<" res: "<<_user_state_guide[id]
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return _user_state_guide[id];
}

void UserStateFilterManagerI::updateUserState(int userId,int state, const Ice::Current& current){
	ostringstream op;
	op << " " << userId;
        if (state&1) {
		_user_state_star.set(userId);
		op << "(1:true)";
        } else {
		_user_state_star.reset(userId);
		op << "(1:false)";
        }

        if (state&5) {
		_user_state_guide.reset(userId);
		op << "(5:false)";
        } else {
		_user_state_guide.set(userId);
		op << "(5:true)";
        }
	MCE_INFO("UserStateFilterManagerI::updateUserState -> " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
}

void UserStateFilterManagerI::updateUserStateBatch(const ::MyUtil::Int2IntMap& statemap, const Ice::Current& current){
	ostringstream op;
        for (Int2IntMap::const_iterator it = statemap.begin(); it != statemap.end(); ++it) {
		op << " " << it->first;
		if (it->second&1) {
			_user_state_star.set(it->first);
			op << "(1:true)";
		} else {
			_user_state_star.reset(it->first);
			op << "(1:false)";
		}

		if (it->second&5) {
			_user_state_guide.reset(it->first);
			op << "(5:false)";
		} else {
			_user_state_guide.set(it->first);
			op << "(5:true)";
		}
        }
	MCE_INFO("UserStateFilterManagerI::updateUserStateBatch -> " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
}

}
}
