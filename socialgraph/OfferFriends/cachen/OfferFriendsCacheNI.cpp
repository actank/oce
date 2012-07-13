#include "OfferFriendsCacheNI.h"

#include "ServiceI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include "OfferFriendsAdapter.h"

using namespace std;
using namespace MyUtil;
using namespace mysqlpp;
using namespace com::xiaonei::xce;
using namespace xce::offerfriends;
using namespace xce::offerfriends::adapter;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(OFFER_CACHE_DATA_I, "OCD", new OfferFriendsCacheNFactory );
	service.registerObjectCache(HIGHSCHOOL_CACHE_DATA_I, "HCD", new HighSchoolCacheNFactory);
	service.getAdapter()->add(&OfferFriendsCacheNManagerI::instance(), service.createIdentity("M", ""));

  string endpoint = service.getCommunicator()->getProperties()->getProperty("OfferFriendsCacheN.Endpoint");
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("OfferFriendsCacheN.Mod");
	OfferFriendsCacheNManagerI::instance().bootstrap(endpoint, mod);

  string ctlEndpoints = service.getCommunicator()->getProperties()->getPropertyWithDefault("Controller.Endpoint","ControllerOfferFriendsCacheN");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Controller.Interval",5);
  ServerStateSubscriber::instance().initialize(ctlEndpoints, &OfferFriendsCacheNManagerI::instance(), mod, interval);

  TaskManager::instance().config(TASK_LEVEL_FILL, ThreadPoolConfig(1,10));
  TaskManager::instance().config(TASK_LEVEL_LOAD, ThreadPoolConfig(1,10));
	TaskManager::instance().config(TASK_LEVEL_REPORT, ThreadPoolConfig(1,100));
  service.registerXceObserver(new ServiceSchemaI);
}

/*****************************************************************************/

Ice::ObjectPtr OfferFriendsCacheNFactory::create(::Ice::Int userId) 
{
	MCE_INFO("OfferFriendsCacheNFactory create userId :" << userId);
	OfferCacheDataNPtr obj = OfferCacheDataNPtr::dynamicCast(OfferCacheDataFactory::instance().create(userId));
	if (obj == 0) {
		ostringstream log;
		obj = OfferCreator::instance().create(userId, 50, log);
	}
	return new OfferCacheDataI(obj);
}

/*****************************************************************************/

Ice::ObjectPtr HighSchoolCacheNFactory::create(::Ice::Int userId) {
	OfferCacheDataNPtr obj = OfferCacheDataNPtr::dynamicCast(OfferCacheDataFactory::instance().create(userId));
	if (obj == 0) {
		ostringstream log;
		//obj = OfferCreator::instance().createHighSchoolData(userId, 50, log);
		obj = OfferCreator::instance().createHighSchoolData(userId, 500, log);
		MCE_INFO("[HighSchoolCacheNFactory] create userId : " << userId << " " << log.str());
	}

	return new OfferCacheDataI(obj);
}

/*****************************************************************************/

void OfferFriendsCacheNManagerI::bootstrap(const string& endpoint, 
							 														 int mod)
{
	MCE_INFO( "[bootstrap] endpoint=" << endpoint << " mod=" << mod );
	_endpoint = endpoint;
	_mod = mod;
}

/*****************************************************************************/

OfferDataNSeq OfferFriendsCacheNManagerI::getFastOffer(::Ice::Int userId, 
																											 ::Ice::Int limit,
																											 const Ice::Current& current) 
{
	ostringstream getFastOfferLog;
  getFastOfferLog << "[getFastOffer][" << current.con->toString().substr(current.con->toString().find("remote", 25)+17) << "]";
  TimeCost tc = TimeCost::create(getFastOfferLog.str(),LOG_LEVEL_INFO);

	OfferDataNSeq res;

	GetInfo info;
	info.userId = userId;
	info.hit = false;
	info.limit = limit;
	info.bestRes = 0;
	info.bestCount = 0;
	info.onlineRes = 0;
	info.offerRes = 0;
	info.offerCount = 0;
	info.timeout = 0;

	OfferCacheDataIPtr obj = locateOfferCacheDataI(userId);
	res = obj->getFast(info,limit);

	ostringstream procLog;
	procLog << "userId=" << userId
		<< " hit=" << info.hit
		<< " limit=" << info.limit
		<< " res=" << info.bestRes+info.onlineRes+info.offerRes << "=" << info.bestRes << "/" << info.bestCount
		<< "+(" << info.onlineRes << "+" << info.offerRes << ")" << "/" << info.offerCount
		<< " timeout=" << info.timeout << "(" << (info.timeout/3600) << "h" << ((info.timeout%3600)/60) << "m)";

	TaskManager::instance().execute( new ReportTask( info ) );
	tc.step( procLog.str(), true );
	return res;
}

OfferDataNSeq OfferFriendsCacheNManagerI::getBestOffer(::Ice::Int userId, 
																											 ::Ice::Int limit,
																											 const Ice::Current& current) 
{
	ostringstream getBestOfferLog;
  getBestOfferLog << "[getBestOffer][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "]";
  TimeCost tc = TimeCost::create(getBestOfferLog.str(),LOG_LEVEL_INFO);

	OfferDataNSeq res;

	GetInfo info;
	info.userId = userId;
	info.hit = false;
	info.limit = limit;
	info.bestRes = 0;
	info.bestCount = 0;
	info.onlineRes = 0;
	info.offerRes = 0;
	info.offerCount = 0;
	info.timeout = 0;

	OfferCacheDataIPtr obj = locateOfferCacheDataI(userId);
	MyUtil::IntSeq ids = obj->getIds();
	map<int,bool> onlinestates = OfferFriendsUtil::getOnlineStateBatch( ids );
	res = obj->getBest(info,onlinestates,limit);

	ostringstream procLog;
	procLog << "userId=" << userId
		<< " hit=" << info.hit
		<< " limit=" << info.limit
		<< " res=" << info.bestRes+info.onlineRes+info.offerRes << "=" << info.bestRes << "/" << info.bestCount
		<< "+(" << info.onlineRes << "+" << info.offerRes << ")" << "/" << info.offerCount
		<< " timeout=" << info.timeout << "(" << (info.timeout/3600) << "h" << ((info.timeout%3600)/60) << "m)";

	TaskManager::instance().execute( new ReportTask( info ) );
	tc.step( procLog.str(), true );
	return res;
}

::xce::offerfriends::OfferDataNSeq OfferFriendsCacheNManagerI::getBestOfferByStage(::Ice::Int stage,
																																 									 ::Ice::Int userId,
																																 									 ::Ice::Int limit,
																																 									 const Ice::Current& current)
{
	MCE_INFO("[OfferFriendsCacheNManagerI] getBestOfferByStage stage:" << stage << " userId:" << userId << " limit:" << limit); 

	if (HIGHSCHOOL == stage) {
		return getHighSchoolRecommend(userId, limit);
	}

	ostringstream getBestOfferLog;
  getBestOfferLog << "[getBestOfferByStage][" 
		<< current.con->toString().substr(current.con->toString().find("remote",25)+17) << "]";
  TimeCost tc = TimeCost::create(getBestOfferLog.str(),LOG_LEVEL_INFO);

	OfferDataNSeq res;

	GetInfo info;
	info.userId = userId;
	info.hit = false;
	info.limit = limit;
	info.bestRes = 0;
	info.bestCount = 0;
	info.onlineRes = 0;
	info.offerRes = 0;
	info.offerCount = 0;
	info.timeout = 0;

	OfferCacheDataIPtr obj = locateOfferCacheDataI(userId);
	MyUtil::IntSeq ids = obj->getIds();
	map<int,bool> onlinestates = OfferFriendsUtil::getOnlineStateBatch( ids );
	res = obj->getBest(info,onlinestates,limit);

	ostringstream procLog;
	procLog << "userId=" << userId
		<< " hit=" << info.hit
		<< " limit=" << info.limit
		<< " res=" << info.bestRes+info.onlineRes+info.offerRes << "=" << info.bestRes << "/" << info.bestCount
		<< "+(" << info.onlineRes << "+" << info.offerRes << ")" << "/" << info.offerCount
		<< " timeout=" << info.timeout << "(" << (info.timeout/3600) << "h" << ((info.timeout%3600)/60) << "m)";

	TaskManager::instance().execute( new ReportTask( info ) );
	tc.step( procLog.str(), true );
	return res;
}

OfferDataNSeq OfferFriendsCacheNManagerI::getRandomOffer(::Ice::Int userId, 
																												 ::Ice::Int limit,
																												 const Ice::Current& current) 
{
	ostringstream getRandomOfferLog;
  getRandomOfferLog << "[getRandomOffer][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "]";
  TimeCost tc = TimeCost::create(getRandomOfferLog.str(),LOG_LEVEL_INFO);

	OfferDataNSeq res;

	GetInfo info;
	info.userId = userId;
	info.hit = false;
	info.limit = limit;
	info.bestRes = 0;
	info.bestCount = 0;
	info.onlineRes = 0;
	info.offerRes = 0;
	info.offerCount = 0;
	info.timeout = 0;

	OfferCacheDataIPtr obj = locateOfferCacheDataI(userId);
	res = obj->getRandom(info,limit);

	ostringstream procLog;
	procLog << "userId=" << userId
		<< " hit=" << info.hit
		<< " limit=" << info.limit
		<< " res=" << info.bestRes+info.onlineRes+info.offerRes << "=" << info.bestRes << "/" << info.bestCount
		<< "+(" << info.onlineRes << "+" << info.offerRes << ")" << "/" << info.offerCount
		<< " timeout=" << info.timeout << "(" << (info.timeout/3600) << "h" << ((info.timeout%3600)/60) << "m)";

	TaskManager::instance().execute( new ReportTask( info ) );
	tc.step( procLog.str(), true );
	return res;
}

RmInfo OfferFriendsCacheNManagerI::apply(::Ice::Int applicant, 
																				 ::Ice::Int acceptor, 
																				 const Ice::Current& current)
{
	ostringstream applyLog;
  applyLog << "[apply][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "]";

	RmInfo info;
	info.userId = applicant;
	info.hit = false;
	info.found = false;
	info.data = OfferDataN();
	info.data.userId = acceptor;
	info.data.weight = 0;
	info.data.sign = 0;
	info.bestCount = 0;
	info.offerCount = 0;
	info.timeout = 0;

	OfferCacheDataIPtr obj = findOfferCacheDataI(applicant);
	if (obj==0) {
		OfferCacheDataNPtr obj_db = OfferCacheDataNPtr::dynamicCast( OfferCacheDataFactory::instance().create(applicant) );
		if ( obj_db ) {
			obj = new OfferCacheDataI( obj_db );
                        ServiceI::instance().getObjectCache()->addObject(OFFER_CACHE_DATA_I,applicant,obj);
			applyLog << " objectcache_size=" << ServiceI::instance().getObjectCache()->locateCategory(OFFER_CACHE_DATA_I)->size();
		}
	}
	if (obj) {
		obj->remove(info,acceptor);
		applyLog << " applicant/acceptor=" << applicant << "/" << acceptor << " hit/found=" << info.hit << "/" << info.found
			<< " src=" << info.bestCount << "+" << info.offerCount
			<< " timeout=" << info.timeout << "(" << (info.timeout/3600) << "h" << ((info.timeout%3600)/60) << "m)";
	} else{
		applyLog << " applicant/acceptor=" << applicant << "/" << acceptor << " hit/found=" << info.hit << "/" << info.found;
	}
	MCE_INFO( applyLog.str() );
	return info;
}

RmInfo OfferFriendsCacheNManagerI::accept(::Ice::Int acceptor, 
																				  ::Ice::Int applicant, 
																					const Ice::Current& current)
{
	ostringstream acceptLog;
  acceptLog << "[accept][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "]";

	RmInfo info;
	info.userId = acceptor;
	info.hit = false;
	info.found = false;
	info.data = OfferDataN();
	info.data.userId = applicant;
	info.data.weight = 0;
	info.data.sign = 0;
	info.bestCount = 0;
	info.offerCount = 0;
	info.timeout = 0;

	OfferCacheDataIPtr obj = findOfferCacheDataI(acceptor);
	if (obj==0) {
		OfferCacheDataNPtr obj_db = OfferCacheDataNPtr::dynamicCast( OfferCacheDataFactory::instance().create(acceptor) );
		if ( obj_db ) {
			obj = new OfferCacheDataI( obj_db );
                        ServiceI::instance().getObjectCache()->addObject(OFFER_CACHE_DATA_I,acceptor,obj);
			acceptLog << " objectcache_size=" << ServiceI::instance().getObjectCache()->locateCategory(OFFER_CACHE_DATA_I)->size();
		}
	}
	if (obj) {
		obj->remove(info,applicant);
		acceptLog << " acceptor/applicant=" << acceptor << "/" << applicant << " hit/found=" << info.hit << "/" << info.found
			<< " src=" << info.bestCount << "+" << info.offerCount
			<< " timeout=" << info.timeout << "(" << (info.timeout/3600) << "h" << ((info.timeout%3600)/60) << "m)";
	} else{
		acceptLog << " acceptor/applicant=" << acceptor << "/" << applicant << " hit/found=" << info.hit << "/" << info.found;
	}
	MCE_INFO( acceptLog.str() );
	return info;
}

RmInfo OfferFriendsCacheNManagerI::denyOffer(::Ice::Int hostId, 
																						 ::Ice::Int guestId, 
																						 const Ice::Current& current)
{
	ostringstream denyOfferLog;
  denyOfferLog << "[denyOffer][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "]";

	RmInfo info;
	info.userId = hostId;
	info.hit = false;
	info.found = false;
	info.data = OfferDataN();
	info.data.userId = guestId;
	info.data.weight = 0;
	info.data.sign = 0;
	info.bestCount = 0;
	info.offerCount = 0;
	info.timeout = 0;

	OfferCacheDataIPtr obj = findOfferCacheDataI(hostId);
	if (obj==0) {
		OfferCacheDataNPtr obj_db = OfferCacheDataNPtr::dynamicCast( OfferCacheDataFactory::instance().create(hostId) );
		if ( obj_db ) {
			obj = new OfferCacheDataI( obj_db );
                        ServiceI::instance().getObjectCache()->addObject(OFFER_CACHE_DATA_I,hostId,obj);
			denyOfferLog << " objectcache_size=" << ServiceI::instance().getObjectCache()->locateCategory(OFFER_CACHE_DATA_I)->size();
		}
	}
	if (obj) {
		obj->remove(info,guestId);
		denyOfferLog << " hostId/guestId=" << hostId << "/" << guestId << " hit/found=" << info.hit << "/" << info.found
			<< " src=" << info.bestCount << "+" << info.offerCount
			<< " timeout=" << info.timeout << "(" << (info.timeout/3600) << "h" << ((info.timeout%3600)/60) << "m)";
	} else{
		denyOfferLog << " hostId/guestId=" << hostId << "/" << guestId << " hit/found=" << info.hit << "/" << info.found;
	}
	MCE_INFO( denyOfferLog.str() );
	return info;
}

RmInfo OfferFriendsCacheNManagerI::ignoreOffer(::Ice::Int hostId, 
																							 ::Ice::Int guestId, 
																							 const Ice::Current& current)
{
	ostringstream ignoreOfferLog;
        ignoreOfferLog << "[ignoreOffer][" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "]";

	RmInfo info;
	info.userId = hostId;
	info.hit = false;
	info.found = false;
	info.data = OfferDataN();
	info.data.userId = guestId;
	info.data.weight = 0;
	info.data.sign = 0;
	info.bestCount = 0;
	info.offerCount = 0;
	info.timeout = 0;

	OfferCacheDataIPtr obj = findOfferCacheDataI(hostId);
	if (obj==0) {
		OfferCacheDataNPtr obj_db = OfferCacheDataNPtr::dynamicCast( OfferCacheDataFactory::instance().create(hostId) );
		if ( obj_db ) {
			obj = new OfferCacheDataI( obj_db );
                        ServiceI::instance().getObjectCache()->addObject(OFFER_CACHE_DATA_I,hostId,obj);
			ignoreOfferLog << " objectcache_size=" << ServiceI::instance().getObjectCache()->locateCategory(OFFER_CACHE_DATA_I)->size();
		}
	}
	if (obj) {
		obj->remove(info,guestId);
		ignoreOfferLog << " hostId/guestId=" << hostId << "/" << guestId << " hit/found=" << info.hit << "/" << info.found
			<< " src=" << info.bestCount << "+" << info.offerCount
			<< " timeout=" << info.timeout << "(" << (info.timeout/3600) << "h" << ((info.timeout%3600)/60) << "m)";
	} else{
		ignoreOfferLog << " hostId/guestId=" << hostId << "/" << guestId << " hit/found=" << info.hit << "/" << info.found;
	}
	MCE_INFO( ignoreOfferLog.str() );
	return info;
}

void OfferFriendsCacheNManagerI::load(Ice::Int userId,
																			const Ice::ObjectPtr& o,
																			const Ice::Current& current)
{
	TaskManager::instance().execute( new LoadTask( userId, o ) );
}

bool OfferFriendsCacheNManagerI::isValid(const Ice::Current&) 
{
	MCE_INFO( "[isValid] " << _isValid );
	IceUtil::RWRecMutex::RLock lock(_validMutex);
	return _isValid;
}

void OfferFriendsCacheNManagerI::setValid(bool valid,
																					const Ice::Current&) 
{
	MCE_INFO( "[setValid] " << valid );
	IceUtil::RWRecMutex::WLock lock(_validMutex);
	_isValid = valid;
	if( valid ) {
		ServerStateSubscriber::instance().setStatus(1);
	} else {
		ServerStateSubscriber::instance().setStatus(0);
	}
}

void OfferFriendsCacheNManagerI::setData(const ::MyUtil::ObjectResultPtr& scoreData,
																				 const Ice::Current& current) 
{
	TaskManager::instance().execute(new FillTask(scoreData));
}

::xce::offerfriends::OfferDataNSeq OfferFriendsCacheNManagerI::getHighSchoolRecommend(int userId, 
																																											int limit) 
{
	MCE_INFO("[OfferFriendsCacheNManagerI] getHighSchoolRecommend userId(" << userId << ") limit(" << limit << ")");
	ostringstream getHighSchoolRecommendLog;
  getHighSchoolRecommendLog << "[getHighSchoolRecommend][";
  TimeCost tc = TimeCost::create(getHighSchoolRecommendLog.str(), LOG_LEVEL_INFO);

	OfferDataNSeq res;

	GetInfo info;
	info.userId = userId;
	info.hit = false;
	info.limit = limit;
	info.bestRes = 0;
	info.bestCount = 0;
	info.onlineRes = 0;
	info.offerRes = 0;
	info.offerCount = 0;
	info.timeout = 0;

	OfferCacheDataIPtr obj = locateHighSchoolCacheDataI(userId);
	MyUtil::IntSeq ids = obj->getIds();
	map<int,bool> onlinestates = OfferFriendsUtil::getOnlineStateBatch( ids );
	res = obj->getBest(info,onlinestates,limit);

	ostringstream procLog;
	procLog << "userId=" << userId
		<< " hit=" << info.hit
		<< " limit=" << info.limit
		<< " res=" << info.bestRes+info.onlineRes+info.offerRes << "=" << info.bestRes << "/" << info.bestCount
		<< "+(" << info.onlineRes << "+" << info.offerRes << ")" << "/" << info.offerCount
		<< " timeout=" << info.timeout << "(" << (info.timeout/3600) << "h" << ((info.timeout%3600)/60) << "m)";

	TaskManager::instance().execute( new ReportTask( info ) );
	tc.step( procLog.str(), true );
	return res;
}

OfferCacheDataIPtr OfferFriendsCacheNManagerI::locateHighSchoolCacheDataI(::Ice::Int userId) {
	return ServiceI::instance().locateObject<OfferCacheDataIPtr>(HIGHSCHOOL_CACHE_DATA_I, userId);
}

OfferCacheDataIPtr OfferFriendsCacheNManagerI::locateOfferCacheDataI(::Ice::Int userId) 
{
	return ServiceI::instance().locateObject<OfferCacheDataIPtr>(OFFER_CACHE_DATA_I, userId);
}

OfferCacheDataIPtr OfferFriendsCacheNManagerI::findOfferCacheDataI(::Ice::Int userId) 
{
	return ServiceI::instance().findObject<OfferCacheDataIPtr>(OFFER_CACHE_DATA_I, userId);
}

//************Tasks ***********************************************************//

void FillTask::handle() {
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
	for (std::map<long, Ice::ObjectPtr>::const_iterator it = _objs->data.begin();
			it!= _objs->data.end(); ++it) {
		try {
			OfferCacheDataNPtr obj = OfferCacheDataNPtr::dynamicCast( it->second );
			res->data.insert( make_pair<long,Ice::ObjectPtr>(it->first,new OfferCacheDataI(obj)) );
		} catch( Ice::Exception e ){
		   MCE_WARN( "FillTask " << it->first << " found Ice::Exception : " << e.what() );
		   continue;
		} catch( ... ) {
		   MCE_WARN( "FillTask " << it->first << " found unknown exception ." );
		   continue;
		}
	}
	
	ServiceI::instance().getObjectCache()->addObjects(OFFER_CACHE_DATA_I,res->data);
	MCE_INFO("FillTask done. task_size=" << _objs->data.size() << " objectcache_size=" 
			<< ServiceI::instance().getObjectCache()->locateCategory(OFFER_CACHE_DATA_I)->size());
}

void LoadTask::handle() {
	if (_obj) {
  	try {
	  	OfferCacheDataIPtr obj = new OfferCacheDataI( OfferCacheDataNPtr::dynamicCast( _obj ) );
	  	ServiceI::instance().getObjectCache()->addObject(OFFER_CACHE_DATA_I,_id,obj);
	  	MCE_INFO( "[LoadTask] userId=" << _id << " done." << " objectcache_size=" 
					<< ServiceI::instance().getObjectCache()->locateCategory(OFFER_CACHE_DATA_I)->size() );
	  } catch(Ice::Exception e) {
	  	MCE_WARN("[LoadTask] userId=" << _id << " found Ice::Exception : " << e.what() );
	  } catch( ... ) {
	  	MCE_WARN("[LoadTask] userId=" << _id << " found unknown exception." );
	  }
	}
}

void ReportTask::handle() {
	try {
		OfferFriendsLoaderAdapter::instance().report( _info );
	} catch( Ice::Exception e ){
		MCE_WARN("[ReportTask] userId=" << _info.userId << " found Ice::Exception : " << e.what() );
	} catch( ... ){
		MCE_WARN("[ReportTask] userId=" << _info.userId << " found unknown exception." );
	}
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
  TaskManager::instance().config(props, "Fill", TASK_LEVEL_FILL, ThreadPoolConfig(1,10));
  TaskManager::instance().config(props, "Load", TASK_LEVEL_LOAD, ThreadPoolConfig(1,10));
  TaskManager::instance().config(props, "Report", TASK_LEVEL_REPORT, ThreadPoolConfig(1,100));
}

