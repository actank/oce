#include "ServiceI.h"
#include "FriendFinderByCommonI.h"
#include "FriendFinderReplicaAdapter.h"
#include "BuddyByIdReplicaAdapter.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "../../../util/cpp/IntSeqSerialize.h"
#include "util/cpp/TimeCost.h"

using namespace xce::friendfinder;
using namespace xce::buddy::adapter;
using namespace xce::friendfinder::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(FRIENDFINDER_BYCOMMON_DATA, "FBC",
			new FriendFinderByCommonDataFactory);
	service.getAdapter()->add(&FriendFinderByCommonManagerI::instance(), service.createIdentity(
			"M", ""));

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendFinderByCommon.Mod");
        string endpoint = service.getCommunicator()->getProperties()->getProperty("FriendFinderByCommon.Endpoint");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendFinderByCommon.Interval", 5);

        // 向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerFriendFinderByCommon",
                        &FriendFinderByCommonManagerI::instance(), mod, interval);

        TaskManager::instance().config(TASK_LEVEL_FILL, ThreadPoolConfig(1,100));
        TaskManager::instance().config(TASK_LEVEL_UPDATE, ThreadPoolConfig(1,1));
        service.registerXceObserver(new ServiceSchemaI);
}

void FriendFinderByCommonData::set( ::Ice::Int id, com::xiaonei::service::CommonFriendSeq& cfs ){
	int data_limit = FRIENDFINDER_BYCOMMON_DATA_LIMIT;
	if (--data_limit==0) {
		return;
	}
	std::multimap<int,CommonFriendItem> tmp;
	for( com::xiaonei::service::CommonFriendSeq::iterator itCfs = cfs.begin(); itCfs != cfs.end(); ++itCfs ){
		CommonFriendItem one;
		one.userId = itCfs->userId;
		one.shareCount = itCfs->shares.size();
		tmp.insert( make_pair<int,CommonFriendItem>( one.shareCount, one) );
	} 
	CommonFriendItemSeq tmpSeq;
	for( std::multimap<int,CommonFriendItem>::reverse_iterator itMMap = tmp.rbegin(); itMMap != tmp.rend(); ++itMMap ) {
		if( itMMap->first <= FRIENDFINDER_BYCOMMON_DATA_SHARE_LIMIT ){
			break;
		}
		tmpSeq.push_back(itMMap->second);
		if (--data_limit==0) {
			break;
		}
	}
	{
		IceUtil::RWRecMutex::WLock lock(_mutex);
		_data.swap( tmpSeq );
	}
        TaskManager::instance().execute( new UpdateToDBTask( id, this) );
}

CommonFriendItemSeq FriendFinderByCommonData::getItemsByShared(int limit){
	if( limit == 0){
		return CommonFriendItemSeq();
	}
	CommonFriendItemSeq res;
	{
		IceUtil::RWRecMutex::RLock lock(_mutex);
		for( CommonFriendItemSeq::iterator it = _data.begin(); it != _data.end(); ++it ) {
			res.push_back(*it);
			if (--limit==0) {
				break;
			}
		}
	}
	return res;
}

CommonFriendItemSeq FriendFinderByCommonData::getItemsById(int limit){
	if( limit == 0){
		return CommonFriendItemSeq();
	}
	std::multimap<int,CommonFriendItem> tmp;
	{
		IceUtil::RWRecMutex::RLock lock(_mutex);
		for( CommonFriendItemSeq::iterator it = _data.begin(); it != _data.end(); ++it ) {
			tmp.insert( make_pair<int,CommonFriendItem>( it->userId, *it ) );
		}
	}
	CommonFriendItemSeq res;
        for( std::multimap<int,CommonFriendItem>::reverse_iterator itMMap = tmp.rbegin(); itMMap != tmp.rend(); ++itMMap ) {
                res.push_back(itMMap->second);
                if (--limit==0) {
                        break;
                }
        }
        return res;
}

MyUtil::IntSeq FriendFinderByCommonData::getIdsByShared(int limit){
	if( limit == 0){
		return MyUtil::IntSeq();
	}
	MyUtil::IntSeq res;
	{
		IceUtil::RWRecMutex::RLock lock(_mutex);
		for( CommonFriendItemSeq::iterator it = _data.begin(); it != _data.end(); ++it ) {
			res.push_back(it->userId);
			if (--limit==0) {
				break;
			}
		}
	}
	return res;
}

MyUtil::IntSeq FriendFinderByCommonData::getIdsAsc(int limit){
	if( limit == 0){
		return MyUtil::IntSeq();
	}
	std::map<int,int> tmp;
	{
		IceUtil::RWRecMutex::RLock lock(_mutex);
		for( CommonFriendItemSeq::iterator it = _data.begin(); it != _data.end(); ++it ) {
			tmp.insert( make_pair<int,int>( it->userId, it->userId ) );
		}
	}
	MyUtil::IntSeq res;
        for( std::map<int,int>::iterator itMap = tmp.begin(); itMap != tmp.end(); ++itMap ) {
                res.push_back(itMap->first);
                if (--limit==0) {
                        break;
                }
        }
        return res;
}

MyUtil::IntSeq FriendFinderByCommonData::getIdsWithIn(::MyUtil::IntSeq& friends_asc,int limit){
	if( limit == 0 ){
		return MyUtil::IntSeq();
	}
	CommonFriendItemSeq cfis;
	{
		IceUtil::RWRecMutex::RLock lock(_mutex);
		cfis = _data;
        }
	MyUtil::IntSeq res;
	for( CommonFriendItemSeq::iterator itCfis = cfis.begin(); itCfis != cfis.end(); ++itCfis ){
		MyUtil::IntSeq::iterator it = lower_bound( friends_asc.begin(), friends_asc.end(), itCfis->userId );
		if( it!=friends_asc.end() && (*it)==itCfis->userId ){
			res.push_back(itCfis->userId);
			if( --limit == 0 ){
				break;
			}
		}
	} 
	return res;
}

int FriendFinderByCommonData::size(){
	IceUtil::RWRecMutex::RLock lock(_mutex);
	return _data.size();
}

bool FriendFinderByCommonData::remove(::Ice::Int friendId){
	IceUtil::RWRecMutex::WLock lock(_mutex);
	for( CommonFriendItemSeq::iterator it = _data.begin(); it != _data.end(); ++it ) {
		if( it->userId == friendId ){
			_data.erase(it);
			return true;
		}
	}
	return false;
}

std::string FriendFinderByCommonData::serialize(){
        std::string result;
        IceUtil::RWRecMutex::RLock lock(_mutex);
        result.resize( sizeof(int) * 2 * _data.size() );
        char* p  = (char*)result.data();
        for( CommonFriendItemSeq::iterator itCfis = _data.begin(); itCfis != _data.end(); ++itCfis ){
                *(int*)p = itCfis->userId;       p += sizeof(int);
                *(int*)p = itCfis->shareCount;         p += sizeof(int);
        }
        return result;
}

Ice::ObjectPtr FriendFinderByCommonDataFactory::create(::Ice::Int userId) {
	MCE_INFO("FriendFinderByCommonDataFactory::create(" << userId << ")" );
	FriendFinderByCommonDataPtr obj = new FriendFinderByCommonData();

	Statement sql;
	sql	<< "SELECT commons FROM friendfinder_bycommon_" << userId%100
			<< " WHERE userId =" << userId;
        QueryRunner("friendfinder_bycommon", CDbRServer).query(
                        sql, FriendFinderByCommonDataResultHandler(obj));

        return obj;
}

bool FriendFinderByCommonDataResultHandler::handle(mysqlpp::Row& row) const
{
        const int num = row["commons"].length() / ( sizeof(int) * 2 );
        if( num <= 0 ){
                return true;
        }

        const char* p = row["commons"].c_str();

        //MCE_INFO("#######################" << num);
        for (int i=0; i<num; ++i)
        {
                int userId = *(int*)p;            p += sizeof(int);
                int shareCount = *(int*)p;              p += sizeof(int);

                CommonFriendItem item;
                item.userId = userId;
                item.shareCount = shareCount;

                _result->_data.push_back(item);
        }
        //MCE_INFO("#######################" << num);
        return true;
}

void FriendFinderByCommonManagerI::setCommonFriendSeq(::Ice::Int userId,const com::xiaonei::service::CommonFriendSeq& cfs,
		const Ice::Current& current){
	MCE_INFO( "[FriendFinderByCommonManagerI::setCommonFriendSeq] userId=" << userId << " cfs.size=" << cfs.size()
		<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	TaskManager::instance().execute( new FillTask( userId, cfs) );
}

CommonFriendItemSeq FriendFinderByCommonManagerI::getCommonFriendItemSeq(::Ice::Int userId, ::Ice::Int limit,
		const Ice::Current& current){
	CommonFriendItemSeq res = locateFriendFinderByCommonData(userId)->getItemsByShared(limit);
	MCE_INFO( "[FriendFinderByCommonManagerI::getCommonFriendItemSeq] userId=" << userId << " res.size=" << res.size() << " limit=" << limit
			<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

CommonFriendItemSeq FriendFinderByCommonManagerI::getCommonFriendItemSeqById(::Ice::Int userId, ::Ice::Int limit,
		const Ice::Current& current){
	CommonFriendItemSeq res = locateFriendFinderByCommonData(userId)->getItemsById(limit);
	MCE_INFO( "[FriendFinderByCommonManagerI::getCommonFriendItemSeqById] userId=" << userId << " res.size=" << res.size() << " limit=" << limit
			<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

Ice::Int FriendFinderByCommonManagerI::getCount(::Ice::Int userId,
			const Ice::Current& current){
	Ice::Int size = locateFriendFinderByCommonData(userId)->size();
	MCE_INFO( "[FriendFinderByCommonManagerI::getCount] userId=" << userId << " size=" << size
			<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return size;
}

void FriendFinderByCommonManagerI::removeCommonFriendItem(::Ice::Int userId, ::Ice::Int friendId,
		const Ice::Current& current){
	if( locateFriendFinderByCommonData(userId)->remove(friendId) ){
		MCE_INFO( "[FriendFinderByCommonManagerI::removeCommonFriendItem] userId=" << userId << " friendId=" << friendId
			<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	}
}

void FriendFinderByCommonManagerI::reload(::Ice::Int userId,
		const Ice::Current& current){
	MCE_INFO( "[FriendFinderByCommonManagerI::reload] userId=" << userId
			<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	ServiceI::instance().reloadObject(FRIENDFINDER_BYCOMMON_DATA, userId);
}

MyUtil::IntSeq FriendFinderByCommonManagerI::getFriendsByCommon(::Ice::Int guestId,::Ice::Int hostId,::Ice::Int limit,
		const Ice::Current& current){
	if( limit == 0 ){
		return MyUtil::IntSeq();
	}

	MyUtil::IntSeq buddies = BuddyByIdCacheAdapter::instance().getFriendListAsc(guestId,-1);
/*
	return locateFriendFinderByCommonData(userId)->getIdsWithIn(buddies,limit);
*/

        com::xiaonei::service::CommonFriendSeq cfs = FriendFinderReplicaAdapter::instance().getCommonFriend(hostId,0,100);

	MyUtil::IntSeq res;
	for( com::xiaonei::service::CommonFriendSeq::iterator itCfs = cfs.begin(); itCfs != cfs.end(); ++itCfs ){
		MyUtil::IntSeq::iterator it = lower_bound( buddies.begin(), buddies.end(), itCfs->userId );
		if( it!=buddies.end() && (*it)==itCfs->userId ){
			res.push_back(itCfs->userId);
			if( --limit == 0 ){
				break;
			}
		}
	} 

	MCE_INFO( "[FriendFinderByCommonManagerI::getFriendsByCommon] guestId=" << guestId
			<< " host.friend.size=" << buddies.size()
			<< " guest.cfs.size=" << cfs.size()
			<< " res.size=" << res.size()
			<< " limit=" << limit
			<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

FriendFinderByCommonDataPtr FriendFinderByCommonManagerI::locateFriendFinderByCommonData(::Ice::Int userId) {
	return ServiceI::instance().locateObject<FriendFinderByCommonDataPtr>(FRIENDFINDER_BYCOMMON_DATA, userId);
}

void FillTask::handle() {
	FriendFinderByCommonDataPtr obj = ServiceI::instance().locateObject<FriendFinderByCommonDataPtr>(FRIENDFINDER_BYCOMMON_DATA, _hostId);
	obj->set( _hostId,_cfs );
}

void UpdateToDBTask::handle() {
        ostringstream sqllog;
	Statement sqlw;
/*
	sqlw	<< "INSERT INTO friendfinder_bycommon_" << _hostId%100 << " (userid,commons)"
                << " VALUES (" << _hostId << "," << _obj->serialize() << ")"
		<< " ON DUPLICATE KEY UPDATE commons=VALUES(commons)";
        //MCE_INFO("[UpdateToDBTask::QueryRunner] execute( " << _hostId << ", commons)");
*/
	sqlw	<< "REPLACE INTO friendfinder_bycommon_" << _hostId%100 << " (userid,commons)"
                << " VALUES (" << _hostId << ",'" << _obj->serialize() << "')";
        sqllog << "SAVE " << _obj->size() << " FOR " << _hostId;
	QueryRunner("friendfinder_bycommon", CDbWServer).execute(sqlw,sqllog.str());
}

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
        TaskManager::instance().config(props, "Fill", TASK_LEVEL_FILL, ThreadPoolConfig(1,100));
        TaskManager::instance().config(props, "UpdateToDB", TASK_LEVEL_UPDATE, ThreadPoolConfig(1,1));
}


