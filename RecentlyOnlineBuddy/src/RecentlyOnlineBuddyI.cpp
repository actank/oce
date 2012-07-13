#include "RecentlyOnlineBuddyI.h"

using namespace xce::recentlyonlinebuddy;

#include "RecentlyOnlineBuddyAdapter.h"
using xce::recentlyonlinebuddy::RecentlyOnlineBuddyAdapter;

#include "BuddyCacheByIdAdapter.h"
using xce::buddy::BuddyCacheByIdAdapter;

#include "OnlineTimeCache.h"
using xce::recentlyonlinebuddy::OnlineTimeCache;
using xce::recentlyonlinebuddy::OnlineTimeCacheInitThread;
using xce::recentlyonlinebuddy::OnlineTimeCacheInitThreadPtr;

#include "ServiceI.h"
using MyUtil::ServiceI;
using MyUtil::TaskManager;
using MyUtil::ServantFactory;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(CATEGORY_DATA, "R",	new RecentlyOnlineBuddyDataFactory);
	service.getAdapter()->add(new RecentlyOnlineBuddyManagerI,
			service.createIdentity("M", ""));
	OnlineTimeCacheInitThreadPtr init = new OnlineTimeCacheInitThread;
	init->start(1024*1024).detach();
	TaskManager::instance().config(TASK_LEVEL_BUDDY_CREATE, MyUtil::ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_BUDDY_ADD, ThreadPoolConfig(0, 1));
	TaskManager::instance().config(TASK_LEVEL_BUDDY_REMOVE, ThreadPoolConfig(0, 1));
	TaskManager::instance().config(TASK_LEVEL_BUDDY_NOTIFY_SO, ThreadPoolConfig(0, 1));
	TaskManager::instance().config(TASK_LEVEL_BUDDY_SO, ThreadPoolConfig(0, 1));
}

/*****************************************************************************/

MyUtil::IntSeq RecentlyOnlineBuddyData::get_most_recent(int limit) {
	IceUtil::Mutex::Lock lock(*this);
	MyUtil::IntSeq result;
	for (std::deque<int>::reverse_iterator it = _buddies.rbegin();
		(int) result.size()	< limit && it != _buddies.rend(); ++it) {
		if (find(result.begin(), result.end(), *it)!=result.end()) continue;
		result.push_back(*it);
	}
	return result;
}

Ice::Int RecentlyOnlineBuddyData::get_count() {
	IceUtil::Mutex::Lock lock(*this);
	return _buddies.size();
}

void RecentlyOnlineBuddyData::add_old_online(const MyUtil::IntSeq& sorted_buddies) {
	IceUtil::Mutex::Lock lock(*this);
	for (MyUtil::IntSeq::const_iterator it = sorted_buddies.begin(); it
			!= sorted_buddies.end(); ++it) {
		_buddies.push_front(*it);
	}
}

void RecentlyOnlineBuddyData::add_recent_online(int userId) {
	IceUtil::Mutex::Lock lock(*this);
	std::deque<int>::iterator it = find(_buddies.begin(), _buddies.end(),
			userId);
	if (it != _buddies.end())
		_buddies.erase(it);
	_buddies.push_back(userId);
}

void RecentlyOnlineBuddyData::remove_online(int userId) {
	IceUtil::Mutex::Lock lock(*this);
	std::deque<int>::iterator it = find(_buddies.begin(), _buddies.end(),
			userId);
	if (it != _buddies.end())
		_buddies.erase(it);
}

/*****************************************************************************/

::MyUtil::IntSeq RecentlyOnlineBuddyManagerI::get(::Ice::Int userId,
		::Ice::Int limit, const Ice::Current& current) {
	MCE_INFO("RecentlyOnlineBuddyManagerI::get(" << userId << "," << limit << ")");
	return locateRecentlyOnlineBuddyData(userId)->get_most_recent(limit);
}

::Ice::Int RecentlyOnlineBuddyManagerI::getCount(::Ice::Int userId, const Ice::Current& current) {
	MCE_INFO("RecentlyOnlineBuddyManagerI::getCount(" << userId << ")");
	return locateRecentlyOnlineBuddyData(userId)->get_count();
}

RecentlyOnlineBuddyDataPtr RecentlyOnlineBuddyManagerI::locateRecentlyOnlineBuddyData(::Ice::Int userId) {
        RecentlyOnlineBuddyDataPtr res = ServiceI::instance().findObject<RecentlyOnlineBuddyDataPtr> (
                        CATEGORY_DATA, userId);
        if(res){
                return res;
        }
        res = new RecentlyOnlineBuddyData;
        TaskManager::instance().execute(new CreateTask(userId));
        return res;
}

/*****************************************************************************/

Ice::ObjectPtr RecentlyOnlineBuddyDataFactory::create(Ice::Int id) {
	RecentlyOnlineBuddyDataPtr result = new RecentlyOnlineBuddyData;
	TaskManager::instance().execute(new CreateTask(id));
	return result;
}

/*****************************************************************************/

void RecentlyOnlineBuddyManagerI::add(::Ice::Int userId,
		::Ice::Int friendId, const Ice::Current& current) {
	MCE_INFO("RecentlyOnlineBuddyManagerI::add(" << userId << "," << friendId << ")");
	TaskManager::instance().execute(new AddTask(userId,friendId));
}

void RecentlyOnlineBuddyManagerI::remove(::Ice::Int userId,
		::Ice::Int friendId, const Ice::Current& current) {
	MCE_INFO("RecentlyOnlineBuddyManagerI::remove(" << userId << "," << friendId << ")");
	TaskManager::instance().execute(new RemoveTask(userId,friendId));
}

void RecentlyOnlineBuddyManagerI::rebuild(::Ice::Int userId, const Ice::Current& current) {
	MCE_INFO("RecentlyOnlineBuddyManagerI::rebuild " << userId);
        TaskManager::instance().execute(new CreateTask(userId));
}

/*****************************************************************************/

void RecentlyOnlineBuddyManagerI::somebodyOnline(::Ice::Int theOnlineOne,
		const Ice::Current&) {
	MCE_INFO("RecentlyOnlineBuddyManagerI::somebodyOnline " << theOnlineOne);
	TaskManager::instance().execute(new SomebodyOnlineTask(theOnlineOne));
}

void RecentlyOnlineBuddyManagerI::notifySomebodyOnline(
		const ::MyUtil::IntSeq& buddies, ::Ice::Int theOnlineOne,
		const Ice::Current& current) {
	MCE_INFO("RecentlyOnlineBuddyManagerI::notifySomebodyOnline buddies.size="
			<< buddies.size() << " theOnlineOne:" << theOnlineOne);
	TaskManager::instance().execute(new NotifySomebodyOnlineTask(buddies,
			theOnlineOne));
}

/*****************************************************************************/

void CreateTask::handle(){
	RecentlyOnlineBuddyDataPtr result = new RecentlyOnlineBuddyData;
	try{
		MyUtil::IntSeq buddies =
			BuddyCacheByIdAdapter::instance().getFriendListOrderByIdDesc(_id);
		MyUtil::IntSeq sorted_buddies = OnlineTimeCache::instance().sort(_id,
				buddies);
		MCE_INFO("CreateTask create " << _id << ", buddies.size = " << sorted_buddies.size() );
		result->add_old_online(sorted_buddies);
	}catch(std::exception& e){
		MCE_WARN( "Create::handle " << _id << " std::exception " << e.what());
	}catch(...){
		MCE_WARN( "Create::handle " << _id << " unknown exception.");
	}
	ServiceI::instance().getObjectCache()->addObject(CATEGORY_DATA,_id, result);
}

void AddTask::handle(){
	RecentlyOnlineBuddyDataPtr result = new RecentlyOnlineBuddyData;
	try{
		MyUtil::IntSeq buddies =
			BuddyCacheByIdAdapter::instance().getFriendListOrderByIdDesc(_userId);
		MyUtil::IntSeq sorted_buddies = OnlineTimeCache::instance().sort(_userId,
				buddies);
		result->add_old_online(sorted_buddies);
		result->add_recent_online(_friendId);
		MCE_INFO("AddTask add " << _friendId << " to " << _userId << ", buddies.size = " << result->get_count() );
	}catch(std::exception& e){
		MCE_WARN( "Create::handle " << _userId << " std::exception " << e.what());
	}catch(...){
		MCE_WARN( "Create::handle " << _userId << " unknown exception.");
	}
	ServiceI::instance().getObjectCache()->addObject(CATEGORY_DATA,_userId, result);
}

void RemoveTask::handle(){
	RecentlyOnlineBuddyDataPtr result = new RecentlyOnlineBuddyData;
	try{
		MyUtil::IntSeq buddies =
			BuddyCacheByIdAdapter::instance().getFriendListOrderByIdDesc(_userId);
		MyUtil::IntSeq sorted_buddies = OnlineTimeCache::instance().sort(_userId,
				buddies);
		result->add_old_online(sorted_buddies);
		result->remove_online(_friendId);
		MCE_INFO("RemoveTask remove " << _friendId << " from " << _userId << ", buddies.size = " << result->get_count() );
	}catch(std::exception& e){
		MCE_WARN( "Create::handle " << _userId << " std::exception " << e.what());
	}catch(...){
		MCE_WARN( "Create::handle " << _userId << " unknown exception.");
	}
	ServiceI::instance().getObjectCache()->addObject(CATEGORY_DATA,_userId, result);
}

/*****************************************************************************/

void SomebodyOnlineTask::handle() {
	RecentlyOnlineBuddyAdapter::instance() .notifySomebodyOnline(
			BuddyCacheByIdAdapter::instance().getFriendListOrderByIdDesc(_theOnlineOne),
			_theOnlineOne);
}

void NotifySomebodyOnlineTask::handle() {
	OnlineTimeCache::instance().set(_theOnlineOne, time(NULL));
	for (MyUtil::IntSeq::const_iterator it = _buddies.begin(); it
			!= _buddies.end(); ++it) {
		try {
			RecentlyOnlineBuddyDataPtr obj = ServiceI::instance()
				.findObject<RecentlyOnlineBuddyDataPtr>(CATEGORY_DATA, *it);
			if (obj) obj->add_recent_online(_theOnlineOne);
		} catch (...) {
			MCE_WARN("NotifySomebodyOnlineTask::handle unknown exception");
		}
	}
}

