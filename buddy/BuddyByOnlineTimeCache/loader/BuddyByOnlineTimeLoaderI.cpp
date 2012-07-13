#include "BuddyByOnlineTimeLoaderI.h"
#include "BuddyByIdReplicaAdapter.h"
#include "BuddyByOnlineTimeReplicaAdapter.h"
#include "OnlineTimeCache.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;
using namespace xce::cacheloader;

void MyUtil::initialize(){
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&BuddyByOnlineTimeLoaderI::instance(), service.createIdentity("L", ""));

	TaskManager::instance().config(TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_LOAD, ThreadPoolConfig(1, 5));
	TaskManager::instance().config(TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_RELOAD, ThreadPoolConfig(1, 5));
	TaskManager::instance().config(TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_ONLINE, ThreadPoolConfig(1, 5));
	TaskManager::instance().config(TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_NOTIFYONLINE, ThreadPoolConfig(1, 5));
	service.registerXceObserver(new ServiceSchemaI);

	OnlineTimeCacheInitThreadPtr init = new OnlineTimeCacheInitThread;
	init->start(1024*1024).detach();

	SetDataHelper::instance().initialize();

	OnlineNotifyHelper::instance().initialize();

	OnlineEventHelper::instance().initialize();

	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("BuddyByOnlineTimeCache.Controller", "ControllerBuddyByOnlineTimeCache");
	int cInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 60);
	int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByOnlineTimeCache.Proxy.Timeout", 300);
	int produceBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByOnlineTimeLoader.ProduceBatchSize", 10000);
	int consumeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByOnlineTimeLoader.ConsumeBatchSize", 3000);
	int writeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByOnlineTimeLoader.WriteBatchSize", 1000);
	int threadSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByOnlineTimeLoader.ThreadSize", 5);

	CacheLoader<BuddyByOnlineTimeCacheManagerPrx>::instance().initialize(
			new BatchUsersIdProducer,
			&BuddyByOnlineTimeLoaderFactory::instance(),
			ctlEndpoint,
			cInterval,
			timeout,
			produceBatchSize,
			consumeBatchSize,
			writeBatchSize,
			threadSize );

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyByOnlineTimeLoader.Mod");
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByOnlineTimeLoader.Interval", 5);
	// 向Controller注册
	ServerStateSubscriber::instance().initialize("ControllerBuddyByOnlineTimeLoader",
			&BuddyByOnlineTimeLoaderI::instance(), mod, sInterval);

}

/*****************************************************************************/

void BuddyByOnlineTimeLoaderI::reload(::Ice::Int hostId, const Ice::Current& current ){
	MCE_DEBUG( "[BuddyByOnlineTimeLoaderI::reload] id=" << hostId );
	TaskManager::instance().execute( new ReloadTask( hostId ) );
}

void BuddyByOnlineTimeLoaderI::load(const MyUtil::Int2IntSeqMap& lists,
		const Ice::Current& current) {
	MCE_INFO("[BuddyByOnlineTimeLoaderI::load] lists.size()=" << lists.size() );
	TaskManager::instance().execute(new LoadTask(lists));
}

void BuddyByOnlineTimeLoaderI::online(::Ice::Int onlineId, const Ice::Current& current) {
	MCE_DEBUG("[BuddyByOnlineTimeLoaderI::online] " << onlineId << " online");
	OnlineTimeCache::instance().set( onlineId, time(0) );
	OnlineEventHelper::instance().invokeDelay(onlineId);
}

/*****************************************************************************/

Ice::ObjectPtr BuddyByOnlineTimeLoaderFactory::create( Ice::Int id) {
	MyUtil::IntSeq buddies;
	try {
		buddies = BuddyByIdCacheAdapter::instance().getFriendListAsc( id, -1 );
	} catch( Ice::Exception e ){
		MCE_WARN("[BuddyByOnlineTimeLoaderFactory::create] id=" << id << " found Ice::Exception : " << e.what() );
		return 0;
	} catch( ... ){
		MCE_WARN("[BuddyByOnlineTimeLoaderFactory::create] id=" << id << " found Ice::Exception" );
		return 0;
	}
	BuddyByOnlineTimeDataPtr obj = new BuddyByOnlineTimeData;
	obj->onlinetime = OnlineTimeCache::instance().getOnlineTime( id, buddies );
	//MyUtil::IntSeq( OnlineTimeCache::instance().sort(id,buddies) ).swap( obj->friends );
	return obj;
}

MyUtil::ObjectResultPtr BuddyByOnlineTimeLoaderFactory::create(const MyUtil::IntSeq& ids) {
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
	MyUtil::Int2IntSeqMap buddymap = BuddyByIdCacheAdapter::instance().getFriendLists( ids );
	for( MyUtil::IntSeq::const_iterator idIt = ids.begin(); idIt != ids.end(); ++idIt ){
		MyUtil::IntSeq buddies;
		MyUtil::Int2IntSeqMap::iterator it = buddymap.find( *idIt );
		if( it != buddymap.end() ){
			buddies = it->second;
		} else{
			res->data[*idIt] = 0;
			continue;
		}
		BuddyByOnlineTimeDataPtr obj = new BuddyByOnlineTimeData;
		obj->onlinetime = OnlineTimeCache::instance().getOnlineTime( *idIt, buddies );
		//MyUtil::IntSeq( OnlineTimeCache::instance().sort(*idIt,buddies) ).swap( obj->friends );
		res->data[*idIt] = obj;
	}
	return res;
}

MyUtil::IntSeq OnlineTimeHelper::sort(int userId,const MyUtil::IntSeq& buddies){
	if( OnlineTimeCache::instance().isValid() ){
		return OnlineTimeCache::instance().sort( userId, buddies );
	}
	//else
	MyUtil::IntSeq result(buddies);
	MyUtil::IntSeq::const_iterator it = buddies.begin();
	if( it == buddies.end() ){
		return result;
	}

	ostringstream ids;
	ids << *it;
	for(;it!=buddies.end();++it){
		ids << ", " << *it;
	}

	Statement sql;
	sql	<< "SELECT id, UNIX_TIMESTAMP(lastlogintime) AS llt"
		<< " FROM user_time"
		<< " WHERE id IN (" << ids.str() << ")";
	std::map<int, int> times;
	QueryRunner("user_time", CDbRServer ).query(
			sql, OnlineTimeHandler( times ));

	std::sort(result.begin(), result.end(), GreaterOnlineTime(times));
	MCE_DEBUG("[OnlineTimeHelper::sort] userId=" << userId << " size=" << result.size() );

	return result;
}
bool OnlineTimeHandler::handle(mysqlpp::Row& res) const {
	int uid = res["id"];
	int time = res["llt"];
	_times.insert(make_pair(uid, time));
	return true;
}

/*****************************************************************************/

void OnlineEventHelper::initialize(){
	invoker_ = new OnlineEventThread(*this);
	invoker_->start(1024*1024).detach();
}

void OnlineEventHelper::invokeDelay( Ice::Int id ){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
	datas_.push_back( id );
	if (datas_.size() >= GET_FRIENDS_LIMIT) mutex_.notify();
}
void OnlineEventHelper::invokeNow( Ice::Int id ){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
	datas_.push_back( id );
	mutex_.notify();
}
void OnlineEventHelper::swap( MyUtil::IntSeq& empty_vector){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	if (datas_.size() < GET_FRIENDS_LIMIT) mutex_.timedWait(IceUtil::Time::seconds(5));
	datas_.swap(empty_vector);
}

void OnlineEventHelper::OnlineEventThread::run() {
	while (true){
		MyUtil::IntSeq online_ids_;
		helper_.swap( online_ids_ );
		if( online_ids_.empty() ){
			MCE_INFO("[OnlineEventHelper::OnlineEventThread::run] 0 users online." );
			continue;
		}
		TaskManager::instance().execute(new OnlineTask( online_ids_ ));
	}
}

/*****************************************************************************/

void OnlineNotifyHelper::initialize(){
	invoker_ = new OnlineNotifyThread(*this);
	invoker_->start(1024*1024).detach();
}

void OnlineNotifyHelper::invokeDelay( int userId, int onlineId ){
	OnlineNote note;
	note.userId = userId;
	note.onlineId = onlineId;
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
	datas_.push_back( note );
	if (datas_.size() >= NOTIFY_ONLINE_LIMIT ) mutex_.notify();
}
void OnlineNotifyHelper::invokeNow( int userId, int onlineId ){
	OnlineNote note;
	note.userId = userId;
	note.onlineId = onlineId;
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
	datas_.push_back( note );
	mutex_.notify();
}
void OnlineNotifyHelper::swap( OnlineNoteSeq& empty_vector){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	if (datas_.size() < NOTIFY_ONLINE_LIMIT ) mutex_.timedWait(IceUtil::Time::seconds(5));
	datas_.swap(empty_vector);
}

void OnlineNotifyHelper::OnlineNotifyThread::run() {
	while (true){
		OnlineNoteSeq online_notes_;
		helper_.swap( online_notes_ );
		if( online_notes_.empty() ){
			MCE_INFO("[OnlineNotifyHelper::OnlineNotifyThread::run] notifyonline 0 users." );
			continue;
		}
		TaskManager::instance().execute(new NotifyOnlineTask( online_notes_ ));
	}
}

/*****************************************************************************/

void SetDataHelper::initialize(){
	setter_ = new SetDataThread(*this);
	setter_->start(1024*1024).detach();
}

void SetDataHelper::invokeDelay( Ice::Int id, const Ice::ObjectPtr& o ){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
	datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
	if (datas_.size() >= 1000) mutex_.notify();
}
void SetDataHelper::invokeNow(){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
	mutex_.notify();
}
void SetDataHelper::invokeNow( Ice::Int id, const Ice::ObjectPtr& o ){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
	datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
	mutex_.notify();
}
void SetDataHelper::swap(vector< pair<long,Ice::ObjectPtr> >& empty_vector){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	if (datas_.size() < 1000) mutex_.timedWait(IceUtil::Time::seconds(1));
	datas_.swap(empty_vector);
}

void SetDataHelper::SetDataThread::run() {
	while (true){
		std::vector< pair<long,Ice::ObjectPtr> > to_be_set_;
		helper_.swap( to_be_set_ );
		if( to_be_set_.empty() ){
			MCE_INFO("[SetDataHelper::SetDataThread::run] setData: 0 objs." );
			continue;
		}
		ostringstream idstr;
		MyUtil::ObjectResultPtr objs = new MyUtil::ObjectResult();
		for ( std::vector< pair<long, Ice::ObjectPtr> >::iterator it = to_be_set_.begin();
				it != to_be_set_.end(); ++it ){
			std::pair<std::map<long, Ice::ObjectPtr>::iterator,bool> ret;
			ret = objs->data.insert(*it);
			if(!ret.second){
                                MCE_WARN("[SetDataHelper::SetDataThread::run] setData: " << it->first << " insert "
                                        << BuddyByOnlineTimeDataPtr::dynamicCast( it->second )->onlinetime.size() << " failed.");
                                objs->data[it->first] = it->second;
                        }
			idstr << " " << it->first << "(" << BuddyByOnlineTimeDataPtr::dynamicCast( it->second )->onlinetime.size() << ")";
		}
		MCE_INFO("[SetDataHelper::SetDataThread::run] setData:" << idstr.str() );
		BuddyByOnlineTimeCacheAdapter::instance().setData(objs);
	}
}

/*****************************************************************************/

void ReloadTask::handle(){
	BuddyByIdDataPtr obj;
	try {
		obj = BuddyByIdDataPtr::dynamicCast( BuddyByOnlineTimeLoaderFactory::instance().create(_id) );
	} catch (Ice::Exception& e) {
		MCE_WARN("[ReloadTask] Ice::Exception (create:" << _id << ") : " << e.what());
		return;
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[ReloadTask] mysqlpp::Exception (create:" << _id << ") : " << e.what());
		return;
	} catch (std::exception& e) {
		MCE_WARN("[ReloadTask] std::excetpion (create:" << _id << ") : " << ": " << e.what());
		return;
	} catch (...) {
		MCE_WARN("[ReloadTask] unknown exception (create:" << _id << ") ." );
		return;
	}
	if( obj == 0 ){
		return;
	}
	SetDataHelper::instance().invokeDelay(_id,obj);
	//SetDataHelper::instance().invokeNow(_id,obj);
}

void LoadTask::handle(){
	for( MyUtil::Int2IntSeqMap::const_iterator it = _lists.begin(); it != _lists.end(); ++it ){
		BuddyByOnlineTimeDataPtr obj = new BuddyByOnlineTimeData;
		obj->onlinetime = OnlineTimeCache::instance().getOnlineTime( it->first, it->second);
		SetDataHelper::instance().invokeDelay(it->first,obj);
	}
	SetDataHelper::instance().invokeNow();
}

void OnlineTask::handle(){
	MyUtil::Int2IntSeqMap buddymap = BuddyByOnlineTimeCacheAdapter::instance().getFriendLists( _ids );
	MCE_INFO("[OnlineTask] " << buddymap.size() << " users online." );
	for( MyUtil::Int2IntSeqMap::const_iterator idIt = buddymap.begin(); idIt != buddymap.end(); ++idIt ){
		for( MyUtil::IntSeq::const_iterator buddyIt = idIt->second.begin(); buddyIt != idIt->second.end(); ++buddyIt ){
			OnlineNotifyHelper::instance().invokeDelay( *buddyIt, idIt->first );
		}
	}
}

void NotifyOnlineTask::handle(){
	BuddyByOnlineTimeCacheAdapter::instance().notifyOnlineBatch( _notes );
	MCE_INFO("[NotifyOnlineTask] notifyonline " << _notes.size() << " users." );
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	TaskManager::instance().config(props, "Load", TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_LOAD, ThreadPoolConfig(1,5));
	TaskManager::instance().config(props, "Reload", TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_RELOAD, ThreadPoolConfig(1,5));
	TaskManager::instance().config(props, "Online", TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_ONLINE, ThreadPoolConfig(1,5));
	TaskManager::instance().config(props, "NotifyOnline", TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_NOTIFYONLINE, ThreadPoolConfig(1,5));
}

