#include "OnlineEventMediatorI.h"
#include "ServiceI.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using xce::mediator::onlineevent::OnlineEventMediatorManagerI;
using xce::mediator::onlineevent::UserCacheNotifyTask;
using xce::mediator::onlineevent::DistUserCacheNotifyTask;
using xce::mediator::onlineevent::OnlineStateFilterNotifyTask;
using xce::mediator::onlineevent::Invoker;
using MyUtil::Int2IntMap;

#include "UserCacheAdapter.h"
using xce::usercache::UserCacheAdapter;

#include "OnlineStateFilterAdapter.h"
using xce::onlinestatefilter::adapter::OnlineStateFilterAdapter;

#include "BuddyByOnlineTimeReplicaAdapter.h"
using xce::buddy::adapter::BuddyByOnlineTimeLoaderAdapter;

#include "DistUserCacheReloaderAdapter.h"
using xce::distcache::usercache::DistUserCacheReloaderAdapter;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&OnlineEventMediatorManagerI::instance(), service.createIdentity("M", ""));
	//	--------------------------------HA--------------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("OnlineEventMediator.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("OnlineEventMediator.Interval",5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerOnlineEventMediator",&OnlineEventMediatorManagerI::instance(),mod,interval);
	{ 
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("OnlineEventMediator.Status",0);
		xce::serverstate::ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status<<" mod : "<<mod<<" interval : "<<interval;
		MCE_WARN( os.str() );
	}

}

void Invoker::invoke(int userId, int online_type) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	bool needsNotify = _cache.empty();
	_cache.insert(make_pair(userId, online_type));

	xce::usercache::OnlineTypePtr tmpOnline = new xce::usercache::OnlineType;
	tmpOnline->userId = userId;
	tmpOnline->online = online_type;
	_onlineVec.push_back( tmpOnline );
	
	if (needsNotify) {
		_mutex.notify();
	}

}

void Invoker::invoke(const Int2IntMap& data) {
	if (data.empty()) {
		return;
	}
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	bool needsNotify = _cache.empty();
	_cache.insert(data.begin(), data.end());

	for( Int2IntMap::const_iterator it = data.begin(); it != data.end(); ++it )
	{ 
		xce::usercache::OnlineTypePtr tmpOnline = new xce::usercache::OnlineType;
		tmpOnline->userId = it->first;
		tmpOnline->online = it->second;
		_onlineVec.push_back( tmpOnline );
	}
	
	if (needsNotify) {
		_mutex.notify();
	}

}

void Invoker::run() {
	while (true) {
		try {
			Int2IntMap batch;
			xce::usercache::OnlineTypeSeq batchVec;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				if (_cache.empty()) {
					_mutex.wait();
					continue;
				}
				batch.swap(_cache);
				batchVec.swap(_onlineVec);
			}
			MyUtil::TaskManager::instance().execute(new DistUserCacheNotifyTask(batchVec));
			MyUtil::TaskManager::instance().execute(new UserCacheNotifyTask(batch));
			MyUtil::TaskManager::instance().execute(new OnlineStateFilterNotifyTask(batch));
			for(Int2IntMap::const_iterator it = batch.begin();it!=batch.end();++it){
				if (it->second == OFFLINE) continue;
				BuddyByOnlineTimeLoaderAdapter::instance().online( it->first );
			}
			
		} catch (Ice::Exception& e) {
			MCE_WARN("OnlineEventMediatorI Invoker::run() Ice::Exception " << e);
		} catch (std::exception& e) {
			MCE_WARN("OnlineEventMediatorI Invoker::run() std::exception " << e.what());
		} catch (...) {
			MCE_WARN("OnlineEventMediatorI Invoker::run() Exception");
		}
	}
}

OnlineEventMediatorManagerI::OnlineEventMediatorManagerI(){
	_invoker.start(1024*1024).detach();

}


void OnlineEventMediatorManagerI::sendOnlineEventSingle(::Ice::Int userId,
		::Ice::Int onlineType, const Ice::Current& current) {
	MCE_DEBUG("OnlineEventMediatorManagerI::sendOnlineEventSingle " << userId << " onlineType : " << onlineType);
	_invoker.invoke(userId, onlineType);

}

void OnlineEventMediatorManagerI::sendOnlineEventBatch(
		const ::MyUtil::Int2IntMap& data, const Ice::Current& current) {

	MCE_DEBUG("OnlineEventMediatorManagerI::sendOnlineEventBatch size=" << data.size());

	std::ostringstream os;
	os<<"OnlineEventMediatorManagerI::sendOnlineEventBatch ";
	for (Int2IntMap::const_iterator it = data.begin(); it != data.end(); ++it) {
		os<<" userID : "<< it->first<< " onlineType : " << it->second<<" , ";
	}

	MCE_DEBUG( os.str() );
	
	_invoker.invoke(data);


}

void UserCacheNotifyTask::handle() {

	UserCacheAdapter::instance().setOnline(batch_);

}

void DistUserCacheNotifyTask::handle() 
{

	try
	{
		DistUserCacheReloaderAdapter::instance().getProxy(0)->setOnlineBatch(batch_);
	}catch( Ice::Exception& e )
	{
		MCE_WARN("DistUserCacheNotifyTask::handle " << e.what() );
	}

}

void OnlineStateFilterNotifyTask::handle() {

	OnlineStateFilterAdapter::instance().updateOnlineStateBatch(batch_);

}

