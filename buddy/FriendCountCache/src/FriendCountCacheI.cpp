#include "FriendCountCacheI.h"
#include "ServiceI.h"
#include <BuddyByIdReplicaAdapter.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(FRIEND_COUNT_CACHE_DATA, "FCC", new FriendCountCacheFactory);
	service.getAdapter()->add(&FriendCountCacheManagerI::instance(),
			service.createIdentity("M", ""));

	TaskManager::instance().config(TASK_LEVEL_FRIEND_COUNT_CACHE_SET, ThreadPoolConfig(1,100));
	TaskManager::instance().config(TASK_LEVEL_FRIEND_COUNT_CACHE_SET_BATCH, ThreadPoolConfig(1,100));
	TaskManager::instance().config(TASK_LEVEL_FRIEND_COUNT_CACHE_RECORD, ThreadPoolConfig(1,1));
	TaskManager::instance().config(TASK_LEVEL_FRIEND_COUNT_CACHE_RECORD_BATCH, ThreadPoolConfig(1,1));
	TaskManager::instance().config(TASK_LEVEL_FRIEND_COUNT_CACHE_RECORD_DEC, ThreadPoolConfig(1,1));
	TaskManager::instance().config(TASK_LEVEL_FRIEND_COUNT_CACHE_LOAD, ThreadPoolConfig(1,1));
	service.registerXceObserver(new ServiceSchemaI);

	LoadFriendCountHelper::instance().initialize();

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendCountCache.Mod");
	string endpoint = service.getCommunicator()->getProperties()->getProperty("FriendCountCache.Endpoint");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendCountCache.Interval", 5);

	// 向Controller注册
	ServerStateSubscriber::instance().initialize("ControllerFriendCountCache",
			&FriendCountCacheManagerI::instance(), mod, interval);
}

/*****************************************************************************/

::Ice::Int FriendCountCacheManagerI::getFriendCount(
		::Ice::Int id, const Ice::Current& current) {
	FriendCountCacheDataPtr obj = locateFriendCountCacheData(id);
        MCE_INFO( "[getFriendCount] id=" << id << " count=" << obj->count );
	return obj->count;
}

::MyUtil::Int2IntMap FriendCountCacheManagerI::getFriendCounts(
		const MyUtil::IntSeq& ids, const Ice::Current& current){
	Int2IntMap res;
	for(MyUtil::IntSeq::const_iterator it = ids.begin();it!=ids.end();++it){
		FriendCountCacheDataPtr obj = locateFriendCountCacheData(*it);
		res.insert( make_pair( *it, obj->count ) );
	}
        MCE_INFO( "[getFriendCounts] ids.size()=" << ids.size() << " res.size()=" << res.size() );
	return res;
}

void FriendCountCacheManagerI::setFriendCount( ::Ice::Int id, ::Ice::Int count, const Ice::Current&){
        MCE_INFO( "[setFriendCount] id=" << id << " count=" << count );
        TaskManager::instance().execute( new SetTask( id, count ) );
}

void FriendCountCacheManagerI::setFriendCounts( const ::MyUtil::Int2IntMap& counts, const Ice::Current&){
        MCE_INFO( "[setFriendCounts] counts.size()=" << counts.size() );
        TaskManager::instance().execute( new SetBatchTask( counts ) );
}

void FriendCountCacheManagerI::dec( ::Ice::Int id, const Ice::Current&){
        MCE_INFO( "[dec] id=" << id );
	findFriendCountCacheData(id)->count--;
        TaskManager::instance().execute( new CountDecTask( id ) );
}

void FriendCountCacheManagerI::reload( ::Ice::Int id, const Ice::Current&){
        MCE_INFO( "[reload] id=" << id );
	LoadFriendCountHelper::instance().invokeNow( id );
}

/*****************************************************************************/

FriendCountCacheDataPtr FriendCountCacheManagerI::locateFriendCountCacheData(::Ice::Int id) {
        return ServiceI::instance().locateObject<FriendCountCacheDataPtr> (
                        FRIEND_COUNT_CACHE_DATA, id);
}

FriendCountCacheDataPtr FriendCountCacheManagerI::findFriendCountCacheData(::Ice::Int id) {
        FriendCountCacheDataPtr obj = ServiceI::instance().findObject<FriendCountCacheDataPtr> (
                        FRIEND_COUNT_CACHE_DATA, id);
	if( obj ){
		return obj;
	}
	obj = new FriendCountCacheData;
	obj->count = 0;
	return obj;
}

/*****************************************************************************/

Ice::ObjectPtr FriendCountCacheFactory::create(Ice::Int id) {
	LoadFriendCountHelper::instance().invokeDelay( id );
	FriendCountCacheDataPtr obj = new FriendCountCacheData;
	obj->count = 0;
	return obj;
}

/*****************************************************************************/

void LoadFriendCountHelper::initialize(){
        loader_ = new LoadFriendCountThread(*this);
        loader_->start(1024*1024).detach();
}

void LoadFriendCountHelper::invokeDelay( Ice::Int id ){
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
        datas_.push_back( id );
        if (datas_.size() >= 10000) mutex_.notify();
}
void LoadFriendCountHelper::invokeNow( Ice::Int id ){
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
        datas_.push_back( id );
        mutex_.notify();
}
void LoadFriendCountHelper::swap( MyUtil::IntSeq& empty_vector){
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
        if (datas_.size() < 10000) mutex_.timedWait(IceUtil::Time::seconds(5));
        datas_.swap(empty_vector);
}

void LoadFriendCountHelper::LoadFriendCountThread::run() {
        while (true){
		MyUtil::IntSeq ids_;
                helper_.swap( ids_ );
                if( ids_.empty() ){
                        MCE_INFO("[LoadFriendCountHelper::LoadFriendCountThread::run] to create 0 objs." );
                        continue;
                }
                MCE_INFO("[LoadFriendCountHelper::LoadFriendCountThread::run] to create " << ids_.size() << " objs." );
        	TaskManager::instance().execute( new LoadTask( ids_ ) );
        }
}

/************Tasks ***********************************************************/

void SetTask::handle() {
	FriendCountCacheDataPtr obj = new FriendCountCacheData;
	obj->count = _count;
        ServiceI::instance().getObjectCache()->addObject(FRIEND_COUNT_CACHE_DATA, _id, obj);
        MCE_INFO("SetTask done. Task size / ObjectCache size : 1 / " << ServiceI::instance().getObjectCache()->locateCategory(FRIEND_COUNT_CACHE_DATA)->size());
        TaskManager::instance().execute( new CountRecordTask( _id, _count ) );
}

void SetBatchTask::handle() {
	MyUtil::ObjectResultPtr objs = new MyUtil::ObjectResult();
	for( ::MyUtil::Int2IntMap::const_iterator it = _counts.begin(); it != _counts.end(); ++it ){
		FriendCountCacheDataPtr obj = new FriendCountCacheData;
		obj->count = it->second;
		objs->data.insert( make_pair<long,Ice::ObjectPtr>(it->first,obj) ); 
	}
        ServiceI::instance().getObjectCache()->addObjects(FRIEND_COUNT_CACHE_DATA,objs->data);
        MCE_INFO("SetBatchTask done. Task size / ObjectCache size : " << objs->data.size() << " / " << ServiceI::instance().getObjectCache()->locateCategory(FRIEND_COUNT_CACHE_DATA)->size());
        TaskManager::instance().execute( new CountRecordBatchTask( _counts ) );
}

void CountRecordTask::handle(){
	Statement sql;
	//sql << "REPLACE INTO user_friendcount (id, friendcount) VALUES (" << _id << "," << _count << ")";
	sql << "INSERT INTO user_friendcount (id, friendcount) VALUES(" << _id << ", " <<  _count << ") ON DUPLICATE KEY UPDATE friendcount = values(friendcount)";
	try {
		QueryRunner("user_friendcount", CDbWServer).schedule( sql );
	} catch (...) {
		MCE_WARN("Replace user_friendcount Exception ");
	}
}

void CountRecordBatchTask::handle(){
	if( _counts.empty() ){
		return;
	}
	ostringstream values;
	MyUtil::Int2IntMap::const_iterator it = _counts.begin();
	values << "(" << it->first << "," << it->second << ")";
	for( ++it; it != _counts.end(); ++it ){
		values << ",(" << it->first << "," << it->second << ")";
	}
	Statement sql;
	sql << "INSERT INTO user_friendcount (id, friendcount) VALUES " << values.str() << " ON DUPLICATE KEY UPDATE friendcount = values(friendcount)";
	try {
		QueryRunner("user_friendcount", CDbWServer).schedule( sql );
	} catch (...) {
		MCE_WARN("Replace user_friendcount Batch Exception ");
	}
}

void CountDecTask::handle(){
	Statement sql;
	sql << "UPDATE user_friendcount"
		<< " SET friendcount = friendcount - 1"
		<< " WHERE id = " << _id;
	try {
		QueryRunner("user_friendcount", CDbWServer).schedule( sql );
	} catch (...) {
		MCE_WARN("Update user_friendcount Exception ");
	}
}

void LoadTask::handle() {
	MyUtil::Int2IntMap counts = BuddyByIdCacheAdapter::instance().getFriendCounts(_ids);
	MyUtil::ObjectResultPtr objs = new MyUtil::ObjectResult();
	MyUtil::Int2IntMap countsTmp;
	int limit = 0;
	for( ::MyUtil::Int2IntMap::const_iterator it = counts.begin(); it != counts.end(); ++it ){
		FriendCountCacheDataPtr obj = new FriendCountCacheData;
		obj->count = it->second;
		objs->data.insert( make_pair<long,Ice::ObjectPtr>(it->first,obj) );
		countsTmp.insert( make_pair<Ice::Int,Ice::Int>(it->first,it->second) );
		if( ++limit >= 1000 ){
			TaskManager::instance().execute( new CountRecordBatchTask( countsTmp ) );
			limit = 0;
			countsTmp.clear();
		}
	}
	if( limit > 0 ){
		TaskManager::instance().execute( new CountRecordBatchTask( countsTmp ) );
		limit = 0;
		countsTmp.clear();
	}
        ServiceI::instance().getObjectCache()->addObjects(FRIEND_COUNT_CACHE_DATA,objs->data);
        MCE_INFO("LoadTask done. Task size / ObjectCache size : " << objs->data.size() << " / " << ServiceI::instance().getObjectCache()->locateCategory(FRIEND_COUNT_CACHE_DATA)->size());
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	TaskManager::instance().config(props, "Set", TASK_LEVEL_FRIEND_COUNT_CACHE_SET, ThreadPoolConfig(1,100));
	TaskManager::instance().config(props, "SetBatch", TASK_LEVEL_FRIEND_COUNT_CACHE_SET_BATCH, ThreadPoolConfig(1,100));
        TaskManager::instance().config(props, "CountRecord", TASK_LEVEL_FRIEND_COUNT_CACHE_RECORD, ThreadPoolConfig(1,1));
        TaskManager::instance().config(props, "CountRecordBatch", TASK_LEVEL_FRIEND_COUNT_CACHE_RECORD_BATCH, ThreadPoolConfig(1,1));
        TaskManager::instance().config(props, "CountRecordDec", TASK_LEVEL_FRIEND_COUNT_CACHE_RECORD_DEC, ThreadPoolConfig(1,1));
	TaskManager::instance().config(props, "Load", TASK_LEVEL_FRIEND_COUNT_CACHE_LOAD, ThreadPoolConfig(1,1));
}

