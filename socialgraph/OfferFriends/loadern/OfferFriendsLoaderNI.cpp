#include "OfferFriendsLoaderNI.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "OfferFriendsAdapter.h"
#include "../share/OfferCreator.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::offerfriends;
using namespace xce::offerfriends::adapter;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&OfferFriendsLoaderNI::instance(), service.createIdentity("L", ""));

	TaskManager::instance().config(TASK_LEVEL_APPLY, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_ACCEPT, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_DENY, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_IGNORE, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_REPORT, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(TASK_LEVEL_REBUILD_NOW, ThreadPoolConfig(1, 10));
	TaskManager::instance().config(TASK_LEVEL_REBUILD_LATER, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_RELOAD, ThreadPoolConfig(1, 10));
	TaskManager::instance().config(TASK_LEVEL_RELOAD_BATCH, ThreadPoolConfig(1, 1));
  service.registerXceObserver(new ServiceSchemaI);

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Controller.Mod");
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Controller.Interval", 5);
	string ctlEndpoint = service.getCommunicator()->getProperties()->getPropertyWithDefault("Controller.ctlEndpoint",
																																										"ControllerOfferFriendsLoaderN");

	ServerStateSubscriber::instance().initialize(ctlEndpoint, &OfferFriendsLoaderNI::instance(), mod, sInterval);
}

//***************************************************************************

void OfferFriendsLoaderNI::apply(::Ice::Int applicant, 
																 ::Ice::Int acceptor,
																 const Ice::Current& current)
{
	MCE_DEBUG("[apply] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) 
			<< "] applicant=" << applicant << " acceptor=" << acceptor );
	TaskManager::instance().execute(new ApplyTask(applicant, acceptor));
}

void OfferFriendsLoaderNI::accept(::Ice::Int acceptor, 
																	::Ice::Int applicant,
																	const Ice::Current& current)
{
	MCE_DEBUG("[accept] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) 
			<< "] acceptor=" << acceptor << " applicant=" << applicant );
	TaskManager::instance().execute(new AcceptTask(acceptor,applicant));
}

void OfferFriendsLoaderNI::denyOffer(::Ice::Int hostId, 
																		 ::Ice::Int guestId,
																		 const Ice::Current& current)
{
	MCE_DEBUG("[denyOffer] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) 
			<< "] hostId=" << hostId << " guestId=" << guestId );
	TaskManager::instance().execute(new DenyTask(hostId,guestId));
}

void OfferFriendsLoaderNI::ignoreOffers(::Ice::Int userId, 
																				const MyUtil::IntSeq& ids, 
																				const Ice::Current& current)
{
	MCE_DEBUG("[ignoreOffers] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) 
			<< "] userId=" << userId << " ids.size()=" << ids.size() );
	if (ids.size() > 0) {
		TaskManager::instance().execute(new IgnoreTask(userId,ids));
	}
}

void OfferFriendsLoaderNI::report(const GetInfo& info,
																  const Ice::Current& current)
{
	MCE_DEBUG("[report] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) 
			<< "] userId=" << info.userId );
	TaskManager::instance().execute(new ReportTask(info));
}

void OfferFriendsLoaderNI::rebuild(::Ice::Int userId, 
																	 bool now, 
																	 const Ice::Current& current) 
{
	if (now) {
		MCE_INFO("[rebuild-now] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) 
				<< "] userId=" << userId );
		TaskManager::instance().execute(new RebuildNowTask(userId));
	} else {
		MCE_INFO("[rebuild-later] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) 
				<< "] userId=" << userId );
		TaskManager::instance().execute(new RebuildLaterTask(userId));
	}
}

void OfferFriendsLoaderNI::rebuildBatch(const MyUtil::IntSeq& userIds, 
																				bool now,
																				const Ice::Current& current) 
{
	if (now) {
		MCE_INFO("[rebuild-batch-now] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) 
				<< "] userIds.size()=" << userIds.size() );
		for (MyUtil::IntSeq::const_iterator it = userIds.begin(); 
				it != userIds.end(); ++it) {
			TaskManager::instance().execute(new RebuildNowTask(*it));
		}
	} else {
		MCE_INFO("[rebuild-batch-later] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) 
				<< "] userIds.size()=" << userIds.size());
		for (MyUtil::IntSeq::const_iterator it = userIds.begin(); 
				it != userIds.end(); ++it) {
			TaskManager::instance().execute(new RebuildLaterTask(*it));
		}
	}
}

string OfferFriendsLoaderNI::debug(::Ice::Int userId,
																	 const Ice::Current& current) 
{
	MCE_INFO("[debug] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) 
			<< "] userId=" << userId );
	ostringstream log;
	OfferCacheDataNPtr obj = OfferCreator::instance().create(userId, 50, log);
	return log.str();
}

void OfferFriendsLoaderNI::reload(::Ice::Int userId,
																	const Ice::Current& current) 
{
	MCE_INFO("[reload] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) 
			<< "] userId=" << userId );
	TaskManager::instance().execute(new ReloadTask(userId));
}
void OfferFriendsLoaderNI::reloadBatch(const MyUtil::IntSeq& userIds,
																			 const Ice::Current& current) 
{
	MCE_INFO("[reload-batch] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) 
			<< "] userIds.size()=" << userIds.size());
	TaskManager::instance().execute(new ReloadBatchTask(userIds));
}

//**************************************************************************************************************

void ApplyTask::handle() 
{
	RmInfo ret = OfferFriendsCacheAdapter::instance().apply(_applicant,_acceptor);
  ostringstream log;
	log << "[ApplyTask] applicant/acceptor=" << _applicant << "/" << ret.data.userId
		<< " hit/found=" << ret.hit << "/" << ret.found
		<< " src=" << ret.bestCount << "+" << ret.offerCount
		<< " timeout=" << ret.timeout << "(" << (ret.timeout/3600) << "h" << ((ret.timeout%3600)/60) << "m) ";
	if (ret.data.sign&(((int)1 << BaseTraitCommonFriend))) {
		log << "|Comm";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoWorkPlace))) {
		log << "|InfWork";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoUniv))) {
		log << "|InfUniv";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoHighSchool))) {
		log << "|InfHigh";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoJuniorSchool))) {
		log << "|InfJuni";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoElementarySchool))) {
		log << "|InfElem";
	}
	if (ret.data.sign&(((int)1 << BaseTraitIPSame))) {
		log << "|IPSame";
	}
	MCE_INFO( log.str() );
}

//**************************************************************************************************************

void AcceptTask::handle() 
{
	RmInfo ret = OfferFriendsCacheAdapter::instance().accept(_acceptor,_applicant);
  ostringstream log;
	log << "[AcceptTask] acceptor/applicant=" << _acceptor << "/" << ret.data.userId
		<< " hit/found=" << ret.hit << "/" << ret.found
		<< " src=" << ret.bestCount << "+" << ret.offerCount
		<< " timeout=" << ret.timeout << "(" << (ret.timeout/3600) << "h" << ((ret.timeout%3600)/60) << "m) ";
	if (ret.data.sign&(((int)1 << BaseTraitCommonFriend))) {
		log << "|Comm";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoWorkPlace))) {
		log << "|InfWork";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoUniv))) {
		log << "|InfUniv";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoHighSchool))) {
		log << "|InfHigh";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoJuniorSchool))) {
		log << "|InfJuni";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoElementarySchool))) {
		log << "|InfElem";
	}
	if (ret.data.sign&(((int)1 << BaseTraitIPSame))) {
		log << "|IPSame";
	}
	MCE_INFO( log.str() );
}

//**************************************************************************************************************

void DenyTask::handle() 
{
	RmInfo ret = OfferFriendsCacheAdapter::instance().denyOffer(_hostId,_guestId);
  ostringstream log;
	log << "[DenyTask] hostId/guestId=" << _hostId << "/" << ret.data.userId
		<< " hit/found=" << ret.hit << "/" << ret.found
		<< " src=" << ret.bestCount << "+" << ret.offerCount
		<< " timeout=" << ret.timeout << "(" << (ret.timeout/3600) << "h" << ((ret.timeout%3600)/60) << "m) ";
	if (ret.data.sign&(((int)1 << BaseTraitCommonFriend))) {
		log << "|Comm";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoWorkPlace))) {
		log << "|InfWork";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoUniv))) {
		log << "|InfUniv";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoHighSchool))) {
		log << "|InfHigh";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoJuniorSchool))) {
		log << "|InfJuni";
	}
	if (ret.data.sign&(((int)1 << BaseTraitSameInfoElementarySchool))) {
		log << "|InfElem";
	}
	if (ret.data.sign&(((int)1 << BaseTraitIPSame))) {
		log << "|IPSame";
	}
	MCE_INFO( log.str() );
}

//**************************************************************************************************************

void IgnoreTask::handle() 
{
	for (MyUtil::IntSeq::iterator it = _ids.begin(); 
			it != _ids.end(); ++it ) {
		RmInfo ret = OfferFriendsCacheAdapter::instance().ignoreOffer(_userId,*it);
		ostringstream log;
		log << "[IgnoreTask] userId/guestId=" << _userId << "/" << ret.data.userId
			<< " hit/found=" << ret.hit << "/" << ret.found
			<< " src=" << ret.bestCount << "+" << ret.offerCount
			<< " timeout=" << ret.timeout << "(" << (ret.timeout/3600) << "h" << ((ret.timeout%3600)/60) << "m) ";
		if (ret.data.sign&(((int)1 << BaseTraitCommonFriend))) {
			log << "|Comm";
		}
		if (ret.data.sign&(((int)1 << BaseTraitSameInfoWorkPlace))) {
			log << "|InfWork";
		}
		if (ret.data.sign&(((int)1 << BaseTraitSameInfoUniv))) {
			log << "|InfUniv";
		}
		if (ret.data.sign&(((int)1 << BaseTraitSameInfoHighSchool))) {
			log << "|InfHigh";
		}
		if (ret.data.sign&(((int)1 << BaseTraitSameInfoJuniorSchool))) {
			log << "|InfJuni";
		}
		if (ret.data.sign&(((int)1 << BaseTraitSameInfoElementarySchool))) {
			log << "|InfElem";
		}
		if (ret.data.sign&(((int)1 << BaseTraitIPSame))) {
			log << "|IPSame";
		}
		MCE_INFO( log.str() );
	}
}

//**************************************************************************************************************

void ReportTask::handle() 
{
	ostringstream log;
	log << "[ReportTask] userId=" << _info.userId
		<< " hit=" << _info.hit
		<< " limit=" << _info.limit
		<< " res=" << _info.bestRes << "/" << _info.bestCount
		<< "+(" << _info.onlineRes << "+" << _info.offerRes << ")" << "/" << _info.offerCount
		<< " timeout=" << _info.timeout << "(" << (_info.timeout/3600) << "h" << ((_info.timeout%3600)/60) << "m)";
	MCE_INFO( log.str() );
}

//**************************************************************************************************************

void RebuildNowTask::handle() 
{
	ostringstream log;
	OfferCacheDataNPtr obj = OfferCreator::instance().create(_userId,50,log);
	OfferFriendsCacheAdapter::instance().load(_userId,obj);
}

//**************************************************************************************************************

void RebuildLaterTask::handle() 
{
	ostringstream log;
	OfferCacheDataNPtr obj = OfferCreator::instance().create(_userId,50,log);
	OfferFriendsCacheAdapter::instance().load(_userId,obj);
}

//**************************************************************************************************************

void ReloadTask::handle()
{
	OfferCacheDataNPtr obj=0;
	try {
		obj = OfferCacheDataNPtr::dynamicCast( OfferCacheDataFactory::instance().create(_userId) );
	} catch (Ice::Exception& e) {
		MCE_WARN("[ReloadTask] Ice::Exception (create:" << _userId << ") : " << e.what());
		return;
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[ReloadTask] mysqlpp::Exception (create:" << _userId << ") : " << e.what());
		return;
	} catch (std::exception& e) {
		MCE_WARN("[ReloadTask] std::excetpion (create:" << _userId << ") : " << ": " << e.what());
		return;
	} catch (...) {
		MCE_WARN("[ReloadTask] unknown exception (create:" << _userId << ") ." );
		return;
	}

	if( obj == 0 ){
		return;
	}

	OfferFriendsCacheAdapter::instance().load(_userId,obj);
}

//**************************************************************************************************************

void ReloadBatchTask::handle() 
{
	MyUtil::ObjectResultPtr objs;
	try {
		objs = OfferCacheDataFactory::instance().create(_userIds);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ReloadBatchTask] Ice::Exception (create:" << _userIds.size() << " objs) : " << e.what());
		return;
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[ReloadBatchTask] mysqlpp::Exception (create:" << _userIds.size() << " objs) : " << e.what());
		return;
	} catch (std::exception& e) {
		MCE_WARN("[ReloadBatchTask] std::excetpion (create:" << _userIds.size() << " objs) : " << ": " << e.what());
		return;
	} catch (...) {
		MCE_WARN("[ReloadBatchTask] unknown exception (create:" << _userIds.size() << " objs) ." );
		return;
	}

	for( MyUtil::ObjectMap::iterator it = objs->data.begin(); it != objs->data.end(); ++it ){
		OfferCacheDataNPtr obj = OfferCacheDataNPtr::dynamicCast( it->second );
		if( obj == 0 ){
			return;
		}
		OfferFriendsCacheAdapter::instance().load(it->first,obj);
	}
	MCE_INFO("[ReloadBatchTask] reload:" << _userIds.size() << " objs done." );
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) 
{
  TaskManager::instance().config(props, "Apply", TASK_LEVEL_APPLY, ThreadPoolConfig(1,1));
  TaskManager::instance().config(props, "Accept", TASK_LEVEL_ACCEPT, ThreadPoolConfig(1,1));
  TaskManager::instance().config(props, "Deny", TASK_LEVEL_DENY, ThreadPoolConfig(1,1));
  TaskManager::instance().config(props, "Ignore", TASK_LEVEL_IGNORE, ThreadPoolConfig(1,1));
  TaskManager::instance().config(props, "Report", TASK_LEVEL_REPORT, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "RebuildNow", TASK_LEVEL_REBUILD_NOW, ThreadPoolConfig(1,10));
  TaskManager::instance().config(props, "RebuildLater", TASK_LEVEL_REBUILD_LATER, ThreadPoolConfig(1,1));
  TaskManager::instance().config(props, "Reload", TASK_LEVEL_RELOAD, ThreadPoolConfig(1,10));
  TaskManager::instance().config(props, "ReloadBatch", TASK_LEVEL_RELOAD_BATCH, ThreadPoolConfig(1,1));
}

