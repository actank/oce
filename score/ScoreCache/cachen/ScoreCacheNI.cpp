#include "ScoreCacheNI.h"
#include "ServiceI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"

using namespace std;
using namespace MyUtil;
using namespace mysqlpp;
using namespace com::xiaonei::xce;
using namespace xce::scorecache;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(SCORE_CACHE_DATA_N, "SCD", new ScoreCacheNFactory);
	service.getAdapter()->add(&ScoreCacheNManagerI::instance(),
			service.createIdentity("M", ""));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("ScoreCacheN.Mod");
        string endpoint = service.getCommunicator()->getProperties()->getProperty("ScoreCacheN.Endpoint");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ScoreCacheN.Interval", 5);
        ScoreCacheNManagerI::instance().bootstrap(endpoint, mod);

        // 向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerScoreCacheN",
                        &ScoreCacheNManagerI::instance(), mod, interval);

	TaskManager::instance().config(TASK_LEVEL_SCORE_CACHE_FILL, ThreadPoolConfig(1,100));
	TaskManager::instance().config(TASK_LEVEL_SCORE_CACHE_LOAD, ThreadPoolConfig(1,100));
	service.registerXceObserver(new ServiceSchemaI);
}

/*****************************************************************************/

Ice::ObjectPtr ScoreCacheNFactory::create(::Ice::Int userId) {
	return new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( ScoreCacheDataNFactory::instance().create(userId) ) );
}

/*****************************************************************************/

void ScoreCacheNManagerI::bootstrap(const string& endpoint, int mod) {
	_endpoint = endpoint;
	_mod = mod;
	MCE_INFO( "[bootstrap] endpoint=" << _endpoint << " mod=" << _mod );
}

ScoreDataN ScoreCacheNManagerI::getScoreDataN(Ice::Int userId,
		const Ice::Current& current){

	struct timeval tvStart, tvEnd;
	gettimeofday( &tvStart, NULL );
	long linStart = tvStart.tv_sec*1000+tvStart.tv_usec/1000;

	ScoreCacheDataNIPtr obj = findScoreCacheDataNI(userId);
	if (!obj ) {
		obj = ScoreCacheDataNIPtr::dynamicCast( ScoreCacheNFactory::instance().create(userId) );
	}
	ScoreDataN res = obj->getScoreDataN(userId,_mask);

	gettimeofday( &tvEnd, NULL );	
	long linEnd = tvEnd.tv_sec*1000+tvEnd.tv_usec/1000;
	long totalMS = linEnd-linStart;

	if( totalMS > 0 ){
		ostringstream log;
		log << "[getScoreDataN][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId
			<< " TOTAL " << totalMS << " ms";
		MCE_INFO( log.str() );
	}

	return res;
}

ScoreDataNMap ScoreCacheNManagerI::getScoreDataNMap(const MyUtil::IntSeq& userIds,
		const Ice::Current& current){

	struct timeval tvStart, tvEnd;
	gettimeofday( &tvStart, NULL );
	long linStart = tvStart.tv_sec*1000+tvStart.tv_usec/1000;

	ScoreDataNMap res;
	for(MyUtil::IntSeq::const_iterator it = userIds.begin();it!=userIds.end();++it) {
		ScoreCacheDataNIPtr obj = findScoreCacheDataNI(*it);
		if (!obj ) {
			obj = ScoreCacheDataNIPtr::dynamicCast( ScoreCacheNFactory::instance().create(*it) );
		}
		res.insert( make_pair( *it, obj->getScoreDataN(*it,_mask) ) );
	}

	gettimeofday( &tvEnd, NULL );	
	long linEnd = tvEnd.tv_sec*1000+tvEnd.tv_usec/1000;
	long totalMS = linEnd-linStart;

	if( totalMS > 0 ){
		ostringstream log;
		log << "[getScoreDataNMap][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userIds.size=" << userIds.size()
			<< " TOTAL " << totalMS << " ms";
		MCE_INFO( log.str() );
	}

	return res;
}

ScoreDataN ScoreCacheNManagerI::getScoreDataNNotNull(Ice::Int userId,
		const Ice::Current& current){

	struct timeval tvStart, tvEnd;
	gettimeofday( &tvStart, NULL );
	long linStart = tvStart.tv_sec*1000+tvStart.tv_usec/1000;

	ScoreCacheDataNIPtr obj = findScoreCacheDataNI(userId);
	if (!obj ) {
		obj = ScoreCacheDataNIPtr::dynamicCast( ScoreCacheNFactory::instance().create(userId) );
	}
	ScoreDataN res = obj->getScoreDataN(userId,_mask);

	gettimeofday( &tvEnd, NULL );	
	long linEnd = tvEnd.tv_sec*1000+tvEnd.tv_usec/1000;
	long totalMS = linEnd-linStart;

	if( totalMS > 0 ){
		ostringstream log;
		log << "[getScoreDataNNotNull][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId
			<< " TOTAL " << totalMS << " ms";
		MCE_INFO( log.str() );
	}

	return res;
}

ScoreDataNMap ScoreCacheNManagerI::getScoreDataNMapNotNull(const MyUtil::IntSeq& userIds,
		const Ice::Current& current){

	struct timeval tvStart, tvEnd;
	gettimeofday( &tvStart, NULL );
	long linStart = tvStart.tv_sec*1000+tvStart.tv_usec/1000;

	ScoreDataNMap res;
	for(MyUtil::IntSeq::const_iterator it = userIds.begin();it!=userIds.end();++it) {
		ScoreCacheDataNIPtr obj = findScoreCacheDataNI(*it);
		if (!obj ) {
			obj = ScoreCacheDataNIPtr::dynamicCast( ScoreCacheNFactory::instance().create(*it) );
		}
		res.insert( make_pair( *it, obj->getScoreDataN(*it,_mask) ) );
	}

	gettimeofday( &tvEnd, NULL );	
	long linEnd = tvEnd.tv_sec*1000+tvEnd.tv_usec/1000;
	long totalMS = linEnd-linStart;

	if( totalMS > 0 ){
		ostringstream log;
		log << "[getScoreDataNMapNotNull][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userIds.size=" << userIds.size()
			<< " TOTAL " << totalMS << " ms";
		MCE_INFO( log.str() );
	}

	return res;
}

Ice::ObjectSeq ScoreCacheNManagerI::setLoginDays( Ice::Int userId, Ice::Int historyLoginDays, Ice::Int continueLoginDays, Ice::Long lastLoginTime, Ice::Int loginType,
		const Ice::Current& current){
	Ice::ObjectSeq res;
	ostringstream log;
	log << "[setLoginDays][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId;
	ScoreCacheDataNIPtr data = findScoreCacheDataNI(userId);
	if (!data) {
		log << " absend!";
		MCE_INFO( log.str() );
		return res;
	}
	ScoreCacheDataNPtr old_data = data->getScoreCacheDataN();
	int abs_count = data->setLogin( historyLoginDays, continueLoginDays, lastLoginTime, loginType );
	log << " hisday(" << old_data->historyLoginDays << "->" << data->historyLoginDays << ")";
	log << " conday(" << old_data->continueLoginDays << "->" << data->continueLoginDays << ")";
	log << " actday." << abs_count << "(" << old_data->activeDays << "->" << data->activeDays << ")";
	log << " time(" << old_data->lastLoginTime << "->" << data->lastLoginTime << ")";
	log << " type(" << old_data->loginType << "->" << data->loginType << ")";
	res.push_back( data->getScoreCacheDataN() );
	res.push_back( old_data );
	MCE_INFO( log.str() );
	return res;
}
Ice::ObjectSeq ScoreCacheNManagerI::updateLoginType(Ice::Int userId, Ice::Int loginType,
		const Ice::Current& current){
	Ice::ObjectSeq res;
	ostringstream log;
	log << "[updateLoginType][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId;
	ScoreCacheDataNIPtr data = findScoreCacheDataNI(userId);
	if (!data) {
		log << " absend!";
		MCE_INFO( log.str() );
		return res;
	}
	ScoreCacheDataNPtr old_data = data->getScoreCacheDataN();
	data->updateLoginType( loginType );
	log << " loginType(" << old_data->loginType << "->" << data->loginType << ")";
	res.push_back( data->getScoreCacheDataN() );
	res.push_back( old_data );
	MCE_INFO( log.str() );
	return res;
}

Ice::ObjectSeq ScoreCacheNManagerI::setScore(Ice::Int userId, Ice::Int score,
		const Ice::Current& current){
	Ice::ObjectSeq res;
	ostringstream log;
	log << "[setScore][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId;
	ScoreCacheDataNIPtr data = findScoreCacheDataNI(userId);
	if (!data) {
		log << " absend!";
		MCE_INFO( log.str() );
		return res;
	}
	ScoreCacheDataNPtr old_data = data->getScoreCacheDataN();
	data->setScore( score );
	log << " score(" << old_data->score << "->" << data->score << ")";
	res.push_back( data->getScoreCacheDataN() );
	res.push_back( old_data );
	MCE_INFO( log.str() );
	return res;
}
Ice::ObjectSeq ScoreCacheNManagerI::incScore(Ice::Int userId, Ice::Int inc,
		const Ice::Current& current){
	Ice::ObjectSeq res;
	ostringstream log;
	log << "[incScore][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId;
	ScoreCacheDataNIPtr data = findScoreCacheDataNI(userId);
	if (!data) {
		log << " absend!";
		MCE_INFO( log.str() );
		return res;
	}
	ScoreCacheDataNPtr old_data = data->getScoreCacheDataN();
	data->incScore( inc );
	log << " score+" << inc << "(" << old_data->score << "->" << data->score << ")";
	res.push_back( data->getScoreCacheDataN() );
	res.push_back( old_data );
	MCE_INFO( log.str() );
	return res;
}
Ice::ObjectSeq ScoreCacheNManagerI::decScore(Ice::Int userId, Ice::Int dec,
		const Ice::Current& current){
	Ice::ObjectSeq res;
	ostringstream log;
	log << "[decScore][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId;
	ScoreCacheDataNIPtr data = findScoreCacheDataNI(userId);
	if (!data) {
		log << " absend!";
		MCE_INFO( log.str() );
		return res;
	}
	ScoreCacheDataNPtr old_data = data->getScoreCacheDataN();
	data->decScore( dec );
	log << " score-" << dec << "(" << old_data->score << "->" << data->score << ")";
	res.push_back( data->getScoreCacheDataN() );
	res.push_back( old_data );
	MCE_INFO( log.str() );
	return res;
}

Ice::ObjectSeq ScoreCacheNManagerI::setAwardCount(Ice::Int userId, Ice::Int awardCount,
		const Ice::Current& current){
	Ice::ObjectSeq res;
	ostringstream log;
	log << "[setAwardCount][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId;
	ScoreCacheDataNIPtr data = findScoreCacheDataNI(userId);
	if (!data) {
		log << " absend!";
		MCE_INFO( log.str() );
		return res;
	}
	ScoreCacheDataNPtr old_data = data->getScoreCacheDataN();
	data->setAwardCount( awardCount );
	log << " awardCount(" << old_data->awardCount << "->" << data->awardCount << ")";
	res.push_back( data->getScoreCacheDataN() );
	res.push_back( old_data );
	MCE_INFO( log.str() );
	return res;
}
Ice::ObjectSeq ScoreCacheNManagerI::incAwardCount(Ice::Int userId, Ice::Int inc,
		const Ice::Current& current){
	Ice::ObjectSeq res;
	ostringstream log;
	log << "[incAwardCount][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId;
	ScoreCacheDataNIPtr data = findScoreCacheDataNI(userId);
	if (!data) {
		log << " absend!";
		MCE_INFO( log.str() );
		return res;
	}
	ScoreCacheDataNPtr old_data = data->getScoreCacheDataN();
	data->incAwardCount( inc );
	log << " awardCount+" << inc << "(" << old_data->awardCount << "->" << data->awardCount << ")";
	res.push_back( data->getScoreCacheDataN() );
	res.push_back( old_data );
	MCE_INFO( log.str() );
	return res;
}
Ice::ObjectSeq ScoreCacheNManagerI::decAwardCount(Ice::Int userId, Ice::Int dec,
		const Ice::Current& current){
	Ice::ObjectSeq res;
	ostringstream log;
	log << "[decAwardCount][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId;
	ScoreCacheDataNIPtr data = findScoreCacheDataNI(userId);
	if (!data) {
		log << " absend!";
		MCE_INFO( log.str() );
		return res;
	}
	ScoreCacheDataNPtr old_data = data->getScoreCacheDataN();
	data->decAwardCount( dec );
	log << " awardCount-" << dec << "(" << old_data->awardCount << "->" << data->awardCount << ")";
	res.push_back( data->getScoreCacheDataN() );
	res.push_back( old_data );
	MCE_INFO( log.str() );
	return res;
}

Ice::ObjectSeq ScoreCacheNManagerI::setOn(Ice::Int userId, Ice::Int pos,
		const Ice::Current& current){
	Ice::ObjectSeq res;
	ostringstream log;
	log << "[setOn][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId;
	ScoreCacheDataNIPtr data = findScoreCacheDataNI(userId);
	if (!data) {
		log << " absend!";
		MCE_INFO( log.str() );
		return res;
	}
	log << " sign." << pos << "(" << data->getSign(pos);
	ScoreCacheDataNPtr old_data = data->getScoreCacheDataN();
	data->setOn( pos );
	log << "->" << data->getSign(pos) << ")";
	res.push_back( data->getScoreCacheDataN() );
	res.push_back( old_data );
	MCE_INFO( log.str() );
	return res;
}
Ice::ObjectSeq ScoreCacheNManagerI::setOff(Ice::Int userId, Ice::Int pos,
		const Ice::Current& current){
	Ice::ObjectSeq res;
	ostringstream log;
	log << "[setOff][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId;
	ScoreCacheDataNIPtr data = findScoreCacheDataNI(userId);
	if (!data) {
		log << " absend!";
		MCE_INFO( log.str() );
		return res;
	}
	log << " sign." << pos << "(" << data->getSign(pos);
	ScoreCacheDataNPtr old_data = data->getScoreCacheDataN();
	data->setOff( pos );
	log << "->" << data->getSign(pos) << ")";
	res.push_back( data->getScoreCacheDataN() );
	res.push_back( old_data );
	MCE_INFO( log.str() );
	return res;
}

Ice::ObjectSeq ScoreCacheNManagerI::updateVipState(Ice::Int userId, Ice::Int state,
		const Ice::Current& current){
	Ice::ObjectSeq res;
	ostringstream log;
	log << "[updateVipState][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId;
	ScoreCacheDataNIPtr data = findScoreCacheDataNI(userId);
	if (!data) {
		log << " absend!";
		MCE_INFO( log.str() );
		return res;
	}
	ScoreCacheDataNPtr old_data = data->getScoreCacheDataN();
	data->updateVipState( state );
	log << " vip(" << old_data->vip << "->" << data->vip << ")";
	res.push_back( data->getScoreCacheDataN() );
	res.push_back( old_data );
	MCE_INFO( log.str() );
	return res;
}

Ice::ObjectSeq ScoreCacheNManagerI::updateVipLevel(Ice::Int userId, Ice::Int level,
		const Ice::Current& current){
	Ice::ObjectSeq res;
	ostringstream log;
	log << "[updateVipLevel][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId;
	ScoreCacheDataNIPtr data = findScoreCacheDataNI(userId);
	if (!data) {
		log << " absend!";
		MCE_INFO( log.str() );
		return res;
	}
	ScoreCacheDataNPtr old_data = data->getScoreCacheDataN();
	data->updateVipLevel( level );
	log << " vip(" << old_data->vip << "->" << data->vip << ")";
	res.push_back( data->getScoreCacheDataN() );
	res.push_back( old_data );
	MCE_INFO( log.str() );
	return res;
}

void ScoreCacheNManagerI::load(Ice::Int userId,const Ice::ObjectPtr& o,
		const Ice::Current& current){
	TaskManager::instance().execute( new LoadTask( userId, o ) );
}

/*****************************************************************************/

void ScoreCacheNManagerI::setMask(Ice::Long mask,
		const Ice::Current& current){
	MCE_INFO( "[setMask] " << mask );
	IceUtil::RWRecMutex::WLock lock(_maskMutex);
	_mask = mask;
}

bool ScoreCacheNManagerI::isValid(const Ice::Current&) {
	MCE_INFO( "[isValid] " << _isValid );
	IceUtil::RWRecMutex::RLock lock(_validMutex);
	return _isValid;
}

void ScoreCacheNManagerI::setValid(bool valid,
		const Ice::Current&) {
	MCE_INFO( "[setValid] " << valid );
	IceUtil::RWRecMutex::WLock lock(_validMutex);
	_isValid = valid;
	if( valid ){
		ServerStateSubscriber::instance().setStatus(1);
	} else{
		ServerStateSubscriber::instance().setStatus(0);
	}
}

void ScoreCacheNManagerI::setData(const ::MyUtil::ObjectResultPtr& scoreDataN,
		const Ice::Current& current) {
	TaskManager::instance().execute(new FillTask(scoreDataN));
}

ScoreCacheDataNIPtr ScoreCacheNManagerI::findScoreCacheDataNI(::Ice::Int userId) {
	return ServiceI::instance().findObject<ScoreCacheDataNIPtr>(
			SCORE_CACHE_DATA_N, userId);
}

ScoreCacheDataNIPtr ScoreCacheNManagerI::locateScoreCacheDataNI(::Ice::Int userId) {
	return ServiceI::instance().locateObject<ScoreCacheDataNIPtr>(
			SCORE_CACHE_DATA_N, userId);
}

/************Tasks ***********************************************************/

void FillTask::handle() {
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
	for ( std::map<long, Ice::ObjectPtr>::const_iterator it = _scoreDataN->data.begin();
			it!= _scoreDataN->data.end(); ++it ){
		try{
			ScoreCacheDataNIPtr obj = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( it->second ) );
			res->data.insert( make_pair<long,Ice::ObjectPtr>(it->first,obj) );
		} catch( Ice::Exception e ){
			MCE_WARN( "FillTask " << it->first << " found Ice::Exception : " << e.what() );
			continue;
		} catch( ... ){
			MCE_WARN( "FillTask " << it->first << " found unknown exception ." );
			continue;
		}
	}

	ServiceI::instance().getObjectCache()->addObjects(SCORE_CACHE_DATA_N,res->data);
	MCE_INFO("FillTask done. task_size=" << _scoreDataN->data.size() << " objectcache_size=" << ServiceI::instance().getObjectCache()->locateCategory(SCORE_CACHE_DATA_N)->size());
}

void LoadTask::handle() {
	if (_obj) {
		try {
			ScoreCacheDataNIPtr obj = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( _obj ) );
			ServiceI::instance().getObjectCache()->addObject(SCORE_CACHE_DATA_N,_id,obj);
			MCE_INFO( "[LoadTask] userId=" << _id << " done." << " objectcache_size=" << ServiceI::instance().getObjectCache()->locateCategory(SCORE_CACHE_DATA_N)->size() );
		} catch( Ice::Exception e ){
			MCE_WARN("[LoadTask] userId=" << _id << " found Ice::Exception : " << e.what() );
		} catch( ... ){
			MCE_WARN("[LoadTask] userId=" << _id << " found unknown exception." );
		}
	}
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	TaskManager::instance().config(props, "Fill", TASK_LEVEL_SCORE_CACHE_FILL, ThreadPoolConfig(1,100));
	TaskManager::instance().config(props, "Load", TASK_LEVEL_SCORE_CACHE_LOAD, ThreadPoolConfig(1,100));
}

