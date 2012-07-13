#include "OnlineManagerI.h"
#include "BuddyCoreAdapter.h"
#include "OnlineStatSyncAdapter.h"
#include "PresenceMsgHandlerAdapter.h"
#include "BuddyList.h"
#include "XmppTools.h"
#include "ConnectionQuery.h"
#include "OnlineCenterAdapter.h"
#include "CommonTask.h"
#include "OnlineBitmapAdapter.h"

using namespace talk::online;
using namespace talk::online::adapter;
using namespace com::xiaonei::service::buddy;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace talk::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace talk::onlinebitmap;
//----------------------------------------------------

//int StatFunc::count = 0;
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(OBS, "OBS", new BuddyListFactory);
	service.getAdapter()->add(&OnlineManagerI::instance(), service.createIdentity("M", ""));
	
	int index = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Service."+ServiceI::instance().getName()+".Index", -1);
	TaskManager::instance().scheduleRepeated(new OnlineSizeStatTask());
	if(index == 0){
		TaskManager::instance().scheduleRepeated(&(OnlineDurationStatTask::instance()));
	}
	TaskManager::instance().scheduleRepeated(new ScoreAddTask(index));
	TaskManager::instance().scheduleRepeated(&(WebOnlineEvictorTask::instance()));
}

//----------------------------------------------------
Ice::ObjectPtr BuddyListFactory::create(Ice::Int id) {
	//MCE_WARN("BuddyListFactory::create --> " << id);
	BuddyListPtr ptr = new BuddyList(id);
	if (ptr->load()) {
		return ptr;
	} else {
		return 0;
	}
}
ObjectResultPtr BuddyListFactory::create(const MyUtil::IntSeq& ids) {
	//MCE_WARN("BuddyListFactory::create --> size=" << ids.size());
	ObjectResultPtr result = new ObjectResult;
	for (size_t i = 0; i < ids.size(); ++i) {
		Ice::ObjectPtr obj = create(ids.at(i));
		if (obj) {
			result->data[ids.at(i)] = obj;
		}
	}
	return result;
}

//----------------------------------------------------

OnlineManagerI::OnlineManagerI() {
	_pool.resize(kUserPoolCount);
	for (int i = 0; i < kUserPoolCount; ++i) {
		_pool[i] = new UserPool();
	}

  sync_bitmap_thread_.start(128 * 1024).detach();
}

UserPoolPtr OnlineManagerI::getPool(int userId){
	if (userId>=0) {
    // TODO : 90% 的位置, 都是空的
		return _pool[userId % kUserPoolCount];
	} else {
		MCE_WARN("OnlineManagerI::getPool --> call getPool and userid:"<<userId<<" is < 0");
		return 0;
	}
}

void OnlineManagerI::scoreCheck(int index){
	MCE_INFO("OnlineManagerI::scoreCheck  index =  " << index);
	for(int i = 0; i < kUserPoolCount;++i){
		_pool[i]->scoreCheck(index);
	}
}

bool OnlineManagerI::presence(const JidPtr& jid, OnlineStat stat, const string& ver, const string& ext, const Str2StrMap& paras, const Ice::Current&){
	TimeStat ts;
	//MCE_WARN("OnlineManagerI::presence --> " << jidToString(jid));
        MyUtil::Str2StrMap::const_iterator it = paras.begin();
        for(; it!=paras.end(); ++it){
                MCE_INFO("~~~~~~~~~~~~ " << jid->userId << " " << it->first << ":" << it->second);
        }

	StatFunc statF("OnlineManagerI::presence --> ***");
	if(jid){
		MCE_INFO("~~OnlineManagerI::presence --> " << jidToString(jid)
				<< " state:" << stat
				<< " ver:" << ver
				<< " ext:" << ext
				<< " paras size:" << paras.size());
	}
	UserPoolPtr pool = getPool(jid->userId);
	if (!pool) {
		return false;
	}
	OnlineStat oldStat = pool->getUserOnlineStat(jid->userId);

	AvatarPtr avatar = new Avatar;
	avatar->jid = jid;
	avatar->stat = stat;
	avatar->ver = ver;
	avatar->ext = ext;
	avatar->stamp = time(NULL);
	if(jid->endpoint == "web"){
		WebOnlineEvictorTask::instance().push(jid->userId);
		//only notify, do not update
		TaskManager::instance().execute(new PresenceTask(avatar, oldStat, oldStat, paras));
		FunStatManager::instance().Stat("OnlineManagerI::presence", ts.getTime(), ts.getTime() >= FUNTIMEOUT);
		return true;
	}

	if(!pool->add(avatar)){
		MCE_WARN("OnlineManagerI::presence --> add to UserOnlineMap error" << jidToString(jid));
		return false;
	}
	try{
		MCE_DEBUG("OnlineManagerI::presence --> notify OnlineStatDispatch, " << jidToString(jid));
		OnlineStatSyncAdapter::instance().presence(jid, stat, ver, ext);
	}catch(Ice::Exception& e){
		MCE_WARN("OnlineManagerI::presence --> notify OnlineStatDispatch error, jid="
				<< jidToString(jid) << " stat=" << stat << " ver=" << ver << "ext=" << ext);
		MCE_WARN("OnlineManagerI::presence --> ice error, " << e);
	}

	OnlineStat newStat = pool->getUserOnlineStat(jid->userId);
	TaskManager::instance().execute(new PresenceTask(avatar, oldStat, newStat, paras));

  // 同步到 OnlineBitmap
	UserOnlineTypePtr userOnlineTypePtr = new UserOnlineType();
	userOnlineTypePtr->userId = jid->userId;
	//userOnlineTypePtr->onlineType = pool->getUserOnlineType(jid->userId);
	userOnlineTypePtr->onlineType = pool->getUserBitmapValue(jid->userId);
  SyncBitmapDataPtr sync = new SyncBitmapData();
  sync->stateSeq.push_back(userOnlineTypePtr);
  sync->userPoolIndex = -1; // 不同步userId不在stateSeq中的user
	try{
		MCE_DEBUG("OnlineManagerI::presence --> SyncBitmap");
		//OnlineBitmapAdapter::instance().setUserStatByOnlineType(userOnlineTypePtr);
		OnlineBitmapAdapter::instance().SyncBitmap(sync);
	}
	catch(Ice::Exception& e){
		MCE_WARN("OnlineManagerI::presence-->OnlineBitmapAdapter::SyncBitmap-->uid:" << jid->userId);
	}

	FunStatManager::instance().Stat("OnlineManagerI::presence", ts.getTime(), ts.getTime() >= FUNTIMEOUT);
	return true;
}

void OnlineManagerI::notify(const BuddyOnlineNotifySeq& notifies,Ice::Int index,  const Ice::Current&){
	//MCE_WARN("OnlineManagerI::notify --> " << notifies.size());
	StatFunc statF("OnlineManagerI::notify --> ***");
	for (size_t i = 0; i < notifies.size(); ++i) {
		UserPoolPtr pool = getPool(notifies.at(i)->host->jid->userId);
		if (!pool) {
			continue;
		}
		bool self = (notifies.at(i)->host->jid->userId %OnlineCenterAdapter::instance().getCluster()) == index;

		MCE_DEBUG("OnlineManagerI::notify --> host=" << jidToString(notifies.at(i)->host->jid));
		if(notifies.at(i)->host->stat != OFFLINE){
			MCE_DEBUG("OnlineManagerI::notify --> online notify");
			//UserPtr user = pool->add(notifies.at(i)->host);
			notifies.at(i)->host->stamp = time(NULL);
			if((!self) && (!pool->add(notifies.at(i)->host))){
				MCE_WARN("OnlineManagerI::notify --> add to UserOnlineMap error" << jidToString(notifies.at(i)->host->jid));
				continue;
			}
			MCE_DEBUG("OnlineManagerI::notify --> buddies count=" << notifies.at(i)->buddyList.size());
			for(size_t j = 0; j < notifies.at(i)->buddyList.size(); ++j){
				BuddyListPtr ptr = ServiceI::instance().findObject<BuddyListPtr>(OBS, notifies.at(i)->buddyList.at(j));
				if (ptr) {
					ptr->join(notifies.at(i)->host->jid->userId);
				}
			}
		}else{
			MCE_DEBUG("OnlineManagerI::notify --> offline notify, buddies=" << notifies.at(i)->buddyList.size());
			if(!self){
				pool->remove(notifies.at(i)->host->jid);
			}
			if(!pool->has(notifies.at(i)->host->jid->userId)){
				for(size_t j = 0; j < notifies.at(i)->buddyList.size(); ++j){
					BuddyListPtr ptr = ServiceI::instance().findObject<BuddyListPtr>(OBS, notifies.at(i)->buddyList.at(j));
					if (ptr) {
						ptr->leave(notifies.at(i)->host->jid->userId);
					}
				}
			}
		}
	}
}

void OnlineManagerI::offline(const JidPtr& jid, const Ice::Current&){
	//MCE_WARN("OnlineManagerI::offline --> " << jidToString(jid));
	TimeStat ts;
	StatFunc statF("OnlineManagerI::offline --> ***");
	if(jid){
		MCE_INFO("~~OnlineManagerI::offline --> " << jidToString(jid));
	}
	UserPoolPtr pool = getPool(jid->userId);
	if (!pool) {
		return;
	}

	OnlineStat oldStat = pool->getUserOnlineStat(jid->userId);
	pool->remove(jid);
	OnlineStat newStat = pool->getUserOnlineStat(jid->userId);
	try{
		MCE_DEBUG("OnlineManagerI::offline --> notify OnlineStatDispatch, " << jidToString(jid));
		OnlineStatSyncAdapter::instance().offline(jid);
	}catch(Ice::Exception& e){
		MCE_WARN("OnlineManagerI::offline --> notify OnlineStatDispatch error, jid=" << jidToString(jid));
		MCE_WARN("OnlineManagerI::offline --> ice error, " << e);
	}
	AvatarPtr avatar = new Avatar;
	avatar->jid = jid;
	avatar->stat = OFFLINE;
	TaskManager::instance().execute(new PresenceTask(avatar, oldStat, newStat, Str2StrMap()));
	

  UserOnlineTypePtr userOnlineTypePtr = new UserOnlineType();
  userOnlineTypePtr->userId = jid->userId;
  userOnlineTypePtr->onlineType = pool->getUserOnlineType(jid->userId);

  SyncBitmapDataPtr sync = new SyncBitmapData();
  sync->stateSeq.push_back(userOnlineTypePtr);
  sync->userPoolIndex = -1; // 不同步userId不在stateSeq中的user

  try{
    MCE_DEBUG("OnlineManagerI::offline --> SyncBitmap()");
    //OnlineBitmapAdapter::instance().setUserStatByOnlineType(userOnlineTypePtr);
    OnlineBitmapAdapter::instance().SyncBitmap(sync);
  } catch(Ice::Exception& e) {
		MCE_WARN("offline OnlineBitmapAdapter::SyncBitmap() error : " << e);
  }
	FunStatManager::instance().Stat("OnlineManagerI::offline", ts.getTime(), ts.getTime() >= FUNTIMEOUT);
}

Ice::Int OnlineManagerI::getBuddyCount(Ice::Int userId, const Ice::Current&) {
	return getBuddyStatSeq(userId).size();
//StatFunc statF("OnlineManagerI::getBuddyCount --> ***");
//BuddyListPtr ptr = ServiceI::instance().locateObject<BuddyListPtr>(OBS, userId);
//if (ptr) {
//	return ptr->size();
//} else {
//	return 0;
//}
}

void OnlineManagerI::ReloadBitmapByUserPool(Ice::Int pool_index, const Ice::Current&){
  MCE_INFO("OnlineManagerI::ReloadBitmapByUserPool --> pool_index = " << pool_index);
  sync_bitmap_thread_.ReloadUserPool(pool_index);
}

//void OnlineManagerI::updateOnlineBitmap(Ice::Int serverIndex, const Ice::Current&){
//  return;
//  //sync_bitmap_thread_.push();
//  sync_bitmap_thread_.ReloadBitmap(serverIndex);
//
//  MCE_DEBUG("OnlineManagerI::updateOnlineBitmap --> serverIndex = " << serverIndex );
//
//  // 该task中, 调用 setOnlineBitmapStat
//	//TaskManager::instance().execute(new UpdateOnlineBitmapTask(serverIndex));
//}

//void OnlineManagerI::setOnlineBitmapStat(int serverIndex){
//	MCE_INFO("OnlineManagerI::setOnlineBitmapStat --> serverIndex = " << serverIndex);
//	vector<UserPoolPtr>::iterator it = _pool.begin();
//        UserOnlineTypeSeq seq;
//        for(; it != _pool.end(); ++it){
//                UserOnlineTypeSeq tmp = (*it)->getOnlineUsersByServerIndex(serverIndex);
//                seq.insert(seq.end(), tmp.begin(), tmp.end());
//        }
//        MCE_DEBUG("UpdateOnlineBitmapTask::handle --->  UserOnlineTypeSeq seq.size() == " << seq.size());
//        try{
//                OnlineBitmapAdapter::instance().load(seq);
//        }catch(Ice::Exception& e){
//                MCE_WARN("PresenceTask::handle --> serUserStatBySeq error, " << e);
//        }
//}


UserOnlineTypeSeq OnlineManagerI::getBuddyStatSeq(Ice::Int userId,
		const Ice::Current&) {
	StatFunc statF("OnlineManagerI::getBuddyStatSeq --> ***");
	//MCE_WARN("OnlineManagerI::getBuddyStatSeq --> " << userId);
	UserOnlineTypeSeq seq;
	BuddyListPtr ptr = ServiceI::instance().locateObject<BuddyListPtr>(OBS, userId);
	if (ptr) {
		set<int> userIds = ptr->buddyIdSet();
		for(set<int>::iterator it=userIds.begin(); it!=userIds.end(); ++it){
			UserPoolPtr pool = getPool(*it);
			if (!pool) {
				continue;
			}
			int onlineType = pool->getUserOnlineType(*it);
			if(onlineType != UNKNOWTYPE){
				UserOnlineTypePtr  info = new UserOnlineType();
				info->userId = *it;
				info->onlineType = onlineType;
				seq.push_back(info);
			}
		}
		return seq;
	}
	return UserOnlineTypeSeq();
}

UserOnlineTypeSeq OnlineManagerI::getUsersStatSeq(const MyUtil::IntSeq& ids,
		const Ice::Current&) {
	StatFunc statF("OnlineManagerI::getBuddyStatSeq --> ***");
	//MCE_WARN("OnlineManagerI::getUsersStatSeq --> " << ids.size());
	UserOnlineTypeSeq seq;
	for(IntSeq::const_iterator it=ids.begin(); it!=ids.end(); ++it){
		UserPoolPtr pool = getPool(*it);
		if (!pool) {
			continue;
		}
		int onlineType = pool->getUserOnlineType(*it);
		if(onlineType != UNKNOWTYPE){
			UserOnlineTypePtr  info = new UserOnlineType();
			info->userId = *it;
			info->onlineType = onlineType;
			seq.push_back(info);
		}
	}
	return seq;
}

MyUtil::IntSeq OnlineManagerI::getBuddyIds(Ice::Int userId, const Ice::Current&) {
	//MCE_WARN("OnlineManagerI::getBuddyIds --> " << userId);
	StatFunc statF("OnlineManagerI::getBuddyIds --> ***");
	IntSeq seq;
	BuddyListPtr ptr = ServiceI::instance().locateObject<BuddyListPtr>(OBS, userId);
	if (ptr) {
		set<int> userIds = ptr->buddyIdSet();
		for(set<int>::iterator it=userIds.begin(); it!=userIds.end(); ++it){
			seq.push_back(*it);
		}
	}
	return seq;//ServiceI::instance().locateObject<BuddyListPtr>(OBS, userId)->buddyIds();
}

com::xiaonei::talk::common::JidSeqMap OnlineManagerI::getBuddyJids(
		Ice::Int userId, Ice::Int onlineType, const Ice::Current&) {
	//MCE_WARN("OnlineManagerI::getBuddyJids --> " << userId);
	StatFunc statF("OnlineManagerI::getBuddyJids --> ***");
	JidSeqMap jmap;
	BuddyListPtr ptr = ServiceI::instance().locateObject<BuddyListPtr>(OBS, userId);
	if (ptr) {
		//MyUtil::IntSeq userIds = ptr->buddyIds();
		set<int> userIds = ptr->buddyIdSet();
		for(set<int>::iterator it=userIds.begin(); it!=userIds.end(); ++it){
			UserPoolPtr pool = getPool(*it);
			if (!pool) {
				continue;
			}
			jmap[*it] = pool->getUserJids(*it, onlineType);
		}
		return jmap;
	}
	return jmap;
}

com::xiaonei::talk::common::JidSeqMap OnlineManagerI::getBuddyJidsAndSelf(
		Ice::Int userId, Ice::Int buddyOnlineType, Ice::Int selfOnlineType,const Ice::Current&) {
	//MCE_WARN("OnlineManagerI::getBuddyJidsAndSelf --> " << userId);
	StatFunc statF("OnlineManagerI::getBuddyJidsAndSelf --> ***");
	TimeStat ts;
	JidSeqMap jmap = getBuddyJids(userId,buddyOnlineType);
	jmap[userId] = getUserJids(userId,selfOnlineType);
	FunStatManager::instance().Stat("OnlineManagerI::getBuddyJidsAndSelf", ts.getTime(), ts.getTime() >= FUNTIMEOUT);
	return jmap;
}

AvatarMap OnlineManagerI::getBuddyAvatars(Ice::Int userId, Ice::Int onlineType, const Ice::Current&){
	//MCE_WARN("OnlineManagerI::getBuddyAvatars --> " << userId);
	StatFunc statF("OnlineManagerI::getBuddyAvatars --> ***");
	AvatarMap amap;
	BuddyListPtr ptr = ServiceI::instance().locateObject<BuddyListPtr>(OBS, userId);
	if (ptr) {
		set<int> userIds = ptr->buddyIdSet();
		MCE_DEBUG("OnlineManagerI::getBuddyAvatars --> online size" << userIds.size());
		for(set<int>::iterator it=userIds.begin(); it!=userIds.end(); ++it){
			UserPoolPtr pool = getPool(*it);
			if (!pool) {
				continue;
			}
			amap[*it] = pool->getUserAvatars(*it, onlineType);
		}
		return amap;
	}
	return amap;

}

AvatarMap OnlineManagerI::getBuddyAvatarsAndSelf(Ice::Int userId,Ice::Int buddyOnlineType, Ice::Int selfOnlineType, const Ice::Current&) {
	//MCE_WARN("OnlineManagerI::getBuddyAvatarsAndSelf --> " << userId);
	StatFunc statF("OnlineManagerI::getBuddyAvatarsAndSelf --> ***");
	AvatarMap amap = getBuddyAvatars(userId,buddyOnlineType);
	amap[userId] = getUserAvatars(userId,selfOnlineType);
	return amap;
}

AvatarSeq OnlineManagerI::getUserAvatars(Ice::Int userId,Ice::Int onlineType, const Ice::Current&){
	//MCE_WARN("OnlineManagerI::getUserAvatars --> " << userId);
	StatFunc statF("OnlineManagerI::getUserAvatars --> ***");
	UserPoolPtr pool = getPool(userId);
	if (!pool) {
		return AvatarSeq();
	}
	//
	return pool->getUserAvatars(userId,onlineType);
}
AvatarPtr OnlineManagerI::getAvatar(const JidPtr& jid, const Ice::Current&){
	//MCE_WARN("OnlineManagerI::getAvatar --> " << jidToString(jid));
	StatFunc statF("OnlineManagerI::getAvatar --> ***");
	UserPoolPtr pool = getPool(jid->userId);
	if (!pool) {
		return 0;
	}
	//
	return pool->getAvatar(jid);
	//return UserOnlineMap::instance().getAvatar(jid);
}
AvatarSeq OnlineManagerI::getAvatars(const JidSeq& jids, const Ice::Current&){
	//MCE_WARN("OnlineManagerI::getAvatars --> ");
	StatFunc statF("OnlineManagerI::getAvatars --> ***");
	AvatarSeq seq;
	for(size_t i=0; i<jids.size(); i++){
		UserPoolPtr pool = getPool(jids.at(i)->userId);
		if (!pool) {
			continue;
		}
		AvatarPtr avatar = pool->getAvatar(jids.at(i));
		if(avatar){
			seq.push_back(avatar);
		}
	}
	MCE_DEBUG("UserOnlineMap::getAvatars --> size " << seq.size());
	return seq;

	//return UserOnlineMap::instance().getAvatars(jids);
}

JidSeq OnlineManagerI::getUserJids(Ice::Int userId, Ice::Int onlineType, const Ice::Current&){
	//MCE_WARN("OnlineManagerI::getUserJids --> " << userId);
	StatFunc statF("OnlineManagerI::getUserJids --> ***");
	TimeStat ts;
	UserPoolPtr pool = getPool(userId);
	if (!pool) {
		return JidSeq();
	}
	FunStatManager::instance().Stat("OnlineManagerI::getUserJids", ts.getTime(), ts.getTime() >= FUNTIMEOUT);
	return pool->getUserJids(userId,onlineType);
}
JidSeqMap OnlineManagerI::getUsersJids(const MyUtil::IntSeq& ids,Ice::Int onlineType, const Ice::Current&){
	//MCE_WARN("OnlineManagerI::getUsersJids --> ");
	StatFunc statF("OnlineManagerI::getUsersJids --> ***");
	TimeStat ts;
	JidSeqMap jmap;
	for(size_t i=0; i<ids.size(); i++){
		UserPoolPtr pool = getPool(ids.at(i));
		if (!pool) {
			continue;
		}
		JidSeq seq = pool->getUserJids(ids.at(i),onlineType);
		if(!seq.empty()){
			jmap[ids.at(i)] = seq;
		}
	}
	MCE_DEBUG("UserOnlineMap::getUsersJids --> size " << jmap.size());
	FunStatManager::instance().Stat("OnlineManagerI::getUsersJids", ts.getTime(), ts.getTime() >= FUNTIMEOUT);
	return jmap;

//	return UserOnlineMap::instance().getUsersJids(ids);
}

AvatarMap OnlineManagerI::getUsersAvatars(const MyUtil::IntSeq& ids,Ice::Int onlineType, const Ice::Current& ){
	//MCE_WARN("OnlineManagerI::getUsersAvatars --> size" << ids.size());
	StatFunc statF("OnlineManagerI::getUsersAvatars --> ***");
	AvatarMap amap;
	for(size_t i=0; i<ids.size(); i++){
		UserPoolPtr pool = getPool(ids.at(i));
		if (!pool) {
			continue;
		}
		amap[ids.at(i)] = pool->getUserAvatars(ids.at(i),onlineType);
	}
	MCE_DEBUG("UserOnlineMap::getUsersAvatars --> size " << amap.size());
	return amap;
}

AvatarSeq OnlineManagerI::getOldestAvatars(int index, int beg, int limit, int type, const Ice::Current&){
	if(index >=0 && index < kUserPoolCount && beg>=0 && limit>0){
		return getPool(index)->getOldestAvatars(beg, limit, type);
	}
	return AvatarSeq();
}

UserOnlineTypeSeq OnlineManagerI::getRandomOnlineUsers(int count, const Ice::Current&){
	UserOnlineTypeSeq seq;
	//srand(time(NULL));
	MCE_INFO("OnlineManagerI::getRandomOnlineUsers --> count:" << count);
	for(int i=0; i < 20; ++i){
		int index = rand()%kUserPoolCount;
		UserOnlineTypeSeq tmp = getPool(index)->getOnlineUsers();
		seq.insert(seq.end(), tmp.begin(), tmp.end());
		if(seq.size() >= count || seq.size() > 1000){
			break;
		}
	}
	int size = seq.size();
	for(int i=0; i < (size - count); i++){
		seq.pop_back();
	}
	return seq;
}

void OnlineManagerI::addBuddy(Ice::Int host, Ice::Int guest, const Ice::Current&){
	//MCE_WARN("OnlineManagerI::addBuddy --> host=" << host << " guest=" << guest);
	StatFunc statF("OnlineManagerI::addBuddy --> ***");
	UserPoolPtr pool = getPool(guest);
	if (!pool) {
		return;
	}
	BuddyListPtr ptr = ServiceI::instance().findObject<BuddyListPtr>(OBS, host);
	if (ptr) {
		if(pool->getUserJids(guest,WP_ONLINE|IM_ONLINE|PHONE_ONLINE).empty()){
			return;
		}
		ptr->join(guest);
	}
}

void OnlineManagerI::removeBuddy(Ice::Int host, Ice::Int guest, const Ice::Current&){
	//MCE_WARN("OnlineManagerI::removeBuddy --> host=" << host << " guest=" << guest);
	StatFunc statF("OnlineManagerI::removeBuddy --> ***");
	BuddyListPtr ptr = ServiceI::instance().findObject<BuddyListPtr>(OBS, host);
	if (ptr) {
		ptr->leave(guest);
	}
}

void OnlineManagerI::closeProxyServer(const string& endpoint,bool isBroadcast, const Ice::Current&){
	//MCE_WARN("call closeProxyServer Task  -->endpoint:"<<endpoint);
	StatFunc statF("OnlineManagerI::closeProxyServer --> ***");
	MyUtil::TaskManager::instance().execute(new ResetTalkProxyTask(endpoint,isBroadcast));
}


bool OnlineManagerI::has(Ice::Int userId){
	//MCE_WARN("OnlineManagerI::has --> " << userId);
	StatFunc statF("OnlineManagerI::has --> ***");
	UserPoolPtr pool = getPool(userId);
	if (!pool) {
		return false;
	}
	return pool->has(userId);
}

pair<int, int> OnlineManagerI::getSize(){
	//MCE_WARN("OnlineManagerI::getSize --> ");
	int sizeTotal = 0;
	int sizeDedup = 0;
	for (int i = 0; i < kUserPoolCount; ++i) {
		sizeTotal += getPool(i)->size(false);
		sizeDedup += getPool(i)->size();
	}
	return make_pair(sizeTotal, sizeDedup);
}

pair<int, int>	OnlineManagerI::getLockCount(pair<int, int>& max, pair<int, int>& min){
	pair<int, int> p = make_pair<int, int>(0, 0);
	max.first = 0;
	max.second = 0;
	min.first = 10000;
	min.second = 10000;
	for (int i = 0; i < kUserPoolCount; ++i) {
		pair<int, int> tmp = getPool(i)->getLockCount();
		p.first += tmp.first;
		p.second += tmp.second;
		if(tmp.first > max.first){
			max.first = tmp.first;
		}
		if(tmp.second > max.second){
			max.second = tmp.second;
		}

		if(tmp.first < min.first){
			min.first = tmp.first;
		}
		if(tmp.second < min.second){
			min.second = tmp.second;
		}
	}
	return p;
}

void OnlineManagerI::remove(const string& endpoint, bool isBroadcast){
	//MCE_WARN("OnlineManagerI::remove --> " << endpoint);
	JidSeq seq;
	map<int, JidSeq> userJids;
	map<int, NotifyType> types;
	for (int i = 0; i < kUserPoolCount; ++i) {
		JidSeq tmp = getPool(i)->remove(endpoint, userJids, types);
	}
	MCE_DEBUG("OnlineManagerI::remove --> endpoint " << endpoint);
	if(!isBroadcast){
		return;
	}
	for(map<int, JidSeq>::iterator it=userJids.begin(); it!=userJids.end(); ++it){
		map<int, NotifyType>::iterator tmpIt = types.find(it->first);
		NotifyType type = AvatarToAll;
		if(tmpIt!=types.end()){
			type = tmpIt->second;
		}
		//AvatarSeq activeAvatars = it
		AvatarSeq self = getUserAvatars(it->first,IM_ONLINE|WP_ONLINE|PHONE_ONLINE);
		AvatarMap avatars;
		if(type!=AvatarToItsOtherAvatars){
			avatars = getBuddyAvatars(it->first,IM_ONLINE);
		}
		MCE_DEBUG("OnlineManagerI::remove --> self size=" << self.size() << " buddies=" << avatars.size());

//		//排除webpagger用户
//		for(AvatarMap::iterator aIt=avatars.begin(); aIt!=avatars.end(); ){
//			for(AvatarSeq::iterator seqIt=aIt->second.begin(); seqIt!=aIt->second.end();){
//				if((*seqIt)->jid->endpoint.empty() || (*seqIt)->jid->endpoint.at(0)!='T'){
//					seqIt = aIt->second.erase(seqIt);
//				}else{
//					++seqIt;
//				}
//			}
//			if(aIt->second.empty()){
//				avatars.erase(aIt++);
//			}else{
//				++aIt;
//			}
//		}
		AvatarSeq activeAvatars;
		for(size_t j=0; j<it->second.size(); j++){
			AvatarPtr a = new Avatar;
			a->jid = it->second.at(j);
			a->stat = OFFLINE;
			activeAvatars.push_back(a);
		}
		if(activeAvatars.empty()){
			continue;
		}
		MCE_DEBUG("OnlineManagerI::remove --> active size" << activeAvatars.size() << " self size=" << self.size() << " buddies=" << avatars.size());
		try{
			PresenceMsgHandlerAdapter::instance().notifyBuddiesOnlineStat(activeAvatars, self, avatars, type, Str2StrMap());
		}catch(Ice::Exception& e){
			MCE_WARN("OnlineManagerI::remove --> notifyBuddiesOfOnlineStat error, " << e);
		}

	}
}

void SyncBitmapThread::run() {
  SyncBitmapDataPtr sync_data = 0;
  while(true) {
    sync_data = 0;
    {
      IceUtil::Mutex::Lock lock(mutex_);

      while(sync_queue_.empty()) {
	      MCE_DEBUG("SyncBitmapThread sleep.");
        cond_.wait(lock);
	      MCE_DEBUG("SyncBitmapThread restart.");
      }

      sync_data = sync_queue_.front();
      sync_queue_.pop_front();
    }

    try{
      MCE_INFO("SyncBitmapThread sync bitmap. pool = " << sync_data->userPoolIndex);
      // TWO_WAY 调用
      //OnlineBitmapAdapter::instance().setUserStatByOnlineType(uot);
      OnlineBitmapAdapter::instance().SyncBitmap(sync_data);
    } catch(Ice::Exception& e) {
      MCE_WARN("OnlineBitmapAdapter::SyncBitmap() ice error : " << e);
    }
  }
}

void SyncBitmapThread::ReloadUserPool(int pool_index) {
  UserPoolPtr pool = OnlineManagerI::instance().getPool(pool_index);

	MCE_DEBUG("SyncBitmapThread pool " << pool_index);

  if (pool) {
    SyncBitmapDataPtr sync = new SyncBitmapData();
    { 
      IceUtil::Mutex::Lock lock(mutex_);
      // 从 pool 中get的时候, 就要加锁
      //sync->stateSeq = pool->getUsersOnlineTypes();
      sync->stateSeq = pool->getUsersBitmapValues();
      sync->userPoolIndex = pool_index;

      sync_queue_.push_back(sync);
      if (sync_queue_.size() == 1)
        cond_.signal();
    }
  }
}

//void SyncBitmapThread::ReloadBitmap(int serverIndex) {
//  int cluster = OnlineCenterAdapter::instance().getCluster();
//  for(int i = serverIndex; i < kUserPoolCount; i += cluster) {
//    UserPoolPtr pool = OnlineManagerI::instance().getPool(i);
//
//	  MCE_DEBUG("SyncBitmapThread pool " << i);
//
//    if (pool) {
//      SyncBitmapDataPtr sync = new SyncBitmapData();
//      { 
//        IceUtil::Mutex::Lock lock(mutex_);
//        // 从 pool 中get的时候, 就要加锁
//        sync->stateSeq = pool->getOnlineUsers();
//        sync->userPoolIndex = i;
//
//        sync_queue_.push_back(sync);
//        if (sync_queue_.size() == 1)
//          cond_.signal();
//      }
//      usleep(15000);
//    }
//  }
//}

bool SyncBitmapThread::Push(const SyncBitmapDataPtr t) {
	MCE_DEBUG("SyncBitmapThread Push()");
  IceUtil::Mutex::Lock lock(mutex_);
  sync_queue_.push_back(t);
  if (sync_queue_.size() == 1)
    cond_.signal();
  return true;
}

//----------------------------------------------------------------------------------------
void ResetTalkProxyTask::handle() {

	//MCE_WARN("[ResetTalkProxyTask::handle] in handle");
/*	JidSeq seq;
	for (size_t i = 0; i < kUserPoolCount; ++i) {
		JidSeq tmp = OnlineManagerI::instance().getPool(i)->remove(_endpoint);
		seq.insert(seq.end(), tmp.begin(), tmp.end());
	}*/

	OnlineManagerI::instance().remove(_endpoint,_isBroadcast);
}

//----------------------------------------------------------------------------------------
void OnlineSizeStatTask::handle() {

	//TaskManager::instance().schedule(this);
	pair<int, int> max;
	pair<int, int> min;
	pair<int, int> p = OnlineManagerI::instance().getSize();
	pair<int, int> lockCount = OnlineManagerI::instance().getLockCount(max, min);
	MCE_INFO("STAT_ONLINE_SIZE -->  TOTAL_ONLINE_SIZE:"<<p.first
			<<"    DEDUP_ONLINE_SIZE:"<<p.second);
	MCE_INFO("~~ lock count r=" << lockCount.first << ", w=" << lockCount.second
			<< ", rmax=" << max.first << ", wmax=" << max.second
			<< ", rmin=" << min.first << ", wmin=" << min.second);

}


//-----------------------------------------------------------------------------------------
void WebOnlineEvictorTask::push(int userId){
	IceUtil::Mutex::Lock lock(_mutex);
	_users.push_back(userId);
}
void WebOnlineEvictorTask::handle() {
	//TaskManager::instance().schedule(this);

	//MCE_WARN("WebOnlineEvictorTask::handle -- >");
	vector<int> users;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_users.swap(users);
	}
	if (users.empty()){
		return;
	}

	JidSeq jids;
	for (size_t i=0; i<users.size(); i++) {
		JidPtr j = new Jid;
		j->userId = users.at(i);
		j->index = 1;
		j->endpoint = "web";
		jids.push_back(j);
	}
	try {
		MCE_DEBUG("WebOnlineEvictorTask::handle --> invoke offlineNotify, size=" << jids.size());
		PresenceMsgHandlerAdapter::instance().offlineNotify(jids);
	} catch (Ice::Exception& e) {
		MCE_WARN("WebOnlineEvictorTask::handle -> offlineNotify -> " << e);
	} catch (...) {
		MCE_WARN("WebOnlineEvictorTask::handle -> offlineNotify -> ...");
	}
}


//-----------------------------------------------------------------------------------------
void OnlineDurationStatTask::push(int userId, int duration){
	IceUtil::Mutex::Lock lock(_mutex);
	_durations[userId] += duration;
}

void OnlineDurationStatTask::handle() {
	//TaskManager::instance().schedule(this);

	map<int, int> onlineDurations;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_durations.swap(onlineDurations);
	}
	if (onlineDurations.empty())
		return;

	MCE_INFO("OnlineSizeStatTask::handle --> write online duration to db, size=" << onlineDurations.size());

	ostringstream sql1;
	int i = 0;
	for (map<int, int>::iterator it = onlineDurations.begin(); it
			!= onlineDurations.end(); ++it) {
		if (i == 0) {
			sql1.str("");
			sql1<< "insert into online_per_day(userid, date ,duration) values";
		} else {
			sql1<<",";
		}
		++i;
		sql1<<"("<<it->first<<",'"<<Date(::time(NULL)).str("%Y-%m-%d")<<"',"<<it->second<<") ";
		if (i % 1000 == 0) {
			sql1
					<<"on duplicate key update duration = duration + values(duration)";
			ConnectionHolder conn("im_stat", CDbWServer);
			mysqlpp::Query query = conn.query();
			query.exec(sql1.str());
			i = 0;
		}
	}
	if (i) {
		sql1 <<"on duplicate key update duration = duration + values(duration)";
		ConnectionHolder conn("im_stat", CDbWServer);
		mysqlpp::Query query = conn.query();
		query.exec(sql1.str());
		i = 0;
	}
}


void ScoreAddTask::handle(){
	int today = Date(time(NULL)).day();
	MCE_INFO("ScoreAddTask::handle --> _taskDay = " << _taskDay << "   today = " << today);
	if(today != _taskDay){
		MCE_INFO("ScoreAddTask::handle --> today != _taskDay run OnlineManagerI::instance().scoreCheck( "<< _index << ")");
		_taskDay = today;
		OnlineManagerI::instance().scoreCheck(_index);
	}
}


