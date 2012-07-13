#include "ServiceI.h"
#include "BuddyApplyCacheI.h"
#include <algorithm>
#include <UserCountAdapter.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace ::xce::buddyapplycache;
using namespace ::MyUtil;
using namespace ::com::xiaonei::xce;
using namespace ::xce::usercount::adapter;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.registerObjectCache(BUDDYAPPLYCACHE, "B", new SyncServantFactory(
			new BuddyApplyCacheFactory));

	service.getAdapter()->add(&BuddyApplyCacheManagerI::instance(),
			service.createIdentity("M", ""));

        // 加入向Controller注册
        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyApplyCacheN.Mod");
        string endpoint = service.getCommunicator()->getProperties()->getProperty("BuddyApplyCacheN.Endpoint");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyApplyCacheN.Interval", 5);
	int hardLimit = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyApplyCacheN.HardLimit", 100);

        ServerStateSubscriber::instance().initialize("ControllerBuddyApplyCacheN",
                       &BuddyApplyCacheManagerI::instance(), mod, interval);
        BuddyApplyCacheManagerI::instance().bootstrap(endpoint, mod, hardLimit);
}

//***************************************************************************

void BuddyApplyCacheManagerI::bootstrap(const string& endpoint, int mod, int hardLimit) {
        _endpoint = endpoint;
        _mod = mod;
        _hardLimit = hardLimit;
        MCE_INFO( "[BuddyApplyCacheManagerI::bootstrap] endpoint=" << _endpoint << " mod=" << _mod << " hardLimit=" << _hardLimit);
}

BuddyApplyCacheDataNSeq BuddyApplyCacheManagerI::getBuddyApplyCache(
		::Ice::Int id, ::Ice::Int begin, ::Ice::Int limit,
		const Ice::Current& current) {
	MCE_DEBUG("getBuddyApplyCacheDataN(" << id
			<< ")");
	return locateBuddyApplyCache(id)->get(begin, limit);
}

BuddyApplyCacheWithTotal BuddyApplyCacheManagerI::getBuddyApplyCacheWithTotal(
		::Ice::Int id, ::Ice::Int begin, ::Ice::Int limit,
		const Ice::Current& current) {
	MCE_DEBUG("getBuddyApplyCacheWithTotal(" << id
			<< ")");
	BuddyApplyCacheWithTotal res;
	BuddyApplyCachePtr cache = locateBuddyApplyCache(id);
	res.caches = cache->get(begin, limit);
	res.totalCount = cache->getTotalCount();
	return res;
}

void BuddyApplyCacheManagerI::removeBuddyApplyCache(::Ice::Int id,
		::Ice::Int applicant, const Ice::Current& current) {
	MCE_DEBUG("removeBuddyApplyCache(" << id
			<< "," << applicant << ")");
	locateBuddyApplyCache(id)->remove(applicant);
}

void BuddyApplyCacheManagerI::clear(::Ice::Int id, const Ice::Current& current) {
	MCE_DEBUG("clear(" << id << ")");
	locateBuddyApplyCache(id)->clear();
}

void BuddyApplyCacheManagerI::rebuildBuddyApplyCache(::Ice::Int id, MyUtil::Str2StrMap& props,
		const Ice::Current& current) {
	MCE_DEBUG("rebuildBuddyApplyCache(" << id
			<< ")");
	locateBuddyApplyCache(id)->push_front(props);
	//从数据库reload是异步过程，有一定延迟，因此先加载到内存中
	ServiceI::instance().getObjectCache()->reloadObject(BUDDYAPPLYCACHE, id);
	locateBuddyApplyCache(id)->push_front(props);
}

::Ice::Int BuddyApplyCacheManagerI::getHardLimit(const Ice::Current& current){
	MCE_DEBUG("getHardLimit()");
	return _hardLimit;
}

BuddyApplyCachePtr BuddyApplyCacheManagerI::locateBuddyApplyCache(::Ice::Int id) {
	return ServiceI::instance().locateObject<BuddyApplyCachePtr> (
			BUDDYAPPLYCACHE, id);
}

BuddyApplyCachePtr BuddyApplyCacheManagerI::findBuddyApplyCache(::Ice::Int id) {
	MCE_DEBUG("findBuddyApplyCache(" << id
			<< ")");
	return ServiceI::instance().findObject<BuddyApplyCachePtr> (
			BUDDYAPPLYCACHE, id);
}

//***************************************************************************

BuddyApplyCache::BuddyApplyCache(::Ice::Int ownerId) :
	_ownerId(ownerId),
	_extra(false)
	//_createTime(time(0))
	{
}

void BuddyApplyCache::push_front(MyUtil::Str2StrMap& props){
	BuddyApplyCacheDataN data;
        data.applicant = (int) props["APPLICANT"];
        data.time = (int) props["TIME"];
        data.why = (string) props["WHY"];
        data.statfrom = (string) props["STAT_FROM"];
        data.groupname = (string) props["GROUP_NAME"];
	BuddyApplyCacheDataNSeq::const_iterator it = _buddyApplyCaches.begin();
	if(it->applicant == data.applicant)
		return;
	for( ; 	it != _buddyApplyCaches.end(); ++it ) {
        	if(it->applicant == data.applicant )
		{
			return;
		}
	}
	if(it == _buddyApplyCaches.end()){
		_buddyApplyCaches.insert(_buddyApplyCaches.begin(),data);
		_buddyApplyCaches.pop_back();	
	}	
}

BuddyApplyCacheDataNSeq BuddyApplyCache::get(int begin, int limit) const {
	//time_t now = time(0);
//	MCE_DEBUG("now it is: " << now << "    _createTime is: " << _createTime );
	//if( now - _createTime > 1800 ) {
	//	MCE_DEBUG("now - _createTime is : " << now - _createTime << "   so rebuild " << _ownerId );
	//	ServiceI::instance().getObjectCache()->reloadObject(BUDDYAPPLYCACHE, _ownerId);
	//}
	IceUtil::Mutex::Lock lock(*this);
	MCE_INFO("[get] userId=" << _ownerId << " _buddyApplyCaches.size()=" << _buddyApplyCaches.size());
	BuddyApplyCacheDataNSeq res;
	if(_buddyApplyCaches.empty()){
		TaskManager::instance().execute(new NoApplyTask(_ownerId));
		return res;
	}
	for (BuddyApplyCacheDataNSeq::const_iterator it =
			_buddyApplyCaches.begin(); it != _buddyApplyCaches.end(); ++it) {
		if (begin-- > 0) {
			continue;
		}
		if (limit > 0 && (int)res.size() >= limit) {
			break;
		}
		res.push_back(*it);
	}

	return res;
}

//void BuddyApplyCache::add(BuddyApplyCacheDataN data) {
//	IceUtil::Mutex::Lock lock(*this);
//	_buddyApplyCaches.push_back(data);
//}

void BuddyApplyCache::remove(int applicant) {
	IceUtil::Mutex::Lock lock(*this);
	for (BuddyApplyCacheDataNSeq::iterator it = _buddyApplyCaches.begin(); it
			!= _buddyApplyCaches.end(); ++it) {
		if (it->applicant == applicant) {
			_buddyApplyCaches.erase(it);
			break;
		}
	}
	if( _extra && _buddyApplyCaches.size() <= 70 ) {
		ServiceI::instance().getObjectCache()->reloadObject(BUDDYAPPLYCACHE, _ownerId);
	}
}

void BuddyApplyCache::clear() {
	IceUtil::Mutex::Lock lock(*this);
	_buddyApplyCaches.clear();
}

//void BuddyApplyCache::sort() {
//	IceUtil::Mutex::Lock lock(*this);
//	std::sort(_buddyApplyCaches.begin(), _buddyApplyCaches.end(), greater);
//}

int BuddyApplyCache::getTotalCount() {
	IceUtil::Mutex::Lock lock(*this);
	if(_extra) {
		return 100;
	}
	return _buddyApplyCaches.size();
}

void BuddyApplyCache::setExtra(bool extra) {
	IceUtil::Mutex::Lock lock(*this);
	_extra = extra;
}

bool BuddyApplyCache::hasExtra() {
	IceUtil::Mutex::Lock lock(*this);
	return _extra;
}

Ice::ObjectPtr BuddyApplyCacheFactory::create(::Ice::Int id) {
	MCE_DEBUG("BuddyApplyCacheFactory::create(" << id << ")");
	BuddyApplyCachePtr buddyApplyCacheResult = new BuddyApplyCache(id);
	Statement sql;
	sql
			<< "SELECT applicant,UNIX_TIMESTAMP(time) AS unix_time,why,stat_from,group_name"
			<< " FROM buddyapply_accpeter_" << abs(id % 100) << " WHERE accepter = '" << id << "'"
			<< " ORDER BY time DESC"
			<< " LIMIT 100";
	BuddyApplyCacheResultHandlerI handler(buddyApplyCacheResult);
	QueryRunner("buddyapplycache_apply", CDbRServer).query(sql, handler);
	if( buddyApplyCacheResult->getTotalCount() == 100 ) {
		buddyApplyCacheResult->setExtra(true);
	} else {
		buddyApplyCacheResult->setExtra(false);
	}
/*BuddyCore do this work now
	TaskManager::instance().execute(new SetGuestRequestCountTask(id, buddyApplyCacheResult->getTotalCount()));
*/
	return buddyApplyCacheResult;
}

void BuddyApplyCacheFactory::SetGuestRequestCountTask::handle() {
	int oldValue = UserCountAdapter::instance().get(_userId,3);
	if (oldValue != _newValue) {
		UserCountAdapter::instance().set(_userId,3,_newValue);
	}
}

//Ice::ObjectPtr BuddyApplyCacheFactory::delegate(::Ice::Long id) {
//	MCE_DEBUG( "here in BuddyApplyCacheFactory::delegate(" << id << ")" );
//	BuddyApplyCachePtr result = new BuddyApplyCache(id);
//	return result;
//}

//***************************************************************************


BuddyApplyCacheResultHandlerI::BuddyApplyCacheResultHandlerI(
		const BuddyApplyCachePtr& result) :
	_result(result) {
}

bool BuddyApplyCacheResultHandlerI::handle(mysqlpp::Row& row) const {
	BuddyApplyCacheDataN data;
	data.applicant = (int) row["applicant"];
	data.time = (int) row["unix_time"];
	data.why = (string) row["why"];
	data.statfrom = (string) row["stat_from"];
	data.groupname = (string) row["group_name"];
	_result->_buddyApplyCaches.push_back(data);
	return true;
}

//***************************************************************************
void NoApplyTask::handle(){
	try {
		MCE_DEBUG("[get] userId=" << _userId << " Set UserCount to 0");
		UserCountAdapter::instance().set(_userId,3,0);
	} catch (Ice::Exception& e) {
		MCE_WARN("UserCountAdapter Ice::Exception " << e);
	} catch (std::exception & e) {
		MCE_WARN("UserCountAdapter Ice::Exception "
				<< e.what());
	} catch (...) {
		MCE_WARN("UserCountAdapter Ice::Exception ");
	}
}
