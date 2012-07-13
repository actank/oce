#include "PhotoStateFilterI.h"
#include <exception>
#include <IceUtil/IceUtil.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::photostatefilter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace xce::clusterstate;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&PhotoStateFilterManagerI::instance(),service.createIdentity("M", ""));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("PhotoStateFilter.Mod", 0);
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("PhotoStateFilter.Interval", 5);
        ServerStateSubscriber::instance().initialize("ControllerPhotoStateFilter", &PhotoStateFilterManagerI::instance(), mod, interval);
        TaskManager::instance().config(TASK_LEVEL_INIT, ThreadPoolConfig(1, 100));
	PhotoStateFilterManagerI::instance().initialize();
}

namespace xce{
namespace photostatefilter {

PhotoStateFilterManagerI::PhotoStateFilterManagerI() {
}

void PhotoStateFilterManagerI::initialize() {
	MCE_INFO("PhotoStateFilterManagerI::initialize -> start ...");

	//TODO: read photostate for all users from local file

	MCE_INFO("PhotoStateFilterManagerI::initialize -> set every bit");
	for (int i=0;i<PHOTOSTATE_FILTER_SIZE;++i) {
		_photo_state.set(i);
	}
	MCE_INFO("PhotoStateFilterManagerI::initialize -> all bit setted!");

	TaskManager::instance().execute( new InitTask(_photo_state) );
}

void InitTask::handle(){
	MCE_INFO("InitTask -> start ...");
	int _max_id = 0;
	int large_id = 0;
//compare the status with DB and the default status
	try {
		do {
			Statement sql;
			sql << "SELECT id, tinyurl FROM user_url where id > " << large_id << " LIMIT 10000";
			map<int, bool> db_data;
			com::xiaonei::xce::QueryRunner("user_url_preloader",com::xiaonei::xce::CDbRServer ).query(
					sql, PhotoStateResultHandler( large_id, db_data )
					);
			{
				for (map<int, bool>::const_iterator it = db_data.begin(); it
						!= db_data.end(); ++it) {
					if( !(it->second) ){
						_photo_state.reset(it->first);
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
bool PhotoStateResultHandler::handle(mysqlpp::Row& res) const {
        int uid = res["id"];
	_large_id = uid > _large_id ? uid : _large_id;
	string photo = res["tinyurl"].data();
	if (photo.empty()) {
		MCE_WARN("PhotoStateResultHandler::handle -> " << uid << " has no tinyurl" );
		_result[uid] = false;
		return true;
	}
	if (photo.at(0)=='0') {
		_result[uid] = false;
	}
        return true;
}
//

ByteSeq PhotoStateFilterManagerI::getPhotoStateMinMax(int min, int max, const Ice::Current& current){
	ByteSeq result;
	if (max <= min) {
		MCE_WARN("PhotoStateFilterManagerI::getPhotoStateMinMax -> min >= max"
                	<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
		return result;
	}
	int n=(max-min)/8;
	for (int i=0;i<n;i++){
		result.push_back(Ice::Byte());
	}
	for(int i = min; i < max; ++i) {
		if (_photo_state[i]) {
			result[(i-min)/8]|=(1<<(i-min)%8);
		}
	}
	MCE_INFO("PhotoStateFilterManagerI::getPhotoStateMinMax -> min: "<<min << " max: "<<max<<" size:"<<result.size()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return result;
}

Int2BoolMap PhotoStateFilterManagerI::getPhotoStateBatch(const MyUtil::IntSeq& ids, const Ice::Current& current){
	ostringstream op;
	Int2BoolMap res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = _photo_state[*it];
		op << *it << "(" << res[*it] << ") ";
	}
	MCE_INFO("PhotoStateFilterManagerI::getPhotoStateBatch -> " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

MyUtil::IntSeq PhotoStateFilterManagerI::getPhotoIdsFrom(const MyUtil::IntSeq& ids, const Ice::Current& current){
	ostringstream op;
	IntSeq res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		if( _photo_state[*it] ){
			res.push_back( *it );
		} else{
			op << *it << " ";
		}
	}
	MCE_INFO("PhotoStateFilterManagerI::getPhotoIdsFrom " << ids.size() << " ids -> res: " << res.size() << " ids. filter: " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

bool PhotoStateFilterManagerI::isPhotoStateTrue(int id, const Ice::Current& current){
	MCE_INFO("PhotoStateFilterManagerI::isPhotoStateTrue -> id: "<<id<<" res: "<<_photo_state[id]
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return _photo_state[id];
}

void PhotoStateFilterManagerI::updatePhotoState(int userid,const string& photo, const Ice::Current& current){
	if (photo.empty()) {
		MCE_WARN("PhotoStateFilterManagerI::updatePhotoState -> " << userid << " has no photo"
                	<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
		_photo_state.reset(userid);
		return;
	}
	if (photo.at(0)=='0') {
		_photo_state.reset(userid);
		MCE_INFO("PhotoStateFilterManagerI::updatePhotoState -> userid: "<<userid<<" photo[0]: \'"<<photo.at(0)<<"\' (false)"
                	<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	}  else {
		_photo_state.set(userid);
		MCE_INFO("PhotoStateFilterManagerI::updatePhotoState -> userid: "<<userid<<" photo[0]: \'"<<photo.at(0)<<"\' (true)"
                	<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	}
}

void PhotoStateFilterManagerI::updatePhotoStateBatch(const ::MyUtil::Int2StrMap& photomap, const Ice::Current& current){
	ostringstream op;
        for (Int2StrMap::const_iterator it = photomap.begin(); it != photomap.end(); ++it) {
		if (it->second.empty()) {
			MCE_WARN("PhotoStateFilterManagerI::updatePhotoState -> " << it->first << " has no photo"
                		<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
			_photo_state.reset(it->first);
			return;
		}
		if (it->second.at(0)=='0') {
			_photo_state.reset(it->first);
			op << it->first << "," << it->second.at(0) << "(false) ";
		}  else {
			_photo_state.set(it->first);
			op << it->first << "," << it->second.at(0) << "(true) ";
		}
        }
	MCE_INFO("PhotoStateFilterManagerI::updatePhotoStateBatch -> " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
}

}
}
