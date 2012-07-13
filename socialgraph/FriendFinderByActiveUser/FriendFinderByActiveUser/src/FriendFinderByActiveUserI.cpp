#include "FriendFinderByActiveUserI.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"


using namespace com::xiaonei::xce;
using namespace xce::friendfinder;
using namespace MyUtil;
using namespace xce::serverstate;
using namespace xce::buddy::adapter;
using namespace xce::friendfinder::adapter;
using namespace xce::socialgraph;
using namespace xce::relationhelper;

UserLoginData* g_userLoginData_ptr = NULL;
 
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.registerObjectCache(USER_LOGIN, "UL", new UserLoginFactory, false);
	service.registerObjectCache(RECOMMEND_DATA, "RD", new RecommendFriendsFactory);

	service.getAdapter()->add( &FriendFinderByActiveUserI::instance(),
			service.createIdentity("FFBU", "") );

	TaskManager::instance().config(LEVEL_REBUILD, ThreadPoolConfig(1, 20));
	TaskManager::instance().config(LEVEL_CREATE, ThreadPoolConfig(1, 40));
	TaskManager::instance().config(LEVEL_REMOVE, ThreadPoolConfig(1, 5));

	ConnectionPoolManager::instance().initialize();

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendFinderByActiveUser.Mod");
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendFinderByActiveUser.Interval", 5);

	//向controller注册
	ServerStateSubscriber::instance().initialize("ControllerFriendFinderByActiveUser", &FriendFinderByActiveUserI::instance(), mod, sInterval);
}

//**********************************************************************************

MyUtil::Int2IntSeqMap RecommendData::get(int limit) {
	MCE_INFO("[RecommendData] get ownerId(" << _ownerId << ") limit(" << limit <<")");
	IceUtil::Mutex::Lock lock(*this);
	checkTimeout();

	if( limit >= (int)_recommendMap.size() || limit < 0) {
		return _recommendMap;
	}

	Int2IntSeqMap result;
	for(Int2IntSeqMap::iterator iter=_recommendMap.begin(); iter!=_recommendMap.end(); ++iter) {
		if(limit != 0) {
			result.insert( make_pair(iter->first, iter->second) );
		}
		--limit;
	}
	return result;
}

MyUtil::Int2IntSeqMap RecommendData::getRandom(int limit) {
	MCE_INFO("[RecommendData] getRandom ownerId(" << _ownerId << ") limit(" << limit << ")");
	IceUtil::Mutex::Lock lock(*this);
	checkTimeout();	

	if( limit >= (int)_recommendMap.size() || limit < 0 ) {
		return _recommendMap;
	}

	vector<int> keyIndex;
	for(Int2IntSeqMap::iterator iter=_recommendMap.begin(); iter!=_recommendMap.end(); ++iter) {
		keyIndex.push_back( iter->first );
	}
	random_shuffle( keyIndex.begin(), keyIndex.end() );
	keyIndex.resize( limit );

	Int2IntSeqMap result;
	for(vector<int>::iterator iter=keyIndex.begin(); iter!=keyIndex.end(); ++iter) {
		result[*iter] = _recommendMap[*iter];
	}
	return result;
}

MyUtil::IntSeq RecommendData::getWriteRecommendRandom(int limit) {
    IceUtil::Mutex::Lock lock(*this);
    checkTimeout(); 

    Int2IntMap writeRecommendRank;
    for(Int2IntSeqMap::iterator iter = _recommendMap.begin();
            iter != _recommendMap.end(); ++iter) {
        if( iter->second.empty() ) {
            writeRecommendRank[ iter->first ] = _friendrank[ iter->first ];
        }
    }

    multimap<int, int> orderedRank;
    for(Int2IntMap::iterator iter = writeRecommendRank.begin(); 
            iter != writeRecommendRank.end(); ++iter) {
        orderedRank.insert( make_pair<int, int>(iter->second, iter->first) );   
    }  
       
    IntSeq writeRecommendList;
    for(multimap<int, int>::reverse_iterator iter = orderedRank.rbegin();
            iter != orderedRank.rend(); ++iter) {
        writeRecommendList.push_back( iter->second );   
    }  

	MCE_INFO("[RecommendData] getWriteRecommendRandom ownerId(" << _ownerId << ") limit(" << 
			limit << ") writeRecommendRank size(" << writeRecommendRank.size() << 
			") writeRecommendList size(" << writeRecommendList.size() << ")");
       
    if( limit >= (int)writeRecommendList.size() || limit <= 0 ) {
        return writeRecommendList;
    }

    writeRecommendList.resize( limit );

    return writeRecommendList;
}

MyUtil::Int2IntSeqMap RecommendData::getRandomByFriendRank(int limit) {
	IceUtil::Mutex::Lock lock(*this);
	checkTimeout();	
	
	Int2IntSeqMap friendRecommendMap;
	Int2IntMap friendRankMap;
	for (Int2IntSeqMap::iterator iter = _recommendMap.begin();
			iter != _recommendMap.end(); ++iter) {
		if ( !iter->second.empty() ) {
			friendRecommendMap[ iter->first ] = iter->second;

			if ( _friendrank[ iter->first ] <= 0 ) {
				friendRankMap[ iter->first ] = 1;
			} else {
				friendRankMap[ iter->first ] = _friendrank[ iter->first ];
			}
		}
	}

	if ( limit >= (int)friendRecommendMap.size() || limit < 0 ) {
		return friendRecommendMap;
	}

	int sum = 0;
	vector<pair<int,int> > fr;
	for (map<int,int>::const_iterator iter = friendRankMap.begin();
			iter != friendRankMap.end(); ++iter) {
		sum += iter->second;
		fr.push_back(*iter);
	}

	MCE_INFO("[RecommendData] getRandomByFriendRank ownerId(" << _ownerId << ") limit(" << limit << ") sum = " << sum);

	Int2IntSeqMap result;

	if (sum <= 0) {                            //亲密度为0时的计算
		for (Int2IntSeqMap::iterator it=friendRecommendMap.begin(); 
				it!=friendRecommendMap.end(); ++it) {
			friendRankMap.insert(make_pair<int,int>(it->first, 1) );	
		}
		sum = (int)friendRecommendMap.size();	
	}

	for (int i = 0; i < limit; ++i) {
		int r = rand() % sum;
		for (vector<pair<int,int> >::iterator iter = fr.begin();
				iter != fr.end(); ++iter) {
			r -= iter->second;
			if (r<=0)
			{
				result[iter->first] = friendRecommendMap[iter->first];
				sum -= iter->second;
				iter->second = 0;
				break;
			}
		}
	}
	return result;
}

MyUtil::IntSeq RecommendData::getFriendsRecommendRandom(int limit) {
	IceUtil::Mutex::Lock lock(*this);
	checkTimeout();	

	IntSeq result;
	Int2IntMap friendRankMap;
	for (Int2IntSeqMap::iterator iter = _recommendMap.begin();
			iter != _recommendMap.end(); ++iter) {
		if ( !iter->second.empty() ) {
			result.push_back( iter->first );

			if ( _friendrank[ iter->first ] <= 0 ) {
				friendRankMap[ iter->first ] = 1;
			} else {
				friendRankMap[ iter->first ] = _friendrank[ iter->first ];
			}
		}
	}

	if ( limit >= (int)friendRankMap.size() || limit < 0 ) {
		return result;
	}

	result.clear();

	int sum = 0;
	vector<pair<int,int> > fr;
	for (map<int,int>::const_iterator iter = friendRankMap.begin();
			iter != friendRankMap.end(); ++iter) {
		sum += iter->second;
		fr.push_back(*iter);
	}

	MCE_INFO("[RecommendData] getFriendsRecommendRandom ownerId(" << _ownerId << ") limit(" << limit << ") sum = " << sum);

	for (int i = 0; i < limit; ++i) {
		int r = 0;
		if (sum > 0) {
			r = rand() % sum;
		} else {
			MCE_WARN("[RecommendData] getFriendsRecommendRandom exception occur sum(" << sum << ")");
			break;
		}

		for (vector<pair<int,int> >::iterator iter = fr.begin();
				iter != fr.end(); ++iter) {
			r -= iter->second;
			if (r<=0) {
				result.push_back( iter->first );
				sum -= iter->second;
				iter->second = 0;
				break;
			}
		}
	}
	return result;
}

MyUtil::IntSeq RecommendData::getRecommendFriendList(int friendId, int begin, int limit) {
	MCE_INFO("[RecommendData] getRecommendFriendList friendId(" << friendId << ") begin(" << begin << ") limit(" << limit << ")");
	IceUtil::Mutex::Lock lock(*this);
	checkTimeout();	
	
	Int2IntSeqMap::iterator fIt = _recommendMap.find(friendId);
	if (fIt == _recommendMap.end() || fIt->second.empty()) {
		return IntSeq();
	}

	const IntSeq& friendList = fIt->second;

	size_t realBegin = begin < 0 ? 0 : begin;
 	realBegin = realBegin < friendList.size() ? realBegin : 0;

	size_t realLimit = limit < 0 ? friendList.size() - realBegin : limit;
	realLimit = realLimit < friendList.size() - realBegin ? realLimit : friendList.size() - realBegin;
	
	MCE_DEBUG("[RecommendData] getRecommendFriendList realBegin=" << realBegin << ", realLimit=" << realLimit);

	IntSeq result;
	for (IntSeq::const_iterator iter = friendList.begin();
			iter != friendList.end(); ++iter) {
		if (realBegin > 0) {
			--realBegin;
			continue;
		}
		result.push_back(*iter);
		if (result.size() >= realLimit) {
			break;
		}
	}
	MCE_DEBUG("[RecommendData] getRecommendFriendList friendId=" << friendId << ", begin:" << begin << ", realBegin:" << 
			realBegin << ", limit:" << limit << ", realLimit:" << realLimit << "   result size=" << result.size());
	return result; 
}

void RecommendData::removeBatch(const MyUtil::IntSeq& idSeq) {
	MCE_INFO("[RecommendData] removeBatch ownerId(" << _ownerId << ") idSeq size(" << idSeq.size() << ")");
	IceUtil::Mutex::Lock lock(*this);
	
	for(IntSeq::const_iterator iter=idSeq.begin(); iter!=idSeq.end(); ++iter) {
		_recommendMap.erase(*iter);
		_friendrank.erase(*iter);
	}	

	if( _recommendMap.empty() ) {
		TaskManager::instance().execute( new BuildTask(_ownerId) );	
	}
}

void RecommendData::checkTimeout() {
	if( time(NULL) - _timeStamp > rebuildInterval ) {
		MCE_INFO("[RecommendData] checkTimeout _ownerId(" << _ownerId << ")");
		TaskManager::instance().execute( new BuildTask(_ownerId) );
		_timeStamp = time(NULL);
	} 
}

//**********************************************************************************

void FriendFinderByActiveUserI::setBitMap(const MyUtil::Int2IntMap& loginMap, const Ice::Current& current) {
	MCE_INFO("[FriendFinderByActiveUserI] setBitMap loginMap size = " << loginMap.size());

	if (g_userLoginData_ptr == NULL) {
		MCE_WARN("[FriendFinderByActiveUserI] setBitMap g_userLoginData_ptr is null");
		g_userLoginData_ptr = new UserLoginData();
	}

	g_userLoginData_ptr->setLoginMap( loginMap );
}

void FriendFinderByActiveUserI::setNegtiveList(const MyUtil::IntSeq& userList, const Ice::Current& current) {
	MCE_INFO("[FriendFinderByActiveUserI] setNegtiveList userList size = " << userList.size());

	if (g_userLoginData_ptr == NULL) {
		MCE_WARN("[FriendFinderByActiveUserI] setNegtiveList g_userLoginData_ptr is null");
		g_userLoginData_ptr = new UserLoginData();
	}

	g_userLoginData_ptr->setNegtiveUserList( userList );
}

void FriendFinderByActiveUserI::clearUserLoginCache(const Ice::Current& current) {
	MCE_DEBUG("[FriendFinderByActiveUserI] clearUserLoginCache");
	if (g_userLoginData_ptr != NULL) {
		MCE_DEBUG("[FriendFinderByActiveUserI] clearUserLoginCache delete g_userLoginData_ptr");
		delete g_userLoginData_ptr; 
		g_userLoginData_ptr = NULL;
	}
	//g_userLoginData_ptr = new UserLoginData();
}

Int2IntSeqMap FriendFinderByActiveUserI::getRecommend(::Ice::Int userId, ::Ice::Int limit, const Ice::Current& current) {
	MCE_INFO("[FriendFinderByActiveUserI] getRecommend userId(" << userId << ") limit(" << limit << ")");
	return locateRecommendData( userId )->get( limit );
}

Int2IntSeqMap FriendFinderByActiveUserI::getRecommendRandom(::Ice::Int userId, ::Ice::Int limit, const Ice::Current& current) {
	MCE_INFO("[FriendFinderByActiveUserI] getRecommendRandom userId(" << userId << ") limit(" << limit << ")");
//	return locateRecommendData( userId )->getRandomByFriendRank( limit );
	return Int2IntSeqMap();
}

IntSeq FriendFinderByActiveUserI::getRecommendFriendList(::Ice::Int userId, ::Ice::Int friendId, ::Ice::Int begin, 
														::Ice::Int limit, const Ice::Current& current) {
	MCE_INFO("[FriendFinderByActiveUserI] getRecommendFriendList userId(" << userId << 
			") friendId(" << friendId << ") begin(" << begin << ") limit(" << limit << ")");
	return locateRecommendData( userId )->getRecommendFriendList(friendId, begin, limit);

}

MyUtil::IntSeq FriendFinderByActiveUserI::getFriendsRecommendRandom(::Ice::Int userId, ::Ice::Int limit, const Ice::Current& current) {
	MCE_INFO("[FriendFinderByActiveUserI] getFriendsRecommendRandom userId(" << userId << ") limit(" << limit << ")");
	return locateRecommendData( userId )->getFriendsRecommendRandom(limit);
}

IntSeq FriendFinderByActiveUserI::getWriteRecommendRandom(::Ice::Int userId, ::Ice::Int limit, const Ice::Current& current) {
	MCE_INFO("[FriendFinderByActiveUserI] getWriteRecommendRandom userId(" << userId << ") limit(" << limit << ")");
	return locateRecommendData( userId )->getWriteRecommendRandom( limit );
}

void FriendFinderByActiveUserI::removeRecommendFriends(::Ice::Int userId, ::Ice::Int friendId, const MyUtil::IntSeq& ids, const Ice::Current& current) {
	MCE_INFO("[FriendFinderByActiveUserI] removeRecommendFriends userId(" << userId << ") friendId(" << friendId << ")");

	if( current.ctx.count(REMOVE_WITH_BLOCK) ) {
		TaskManager::instance().execute( new RemoveTask(userId, friendId, ids, true) );
	} else {
		TaskManager::instance().execute( new RemoveTask(userId, friendId, ids, false) );
	}
}

MyUtil::IntSeq FriendFinderByActiveUserI::getActiveUser(::Ice::Int userId, const MyUtil::IntSeq& userList, const Ice::Current& current) {
	MCE_INFO("[FriendFinderByActiveUserI] getActiveUser userId(" << userId << ") userList size(" << userList.size() << ")");

	if ( !g_userLoginData_ptr ) {
		MCE_DEBUG("[FriendFinderByActiveUserI] getActiveUser no object");
		return IntSeq();
	}
	
	return g_userLoginData_ptr->getActiveUser( userList );
}

RecommendDataPtr FriendFinderByActiveUserI::locateRecommendData(::Ice::Int id) {
	return ServiceI::instance().locateObject<RecommendDataPtr>( RECOMMEND_DATA, id );
}

//**********************************************************************************

void UserLoginData::setLoginMap(const MyUtil::Int2IntMap& loginMap) {
	IceUtil::RWRecMutex::WLock lock(_init_flag_mutex);

	for(Int2IntMap::const_iterator iter=loginMap.begin(); iter!=loginMap.end(); ++iter) {
		_userLoginMap[iter->first] = bitset<32>(iter->second);
	}
	MCE_INFO("[UserLoginData] setLoginMap _userLoginMap size(" << _userLoginMap.size() << ")");
}

void UserLoginData::setNegtiveUserList(const MyUtil::IntSeq& userList) {
	IceUtil::RWRecMutex::WLock lock(_init_flag_mutex);
	
	for(IntSeq::const_iterator iter=userList.begin(); iter!=userList.end(); ++iter) {
		_negtiveUserList.insert( *iter );
	}		
	MCE_INFO("[UserLoginData] setNegtiveUserList _negtiveUserList size(" << _negtiveUserList.size() << ")");
}

MyUtil::Int2IntSeqMap UserLoginData::getRecommendFriends(int userId) {
	IceUtil::RWRecMutex::RLock lock(_init_flag_mutex);
	MCE_INFO("[UserLoginData] getRecommendFriends userId(" << userId << ") _userLoginMap size(" << 
			_userLoginMap.size() << ") ACTIVEDAYS(" << ACTIVEDAYS << ")");

	struct timeval tvStart,tvEnd;
	double linStart = 0, linEnd = 0, lTime = 0;
	gettimeofday( &tvStart, NULL );

	Int2IntSeqMap result;
	UserLoginMapType::iterator fIt = _userLoginMap.find(userId); 
	if( fIt == _userLoginMap.end() || (int)fIt->second.count() < ACTIVEDAYS ) {            //不是活跃用户
		return result;
	}	
	
	vector<int> friendList;
	try {
		friendList = BuddyByIdCacheAdapter::instance().getFriendListAsc(userId, -1);
	} catch ( Ice::Exception& e ) {
		MCE_WARN(time(NULL) << "[UserLoginData] getRecommendFriends BuddyByIdCacheAdapter Ice Exception : " << e.what() << "\n");
	} catch ( ... ) {
		MCE_WARN(time(NULL) << "[UserLoginData] getRecommendFriends BuddyByIdCacheAdapter Unknow Exception");
	}
	
	MCE_DEBUG("[UserLoginData] getRecommendFriends friendList size(" << friendList.size() << ")");	

	vector<int> negtiveFriendsList;
	for (vector<int>::iterator iter = friendList.begin(); iter != friendList.end(); ++iter) {
		if( _negtiveUserList.count(*iter) > 0 ) {
			negtiveFriendsList.push_back( *iter );				//找出不活跃的好友
		}
	}
	if( negtiveFriendsList.empty() ) {                 
		return result;
	}

	Int2IntSeqMap friendsMap;       	
	try {
		friendsMap = BuddyByIdCacheAdapter::instance().getFriendLists( negtiveFriendsList );       	
	} catch ( Ice::Exception& e ) {
		MCE_WARN(time(NULL) << "[UserLoginData] getRecommendFriends BuddyByIdCacheAdapter Ice Exception : " << e.what() << "\n");
	} catch ( ... ) {
		MCE_WARN(time(NULL) << "[UserLoginData] getRecommendFriends BuddyByIdCacheAdapter Unknow Exception");
	}

	MCE_DEBUG("[UserLoginData] getRecommendFriends negtiveFriendsList size(" << negtiveFriendsList.size() << 
				") friendsMap size(" << friendsMap.size() << ")");

	vector<int> recommendFriend;
	ostringstream idPair;
	for (Int2IntSeqMap::iterator iter = friendsMap.begin(); 
			iter != friendsMap.end(); ++iter) {
		recommendFriend = FriendFinderInfoCacheReplicaAdapter::instance().getGuestInfoMatchFriendList(userId, iter->first, -1); 
		result[iter->first] = vector<int>();

		idPair << "(" << iter->first << "," << recommendFriend.size() << ") ";

		for (vector<int>::iterator it = recommendFriend.begin(); 
				it != recommendFriend.end(); ++it) {
			UserLoginMapType::iterator ff = _userLoginMap.find( *it );
			if (ff == _userLoginMap.end() || (int)ff->second.count() < ACTIVEDAYS) {                  //过滤不活跃的推荐
				continue;
			}
	
			vector<int>::iterator fIt = lower_bound( iter->second.begin(), iter->second.end(), *it ); 		//过滤好友列表
			if (fIt == iter->second.end() || *fIt != *it) {
				result[iter->first].push_back( *it );
			}
		}
		recommendFriend.clear();
	}

	//AppendSingleRelationFriend(userId, result);         //添加单向好友
 
	Int2IntSeqMap blockFriends;  
	IntSeq tmpVec;

	RelationBatchHelper::getBlockResult( userId, blockFriends);         	

	MCE_DEBUG("[UserLoginData] getRecommendFriends recommend Friends " << idPair.str() << " blockFriends size=" << blockFriends.size());
	
	IntSeq recf;
	for (Int2IntSeqMap::iterator iter = blockFriends.begin();        //过滤block列表
			iter != blockFriends.end(); ++iter) {
		recf = result[iter->first];
		for (IntSeq::iterator it = recf.begin();
				it != recf.end(); ++it) {
			if( find(iter->second.begin(), iter->second.end(), *it) == iter->second.end() ) {
				tmpVec.push_back( *it );
			}
		}

		if( tmpVec.empty() ) {                 //如果没有可以推荐的人则不再出现该用户
			result.erase( iter->first );
		} else {
			result[iter->first] = tmpVec;
		}

		tmpVec.clear();		
		recf.clear();
	}

	ostringstream idEmptySize;
	ostringstream idSize;
	for (Int2IntSeqMap::iterator iter = result.begin();
			iter != result.end(); ++iter) {
		if (iter->second.empty()) {
			idEmptySize << iter->first << ", ";
		} else {
			idSize << "(" << iter->first << "," << iter->second.size() << ") ";
		}
	}

	//AppendUserFriends(result, friendList, friendsMap);

	for (Int2IntSeqMap::iterator iter=result.begin(); iter!=result.end(); ++iter) {      //精简内存
		IntSeq( iter->second ).swap( iter->second );
	}

	gettimeofday( &tvEnd, NULL );
	linStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);
	linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
	lTime = linEnd - linStart;
	lTime = lTime/1000000;

	MCE_DEBUG("write :: " << idEmptySize.str() << " friend :: " << idSize.str() << 
				" result size=" << result.size() << " cost time=" << lTime << "second!");
	return result;
}

void UserLoginData::AppendUserFriends(MyUtil::Int2IntSeqMap& result, 
									const MyUtil::IntSeq& userFriends, 	
									const MyUtil::Int2IntSeqMap& negtiveFriendsMap) {
	MCE_INFO("[UserLoginData] AppendUserFriends");
	for (Int2IntSeqMap::iterator iter = result.begin();
			iter != result.end(); ++iter) {
		IntSeq& recommendList = iter->second;
		IntSeq tempRecommendList(recommendList);

		Int2IntSeqMap::const_iterator fIt = negtiveFriendsMap.find(iter->first);

		if (fIt != negtiveFriendsMap.end() && !recommendList.empty()) {
			for (IntSeq::const_iterator it = userFriends.begin();
					it != userFriends.end(); ++it) {
				if (find(tempRecommendList.begin(), tempRecommendList.end(), *it) != tempRecommendList.end()) {  //过滤好友中已经推出的好友
					continue;
				}

				UserLoginMapType::iterator ff = _userLoginMap.find( *it );
				if (ff == _userLoginMap.end() || (int)ff->second.count() < ACTIVEDAYS) {                  //过滤不活跃的推荐
					continue;
				}
				
				vector<int>::const_iterator userFriendIter = lower_bound(fIt->second.begin(), fIt->second.end(), *it);    //过滤好友列表
				if (userFriendIter == fIt->second.end() || *userFriendIter != *it) {
					recommendList.push_back(*it);
				}
			}
		}
	}
}

void UserLoginData::AppendSingleRelationFriend(int userId, MyUtil::Int2IntSeqMap& result) {
	HotFeedSeq singleFeedList = HotFeedRecommendCacheAdapter::instance().Get(userId, -1);

	ostringstream singleFeedListStream;
	for (HotFeedSeq::iterator iter = singleFeedList.begin();
			iter != singleFeedList.end(); ++iter) {
		if (result.find(iter->userId) == result.end()) {
			result.insert(make_pair<int, IntSeq>(iter->userId, IntSeq()));
			singleFeedListStream << iter->userId << ", ";
		}
	}
	singleFeedListStream << singleFeedList.size();
	MCE_INFO("[UserLoginData] AppendSingleRelationFriend userId(" << userId << ") detail " << singleFeedListStream.str());
}

MyUtil::IntSeq UserLoginData::getActiveUser(const MyUtil::IntSeq& userList) {
	MCE_INFO("[UserLoginData] getActiveUser userList size = " << userList.size());
	IntSeq result;
	for(IntSeq::const_iterator iter=userList.begin();
			iter!=userList.end(); ++iter) {
		if( _negtiveUserList.count(*iter) <= 0 ) {       //活跃用户
			result.push_back(*iter);
		}
	}	
	return result; 
}

UserLoginData::~UserLoginData() {
	MCE_INFO("[UserLoginData] ~UserLoginData");
	_userLoginMap.clear();
	_negtiveUserList.clear();
}

//**********************************************************************************

Ice::ObjectPtr RecommendFriendsFactory::create(::Ice::Int id) {
	MCE_DEBUG("[RecommendFriendsFactory] create id = " << id);

	TaskManager::instance().execute( new CreateTask(id) );

	RecommendDataPtr tmp = new RecommendData( id, Int2IntSeqMap(), map<int,int>());
	return tmp;
}

//**********************************************************************************

void CreateTask::handle() {
	Int2IntSeqMap recommendResult;
	Int2IntMap friendRank;

	RelationBatchHelper::getRecommendResult( _userId, recommendResult, friendRank );

	MCE_INFO("CreateTask::create -> id=" << _userId << " recommendResult(" << recommendResult.size() << ") friendRank(" << friendRank.size() << ")");

	RecommendDataPtr result = new RecommendData( _userId, recommendResult, friendRank );
	ServiceI::instance().getObjectCache()->addObject( RECOMMEND_DATA, _userId, result );

	if( (int)recommendResult.size() <= REBUILD_THRESHOLD ) {
		MCE_DEBUG("CreateTask::handle BuildTask _userId(" << _userId << ")");
		TaskManager::instance().execute( new BuildTask(_userId) );
	}	
}

//**********************************************************************************

void BuildTask::handle() {
	MCE_INFO("[BuildTask] build userId(" << _userId << ")");
	struct timeval tvStart,tvEnd;
	double linStart = 0, linEnd = 0, lTime = 0;
	gettimeofday( &tvStart, NULL );

	if ( !g_userLoginData_ptr ) {
		MCE_DEBUG("[BuildTask] handle no UserLoginDataPtr object");
		return;
	}

	Int2IntSeqMap recommends = g_userLoginData_ptr->getRecommendFriends( _userId );
	if (recommends.empty()) {
		MCE_DEBUG("BuildTask::handle userid(" << _userId << ") no cal result");

		RecommendDataPtr target = ServiceI::instance().findObject<RecommendDataPtr>( RECOMMEND_DATA, _userId );     //用户变得不在活跃，删除cache
		if (target) {
			ServiceI::instance().removeObject( RECOMMEND_DATA, _userId );
			Statement sql;
			sql << "delete from " << TableName::recommend_friends_table(_userId)
					<< " where owner_id=" << _userId;
			QueryRunner("friendfinder_activeuser", CDbWServer, 
					TableName::recommend_friends_table(_userId)).execute(sql);
		}

		return;	  
	}
	
	//--------add friendrank------------
	RankSeq vecRank = FriendRankCacheAdapter::instance().GetRank(_userId);
	map<int,int> friendrank;
	for (Int2IntSeqMap::iterator it = recommends.begin(); 
			it != recommends.end(); ++it) {
		friendrank.insert(make_pair<int,int>(it->first, 1) );	
	}
	
	//MCE_INFO("FriendRank" << _userId << ":");
	int rankvalue = 0;
	for (RankSeq::const_iterator iter = vecRank.begin();
			iter != vecRank.end(); ++iter) {
		if ( recommends.find(iter->userid) != recommends.end() ) {
			rankvalue = 1;
			if (rankvalue >0 && rankvalue<=100) {
				rankvalue = iter->weight;
			}
			friendrank[iter->userid] = rankvalue;
		}
	}
	//--------------------

	Statement sqlDelete;
	sqlDelete << "delete from " << TableName::recommend_friends_table(_userId)
			<< " where owner_id=" << _userId;
	QueryRunner("friendfinder_activeuser", CDbWServer, 
			TableName::recommend_friends_table(_userId)).execute(sqlDelete);

	Statement sqlInsert;
	ostringstream sqllog;
	sqlInsert << "insert into " << TableName::recommend_friends_table(_userId)
			<< " (owner_id, friend_id, friend_rank, strong_recommend_ids) values ";
	sqllog << "SAVE " << _userId << " FOR"; 

	for(Int2IntSeqMap::iterator iter=recommends.begin(); 
			iter!=recommends.end(); ++iter) {
		sqllog << " (" << iter->first << " , ";
		if( iter != recommends.begin() ) {
			sqlInsert << ", ";
		}
		sqlInsert << "(" << _userId << ", " << iter->first << ", ";
	
		int rank = 0;
		map<int, int>::iterator fIt = friendrank.find( iter->first );
		if( fIt != friendrank.end() ) {
			rank = fIt->second;	
		}		
		sqllog << rank << ")";	

		sqlInsert << rank << ", " << mysqlpp::quote 
				<< IntSeqSerialize::serialize( iter->second ) << ")";
	}
	sqlInsert << " on duplicate key update " << "friend_rank=values(friend_rank), strong_recommend_ids=values(strong_recommend_ids);";
	
	QueryRunner("friendfinder_activeuser", CDbWServer, 
			TableName::recommend_friends_table(_userId)).execute(sqlInsert, sqllog.str());

	RecommendDataPtr result = new RecommendData( _userId, recommends, friendrank );
	ServiceI::instance().addObject( result, RECOMMEND_DATA, _userId );

	gettimeofday( &tvEnd, NULL );
	linStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);
	linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
	lTime = linEnd - linStart;
	lTime = lTime/1000000;
	MCE_DEBUG( "[BuildTask::done] userId=" << _userId << " time cost : " << lTime << " second!" );
}

//**********************************************************************************

void RemoveTask::handle() {
	try {
		Statement sqlDelete;
		sqlDelete << "delete from " << TableName::recommend_friends_table(_userId)
				<< " where owner_id=" << _userId << " and friend_id=" << _friendId;
		QueryRunner("friendfinder_activeuser", CDbWServer,
			TableName::recommend_friends_table(_userId)).execute(sqlDelete);
	
		if(_block) {
			MCE_INFO("_block : true");
			Statement sqlWithBlock;
			sqlWithBlock << "insert into " << TableName::recommend_block_table(_userId)
					<< " (owner_id, friend_id, block_ids) values (" << _userId << ", "
					<< _friendId << ", " << mysqlpp::quote << IntSeqSerialize::serialize(_blockIds) << ")";
			sqlWithBlock << " on duplicate key update " << "block_ids=values(block_ids);";
			QueryRunner("friendfinder_activeuser", CDbWServer,
				TableName::recommend_block_table(_userId)).execute(sqlWithBlock);
		} else {
			MCE_INFO("_block : false");
		}

		RecommendDataPtr obj = ServiceI::instance().findObject<RecommendDataPtr>(RECOMMEND_DATA, _userId);
		if(!obj) {
			return;
		}
	
		IntSeq removeItems;
		removeItems.push_back( _friendId );
		obj->removeBatch( removeItems );
	} catch ( Ice::Exception& e ) {
		MCE_WARN("[RemoveTask] handle userId(" << _userId << ") ice exception " << e.what());
	} catch ( std::exception& e) {
		MCE_WARN("[RemoveTask] handle userId(" << _userId << ")  std exception " << e.what());
	} catch ( ... ) {
		MCE_WARN("[RemoveTask] handle userId(" << _userId << ") unknow exception");
	}
}
