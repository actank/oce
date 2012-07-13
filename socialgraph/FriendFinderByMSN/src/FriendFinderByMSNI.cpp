#include "ServiceI.h"
#include "FriendFinderByMSNI.h"
#include <boost/lexical_cast.hpp>
#include "BuddyCoreAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::friendfinder;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace xce::serverstate;
using namespace com::xiaonei::service::buddy;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(FRIENDFINDER_BYMSN_DATA, "FBM", new FriendFinderByMSNDataFactory);
	service.getAdapter()->add(&FriendFinderByMSNManagerI::instance(),
		service.createIdentity("M", ""));

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendFinderByMSN.Mod");
        string endpoint = service.getCommunicator()->getProperties()->getProperty("FriendFinderByMSN.Endpoint");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendFinderByMSN.Interval", 5);

        // 向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerFriendFinderByMSN",
                        &FriendFinderByMSNManagerI::instance(), mod, interval);

        TaskManager::instance().config(TASK_LEVEL_ADD, ThreadPoolConfig(1,1));
        TaskManager::instance().config(TASK_LEVEL_REMOVE, ThreadPoolConfig(1,1));
        TaskManager::instance().config(TASK_LEVEL_AUTO_APPLY, ThreadPoolConfig(1,1));
        service.registerXceObserver(new ServiceSchemaI);
}

////***************************************************************************

MSNdata FriendFinderByMSNData::getMSNdata(){
	IceUtil::Mutex::Lock lock(mutex_);
	return msndata_;
}

MSNmap FriendFinderByMSNData::getPUAmap(){
	IceUtil::Mutex::Lock lock(mutex_);
	return msndata_.puamap;
}
MyUtil::IntSeq FriendFinderByMSNData::getPUAids(){
	IceUtil::Mutex::Lock lock(mutex_);
	MyUtil::IntSeq res;
	for( MSNmap::const_iterator itMap = msndata_.puamap.begin(); itMap != msndata_.puamap.end(); ++itMap ){
		for( MyUtil::IntSeq::const_iterator itSeq = itMap->second.begin(); itSeq != itMap->second.end(); ++itSeq ){
			res.push_back( *itSeq );
		}
	}
	return res;
}
MyUtil::IntSeq FriendFinderByMSNData::getPUAidsWithMask(const MyUtil::IntSeq& mask){
	IceUtil::Mutex::Lock lock(mutex_);
	MyUtil::IntSeq res;
	for( MSNmap::const_iterator itMap = msndata_.puamap.begin(); itMap != msndata_.puamap.end(); ++itMap ){
		for( MyUtil::IntSeq::const_iterator itSeq = itMap->second.begin(); itSeq != itMap->second.end(); ++itSeq ){
			MyUtil::IntSeq::const_iterator itMask = find( mask.begin(),mask.end(),*itSeq);
			if( itMask == mask.end() ){
				res.push_back( *itSeq );
			}
		}
	}
	return res;
}

MSNmap FriendFinderByMSNData::getPUKmap(){
	IceUtil::Mutex::Lock lock(mutex_);
	return msndata_.puamap;
}
MyUtil::IntSeq FriendFinderByMSNData::getPUKids(){
	IceUtil::Mutex::Lock lock(mutex_);
	MyUtil::IntSeq res;
	for( MSNmap::const_iterator itMap = msndata_.pukmap.begin(); itMap != msndata_.pukmap.end(); ++itMap ){
		for( MyUtil::IntSeq::const_iterator itSeq = itMap->second.begin(); itSeq != itMap->second.end(); ++itSeq ){
			res.push_back( *itSeq );
		}
	}
	return res;
}
MyUtil::IntSeq FriendFinderByMSNData::getPUKidsRandom(int limit){
	MyUtil::IntSeq allids = getPUKids();
	IceUtil::Mutex::Lock lock(mutex_);

	if ((int) allids.size() <= limit) {
		return allids;
	}
	map<int, int> randomMap;
	vector<int> randomList;
	while ((int) randomMap.size() < limit) {
		int pos = rand() % allids.size();
		if (!randomMap.count(pos)) {
			randomMap.insert(make_pair(pos, 1));
			randomList.push_back(pos);
		}
	}
	IntSeq res;
	for (vector<int>::iterator it = randomList.begin(); it
			!= randomList.end(); ++it) {
		res.push_back(allids.at(*it));
	}

	return res;
}
MyUtil::IntSeq FriendFinderByMSNData::getPUKidsWithMask(const MyUtil::IntSeq& mask){
	IceUtil::Mutex::Lock lock(mutex_);
	MyUtil::IntSeq res;
	for( MSNmap::const_iterator itMap = msndata_.pukmap.begin(); itMap != msndata_.pukmap.end(); ++itMap ){
		for( MyUtil::IntSeq::const_iterator itSeq = itMap->second.begin(); itSeq != itMap->second.end(); ++itSeq ){
			MyUtil::IntSeq::const_iterator itMask = find( mask.begin(),mask.end(),*itSeq);
			if( itMask == mask.end() ){
				res.push_back( *itSeq );
			}
		}
	}
	return res;
}

void FriendFinderByMSNData::add(int hostId,int guestId,int type){
	IceUtil::Mutex::Lock lock(mutex_);
	if(type<10){
		MyUtil::IntSeq::iterator itSeq = find( msndata_.puamap[type].begin(),msndata_.puamap[type].end(),guestId );
		if( itSeq == msndata_.puamap[type].end() ){
			msndata_.puamap[type].push_back( guestId );
			TaskManager::instance().execute( new AddToDBTask(hostId,guestId,type) );
		}
	} else{
		MyUtil::IntSeq::iterator itSeq = find( msndata_.pukmap[type].begin(),msndata_.pukmap[type].end(),guestId );
		if( itSeq == msndata_.pukmap[type].end() ){
			msndata_.pukmap[type].push_back( guestId );
			TaskManager::instance().execute( new AddToDBTask(hostId,guestId,type) );
		}
	}
}

void FriendFinderByMSNData::removePUA(int hostId,int guestId){
	IceUtil::Mutex::Lock lock(mutex_);
	for( MSNmap::iterator itMap = msndata_.puamap.begin(); itMap != msndata_.puamap.end(); ++itMap ){
		MyUtil::IntSeq::iterator itSeq = find( itMap->second.begin(),itMap->second.end(),guestId );
		if( itSeq != itMap->second.end() ){
			itMap->second.erase( itSeq );
			MCE_INFO("[removePUA] hostId=" << hostId << " guestId=" << guestId );
			TaskManager::instance().execute( new RemoveFromDBTask(hostId,guestId) );
		}
	}
}
void FriendFinderByMSNData::removePUK(int hostId,int guestId){
	IceUtil::Mutex::Lock lock(mutex_);
	for( MSNmap::iterator itMap = msndata_.pukmap.begin(); itMap != msndata_.pukmap.end(); ++itMap ){
		MyUtil::IntSeq::iterator itSeq = find( itMap->second.begin(),itMap->second.end(),guestId );
		if( itSeq != itMap->second.end() ){
			itMap->second.erase( itSeq );
			MCE_INFO("[removePUK] hostId=" << hostId << " guestId=" << guestId );
			TaskManager::instance().execute( new RemoveFromDBTask(hostId,guestId) );
		}
	}
}

void FriendFinderByMSNData::set(int guestId,int type){
	IceUtil::Mutex::Lock lock(mutex_);
	if(type<10){
		msndata_.puamap[type].push_back( guestId );
	} else{
		msndata_.pukmap[type].push_back( guestId );
	}
}

////***************************************************************************

Ice::ObjectPtr FriendFinderByMSNDataFactory::create(::Ice::Int id) {
        MCE_INFO( "FriendFinderByMSNDataFactory::create(" << id << ")" );
	FriendFinderByMSNDataPtr fbm = new FriendFinderByMSNData();
        Statement sql_puk;
        sql_puk		<< "SELECT guest,type FROM xn_puk" 
                        << " WHERE host=" << id;
        QueryRunner("buddygroup", CDbRServer).query(
                        sql_puk, FriendFinderByMSNDataResultHandler(id,fbm));

	MyUtil::IntSeq toApply;
        Statement sql_apply;
        sql_apply	<< "SELECT host,type FROM xn_puk" 
			<< " WHERE guest=" << id;
        QueryRunner("buddygroup", CDbRServer).query(
                        sql_apply, FriendFinderByMSNApplyResultHandler(toApply));

	Statement sql_filter;
	sql_filter     << "SELECT guest FROM relation_" << (id % 100)
		<< " WHERE host=" << id;

	ostringstream pattern;
	pattern << "relation_" << (id % 100);

	QueryRunner("relation_buddycache", CDbRServer, pattern.str()).query(
			sql_filter, FriendResultHandler( id,fbm,toApply ) );
	TaskManager::instance().execute( new AutoApplyTask(id,fbm,toApply) );
	return fbm;
}

bool FriendFinderByMSNDataResultHandler::handle(mysqlpp::Row& row) const
{
        int guestId = (int) row["guest"];
        int type = (int) row["type"];
	if( _hostId == guestId ){
		return true;
	}
        _result->set(guestId,type);
	return true;
}

bool FriendFinderByMSNApplyResultHandler::handle(mysqlpp::Row& row) const
{
        int hostId = (int) row["host"];
        int type = (int) row["type"];
	if( type < 10 ){
        	_apply.push_back( hostId );
	}
	return true;
}

bool FriendResultHandler::handle(mysqlpp::Row& row) const
{
        int guestId = (int) row["guest"];
        _obj->removePUA(_id,guestId);
        _obj->removePUK(_id,guestId);
	MyUtil::IntSeq::iterator it = find( _apply.begin(),_apply.end(),guestId );
	if( it != _apply.end() ){
		_apply.erase( it );
	}
	return true;
}

////***************************************************************************


MSNdata FriendFinderByMSNManagerI::getMSNdata(::Ice::Int id,
		const Ice::Current& current){
	MSNdata res = locateFriendFinderByMSNData(id)->getMSNdata();
	MCE_INFO("[FriendFinderByMSNManagerI::getMSNdata] id=" << id << " pua.type.size=" << res.puamap.size() << " puk.type.size=" << res.pukmap.size()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

MSNmap FriendFinderByMSNManagerI::getPUAmap(::Ice::Int id,
		const Ice::Current& current){
	MSNmap res = locateFriendFinderByMSNData(id)->getPUAmap();
	MCE_INFO("[FriendFinderByMSNManagerI::getPUAmap] id=" << id << " pua.type.size=" << res.size()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}
MyUtil::IntSeq FriendFinderByMSNManagerI::getPUAids(::Ice::Int id,
		const Ice::Current& current){
	MyUtil::IntSeq res = locateFriendFinderByMSNData(id)->getPUAids();
	MCE_INFO("[FriendFinderByMSNManagerI::getPUAids] id=" << id << " pua.ids.size=" << res.size()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}
MyUtil::IntSeq FriendFinderByMSNManagerI::getPUAidsWithMask(::Ice::Int id,const MyUtil::IntSeq& mask,
		const Ice::Current& current){
	MyUtil::IntSeq res = locateFriendFinderByMSNData(id)->getPUAidsWithMask(mask);
	MCE_INFO("[FriendFinderByMSNManagerI::getPUAidsWithMask] id=" << id << " puk.ids.size=" << res.size()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

MSNmap FriendFinderByMSNManagerI::getPUKmap(::Ice::Int id,
		const Ice::Current& current){
	MSNmap res = locateFriendFinderByMSNData(id)->getPUKmap();
	MCE_INFO("[FriendFinderByMSNManagerI::getPUKmap] id=" << id << " puk.type.size=" << res.size()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}
MyUtil::IntSeq FriendFinderByMSNManagerI::getPUKids(::Ice::Int id,
		const Ice::Current& current){
	MyUtil::IntSeq res = locateFriendFinderByMSNData(id)->getPUKids();
	MCE_INFO("[FriendFinderByMSNManagerI::getPUKids] id=" << id << " puk.ids.size=" << res.size()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}
MyUtil::IntSeq FriendFinderByMSNManagerI::getPUKidsRandom(::Ice::Int id,::Ice::Int limit,
		const Ice::Current& current){
	MyUtil::IntSeq res = locateFriendFinderByMSNData(id)->getPUKidsRandom(limit);
	MCE_INFO("[FriendFinderByMSNManagerI::getPUKidsRandom] id=" << id << " puk.ids.size=" << res.size() << " limit=" << limit
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}
MyUtil::IntSeq FriendFinderByMSNManagerI::getPUKidsWithMask(::Ice::Int id,const MyUtil::IntSeq& mask,
		const Ice::Current& current){
	MyUtil::IntSeq res = locateFriendFinderByMSNData(id)->getPUKidsWithMask(mask);
	MCE_INFO("[FriendFinderByMSNManagerI::getPUKidsWithMask] id=" << id << " puk.ids.size=" << res.size()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

void FriendFinderByMSNManagerI::add(::Ice::Int hostId,::Ice::Int guestId,::Ice::Int type,
		const Ice::Current& current){
	MCE_INFO("[FriendFinderByMSNManagerI::add] hostId=" << hostId << " guestId=" << guestId << " type=" << type
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	findFriendFinderByMSNData(hostId)->add(hostId,guestId,type);
}

void FriendFinderByMSNManagerI::remove(::Ice::Int hostId,::Ice::Int guestId,
		const Ice::Current& current){
	FriendFinderByMSNDataPtr obj = findFriendFinderByMSNData(hostId);
	obj->removePUA(hostId,guestId);
	obj->removePUK(hostId,guestId);
}
void FriendFinderByMSNManagerI::removePUA(::Ice::Int hostId,::Ice::Int guestId,
		const Ice::Current& current){
	locateFriendFinderByMSNData(hostId)->removePUA(hostId,guestId);
}
void FriendFinderByMSNManagerI::removePUK(::Ice::Int hostId,::Ice::Int guestId,
		const Ice::Current& current){
	locateFriendFinderByMSNData(hostId)->removePUK(hostId,guestId);
}

void FriendFinderByMSNManagerI::reload(::Ice::Int id,
		const Ice::Current& current){
	MCE_INFO("[FriendFinderByMSNManagerI::reload] id=" << id
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
        ServiceI::instance().reloadObject(FRIENDFINDER_BYMSN_DATA, id);
}

FriendFinderByMSNDataPtr FriendFinderByMSNManagerI::locateFriendFinderByMSNData(::Ice::Int id) {
	FriendFinderByMSNDataPtr obj = ServiceI::instance().locateObject<FriendFinderByMSNDataPtr> (FRIENDFINDER_BYMSN_DATA, id);
	return obj;
}

FriendFinderByMSNDataPtr FriendFinderByMSNManagerI::findFriendFinderByMSNData(::Ice::Int id) {
	FriendFinderByMSNDataPtr obj = ServiceI::instance().findObject<FriendFinderByMSNDataPtr> (FRIENDFINDER_BYMSN_DATA, id);
	if( obj ){
		return obj;
	}
	return new FriendFinderByMSNData;
}

////***************************************************************************

void AddToDBTask::handle() {
        Statement sqlw;
        sqlw    << "INSERT INTO xn_puk (host,guest,type)"
                << " VALUES (" << _hostId << "," << _guestId << "," << _type << ")"
		<< " ON DUPLICATE KEY UPDATE " << "host=VALUES(host), guest=VALUES(guest), type=VALUES(type);";
        QueryRunner("buddygroup", CDbWServer).execute(sqlw);
}

void RemoveFromDBTask::handle() {
        Statement sqlw;
        sqlw    << "REMOVE FROM xn_puk"
                << " WHERE host=" << _hostId << " AND guest=" << _guestId;
        QueryRunner("buddygroup", CDbWServer).execute(sqlw);
}

void AutoApplyTask::handle() {
	MyUtil::IntSeq applies = _obj->getPUAids();
	for( MyUtil::IntSeq::const_iterator it=applies.begin(); it!=applies.end(); ++it ){
		Applyship apply;
		apply.applicant = _hostId;
		apply.accepter = *it;
		BuddyCoreAdapter::instance().addApply( apply, ApplyInfo(), Ice::Context() );
		MCE_INFO("[AutoApplyTask::handle] hostId=" << _hostId << " guestId=" << *it );
	}
	for( MyUtil::IntSeq::const_iterator it=_apply.begin(); it!=_apply.end(); ++it ){
		Applyship apply;
		apply.applicant = *it;
		apply.accepter = _hostId;
		BuddyCoreAdapter::instance().addApply( apply, ApplyInfo(), Ice::Context() );
		TaskManager::instance().execute( new RemoveFromDBTask(*it,_hostId) );
		MCE_INFO("[AutoApplyTask::handle] hostId=" << *it << " guestId=" << _hostId );
	}
}

////***************************************************************************

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
        TaskManager::instance().config(props, "AddToDB", TASK_LEVEL_ADD, ThreadPoolConfig(1,1));
        TaskManager::instance().config(props, "RemoveFromDB", TASK_LEVEL_REMOVE, ThreadPoolConfig(1,1));
        TaskManager::instance().config(props, "AutoApply", TASK_LEVEL_AUTO_APPLY, ThreadPoolConfig(1,1));
}


