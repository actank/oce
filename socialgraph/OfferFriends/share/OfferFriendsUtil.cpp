#include "OfferFriendsUtil.h"
#include "CommonFriendUtil.h"
#include <BuddyByIdReplicaAdapter.h>
#include <FriendCountCacheReplicaAdapter.h>
#include <FriendFinderReplicaAdapter.h>
#include <FriendFinderInfoCacheReplicaAdapter.h>
#include <FriendFinderByIPReplicaAdapter.h>
#include <IPRecordReplicaAdapter.h>
#include <PhotoStateFilterAdapter.h>
#include <OnlineStateFilterAdapter.h>
#include <StatusCacheAdapter.h>
#include <UserStateFilterAdapter.h>

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::offerfriends;

using namespace com::xiaonei::service;
using namespace xce::friendfinder;

using namespace xce::buddy::adapter;
using namespace xce::friendfinder::adapter;
using namespace xce::iprecord::adapter;

using namespace xce::photostatefilter::adapter;
using namespace xce::onlinestatefilter::adapter;
using namespace xce::userstatefilter::adapter;
using namespace xce::statuscache;

MyUtil::IntSeq OfferFriendsUtil::getFriendSeq(int userId) {
        MyUtil::IntSeq buddies;
        try {
                buddies = BuddyByIdCacheAdapter::instance().getFriendListAsc( userId, -1 );
        } catch( Ice::Exception e ){
                MCE_WARN("[OfferFriendsUtil::getFriendSeq] userId=" << userId << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[OfferFriendsUtil::getFriendSeq] userId=" << userId << " found Unknown::Exception" );
        }
	return buddies;
}

MyUtil::Int2IntSeqMap OfferFriendsUtil::getFriendSeqBatch(const MyUtil::IntSeq& ids) {
        Int2IntSeqMap result;
        if (ids.empty()) {
                return result;
        }
        return BuddyByIdCacheAdapter::instance().getFriendLists(ids);
}

MyUtil::Int2IntMap OfferFriendsUtil::getFriendCountBatch(const MyUtil::IntSeq& ids) {
        MyUtil::Int2IntMap result;
        if (ids.empty()) {
                return result;
        }
	return FriendCountCacheAdapter::instance().getFriendCounts(ids);
}

MyUtil::IntSeq OfferFriendsUtil::getApplySeq(int userId) {
	IntSeq result;
	if( userId <= 0 ) {
		return result;
	}

	ostringstream tableName;
	tableName << "buddyapply_applicant_" << (userId % 100);	

	Statement oss;
	oss << "SELECT accepter FROM " << tableName.str() << " WHERE applicant=" << userId << " limit 100";
	QueryRunner("offerfriends_apply", CDbRServer).query( oss, ApplyResultHandler(result) );
	return result;
}

MyUtil::IntSeq OfferFriendsUtil::getBlockSeq(int id) {
        IntSeq result;
        if (id<=0) {
                return result;
        }
        Statement oss;
        oss << "SELECT guest FROM relation_block  WHERE host = " << id;
        BlockResultHandler rsh(result, "guest");
        QueryRunner("friendfinder_block", CDbRServer).query(oss, rsh);
        return result;
}

MyUtil::IntSeq OfferFriendsUtil::getCommBlockSeq(int id) {
        IntSeq result;
        if (id<=0) {
                return result;
        }
	ostringstream patt;
	patt << "common_friends_block_" << (id % 100);

        Statement oss;
        oss << "SELECT block_id FROM " << patt.str() << " WHERE owner_id = " << id;
        BlockResultHandler rsh(result, "block_id");
        QueryRunner("friendfinder_common_friends_new", CDbRServer, patt.str() ).query(oss, rsh);
        return result;
}

CommonFriendSeq OfferFriendsUtil::getCommonFriend(int userId, int commLimit) {
        CommonFriendSeq commonFriends;
        try{
                commonFriends = FriendFinderReplicaAdapter::instance().getCommonFriend(userId, 0, commLimit);
        } catch( Ice::Exception e ){
                MCE_WARN("[OfferFriendsUtil::getCommonFriend] userId=" << userId << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[OfferFriendsUtil::getCommonFriend] userId=" << userId << " found Unknown::Exception" );
        }
	return commonFriends;
}

CommonFriendSeq OfferFriendsUtil::calCommonFriend(int userId,
		const MyUtil::IntSeq& applyList,
		const MyUtil::IntSeq& friendList,
		const MyUtil::IntSeq& blockList,
		const MyUtil::IntSeq& commBlockList,
		int commLimit){
        MyUtil::Int2IntSeqMap ff(getFriendSeqBatch(friendList));
        MyUtil::Int2IntSeqMap af(getFriendSeqBatch(applyList));
	CommonFriendCalculator calc(userId,applyList,friendList,blockList,commBlockList,ff,af);
	return calc.getComm(commLimit);
}

void OfferFriendsUtil::rebuildFriendFinder(int userId) {
	try{
		FriendFinderReplicaAdapter::instance().rebuildCommonFriend(userId);
	} catch( Ice::Exception e ){
		MCE_WARN("[OfferFriendsUtil::rebuildFriendFinder] userId=" << userId << " found Ice::Exception : " << e.what() );
	} catch( ... ){
		MCE_WARN("[OfferFriendsUtil::rebuildFriendFinder] userId=" << userId << " found Unknown::Exception" );
	}
}

IPNoteSeq OfferFriendsUtil::getFreqIP(int userId, int ipLimit) {
	IPNoteSeq ipSeq;
        try{
                ipSeq = FriendFinderByIPAdapter::instance().getFreqIP(userId, ipLimit);
        } catch( Ice::Exception e ){
                MCE_WARN("[OfferFriendsUtil::getFreqIP] userId=" << userId << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[OfferFriendsUtil::getFreqIP] userId=" << userId << " found Unkown::Exception : " );
        }
	return ipSeq;
}

MyUtil::IntSeq OfferFriendsUtil::getUsersWithMaskByIP(Ice::Long ip,Ice::Int limit,const MyUtil::IntSeq& mask){
	MyUtil::IntSeq ipFriendList;
        try{
		ipFriendList = IPRecordAdapter::instance().getUsersWithMask( ip, limit, mask );
        } catch( Ice::Exception e ){
                MCE_WARN("[OfferFriendsUtil::getUsersWithMask] ip=" << ip << " limit=" << limit << " mask.size=" << mask.size() << " found Ice::Exception : " << e.what() ); 
        } catch( ... ){
                MCE_WARN("[OfferFriendsUtil::getUsersWithMask] ip=" << ip << " limit=" << limit << " mask.size=" << mask.size() << " found Unknown::Exception : " ); 
        }
        return ipFriendList;
}

NetworkInfo OfferFriendsUtil::getNetworkInfoWithHostage(int userId, int size, const std::vector<int>& friendsList){
	struct timeval tvStart, tvEnd;
	double linStart, linEnd, linTime;

	gettimeofday( &tvStart, NULL );
	NetworkInfo infoResult;
	try {
		infoResult = FriendFinderInfoCacheReplicaAdapter::instance().getNetworkInfoWithHostage( userId );
	} catch( Ice::Exception e ){
		MCE_WARN("[OfferFriendsUtil::getUsersWithMask] getNetworkInfoWithHostage userId=" << userId << " found Ice::Exception : " << e.what());
	} catch( ... ){
		MCE_WARN("[OfferFriendsUtil::getUsersWithMask] getNetworkInfoWithHostage userId=" << userId << " found unknown exception : " );
	} 

	gettimeofday( &tvEnd, NULL );

	linStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);
	linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
	linTime = linEnd - linStart;
	linTime = linTime/1000000;
	
	int listSize = infoResult.univInfo.size() + infoResult.highschoolInfo.size() + infoResult.juniorschoolInfo.size() + infoResult.elementaryschoolInfo.size(); 

	if(listSize <= 0) {
		return infoResult;
	}

	int sectionSize = size/listSize;

	vector<int> sizeVolum( listSize, sectionSize );

	int pos = 0;

	for(ItemSeq::iterator iter=infoResult.univInfo.begin(); iter!=infoResult.univInfo.end(); ++iter) {
		filterHostFriends(userId, iter->idSeq, friendsList);
		//MCE_DEBUG( "univ idSeq size : " << iter->idSeq.size() );
		//MCE_DEBUG( "sizeVolum" << pos << ": " << sizeVolum[pos] );
		if( (int)iter->idSeq.size() >= sizeVolum[pos] ) {
			iter->idSeq.resize( sizeVolum[pos] );
		} else if( pos < listSize - 1 ) {
			sizeVolum[pos + 1] += sizeVolum[pos] - iter->idSeq.size();
		}
		++pos;
	}
	
	for(ItemSeq::iterator iter=infoResult.highschoolInfo.begin(); iter!=infoResult.highschoolInfo.end(); ++iter) {
		filterHostFriends(userId, iter->idSeq, friendsList);
		//MCE_DEBUG( "highschool idSeq size : " << iter->idSeq.size() );
		//MCE_DEBUG( "sizeVolum" << pos << ": " << sizeVolum[pos] );
		if( (int)iter->idSeq.size() >= (int)sizeVolum[pos] ) {
			iter->idSeq.resize( sizeVolum[pos] );
		} else if( pos < listSize - 1 ) {
			sizeVolum[pos + 1] += sizeVolum[pos] - iter->idSeq.size();
		}
		++pos;
	}
	
	for(ItemSeq::iterator iter=infoResult.juniorschoolInfo.begin(); iter!=infoResult.juniorschoolInfo.end(); ++iter) {
		filterHostFriends(userId, iter->idSeq, friendsList);
		//MCE_DEBUG( "juniorschool idSeq size : " << iter->idSeq.size() );
		//MCE_DEBUG( "sizeVolum" << pos << ": " << sizeVolum[pos] );
		if( (int)iter->idSeq.size() >= (int)sizeVolum[pos] ) {
			iter->idSeq.resize( sizeVolum[pos] );
		} else if( pos < listSize - 1 ) {
			sizeVolum[pos + 1] += sizeVolum[pos] - iter->idSeq.size();
		}
		++pos;
	}
	
	for(ItemSeq::iterator iter=infoResult.elementaryschoolInfo.begin(); iter!=infoResult.elementaryschoolInfo.end(); ++iter) {
		filterHostFriends(userId, iter->idSeq, friendsList);
		//MCE_DEBUG( "elementaryschool idSeq size : " << iter->idSeq.size() );
		//MCE_DEBUG( "sizeVolum" << pos << ": " << sizeVolum[pos] );
		if( (int)iter->idSeq.size() >= (int)sizeVolum[pos] ) {
			iter->idSeq.resize( sizeVolum[pos] );
		} else if( pos < listSize - 1 ) {
			sizeVolum[pos + 1] += sizeVolum[pos] - iter->idSeq.size();
		}
		++pos;
	}	

	return infoResult;
}

NetworkInfo OfferFriendsUtil::getNetworkInfoWithHostageFromDB(int userId, int size, const std::vector<int>& friendsList){
	struct timeval tvStart, tvEnd;
	double linStart, linEnd, linTime;

	gettimeofday( &tvStart, NULL );
	NetworkInfo infoResult;
	try {
		infoResult = FriendFinderInfoCacheReplicaAdapter::instance().getNetworkInfoWithHostageFromDB( userId );
	} catch( Ice::Exception e ){
		MCE_WARN("[OfferFriendsUtil::getUsersWithMask] getNetworkInfoWithHostageFromDB userId=" << userId << " found Ice::Exception : " << e.what());
	} catch( ... ){
		MCE_WARN("[OfferFriendsUtil::getUsersWithMask] getNetworkInfoWithHostageFromDB userId=" << userId << " found unknown exception : " );
	} 

	gettimeofday( &tvEnd, NULL );

	linStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);
	linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
	linTime = linEnd - linStart;
	linTime = linTime/1000000;
	
	int listSize = infoResult.univInfo.size() + infoResult.highschoolInfo.size() + infoResult.juniorschoolInfo.size() + infoResult.elementaryschoolInfo.size(); 

	if(listSize <= 0) {
		return infoResult;
	}

	int sectionSize = size/listSize;

	vector<int> sizeVolum( listSize, sectionSize );

	int pos = 0;

	for(ItemSeq::iterator iter=infoResult.univInfo.begin(); iter!=infoResult.univInfo.end(); ++iter) {
		filterHostFriends(userId, iter->idSeq, friendsList);
		//MCE_DEBUG( "univ idSeq size : " << iter->idSeq.size() );
		//MCE_DEBUG( "sizeVolum" << pos << ": " << sizeVolum[pos] );
		if( (int)iter->idSeq.size() >= sizeVolum[pos] ) {
			iter->idSeq.resize( sizeVolum[pos] );
		} else if( pos < listSize - 1 ) {
			sizeVolum[pos + 1] += sizeVolum[pos] - iter->idSeq.size();
		}
		++pos;
	}
	
	for(ItemSeq::iterator iter=infoResult.highschoolInfo.begin(); iter!=infoResult.highschoolInfo.end(); ++iter) {
		filterHostFriends(userId, iter->idSeq, friendsList);
		//MCE_DEBUG( "highschool idSeq size : " << iter->idSeq.size() );
		//MCE_DEBUG( "sizeVolum" << pos << ": " << sizeVolum[pos] );
		if( (int)iter->idSeq.size() >= (int)sizeVolum[pos] ) {
			iter->idSeq.resize( sizeVolum[pos] );
		} else if( pos < listSize - 1 ) {
			sizeVolum[pos + 1] += sizeVolum[pos] - iter->idSeq.size();
		}
		++pos;
	}
	
	for(ItemSeq::iterator iter=infoResult.juniorschoolInfo.begin(); iter!=infoResult.juniorschoolInfo.end(); ++iter) {
		filterHostFriends(userId, iter->idSeq, friendsList);
		//MCE_DEBUG( "juniorschool idSeq size : " << iter->idSeq.size() );
		//MCE_DEBUG( "sizeVolum" << pos << ": " << sizeVolum[pos] );
		if( (int)iter->idSeq.size() >= (int)sizeVolum[pos] ) {
			iter->idSeq.resize( sizeVolum[pos] );
		} else if( pos < listSize - 1 ) {
			sizeVolum[pos + 1] += sizeVolum[pos] - iter->idSeq.size();
		}
		++pos;
	}
	
	for(ItemSeq::iterator iter=infoResult.elementaryschoolInfo.begin(); iter!=infoResult.elementaryschoolInfo.end(); ++iter) {
		filterHostFriends(userId, iter->idSeq, friendsList);
		//MCE_DEBUG( "elementaryschool idSeq size : " << iter->idSeq.size() );
		//MCE_DEBUG( "sizeVolum" << pos << ": " << sizeVolum[pos] );
		if( (int)iter->idSeq.size() >= (int)sizeVolum[pos] ) {
			iter->idSeq.resize( sizeVolum[pos] );
		} else if( pos < listSize - 1 ) {
			sizeVolum[pos + 1] += sizeVolum[pos] - iter->idSeq.size();
		}
		++pos;
	}	

	return infoResult;
}

HighSchoolInfo OfferFriendsUtil::getHighSchoolInfo(int userId, int resultSize, const std::vector<int>& friendsList) {
	struct timeval tvStart, tvEnd;
	double linStart, linEnd, linTime;

	gettimeofday( &tvStart, NULL );
	HighSchoolInfo infoResult;
	try {
		infoResult = FriendFinderInfoCacheReplicaAdapter::instance().getHighSchoolInfo(userId);
	} catch( Ice::Exception e ){
		MCE_WARN("[OfferFriendsUtil::getUsersWithMask] getHighSchoolInfo userId=" << userId << " found Ice::Exception : " << e.what());
	} catch( ... ) {
		MCE_WARN("[OfferFriendsUtil::getUsersWithMask] getHighSchoolInfo userId=" << userId << " found unknown exception : " );
	} 

	gettimeofday( &tvEnd, NULL );

	linStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);
	linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
	linTime = linEnd - linStart;
	linTime = linTime/1000000;
	
	filterHostFriendsForHighSchool(userId, infoResult.highschoolseq, friendsList);
	filterHostFriendsForHighSchool(userId, infoResult.juniorschoolseq, friendsList);
	filterHostFriendsForHighSchool(userId, infoResult.collegeseq, friendsList);

	/*根据用户登录时间排序*/
	SortByUserLoginTime(userId, infoResult.highschoolseq);
	SortByUserLoginTime(userId, infoResult.juniorschoolseq);
	SortByUserLoginTime(userId, infoResult.collegeseq);

	int highschool_size = (int)infoResult.highschoolseq.size();
	int juniorschool_size = (int)infoResult.juniorschoolseq.size();
	int college_size = (int)infoResult.collegeseq.size();	
	MCE_INFO("[OfferFriendsUtil] getHighSchoolInfo userId:" << userId << " resultSize:" << resultSize
			<< " highschool_size:" << highschool_size << " juniorschool_size:" << juniorschool_size
			<< " college_size:" << college_size << " linTime:" << linTime);

	if (highschool_size >= resultSize) {
		if (!GetRecommendByRate(userId, resultSize, infoResult.highschoolseq)) {
			infoResult.highschoolseq.resize(resultSize);
		} 
		infoResult.juniorschoolseq.clear();
		infoResult.collegeseq.clear();
	} else if (juniorschool_size >= (resultSize - highschool_size)) {
		if (!GetRecommendByRate(userId, resultSize - highschool_size, infoResult.juniorschoolseq)) {
			infoResult.juniorschoolseq.resize(resultSize - highschool_size);	
		}
		infoResult.collegeseq.clear();
	} else if (college_size >= (resultSize - highschool_size - juniorschool_size)) {
		if (!GetRecommendByRate(userId, resultSize - highschool_size - juniorschool_size, infoResult.collegeseq)) {
			infoResult.collegeseq.resize(resultSize - highschool_size - juniorschool_size);
		}
	}

	return infoResult;
}

bool OfferFriendsUtil::GetRecommendByRate(int userId, int resultSize, xce::friendfinder::RecommendItemSeq& itemseq) {
	std::vector<int> ids;
	ids.push_back(userId);
	std::map<int, int> user_gender_map = GetUserGender(ids);
	std::map<int, int>::iterator fit = user_gender_map.find(userId);
	if (fit == user_gender_map.end()) {
		return false;
	}

	int different_size = (int)(resultSize * 0.7); //异性比例提高到70%
	int same_size = (resultSize - different_size);

	MCE_INFO("[OfferFriendsUtil] GetRecommendByRate userId(" << userId << ") resultSize(" << resultSize << ") itmeseq(" 
					<< itemseq.size() << ") different_size(" << different_size << ") same_size(" << same_size << ")");

	RecommendItemSeq result_vec;	
	RecommendItemSeq backend_vec;	
	for (RecommendItemSeq::iterator iter = itemseq.begin(); iter != itemseq.end(); ++iter) {
		if (iter->gender != fit->second && different_size > 0) {
			--different_size;
			result_vec.push_back(*iter);	
		} else if (iter->gender == fit->second && same_size > 0) {
			--same_size;
			result_vec.push_back(*iter);
		} else {
			backend_vec.push_back(*iter);
		}
	}

	if (different_size == 0 && same_size == 0) {
		itemseq.swap(result_vec);
	} else if (different_size > 0) {
		for (RecommendItemSeq::iterator iter = backend_vec.begin();
				iter != backend_vec.end(); ++iter) {
			if (different_size >= 0) {
				--different_size;
				result_vec.push_back(*iter);
			}
		}
		itemseq.swap(result_vec);
	} else if (same_size > 0) {
		for (RecommendItemSeq::iterator iter = backend_vec.begin();
				iter != backend_vec.end(); ++iter) {
			if (same_size >= 0) {
				--same_size;
				result_vec.push_back(*iter);
			}
		}
		itemseq.swap(result_vec);
	}
	std::random_shuffle(itemseq.begin(), itemseq.end());  //结果作乱序处理

	return true;
}

std::map<int, int> OfferFriendsUtil::GetUserGender(const std::vector<int>& ids) {
	Statement sql;
	sql << "SELECT id, gender FROM user_born WHERE id in (";
	for (std::vector<int>::const_iterator iter = ids.begin();
			iter != ids.end(); ++iter) {
		sql << *iter;
		if (iter != ids.end() - 1) {
			sql << ", ";
		}
	}	
	sql << ")";

	std::map<int, int> user_gender_map;
	try {
		QueryRunner("user_born_friendrank", CDbRServer).query(sql, UserGenderHandler(user_gender_map));
	} catch (Ice::Exception& e) {
		MCE_WARN("[OfferFriendsUtil] GetUserGender Ice Exception " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[OfferFriendsUtil] GetUserGender std exception " << e.what());
	} catch (...) {
		MCE_WARN("[OfferFriendsUtil] GetUserGender unknown exception");
	}

	return user_gender_map;
}

void OfferFriendsUtil::SortByUserLoginTime(int userId, xce::friendfinder::RecommendItemSeq& itemseq) {
	MCE_INFO("[OfferFriendsUtil] SortByUserLoginTime userId(" << userId << ") itemseq size:" << itemseq.size());
	if (itemseq.empty()) {
		return;
	}

	std::vector<int> idlist;
	for (RecommendItemSeq::iterator iter = itemseq.begin();
			iter != itemseq.end(); ++iter) {
		idlist.push_back(iter->uid);
	}

	std::map<int, long> userLoginMap;
	getUserLoginMap(idlist, userLoginMap);

	std::map<long, RecommendItem> order_item_map;
	for (RecommendItemSeq::iterator iter = itemseq.begin();
			iter != itemseq.end(); ++iter) {
		std::map<int, long>::iterator fit = userLoginMap.find(iter->uid);
		if (fit != userLoginMap.end()) {
			order_item_map.insert(std::make_pair<long, RecommendItem>(fit->second, *iter));
		}
	}

	itemseq.clear();
	for (std::map<long, RecommendItem>::reverse_iterator iter = order_item_map.rbegin();
			iter != order_item_map.rend(); ++iter) {
		itemseq.push_back(iter->second);
	}
}

void OfferFriendsUtil::getUserLoginMap(const std::vector<int>& idlist, std::map<int, long>& userLoginMap) {
	Statement sql;
	sql << "SELECT id, UNIX_TIMESTAMP(lastlogintime) FROM user_time WHERE id in (";
	for (std::vector<int>::const_iterator iter = idlist.begin();
			iter != idlist.end(); ++iter) {
		sql << *iter;
		if (iter != idlist.end() - 1) {
			sql << ", ";
		}
	}
	sql << ")";
	try {
		QueryRunner("user_time", CDbWServer).query(sql, UserLoginHandler(userLoginMap));
	} catch (Ice::Exception& e) {
		MCE_WARN("[OfferFriendsUtil] getUserLoginMap Ice Exception: " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[OfferFriendsUtil] getUserLoginMap std Exception: " << e.what());
	} catch (...) {
		MCE_WARN("[OfferFriendsUtil] getUserLoginMap unknown Exception");
	}
}

MyUtil::IntSeq OfferFriendsUtil::getPhotoIdsFrom(const MyUtil::IntSeq& ids){
        try {
                return PhotoStateFilterAdapter::instance().getPhotoIdsFrom( ids );
        } catch( Ice::Exception e ){
                MCE_WARN("[OfferFriendsUtil::getPhotoIdsFrom] ids.size()=" << ids.size() << " found Ice::Exception : " << e.what() );
		return ids;
        } catch( ... ){
                MCE_WARN("[OfferFriendsUtil::getPhotoIdsFrom] ids.size()=" << ids.size() << " found unknown exception : " );
		return ids;
        }
	return ids;
}

map<int,bool> OfferFriendsUtil::getPhotoStateBatch(const MyUtil::IntSeq& ids){
        try {
                return PhotoStateFilterAdapter::instance().getPhotoStateBatch( ids );
        } catch( Ice::Exception e ){
                MCE_WARN("[OfferFriendsUtil::getPhotoStateBatch] ids.size()=" << ids.size() << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[OfferFriendsUtil::getPhotoStateBatch] ids.size()=" << ids.size() << " found unknown exception" );
        }
	map<int,bool> res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = true;
	}
	return res;
}

map<int,bool> OfferFriendsUtil::getOnlineStateBatch(const MyUtil::IntSeq& ids){
        try {
                return OnlineStateFilterAdapter::instance().getOnlineStateBatch( ids );
        } catch( Ice::Exception e ){
                MCE_WARN("[OfferFriendsUtil::getOnlineStateBatch] ids.size()=" << ids.size() << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[OfferFriendsUtil::getOnlineStateBatch] ids.size()=" << ids.size() << " found unknown exception" );
        }
	map<int,bool> res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = false;
	}
	return res;
}

map<int,bool> OfferFriendsUtil::getStatusBatch(const MyUtil::IntSeq& ids){
	try {
  	return StatusCacheAdapter::instance().getStatusBatch(ids );
	} catch( Ice::Exception e ){
	  MCE_WARN("[OfferFriendsUtil::getStatusBatch] ids.size()=" << ids.size() << " found Ice::Exception : " << e.what() );
	} catch( ... ){
	  MCE_WARN("[OfferFriendsUtil::getStatusBatch] ids.size()=" << ids.size() << " found unknown exception" );
	}
	map<int, bool> res;
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); 
			it != ids.end(); ++it ){
		res[*it] = false;
	}
	return res;
}

map<int,bool> OfferFriendsUtil::getStateStarBatch(const MyUtil::IntSeq& ids){
        try {
                return UserStateFilterAdapter::instance().getStateStarBatch( ids );
        } catch( Ice::Exception e ){
                MCE_WARN("[OfferFriendsUtil::getStateStarBatch] ids.size()=" << ids.size() << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[OfferFriendsUtil::getStateStarBatch] ids.size()=" << ids.size() << " found unknown exception" );
        }
	map<int,bool> res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = true;
	}
	return res;
}

map<int,bool> OfferFriendsUtil::getStateGuideBatch(const MyUtil::IntSeq& ids){
        try {
                return UserStateFilterAdapter::instance().getStateGuideBatch( ids );
        } catch( Ice::Exception e ){
                MCE_WARN("[OfferFriendsUtil::getStateGuideBatch] ids.size()=" << ids.size() << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[OfferFriendsUtil::getStateGuideBatch] ids.size()=" << ids.size() << " found unknown exception" );
        }
	map<int,bool> res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = false;
	}
	return res;
}

map<int,int> OfferFriendsUtil::getViewCountMap(const MyUtil::IntSeq& ids){
	map<int,int> res;
        if( ids.empty() ){
		return res;
	}
	ostringstream idstr;
        MyUtil::IntSeq::const_iterator it = ids.begin();
	idstr << "(" << *it;
        for ( ++it; it!=ids.end(); ++it) {
		idstr << ", " << *it;
        }
	idstr << ")";

	Statement sql;
	sql	<< "SELECT id,viewcount FROM view_count"
		<< " WHERE id IN " << idstr.str();

	QueryRunner("user_view_count_preloader", CDbRServer).query( sql, ViewCountHandler(res) );

        return res;
}

void OfferFriendsUtil::filterHostFriends(int userId, std::vector<int>& infoFriends, const std::vector<int>& friendsList) {
	vector<int> result;

	for(vector<int>::iterator iter=infoFriends.begin(); iter!=infoFriends.end(); ++iter) {
		vector<int>::const_iterator fIt = lower_bound( friendsList.begin(), friendsList.end(), *iter );
		if( ( fIt != friendsList.end() && *fIt == *iter )
			 || ( *iter == userId ) ) {
			continue;
		} 
		result.push_back( *iter );
	}
	
	random_shuffle( result.begin(), result.end() );                 //随机排序资料匹配结果
	infoFriends = result;
}

void OfferFriendsUtil::filterHostFriendsForHighSchool(int userId, RecommendItemSeq& itemseq, const std::vector<int>& friendsList) {
	RecommendItemSeq result;

	for(RecommendItemSeq::iterator iter = itemseq.begin(); 
			iter != itemseq.end(); ++iter) {
		vector<int>::const_iterator fIt = lower_bound( friendsList.begin(), friendsList.end(), iter->uid );
		if( ( fIt != friendsList.end() && *fIt == iter->uid )
			 || ( iter->uid == userId ) ) {
			continue;
		} 
		result.push_back( *iter );
	}
	
	random_shuffle( result.begin(), result.end() );                 //随机排序资料匹配结果
	itemseq = result;
}

//***************************************************************************************************************

ApplyResultHandler::~ApplyResultHandler() {
}

bool ApplyResultHandler::handle(mysqlpp::Row& row) const {
	int friendId = (int)row["accepter"];
	_result.push_back( friendId );
	return true;
} 

BlockResultHandler::BlockResultHandler(MyUtil::IntSeq& results, const std::string& toField) :
        _results(results), _toField(toField) {
}

BlockResultHandler::~BlockResultHandler() {
}

bool BlockResultHandler::handle(mysqlpp::Row& row) const {
        int to = (int)row[_toField.c_str()];
        _results.push_back(to);
        return true;
}

bool ViewCountHandler::handle(mysqlpp::Row& row) const {
	int userId = (int)row["id"];
	int viewCount = (int)row["viewcount"];
	_viewCountMap.insert( make_pair<int, int>(userId, viewCount) );
	return true;
}

bool UserLoginHandler::handle(mysqlpp::Row& row) const {
	int uid = (int)row[0];
	userLoginMap_[uid] = (long)row[1];
	return true;
}

bool UserGenderHandler::handle(mysqlpp::Row& row) const {
	int userid = (int)row["id"];
	std::string gender = std::string(row["gender"]);
	if ("男生" == gender) {
		user_gender_map_[userid] = 1;	
	} else {
		user_gender_map_[userid] = 0;
	}
	return true;
}

