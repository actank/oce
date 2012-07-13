#include "ILikeItCacheI.h"
#include "UtilCxx/src/Allocator.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include "OceCxxAdapter/src/ILikeItCacheAdapter.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::platform::storage;
using namespace xce::usercache;
using namespace xce::serverstate;
using namespace xce::clusterstate;
using namespace xce::ilikeit;

// ================ MyUtil::initialize =====================
void MyUtil::initialize() {

	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(ILIKEIT_CACHE, "IL", new ILikeItCacheFactoryI, false);
	service.getAdapter()->add(&ILikeItCacheI::instance(), service.createIdentity("M", ""));

	int cluster = service.getCommunicator()->getProperties()->getPropertyAsInt("ILikeItCache.Cluster");
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("ILikeItCache.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ILikeItCache.Interval", 5);
	int syncCacheNum = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ILikeItCache.SyncCache", 1);
	string ctlEndpoint = service.getCommunicator()
		->getProperties()->getPropertyWithDefault("ILikeItCache.ControllerEndpoint", "ControllerILikeItCacheM");

	// 加入向Controller注册
	ServerStateSubscriber::instance().initialize(ctlEndpoint, 
			&ILikeItCacheI::instance(), mod, interval);

	Ice::ObjectPrx localPrx = ServerStateSubscriber::instance().getProxySelf(); // 获取本地proxy	
	ILikeItCacheI::instance().bootstrap(cluster, mod, syncCacheNum, localPrx);
  	
	HeadUrlUtil::instance();
	ILikeItCacheAdapter::instance();
  ServerStateSubscriber::instance().setStatus(0);
}

// =============== ILikeItCacheI主类 =======================
void ILikeItCacheI::bootstrap(int cluster, int mod, int syncCacheNum, Ice::ObjectPrx localPrx) {

	_cluster = cluster;
	_mod = mod;
	_syncCache = (syncCacheNum > 0);
	//MCE_DEBUG("_syncCache..............."<<_syncCache);
	_valid = false;

	if ( localPrx ) {
		_localPrx = ILikeItCachePrx::uncheckedCast(localPrx);
	} else {
		_localPrx = NULL;
	}

	// 初始化Thread
	_syncThread.initialize(_mod, _localPrx);
	if ( _syncCache ) {
		_syncThread.start(128*1024).detach();
	}

	MCE_INFO("bootstrap cluster="<<_cluster << " mod="<<_mod << " syncCache:" << _syncCache);
}

bool ILikeItCacheI::syncCache() {
	return _syncCache;
}

Ice::Int ILikeItCacheI::getLikeCount(const GlobalIdPtr& gid, const Ice::Current&) {
	return locateLikeDataWithoutDB(gid)->users.size();
}

MyUtil::IntSeq ILikeItCacheI::getLikeCache(const GlobalIdPtr& gid, const Ice::Current&) {
	MCE_DEBUG("[ILikeItCacheI::getLikeCache] get : type " 
			<< gid->type << " objectid " << gid->id << " uid ");
	return locateLikeDataWithoutDB(gid)->users;
}

LikeInfoPtr ILikeItCacheI::getLikeDetail(const GlobalIdPtr& gid, ::Ice::Int uid, ::Ice::Int limit, bool showStrangers, const Ice::Current&) {
//	TimeCost tc = TimeCost::create("GetLikeDetail");

	LikeInfoPtr result = new LikeInfo;

	result->detailLimit = limit;
	result->showStrangers = showStrangers;
	result->withFriendList = (limit > 0)?true:false;

	// 获取对象本身所有顶过的用户id
	MyUtil::IntSeq users;
	if(isValid()){
		users = locateLikeDataWithoutDB(gid)->users;
		//tc.step("locateLikeDataWithoutDB");
	} else {
		users = locateLikeData(gid)->users;
		//tc.step("locateLikeData");
	}
	// 获取的指定用户的好友列表(只有获取详情时才取)
	vector<int> friendList;
	if ( result->withFriendList ) {
		result->withFriendList = getFriendList(friendList, uid);
		//tc.step("getFriendList");
	}
	//if(binary_search(users.begin(), users.end(), uid))
	//	result->selfCount = 1;
	//else
	//	result->selfCount = 0;

	// 取得总数
	result->totalCount = users.size();
	result->selfCount = 0;
	if(users.empty()){
		return result;
	}
	if(limit==0){
		result->selfCount =( binary_search(users.begin(), users.end(), uid)? 1:0);
		//tc.step("searchMySelf");
		//result->selfCount = 0;
		MCE_INFO("ILikeItCache::getLikeDetail "<<uid<<" "<<limit<<" "<<result->selfCount);
		return result;
	}

	// 排序、组合
	vector<int> usersSort;
	getCountAndSort(usersSort, result->selfCount, result->friendCount, users, uid, friendList, result->withFriendList, result->showStrangers, limit);
	//tc.step("getCountAndSort");

	// 加入用户详情
	if ( limit > 0 ) {		
		getUserDetail(result->detailList, usersSort);
		//tc.step("getUserDetail");
	}

	return result;
}
LikeInfo2Ptr ILikeItCacheI::getLikeDetail2(const GlobalIdPtr& gid, ::Ice::Int uid, ::Ice::Int limit, bool showStrangers, const Ice::Current&) {
	//TimeCost tc = TimeCost::create("GetLikeDetail2");

	LikeInfo2Ptr result = new LikeInfo2;

	result->detailLimit = limit;
	result->showStrangers = showStrangers;
	result->withFriendList = (limit > 0)?true:false;

	// 获取对象本身所有顶过的用户id
        MyUtil::IntSeq users;
        if(isValid()){
                users = locateLikeDataWithoutDB(gid)->users;
               // tc.step("locateLikeDataWithoutDB");
        } else {
                users = locateLikeData(gid)->users;
               // tc.step("locateLikeData");
        }


	// 获取的指定用户的好友列表(只有获取详情时才取)
	vector<int> friendList;
	if ( result->withFriendList ) {
		result->withFriendList = getFriendList(friendList, uid);
	}
	//if(binary_search(users.begin(), users.end(), uid))
	//	result->selfCount = 1;
	//else
	//	result->selfCount = 0;

	// 取得总数
	result->totalCount = users.size();
	result->selfCount = 0;
	if(users.size()==0){
		return result;
	}
	if(limit==0){
		result->selfCount =( binary_search(users.begin(), users.end(), uid)? 1:0);
		//result->selfCount = 0;
		MCE_INFO("ILikeItCache::getLikeDetail "<<uid<<" "<<limit<<" "<<result->selfCount);
		return result;
	}

	// 排序、组合
	vector<int> usersSort;
	getCountAndSort(usersSort, result->selfCount, result->friendCount, users, uid, friendList, result->withFriendList, result->showStrangers, limit);

	// 加入用户详情
	if ( limit > 0 ) {		
		getUserDetail2(result->detailList, usersSort);
	}

	return result;
}

LikeInfoSeq ILikeItCacheI::getLikeDetailBatch(const GlobalIdSeq& gids, ::Ice::Int uid, ::Ice::Int limit, ::Ice::Int detailNum, const Ice::Current& ctr)  {

	TimeCost tc = TimeCost::create("GetLikeDetailBatch");
	LikeInfoSeq results;
	
	// 获取对象本身所有顶过的用户id
	LikeDataMap likeMap = findLikeDataBatch(gids);
	tc.step("locateLikeDataBatchWithoutDB");
	// 获取的指定用户的好友列表(只有获取详情时才取)
	vector<int> friendList;
	bool withFriends = false;
	if ( (limit > 0) && (detailNum > 0) ) {
		withFriends = getFriendList(friendList, uid);
		tc.step("getFriendList");
	}

	// 依次处理各对象
	for ( size_t i = 0; i < gids.size(); ++i ) {

		LikeInfoPtr result = new LikeInfo();
		
		result->detailLimit = (i < (size_t)detailNum)?limit:0;
		result->showStrangers = (i < (size_t)detailNum)?true:false;
		result->withFriendList = (i < (size_t)detailNum)?withFriends:false;

		LikeDataMap::iterator itL = likeMap.find(GlobalIdTrans::GlobalId2Long(gids[i]));
		if ( itL == likeMap.end() ) {
			result->selfCount = 0;
			result->friendCount = 0;
			result->totalCount = 0;
			results.push_back(result);
			continue;
		}

		// 获取对象本身所有顶过的用户id
		MyUtil::IntSeq users = (*itL).second->users;

		// 取得总数
		result->totalCount = users.size();

		// 排序、组合
		vector<int> usersSort;
		getCountAndSort(usersSort, result->selfCount, result->friendCount, users, uid, friendList, result->withFriendList, result->showStrangers, result->detailLimit);

		// 加入用户详情
		if ( result->detailLimit > 0 ) {		
			getUserDetail(result->detailList, usersSort);
		}

		results.push_back(result);
	}
	tc.step("GetAllDetail");

	return results;
}

LikeInfo2Seq ILikeItCacheI::getLikeDetailBatch2(const GlobalIdSeq& gids, ::Ice::Int uid, ::Ice::Int limit, ::Ice::Int detailNum, const Ice::Current& ctr)  {

	TimeCost tc = TimeCost::create("GetLikeDetailBatch2");
	LikeInfo2Seq results;
	
	// 获取对象本身所有顶过的用户id
	LikeDataMap likeMap = findLikeDataBatch(gids);
	// 获取的指定用户的好友列表(只有获取详情时才取)
	vector<int> friendList;
	bool withFriends = false;
	if ( (limit > 0) && (detailNum > 0) ) {
		withFriends = getFriendList(friendList, uid);
	}

	// 依次处理各对象
	for ( size_t i = 0; i < gids.size(); ++i ) {

		LikeInfo2Ptr result = new LikeInfo2();
		
		result->detailLimit = (i < (size_t)detailNum)?limit:0;
		result->showStrangers = (i < (size_t)detailNum)?true:false;
		result->withFriendList = (i < (size_t)detailNum)?withFriends:false;

		LikeDataMap::iterator itL = likeMap.find(GlobalIdTrans::GlobalId2Long(gids[i]));
		if ( itL == likeMap.end() ) {
			result->selfCount = 0;
			result->friendCount = 0;
			result->totalCount = 0;
			results.push_back(result);
			continue;
		}

		// 获取对象本身所有顶过的用户id
		MyUtil::IntSeq users = (*itL).second->users;

		// 取得总数
		result->totalCount = users.size();

		// 排序、组合
		vector<int> usersSort;
		getCountAndSort(usersSort, result->selfCount, result->friendCount, users, uid, friendList, result->withFriendList, result->showStrangers, result->detailLimit);

		// 加入用户详情
		if ( result->detailLimit > 0 ) {		
			getUserDetail2(result->detailList, usersSort);
		}

		results.push_back(result);
	}

	return results;
}

LikeInfoSimplePtr ILikeItCacheI::getLikeInfoSimple(const GlobalIdPtr& gid, int limit, const Ice::Current&) {

	LikeInfoSimplePtr result = new LikeInfoSimple;
	LikeDataPtr likeData = NULL;
	if(isValid()){
		likeData = locateLikeDataWithoutDB(gid);
	} else {
		likeData = locateLikeData(gid);
	}
	if ( likeData != 0 ) {
		result->totalCount = likeData->users.size();
		if ( limit >= result->totalCount ) {
			result->users = likeData->users;
		} else {
			result->users.assign(likeData->users.begin(), likeData->users.begin()+limit);
		}
	} else {
		result->totalCount = 0;
	}
	return result;
}


LikeInfoSimpleSeq ILikeItCacheI::getLikeInfoSimpleBatch(const GlobalIdSeq& gids, int limit, const Ice::Current&) {

	LikeInfoSimpleSeq result;
	LikeDataMap likeMap = findLikeDataBatch(gids);
	for (GlobalIdSeq::const_iterator it = gids.begin(); it != gids.end(); ++it) {
		long gidLong = GlobalIdTrans::GlobalId2Long(*it);
		LikeDataMap::iterator itL = likeMap.find(gidLong);
		LikeInfoSimplePtr simple = new LikeInfoSimple;
		if ( itL != likeMap.end() ) {
			simple->totalCount = (*itL).second->users.size();
			if ( limit >= simple->totalCount ) {
				simple->users = (*itL).second->users;
			} else {
				simple->users.assign((*itL).second->users.begin(), (*itL).second->users.begin()+limit);
			}
		} else {
			simple->totalCount = 0;
		}
		result.push_back(simple);
	}
	return result;
}

int ILikeItCacheI::isLiked(const GlobalIdPtr& gid, int uid, const Ice::Current&) {

	LikeDataPtr likeData = NULL;
	if(isValid()){
		likeData = locateLikeDataWithoutDB(gid);
	} else {
		likeData = locateLikeData(gid);
	}
	if (likeData != NULL ) {
		return binary_search(likeData->users.begin(), likeData->users.end(), uid)?1:0;
	} else {
		return 0;
	}
}

MyUtil::IntSeq ILikeItCacheI::isLikedBatch(const GlobalIdSeq& gids, int uid, const Ice::Current&) {

	MyUtil::IntSeq result;
	LikeDataMap likeMap = findLikeDataBatch(gids);
	for (GlobalIdSeq::const_iterator it = gids.begin(); it != gids.end(); ++it) {
		long gidLong = GlobalIdTrans::GlobalId2Long(*it);
		int liked = 0;
		LikeDataMap::iterator itL = likeMap.find(gidLong);
		if ( itL != likeMap.end() ) {
			liked = binary_search((*itL).second->users.begin(), (*itL).second->users.end(), uid)?1:0;
		} 
		result.push_back(liked);
	}
	return result;

}

bool ILikeItCacheI::getFriendList(vector<int>& friends, int uid) {

	try {
		friends = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendListAsc(uid, FRIENDS_LIMIT);
	} catch (Ice::Exception& e) {
		return false;
		MCE_WARN("[ILikeItCacheI::getFriends] getFriendList of " << uid << " Ice::Excetpion : " << e.what());
	} catch (std::exception& e) {
		return false;
		MCE_WARN("[ILikeItCacheI::getFriends] getFriendList of " << uid << " std::excetpion : " << e.what());
	} catch (...) {
		return false;
		MCE_WARN("[ILikeItCacheI::getFriends] getFriendList of " << uid << " Excetpion");
	}
	return true;	
}

void ILikeItCacheI::setData(const ::MyUtil::ObjectResultPtr& objects, const Ice::Current& crt) {

	// 不区分belongsToMe, 由客户端在推送时判断
	ServiceI::instance().getObjectCache()->addObjects(ILIKEIT_CACHE, objects->data);
	MCE_INFO("[ILikeItCacheI::setData] set " << objects->data.size() << " data(s), ObjectCache IL -> size: " 
			<<  ServiceI::instance().getObjectCache()->locateCategory(ILIKEIT_CACHE)->size());

}

void ILikeItCacheI::load(const GlobalIdSeq& gids, const Ice::Current& crt) {
	MyUtil::LongSeq gidsMe;
	for (GlobalIdSeq::const_iterator it = gids.begin(); it != gids.end(); ++it) {
		if (belongsMe(*it)) {
			gidsMe.push_back(GlobalIdTrans::GlobalId2Long(*it));
			MCE_DEBUG("ILikeItCacheI::load IS " << *it);
		} else {
			MCE_DEBUG("ILikeItCacheI::load NOT " << *it);
		}
	}
	ServiceI::instance().getObjectCache()->reloadObjects(ILIKEIT_CACHE, gidsMe);
	MCE_INFO("ILikeItCacheI::reload " << gids.size() << " objects. " << gidsMe.size() << " belongs to me");
}

void ILikeItCacheI::addLikeship(const LikeshipPtr& ship, const Ice::Current& ctr) {
	//MCE_DEBUG("valid............"<<isValid());
	if(isValid()){
		addLikeshipValid(ship, ctr);
	} else {
		addLikeshipInValid(ship, ctr);
	}

}

void ILikeItCacheI::addLikeshipValid(const LikeshipPtr& ship, const Ice::Current& ctr) {
//	MCE_DEBUG("addLikeshipValid");
        // 获取相关cache
        LikeDataPtr data = findLikeData(ship->gid);
        if(data){
		//MCE_DEBUG("in MEM");
                vector<int>::iterator it=lower_bound(data->users.begin(), data->users.end(), ship->uid);
                if(it==data->users.end() || *it!=ship->uid){
                        data->users.insert(it, ship->uid);
                        MCE_INFO("[ILikeItCacheI::addLikeship] add " << ship->uid << " to " << ship->gid->type << ":" << ship->gid->id);
                }
        } else {
		//MCE_DEBUG("out MEM");
                LikeDataPtr dataTmp = new LikeData;
                dataTmp->users.push_back(ship->uid);
                ServiceI::instance().getObjectCache()->addObject(ILIKEIT_CACHE, GlobalIdTrans::GlobalId2Long(ship->gid), dataTmp);
		//sleep(5);
	//	LikeDataPtr result =ServiceI::instance().findObject<LikeDataPtr>(ILIKEIT_CACHE, GlobalIdTrans::GlobalId2Long(ship->gid));
	//	MCE_DEBUG(ServiceI::instance().findObject<LikeDataPtr>(ILIKEIT_CACHE, GlobalIdTrans::GlobalId2Long(ship->gid)));
        }
}

void ILikeItCacheI::addLikeshipInValid(const LikeshipPtr& ship, const Ice::Current& ctr) {
   
	//MCE_DEBUG("addLikeshipInValid");
        // 获取相关cache
        LikeDataPtr data = locateLikeData(ship->gid);
        
        vector<int>::iterator it=lower_bound(data->users.begin(), data->users.end(), ship->uid);
        if(it==data->users.end() || *it!=ship->uid){
              data->users.insert(it, ship->uid);
              MCE_INFO("[ILikeItCacheI::addLikeship] add " << ship->uid << " to " << ship->gid->type << ":" << ship->gid->id);
        }
}

void ILikeItCacheI::removeLikeship(const LikeshipPtr& ship, const Ice::Current& ctr) {
	if(isValid()){
		removeLikeshipValid(ship, ctr);
	} else {
		removeLikeshipInValid(ship, ctr);
	}
}

void ILikeItCacheI::removeLikeshipValid(const LikeshipPtr& ship, const Ice::Current& ctr) {

	// 获取相关cache
	LikeDataPtr data = findLikeData(ship->gid);
	if(data){
	        MyUtil::IntSeq::iterator itPos = find(data->users.begin(), data->users.end(), ship->uid);
	        if ( itPos != data->users.end() ) {
	                data->users.erase(itPos);
	                MCE_INFO("[ILikeItCacheI::removeLikeship] remove " << ship->uid << " from " << ship->gid->type << ":" << ship->gid->id);      
 	       }
	}
}

void ILikeItCacheI::removeLikeshipInValid(const LikeshipPtr& ship, const Ice::Current& ctr) {

        // 获取相关cache
        LikeDataPtr data = locateLikeData(ship->gid);
        MyUtil::IntSeq::iterator itPos = find(data->users.begin(), data->users.end(), ship->uid);
        if ( itPos != data->users.end() ) {
                data->users.erase(itPos);
                MCE_INFO("[ILikeItCacheI::removeLikeship] remove " << ship->uid << " from " << ship->gid->type << ":" << ship->gid->id);     
        }
}


void ILikeItCacheI::setValid(bool newState, const Ice::Current& ctr) {
	MCE_INFO("ILikeItCacheI::setValid -> "<< newState);
	IceUtil::RWRecMutex::RLock lock(_validMutex);
	_valid = newState;
	ServerStateSubscriber::instance().setStatus((int)newState);
}

bool ILikeItCacheI::isValid(const Ice::Current& ctr) {
	IceUtil::RWRecMutex::WLock lock(_validMutex);
	return _valid;
}

void ILikeItCacheI::getCountAndSort(vector<int>& usersSort, int& selfCount, int& friendCount, const vector<int>& users, int selfId, const vector<int>& friendList, bool withFriends, bool showStrangers, int limit) {
	
	selfCount =( binary_search(users.begin(), users.end(), selfId)? 1:0);
	MyUtil::IntSeq usersFriends, usersOther;
	int otherSize = 0;
	for ( MyUtil::IntSeq::const_iterator it = users.begin(); it != users.end(); ++it) {
		if((*it)==selfId) {
			continue;
		}
		if ( binary_search(friendList.begin(), friendList.end(), (*it)) ) {
			usersFriends.push_back(*it);			
			if(int(usersFriends.size())>=limit)
					break;
		} else if ( otherSize < limit ) {	// 陌生人列表最多保存limit个,多了没有必要
			usersOther.push_back(*it);
			++otherSize;
			if(otherSize>=limit && !withFriends)
				break;
		}
	}
	friendCount = usersFriends.size();

	// 进行排序
	usersSort.clear();
	if ( limit > 0 ) {		
		// 生成列表
		if ( selfCount > 0 ) {
			usersSort.push_back(selfId);
		}
		size_t FEnd = (usersFriends.size()<(limit-usersSort.size()))?
				(usersFriends.size()):(limit-usersSort.size());
		for ( size_t i = 0; i < FEnd; i++ ) {
			usersSort.push_back(usersFriends[i]);
		}
		if ( ( showStrangers ) && ( usersSort.size() < size_t(limit)) ) {
			size_t OEnd = (usersOther.size()<(limit - usersSort.size()))?
				(usersOther.size()):(limit - usersSort.size());
			for ( size_t i = 0; i < OEnd; i++ ) {
				usersSort.push_back(usersOther[i]);
			}	
		}
	}
}

void ILikeItCacheI::getUserDetail(UserDetailSeq& details, const vector<int>& uids) {

	map<int, UserCachePtr> usersCache;
	map<int, UserDetailPtr> userMap;
	try {
		usersCache = UserCacheAdapter::instance().GetUserCache(uids);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItCacheI::getLikeDetail] getUserCacheList Ice::Excetpion : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItCacheI::getLikeDetail] getUserCacheList std::excetpion : " << e.what());
	} catch (...) {
		MCE_WARN("[ILikeItCacheI::getLikeDetail] getUserCacheList Excetpion");
	}
	MCE_DEBUG("[ILikeItCacheI::getLikeDetail] get " << usersCache.size() << " userdetails for " << uids.size() << " users");

	for ( map<int, UserCachePtr>::iterator it = usersCache.begin(); it != usersCache.end(); ++it) {

		UserDetailPtr detail = new UserDetail();
		detail->uid = it->first;
		detail->name = it->second->name();
		string imgOri = it->second->tinyurl();
		detail->img = HeadUrlUtil::instance().getHeadFullUrl(imgOri);
		// detail->img = imgOri;

		if ( it->first >=  0 ) {
			userMap.insert( make_pair<int, UserDetailPtr>(it->first, detail) );
		}
	}
	for ( vector<int>::const_iterator itUser = uids.begin(); 
			itUser != uids.end(); ++itUser) {
		if ( userMap.find(*itUser) != userMap.end() ) {
			details.push_back(userMap[*itUser]);
		} else {
			UserDetailPtr detail = new UserDetail;
			detail->uid = (*itUser);
			detail->name = "";
			detail->img = "";
			details.push_back(detail);
		}
	}
}

void ILikeItCacheI::getUserDetail2(UserDetail2Seq& details, const vector<int>& uids) {

	map<int, UserCachePtr> usersCache;
	map<int, UserDetail2Ptr> userMap;
	try {
		usersCache = UserCacheAdapter::instance().GetUserCache(uids);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItCacheI::getLikeDetail] getUserCacheList Ice::Excetpion : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItCacheI::getLikeDetail] getUserCacheList std::excetpion : " << e.what());
	} catch (...) {
		MCE_WARN("[ILikeItCacheI::getLikeDetail] getUserCacheList Excetpion");
	}
	MCE_DEBUG("[ILikeItCacheI::getLikeDetail] get " << usersCache.size() << " userdetails for " << uids.size() << " users");

	for ( map<int, UserCachePtr>::iterator it = usersCache.begin(); it != usersCache.end(); ++it) {

		UserDetail2Ptr detail = new UserDetail2;
		detail->uid = it->first;
		detail->name = it->second->name();
		string imgOri = it->second->tinyurl();
		detail->img = HeadUrlUtil::instance().getHeadFullUrl(imgOri);
		MCE_DEBUG("UserId : "<<it->first<<" IsKEEPUSE: "<<it->second->IsKEEPUSE());
		detail->isKeepUse= it->second->IsKEEPUSE();
		// detail->img = imgOri;

		if ( it->first >=  0 ) {
			userMap.insert( make_pair<int, UserDetail2Ptr>(it->first, detail) );
		}
	}
	for ( vector<int>::const_iterator itUser = uids.begin(); 
			itUser != uids.end(); ++itUser) {
		if ( userMap.find(*itUser) != userMap.end() ) {
			details.push_back(userMap[*itUser]);
		} else {
			UserDetail2Ptr detail = new UserDetail2;
			detail->uid = (*itUser);
			detail->name = "";
			detail->img = "";
			detail->isKeepUse= false;
			details.push_back(detail);
		}
	}
}
LikeDataPtr ILikeItCacheI::locateLikeDataWithoutDB(const GlobalIdPtr& gid) {
	MCE_DEBUG("[ILikeItCacheI::locateLikeDataWithoutDB] find " << gid->type << ":" << gid->id 
			<< " as " << GlobalIdTrans::GlobalId2Long(gid));
	LikeDataPtr result=new LikeData;
	LikeDataPtr resultTmp=ServiceI::instance().findObject<LikeDataPtr>(ILIKEIT_CACHE, GlobalIdTrans::GlobalId2Long(gid));
	if(resultTmp){
		return resultTmp;
	}
	return result;
}

LikeDataPtr ILikeItCacheI::findLikeData(const GlobalIdPtr& gid) {
        MCE_DEBUG("[ILikeItCacheI::findLikeData] find " << gid->type << ":" << gid->id
                        << " as " << GlobalIdTrans::GlobalId2Long(gid));
        return ServiceI::instance().findObject<LikeDataPtr>(ILIKEIT_CACHE, GlobalIdTrans::GlobalId2Long(gid));
}


LikeDataPtr ILikeItCacheI::locateLikeData(const GlobalIdPtr& gid) {
        MCE_DEBUG("[ILikeItCacheI::locateLikeData] find " << gid->type << ":" << gid->id
                        << " as " << GlobalIdTrans::GlobalId2Long(gid));
        return ServiceI::instance().locateObject<LikeDataPtr>(ILIKEIT_CACHE, GlobalIdTrans::GlobalId2Long(gid));
}


/*LikeDataMap ILikeItCacheI::locateLikeDataBatchWithoutDB(const GlobalIdSeq& gids) {

	LikeDataMap result;
	vector<long> longGids;
	for ( GlobalIdSeq::const_iterator it = gids.begin(); it != gids.end(); ++it ) {
		longGids.push_back(GlobalIdTrans::GlobalId2Long(*it));
	}
	ObjectResultPtr objResults = ServiceI::instance().getObjectCache()->findObjects(ILIKEIT_CACHE, longGids);
	if(objResults){
	        for ( ObjectMap::iterator it = objResults->data.begin(); it != objResults->data.end(); ++it ) {
                // if ( (*it).second != NULL ) {
                	if ( (*it).second  ) {
                        	LikeDataPtr ptr = LikeDataPtr::dynamicCast((*it).second);
	                        result.insert( make_pair<long, LikeDataPtr>((*it).first, ptr));
        	        }
        	}
	        MCE_DEBUG("[ILikeItCacheI::locateLikeDataBatchWithoutDB] finded " << result.size() << " likedata(s)");
	}
	
	return result;
}
*/
LikeDataMap ILikeItCacheI::findLikeDataBatch(const GlobalIdSeq& gids) {

        LikeDataMap result;
        vector<long> longGids;
        for ( GlobalIdSeq::const_iterator it = gids.begin(); it != gids.end(); ++it ) {
                longGids.push_back(GlobalIdTrans::GlobalId2Long(*it));
        }
	
//	for(vector<long>::iterator it = longGids.begin();it!=longGids.end();++it){
//		MCE_DEBUG("longGids:"<<*it);
//	}

	ObjectResultPtr objResults = NULL;
 
      
	if(isValid()){
		objResults = ServiceI::instance().getObjectCache()->findObjects(ILIKEIT_CACHE, longGids);
	} else {
		objResults = ServiceI::instance().getObjectCache()->locateObjects(ILIKEIT_CACHE, longGids);
	}
	

        if(objResults){
                for ( ObjectMap::iterator it = objResults->data.begin(); it != objResults->data.end(); ++it ) {
		//if ( (*it).second != NULL ) {
                        if ( (*it).second  ) {
                                LikeDataPtr ptr = LikeDataPtr::dynamicCast((*it).second);
                                result.insert( make_pair<long, LikeDataPtr>((*it).first, ptr));
                        }
                }
                MCE_DEBUG("[ILikeItCacheI::locateLikeDataBatchWithoutDB] finded " << result.size() << " likedata(s)");
        }

        return result;
}



bool ILikeItCacheI::belongsMe(const GlobalIdPtr& gid) {
	return ( gid->id % _cluster == _mod );
}

// ================= Factory部分 ==============================
// ---------------- ILikeItCacheFactoryI -------------------
Ice::ObjectPtr ILikeItCacheFactoryI::create(Ice::Long gidLong) {
	
//	TimeCost tc = TimeCost::create("FactoryCreate");

	MCE_DEBUG("[ILikeItCacheFactoryI::create] Loading " << gidLong);
	LikeDataPtr result = new LikeData;
	{
		Statement sql;
		ostringstream tmp;
		tmp << "SELECT " << FIELD_UID  << " FROM " << TABLE_LIKESHIP << (gidLong % TABLE_NUM) 
			<< " WHERE " << FIELD_GlobalId << " = " << gidLong;
		sql << tmp.str();
		MCE_DEBUG("[ILikeItCacheFactoryI::create] sql : " << tmp.str());
		try {
			QueryRunner(DB_DESC_ILIKEIT, CDbRServer).query(sql, SingleLikeshipHandler(result));
			// 排序, 方便直接修改Cache -- 090901 by zhanghan
			sort(result->users.begin(), result->users.end());
		} catch (Ice::Exception& e) {
			MCE_WARN("[ILikeItCacheFactoryI::create(single)] create from sql Ice::Exception : " << e.what());
		} catch (...) {
			MCE_WARN("[ILikeItCacheFactoryI::create(single)] create from sql Ice::Exception ");
		}
	}
	

	// 如果需要同步其他Cache, 则异步同步
	if ( ILikeItCacheI::instance().syncCache() && !result->users.empty()) {
		//MCE_DEBUG("syncCache..............");
		ILikeItCacheI::instance().invokeSync(gidLong, result);
	}
	MCE_DEBUG("[ILikeItCacheFactoryI::create]Loading " << (gidLong/OBJECT_MASK) << ":" << (gidLong%OBJECT_MASK) << " done.");
	return result;
}

MyUtil::ObjectResultPtr ILikeItCacheFactoryI::create(const MyUtil::LongSeq& gidLongs) {

	MCE_DEBUG("Loading " << gidLongs.size() << " objects");

	map<long, LikeDataPtr> dataMap;
	ObjectResultPtr results = new ObjectResult;	
	if (gidLongs.empty()) {
		return results;
	}

	// 将所有gid按模TABLE_NUM分组
	vector< vector<long> > gidGroup(TABLE_NUM);
	for ( size_t i = 0; i < gidLongs.size(); i++ ) {
		gidGroup[gidLongs[i]%TABLE_NUM].push_back(gidLongs[i]);
	}

	// 依次load各组
	for ( size_t group = 0; group < TABLE_NUM; ++group ) { 

		size_t beginPos = 0;
		size_t endPos = 0;
		while ( beginPos < gidGroup[group].size() )
		{
			ostringstream osIdblock;
			endPos = ((beginPos+LOAD_BATCH_SIZE) > gidGroup[group].size())?
				(gidGroup[group].size()):(beginPos+LOAD_BATCH_SIZE);
			for ( size_t i = beginPos; i < endPos; ++i ) {
				if ( i > beginPos )  {
					osIdblock << ",";
				}
				osIdblock << gidGroup[group][i];
			}
			beginPos = endPos;
	
			Statement sql;
			ostringstream tmp;
			tmp << "SELECT " << FIELD_UID  << "," << FIELD_GlobalId << " FROM " << TABLE_LIKESHIP << group 
				<< " WHERE " << FIELD_GlobalId << " IN (" << osIdblock.str() << ")";			
			sql << tmp.str();
			try {
				QueryRunner(DB_DESC_ILIKEIT, CDbRServer).query(sql,  BatchLikeshipHandler(dataMap));
			} catch (Ice::Exception& e) {
				MCE_WARN("[ILikeItCacheFactoryI::create(batch)] create from sql Ice::Exception : " << e.what());
			} catch (...) {
				MCE_WARN("[ILikeItCacheFactoryI::create(batch)] create from sql Ice::Exception ");
			}		
		}

	}

	// 转成ObjectResult结果，如果需要同步其他cache，则异步同步
	for ( map<long, LikeDataPtr>::iterator it = dataMap.begin(); it != dataMap.end(); ++it) {
		sort((*it).second->users.begin(), (*it).second->users.end());
		results->data.insert( make_pair<long, Ice::ObjectPtr>((*it).first, (*it).second) );
		
		//同步
	     LikeDataPtr likeData = (*it).second;
             if(ILikeItCacheI::instance().syncCache()&&!likeData->users.empty()){
	                //MCE_DEBUG("syncCaches......"); 
			ILikeItCacheI::instance().invokeSync((*it).first, likeData);
             }

	}
	// 修正策略:对于没有在数据库读到的结果，也要返回"空对象"，而不是置之不理 -- 090723 by zhanghan
	for ( vector<long>::const_iterator it = gidLongs.begin(); it != gidLongs.end(); ++it ) {
		if ( dataMap.find(*it) == dataMap.end() ) {
			LikeDataPtr ld = new LikeData();
			results->data.insert( make_pair<long, Ice::ObjectPtr>((*it), ld) );
		}
	}
	
	return results;
}

// =================== ResultHandler部分 ================================
// ----------------- SingleLikeshipHandler ------------------------------
bool SingleLikeshipHandler::handle(mysqlpp::Row& row) const {
	
	int uid = (int) row[FIELD_UID.c_str()];
	_result->users.push_back(uid);
	return true;
}

// ----------------- BatchLikeshipHandler ------------------------------
bool BatchLikeshipHandler::handle(mysqlpp::Row& row) const {
	
	int uid = (int) row[FIELD_UID.c_str()];
	long gidLong = (long) row[FIELD_GlobalId.c_str()];

	map<long, LikeDataPtr>::iterator it = _result.find(gidLong);
	if ( it != _result.end() ) {
		(*it).second->users.push_back(uid);
	} else {
		LikeDataPtr ld = new LikeData();
		ld->users.push_back(uid);
		_result.insert( make_pair<long, LikeDataPtr>(gidLong, ld) );
	}
	return true;
}


// =============== Thread ==============================
// ------------- SyncCacheThread ----------------------------
void SyncCacheThread::invoke(long gidLong, const LikeDataPtr& likeData) {
	
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	MCE_DEBUG("[SyncCacheThread::invoke] add a likeData gidLong : " << gidLong);
	_cache.insert( make_pair<long, ObjectPtr>(gidLong, likeData));
	if (_cache.size()>10000) {
		_mutex.notify();
	}
}

void SyncCacheThread::run() {
	while (true) {
		try {
			ObjectResultPtr batch = new ObjectResult;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				if (_cache.size()<10000) {
					_mutex.wait();
					continue;
				}
				//MCE_DEBUG("_cache.size()...............before:"<<_cache.size());
				batch->data.swap(_cache);
			}

			
	/*		for(std::map<long, Ice::ObjectPtr>::iterator it = batch->data.begin();it!=batch->data.end();it++){
				LikeDataPtr ptr = LikeDataPtr::dynamicCast((*it).second);
				MyUtil::IntSeq users=ptr->users;
				for(MyUtil::IntSeq::iterator itSeq = users.begin();itSeq!=users.end();++itSeq){
					MCE_DEBUG("caches............."<<*itSeq);
				}
				
			}  */
			//MCE_DEBUG("_cache.size()......................after:"<<_cache.size());
			//MCE_DEBUG("batch->data.size()......................:"<<batch->data.size());

			ILikeItCacheAdapter::instance().setData(batch, _mod, _localPrx);
			sleep(SYNC_INTERVAL);
			MCE_DEBUG("[SyncCacheThread::run] add a likebatch task for " << batch->data.size() << " like data(s)");
		} catch (Ice::Exception& e) {
			MCE_WARN("[SyncCacheThread::run]InvokeAdd main loop Ice::Exception : " << e.what());
		} catch (std::exception& e) {
			MCE_WARN("[SyncCacheThread::run]InvokeAdd main loop std::excetpion : " << e.what());
		} catch (...) {
			MCE_WARN("[SyncCacheThread::run]InvokeAdd main loop UNKNOWN exception");
		}
	}
}

