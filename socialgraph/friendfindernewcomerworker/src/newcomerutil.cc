#include "newcomerutil.h"
#include <BuddyByIdReplicaAdapter.h>
#include <FriendCountCacheReplicaAdapter.h>
//#include <FriendFinderReplicaAdapter.h>
#include <FriendFinderInfoCacheReplicaAdapter.h>
#include <FriendFinderByIPReplicaAdapter.h>
#include <IPRecordReplicaAdapter.h>
#include <PhotoStateFilterAdapter.h>
#include <OnlineStateFilterAdapter.h>
#include <StatusCacheAdapter.h>
#include <UserStateFilterAdapter.h>
#include <UserNetworkAdapter.h>

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::socialgraph;
using namespace xce::socialgraph::friendfindernewcomer;

//using namespace com::xiaonei::service;
using namespace xce::friendfinder;
using namespace mop::hi::svc::model;

using namespace xce::buddy::adapter;
using namespace xce::friendfinder::adapter;
using namespace xce::iprecord::adapter;

using namespace xce::photostatefilter::adapter;
using namespace xce::onlinestatefilter::adapter;
using namespace xce::userstatefilter::adapter;
using namespace xce::statuscache;

using namespace mop::hi::svc::adapter;

MyUtil::IntSeq NewComerUtil::getFriendSeq(int userId) {
        MyUtil::IntSeq buddies;
        try {
                buddies = BuddyByIdCacheAdapter::instance().getFriendListAsc( userId, -1 );
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::getFriendSeq] userId=" << userId << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[NewComerUtil::getFriendSeq] userId=" << userId << " found Unknown::Exception" );
        }
	return buddies;
}

MyUtil::Int2IntSeqMap NewComerUtil::getFriendSeqBatch(const MyUtil::IntSeq& ids) {
        Int2IntSeqMap result;
        if (ids.empty()) {
                return result;
        }
        return BuddyByIdCacheAdapter::instance().getFriendLists(ids);
}

MyUtil::Int2IntMap NewComerUtil::getFriendCountBatch(const MyUtil::IntSeq& ids) {
        MyUtil::Int2IntMap result;
        if (ids.empty()) {
                return result;
        }
	return FriendCountCacheAdapter::instance().getFriendCounts(ids);
}

MyUtil::IntSeq NewComerUtil::getApplySeq(int userId) {
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

MyUtil::IntSeq NewComerUtil::getBlockSeq(int id) {
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

MyUtil::IntSeq NewComerUtil::getCommBlockSeq(int id) {
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

/*
CommonFriendSeq NewComerUtil::getCommonFriend(int userId, int commLimit) {
        CommonFriendSeq commonFriends;
        try{
                commonFriends = FriendFinderReplicaAdapter::instance().getCommonFriend(userId, 0, commLimit);
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::getCommonFriend] userId=" << userId << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[NewComerUtil::getCommonFriend] userId=" << userId << " found Unknown::Exception" );
        }
	return commonFriends;
}

void NewComerUtil::rebuildFriendFinder(int userId) {
	try{
		FriendFinderReplicaAdapter::instance().rebuildCommonFriend(userId);
	} catch( Ice::Exception e ){
		MCE_WARN("[NewComerUtil::rebuildFriendFinder] userId=" << userId << " found Ice::Exception : " << e.what() );
	} catch( ... ){
		MCE_WARN("[NewComerUtil::rebuildFriendFinder] userId=" << userId << " found Unknown::Exception" );
	}
}
*/

IPNoteSeq NewComerUtil::getFreqIP(int userId, int ipLimit) {
	IPNoteSeq ipSeq;
        try{
                ipSeq = FriendFinderByIPAdapter::instance().getFreqIP(userId, ipLimit);
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::getFreqIP] userId=" << userId << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[NewComerUtil::getFreqIP] userId=" << userId << " found Unkown::Exception : " );
        }
	return ipSeq;
}

MyUtil::IntSeq NewComerUtil::getUsersWithMaskByIP(Ice::Long ip,Ice::Int limit,const MyUtil::IntSeq& mask){
	MyUtil::IntSeq ipFriendList;
        try{
		ipFriendList = IPRecordAdapter::instance().getUsersWithMask( ip, limit, mask );
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::getUsersWithMask] ip=" << ip << " limit=" << limit << " mask.size=" << mask.size() << " found Ice::Exception : " << e.what() ); 
        } catch( ... ){
                MCE_WARN("[NewComerUtil::getUsersWithMask] ip=" << ip << " limit=" << limit << " mask.size=" << mask.size() << " found Unknown::Exception : " ); 
        }
        return ipFriendList;
}

NetworkInfo NewComerUtil::getNetworkInfoWithHostage(int userId, int size, const std::vector<int>& friendsList){
	struct timeval tvStart, tvEnd;
	double linStart, linEnd, linTime;

	gettimeofday( &tvStart, NULL );
	NetworkInfo infoResult;
	try {
		infoResult = FriendFinderInfoCacheReplicaAdapter::instance().getNetworkInfoWithHostage( userId );
	} catch( Ice::Exception e ){
		MCE_WARN("[NewComerUtil::getUsersWithMask] getNetworkInfoWithHostage userId=" << userId << " found Ice::Exception : " << e.what());
	} catch( ... ){
		MCE_WARN("[NewComerUtil::getUsersWithMask] getNetworkInfoWithHostage userId=" << userId << " found unknown exception : " );
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

NetworkInfo NewComerUtil::getNetworkInfoWithHostageFromDB(int userId, int size, const std::vector<int>& friendsList){
	struct timeval tvStart, tvEnd;
	double linStart, linEnd, linTime;

	gettimeofday( &tvStart, NULL );
	NetworkInfo infoResult;
	try {
		infoResult = FriendFinderInfoCacheReplicaAdapter::instance().getNetworkInfoWithHostageFromDB( userId );
	} catch( Ice::Exception e ){
		MCE_WARN("[NewComerUtil::getUsersWithMask] getNetworkInfoWithHostageFromDB userId=" << userId << " found Ice::Exception : " << e.what());
	} catch( ... ){
		MCE_WARN("[NewComerUtil::getUsersWithMask] getNetworkInfoWithHostageFromDB userId=" << userId << " found unknown exception : " );
	} 

	gettimeofday( &tvEnd, NULL );

	linStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);
	linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
	linTime = linEnd - linStart;
	linTime = linTime/1000000;
	
	int listSize = infoResult.univInfo.size() + infoResult.highschoolInfo.size() + infoResult.juniorschoolInfo.size() + infoResult.elementaryschoolInfo.size(); 

	if(listSize <= 0 || size < 0 ) {
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

MyUtil::IntSeq NewComerUtil::getUnivData(int userId, const MyUtil::StrSeq& condition){
	MyUtil::IntSeq res;
	try {
		res = FriendFinderInfoCacheReplicaAdapter::instance().getUnivData( userId, condition );
	} catch( Ice::Exception e ){
		MCE_WARN("[NewComerUtil::getUnivData] userId=" << userId << " found Ice::Exception : " << e.what());
	} catch( ... ){
		MCE_WARN("[NewComerUtil::getUnivData] userId=" << userId << " found unknown exception : " );
	} 
	return res;
}

MyUtil::IntSeq NewComerUtil::getHighschoolData(int userId, const MyUtil::StrSeq& condition){
	MyUtil::IntSeq res;
	try {
		res = FriendFinderInfoCacheReplicaAdapter::instance().getHighschoolData( userId, condition );
	} catch( Ice::Exception e ){
		MCE_WARN("[NewComerUtil::getHighschoolData] userId=" << userId << " found Ice::Exception : " << e.what());
	} catch( ... ){
		MCE_WARN("[NewComerUtil::getHighschoolData] userId=" << userId << " found unknown exception : " );
	} 
	return res;
}

MyUtil::IntSeq NewComerUtil::getJuniorschoolData(int userId, const MyUtil::StrSeq& condition){
	MyUtil::IntSeq res;
	try {
		res = FriendFinderInfoCacheReplicaAdapter::instance().getJuniorschoolData( userId, condition );
	} catch( Ice::Exception e ){
		MCE_WARN("[NewComerUtil::getJuniorschoolData] userId=" << userId << " found Ice::Exception : " << e.what());
	} catch( ... ){
		MCE_WARN("[NewComerUtil::getJuniorschoolData] userId=" << userId << " found unknown exception : " );
	} 
	return res;
}

MyUtil::IntSeq NewComerUtil::getPhotoIdsFrom(const MyUtil::IntSeq& ids){
        try {
                return PhotoStateFilterAdapter::instance().getPhotoIdsFrom( ids );
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::getPhotoIdsFrom] ids.size()=" << ids.size() << " found Ice::Exception : " << e.what() );
		return ids;
        } catch( ... ){
                MCE_WARN("[NewComerUtil::getPhotoIdsFrom] ids.size()=" << ids.size() << " found unknown exception : " );
		return ids;
        }
	return ids;
}

map<int,bool> NewComerUtil::getPhotoStateBatch(const MyUtil::IntSeq& ids){
        try {
                return PhotoStateFilterAdapter::instance().getPhotoStateBatch( ids );
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::getPhotoStateBatch] ids.size()=" << ids.size() << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[NewComerUtil::getPhotoStateBatch] ids.size()=" << ids.size() << " found unknown exception" );
        }
	map<int,bool> res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = true;
	}
	return res;
}

bool NewComerUtil::isPhotoStateTrue(int userId){
        try {
                return PhotoStateFilterAdapter::instance().isPhotoStateTrue( userId );
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::isPhotoStateTrue] userId=" << userId << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[NewComerUtil::isPhotoStateTrue] userId=" << userId << " found unknown exception" );
        }
	return true;
}

map<int,bool> NewComerUtil::getOnlineStateBatch(const MyUtil::IntSeq& ids){
        try {
                return OnlineStateFilterAdapter::instance().getOnlineStateBatch( ids );
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::getOnlineStateBatch] ids.size()=" << ids.size() << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[NewComerUtil::getOnlineStateBatch] ids.size()=" << ids.size() << " found unknown exception" );
        }
	map<int,bool> res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = false;
	}
	return res;
}

bool NewComerUtil::isOnlineStateTrue(int userId){
        try {
                return OnlineStateFilterAdapter::instance().isOnlineStateTrue( userId );
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::isOnlineStateTrue] userId=" << userId << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[NewComerUtil::isOnlineStateTrue] userId=" << userId << " found unknown exception" );
        }
	return false;
}

map<int,bool> NewComerUtil::getStatusBatch(const MyUtil::IntSeq& ids){
        try {
                return StatusCacheAdapter::instance().getStatusBatch( ids );
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::getStatusBatch] ids.size()=" << ids.size() << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[NewComerUtil::getStatusBatch] ids.size()=" << ids.size() << " found unknown exception" );
        }
	map<int,bool> res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = false;
	}
	return res;
}

bool NewComerUtil::isStatusTrue(int userId){
        try {
                return StatusCacheAdapter::instance().isStatusTrue( userId );
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::isStatusTrue] userId=" << userId << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[NewComerUtil::isStatusTrue] userId=" << userId << " found unknown exception" );
        }
	return false;
}

map<int,bool> NewComerUtil::getStateStarBatch(const MyUtil::IntSeq& ids){
        try {
                return UserStateFilterAdapter::instance().getStateStarBatch( ids );
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::getStateStarBatch] ids.size()=" << ids.size() << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[NewComerUtil::getStateStarBatch] ids.size()=" << ids.size() << " found unknown exception" );
        }
	map<int,bool> res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = true;
	}
	return res;
}

bool NewComerUtil::isStateStarTrue(int userId){
        try {
                return UserStateFilterAdapter::instance().isStateStarTrue( userId );
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::isStateStarTrue] userId=" << userId << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[NewComerUtil::isStateStarTrue] userId=" << userId << " found unknown exception" );
        }
	return true;
}

map<int,bool> NewComerUtil::getStateGuideBatch(const MyUtil::IntSeq& ids){
        try {
                return UserStateFilterAdapter::instance().getStateGuideBatch( ids );
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::getStateGuideBatch] ids.size()=" << ids.size() << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[NewComerUtil::getStateGuideBatch] ids.size()=" << ids.size() << " found unknown exception" );
        }
	map<int,bool> res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = false;
	}
	return res;
}

bool NewComerUtil::isStateGuideTrue(int userId){
        try {
                return UserStateFilterAdapter::instance().isStateGuideTrue( userId );
        } catch( Ice::Exception e ){
                MCE_WARN("[NewComerUtil::isStateGuideTrue] userId=" << userId << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[NewComerUtil::isStateGuideTrue] userId=" << userId << " found unknown exception" );
        }
	return false;
}

map<int,int> NewComerUtil::getViewCountMap(const MyUtil::IntSeq& ids){
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

void NewComerUtil::filterHostFriends(int userId, std::vector<int>& infoFriends, const std::vector<int>& friendsList) {
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

string NewComerUtil::getSchoolName(int userId,int schoolId){
        NetworkBigObjectNPtr obj = UserNetworkAdapter::instance().getBigObjectN(userId);

	for( UniversityInfoSeq::iterator it=obj->universitys.begin(); it!=obj->universitys.end(); ++it ){
		if( (*it)->universityId == schoolId ){
			return (*it)->universityName;
		}
	}
	for( HighSchoolInfoSeq::iterator it=obj->highschools.begin(); it!=obj->highschools.end(); ++it ){
		if( (*it)->HighSchoolId == schoolId ){
			return (*it)->HighSchoolName;
		}
	}
	return "";
	//WorkspaceInfoSeq workspaces;
	//CollegeInfoSeq collages;
	//ElementarySchoolInfo elementary;
	//JuniorHighSchoolInfo juniorhighschool;
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

