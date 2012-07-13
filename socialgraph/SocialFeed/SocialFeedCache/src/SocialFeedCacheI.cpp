#include "SocialFeedCacheI.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::socialgraph;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace xce::serverstate;
using namespace xce::helper;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	
	service.registerObjectCache(FEED_DATA, "FD", new SocialFeedFactory);

	service.getAdapter()->add( &SocialFeedCacheI::instance(),
			service.createIdentity("SFC", ""));

	ConnectionPoolManager::instance().initialize();

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("SocialFeedCache.Mod");
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("SocialFeedCache.Interval", 5);

	//向controller注册
	ServerStateSubscriber::instance().initialize("ControllerSocialFeedCache", &SocialFeedCacheI::instance(), mod, sInterval);
}

//***************************************************************************************

FeedSeq FeedData::get(int limit) {
	MCE_INFO("[FeedData] get _ownerId(" << _ownerId << ") limit(" << limit << ")");
	IceUtil::Mutex::Lock lock(*this);

	if( limit >= (int)_feedList.size() || limit <= 0 ) {
		return _feedList;
	}	
	
	FeedSeq result;
	for(FeedSeq::iterator iter=_feedList.begin(); 
			iter!=_feedList.end(); ++iter) {
		if( limit > 0 ) {
			result.push_back( *iter );
		 	--limit;
		}
	}
	return result;
}

void FeedData::remove(const Feed& feed) {
	MCE_INFO("[FeedData] remove _ownerId(" << _ownerId << ")");
	IceUtil::Mutex::Lock lock(*this);
	for(FeedSeq::iterator iter=_feedList.begin();           //可修改
			iter!=_feedList.end(); ++iter) {
		if(iter->friendIds == feed.friendIds && iter->newCommonIds == feed.newCommonIds) {
			_feedList.erase( iter );
			break;
		}
	}
}

//***************************************************************************************

void SocialFeedCacheI::set(::Ice::Int hostId, const FeedSeq& feedList, const Ice::Current& current) {
	MCE_INFO("[SocialFeedCacheI] set hostId(" << hostId << ") feedlist size(" << feedList.size() << ")");
	FeedDataPtr obj = new FeedData( hostId, feedList );
	ServiceI::instance().addObject( obj, FEED_DATA, hostId );
}

void SocialFeedCacheI::remove(::Ice::Int hostId, const MyUtil::IntSeq& friendIds, 
							const MyUtil::IntSeq& newCommonIds, const Ice::Current& current) {
	MCE_INFO("[SocialFeedCacheI] remove hostId(" << hostId << ") friendIds size(" << 
			friendIds.size() << ") newCommonIds size(" << newCommonIds.size() << ")");
	Feed feedItem;
	feedItem.friendIds = friendIds;
	feedItem.newCommonIds = newCommonIds;

	TaskManager::instance().execute( new RemoveTask(hostId, feedItem) );
}

FeedSeq SocialFeedCacheI::get(::Ice::Int hostId, ::Ice::Int limit, const Ice::Current& current) {
	MCE_INFO("[SocialFeedCacheI] get hostId(" << hostId << ") limit(" << limit << ")");
	return locateFeedData(hostId)->get(limit);
}

FeedDataPtr SocialFeedCacheI::locateFeedData(::Ice::Int id) {
	return ServiceI::instance().locateObject<FeedDataPtr>( FEED_DATA, id );
}

//***************************************************************************************

Ice::ObjectPtr SocialFeedFactory::create(::Ice::Int id) {
	MCE_INFO("[SocialFeedFactory] create id(" << id << ")");
	FeedDataPtr result = new FeedData( id, FeedSeq() );
	return result;
}

//***************************************************************************************

void RemoveTask::handle() {
	MCE_INFO("[RemoveTask] handle _ownerId(" << _ownerId << ")");
	FeedDataPtr obj = ServiceI::instance().findObject<FeedDataPtr>( FEED_DATA, _ownerId );
	if( !obj ) {
		return;
	}
	obj->remove( _feed );

	SocialFeedHelper::removeSocialFeed( _ownerId, _feed );
}
