#include "ServiceI.h"
#include "TrackSummaryI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace ::xce::activetrack;
using namespace ::MyUtil;
using namespace ::com::xiaonei::xce;
using namespace ::xce::notifysender::adapter;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	service.getAdapter()->add(new TrackSummaryI(props), service.createIdentity("TS", ""));

	service.registerObjectCache(TRACK, "T",	new SyncServantFactory( new TrackFactory ));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("TrackSummary.Mod");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("TrackSummary.Interval", 5);
        string controllerEndpoints = service.getCommunicator()->getProperties()->getProperty("TrackSummary.ControllerEndpoint");
        ServerStateSubscriber::instance().initialize(controllerEndpoints, new TrackSummaryI(props), mod, interval);
}

MyUtil::Str2StrMap Track::get() {
	IceUtil::Mutex::Lock lock(*this);
	return _track;
}
void Track::set(std::string key, std::string value) {
	IceUtil::Mutex::Lock lock(*this);
	std::map<std::string,std::string>::iterator it = _track.find(key);
	if(it==_track.end()){
		_track.insert( std::map<std::string,std::string>::value_type( key, value ) );
	}else{
		it->second = value;
	}
}

void Track::remove(std::string& key){
	IceUtil::Mutex::Lock lock(*this);
	std::map<std::string,std::string>::iterator it = _track.find(key);
	if(it==_track.end()){
		return;
	};
	_track.erase(it);
}

Ice::ObjectPtr TrackFactory::create(::Ice::Int id) {
	MCE_DEBUG( "TrackFactory::create(" << id << ")" );
	TrackPtr result = new Track;
//	ostringstream patten;
//	patten << "xxx" << (id % 10);
//	ostringstream sql;
//	sql << "SELECT type,status FROM " << patten.str() << " WHERE userid="
//			<< id;
//	PreferenceResultHandlerI handler(result);
//	QueryRunner("xxx", CDbRServer, patten.str()).query(sql.str(), handler);
	return result;
}

TrackResultHandlerI::TrackResultHandlerI(const TrackPtr result):
	_result(result){
}

bool TrackResultHandlerI::handle(mysqlpp::Row& row) const {
	std::string type = (std::string) row["type"];
	std::string status = (std::string) row["status"];
	_result->set( type, status );
	return true;
}

TrackSummaryI::TrackSummaryI(Ice::PropertiesPtr props){
	_actions.push_back(new LastURLAction);
	_actions.push_back(new AIGuideAction(props));
}

MyUtil::Str2StrMap TrackSummaryI::get(Ice::Int userId, const Ice::Current& current){
	MCE_DEBUG( "[TrackSummaryI]: get(" << userId << ")" );
	return locateTrack(userId)->get();
}

void TrackSummaryI::set(Ice::Int userId, const std::string& key, const std::string& value, const Ice::Current& current){
	MCE_INFO( "[TrackSummaryI]: set(" << userId << ", " << key << ", " << value << ", " << ")" );
	locateTrack(userId)->set(key,value);
}

void TrackSummaryI::access(
		Ice::Int userId,
		const std::string& ip,
		const std::string& url,
		const Ice::Current& current){
	MCE_DEBUG( "[TrackSummaryI]: access(" << userId << "," << ip << "," << url << ")" );
	for(std::vector<ActionPtr>::iterator it=_actions.begin(); it!=_actions.end(); it++ ){
		TaskManager::instance().execute(new ActionTask(userId,ip,url,*it,locateTrack(userId)));
	}
}

TrackPtr TrackSummaryI::locateTrack(Ice::Int userId){
	return ServiceI::instance().locateObject<TrackPtr> (TRACK, userId);
}

void ActionTask::handle() {
	_action->execute(_ownerId,_ip,_url,_userTrack);
}
