#include "ScoreCacheAdapter.h"

using namespace xce::scorecache;
using namespace xce::scorecache::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= ScoreCacheNAdapter =================================

ScoreDataN ScoreCacheNAdapter::getScoreDataN(int userId){
	return getProxy(userId)->getScoreDataN(userId);
}
ScoreDataNMap ScoreCacheNAdapter::getScoreDataNMap(const MyUtil::IntSeq& ids){
	if (ids.empty()){
		return ScoreDataNMap();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[ScoreCacheNAdapter::getScoreDataNMap] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return ScoreDataNMap();
	}
	if (numCluster == 0 || numCluster == 1) {
		try {
			return getProxy(ids.at(0))->getScoreDataNMap(ids);
		} catch (...) {
			return ScoreDataNMap();
		}
	} else {
		vector< vector<int> > seperatedIds;
		seperatedIds.resize(numCluster);
		for (MyUtil::IntSeq::const_iterator it = ids.begin(); it
				!= ids.end(); ++it) {
			seperatedIds.at(*it % numCluster).push_back(*it);
		}
		ScoreDataNMap result;
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			ScoreDataNMap someResult;
			if( seperatedIds.at(posCluster).size() > 0 ){
				try {
					someResult = getProxy(posCluster)->getScoreDataNMap(seperatedIds.at(posCluster));
				} catch (...) {
					MCE_WARN( "ScoreCacheNAdapter::getScoreDataNMap from Proxy(" << posCluster << ") handle unknown exception." );
				}
				result.insert( someResult.begin(), someResult.end() );
			}
		}
		return result;
	}
}
ClientScoreDataN ScoreCacheNAdapter::getClientScoreDataN(int userId){
	ScoreDataN sdn = getProxy(userId)->getScoreDataN(userId);
	return ClientScoreDataN( sdn );
}

ScoreDataN ScoreCacheNAdapter::getScoreDataNNotNull(int userId){
	return getProxy(userId)->getScoreDataNNotNull(userId);
}
ScoreDataNMap ScoreCacheNAdapter::getScoreDataNMapNotNull(const MyUtil::IntSeq& ids){
	if (ids.empty()){
		return ScoreDataNMap();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[ScoreCacheNAdapter::getScoreDataNMapNotNull] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return ScoreDataNMap();
	}
	if (numCluster == 0 || numCluster == 1) {
		try {
			return getProxy(ids.at(0))->getScoreDataNMapNotNull(ids);
		} catch (...) {
			return ScoreDataNMap();
		}
	} else {
		vector< vector<int> > seperatedIds;
		seperatedIds.resize(numCluster);
		for (MyUtil::IntSeq::const_iterator it = ids.begin(); it
				!= ids.end(); ++it) {
			seperatedIds.at(*it % numCluster).push_back(*it);
		}
		ScoreDataNMap result;
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			ScoreDataNMap someResult;
			if( seperatedIds.at(posCluster).size() > 0 ){
				try {
					someResult = getProxy(posCluster)->getScoreDataNMapNotNull(seperatedIds.at(posCluster));
				} catch (...) {
					MCE_WARN( "ScoreCacheNAdapter::getScoreDataNMapNotNull from Proxy(" << posCluster << ") handle unknown exception." );
				}
				result.insert( someResult.begin(), someResult.end() );
			}
		}
		return result;
	}
}

vector<ScoreDataN> ScoreCacheNAdapter::getScoreDataNAllPrx(int userId){
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	if( prxs.size() <= 0 ){
		return vector<ScoreDataN>();
	}
	vector<ScoreDataN> result;
	for( vector<ScoreCacheNManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
		int trys = 3;
		do{
			try {
				ScoreDataN data = (*itPrx)->getScoreDataNNotNull(userId);
				result.push_back( data );
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::getScoreDataNAllPrx("<<trys<<")] userId=" << userId
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::getScoreDataNAllPrx("<<trys<<")] userId=" << userId
						<< " found unknown exception" );
			}
		} while( --trys>0 );
		if( trys == 0 ){
			return vector<ScoreDataN>();
		}
	}
	return result;
}

vector<ScoreDataNMap> ScoreCacheNAdapter::getScoreDataNMapAllPrx(const MyUtil::IntSeq& ids){
	if (ids.empty()){
		return vector<ScoreDataNMap>();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[ScoreCacheNAdapter::getScoreDataNMapAllPrx] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return vector<ScoreDataNMap>();
	}

	vector<ScoreDataNMap> result;
	vector<ScoreCacheNManagerPrx> prx0 = getAllProxySeq( 0 );
	int prx_count = prx0.size();
	result.resize(prx_count);
	if( prx_count <= 0 ){
		return result;
	}

	if (numCluster == 0 || numCluster == 1) {
		for( int prx_index=0; prx_index<prx_count; ++prx_index ){
			int trys = 3;
			do{
				try {
					ScoreDataNMap data = prx0.at(prx_index)->getScoreDataNMapNotNull(ids);
					result.at(prx_index).insert( data.begin(), data.end() );
					break;
				} catch( Ice::Exception e ){
					MCE_WARN("[ScoreCacheNAdapter::getScoreDataNMapAllPrx("<<trys<<")]"
							<< " found Ice::Exception : " << e.what() );
				} catch( ... ){
					MCE_WARN("[ScoreCacheNAdapter::getScoreDataNMapAllPrx("<<trys<<")]"
							<< " found unknown exception" );
				}
			} while( --trys>0 );
		}
		return result;
	} else {
		vector< vector<int> > seperatedIds;
		seperatedIds.resize(numCluster);
		for (MyUtil::IntSeq::const_iterator it = ids.begin(); it
				!= ids.end(); ++it) {
			seperatedIds.at(*it % numCluster).push_back(*it);
		}

		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			if( seperatedIds.at(posCluster).size() == 0 ) continue;
			vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( posCluster );

			for( size_t prx_index=0; prx_index<prxs.size(); ++prx_index ){
				int trys = 3;
				do{
					try {
						ScoreDataNMap data = prxs.at(prx_index)->getScoreDataNMapNotNull(seperatedIds.at(posCluster));
						result.at(prx_index).insert( data.begin(), data.end() );
						break;
					} catch( Ice::Exception e ){
						MCE_WARN("[ScoreCacheNAdapter::getScoreDataNMapAllPrx("<<trys<<")]"
								<< " found Ice::Exception : " << e.what() );
					} catch( ... ){
						MCE_WARN("[ScoreCacheNAdapter::getScoreDataNMapAllPrx("<<trys<<")]"
								<< " found unknown exception" );
					}
				} while( --trys>0 );
			}
		}
		return result;
	}
}

Ice::ObjectSeq ScoreCacheNAdapter::setLoginDays( int userId, int historyLoginDays, int continueLoginDays, long lastLoginTime, int loginType, bool& succ ){
	succ = true;
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	if( prxs.size() <= 0 ){
		succ = false;
		return Ice::ObjectSeq();
	}
	Ice::ObjectSeq datas;
	bool done = false;
	ostringstream log;
	for( size_t i=0; i<prxs.size(); ++i ){
		int trys = 3;
		do{
			try {
				Ice::ObjectSeq tmp = prxs[i]->setLoginDays(userId,historyLoginDays,continueLoginDays,lastLoginTime,loginType);
				log <<"("<<i<<"."<<trys<<")";
				if( !done && tmp.size()==2 ){
					datas = tmp;
					done = true;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::setLoginDays("<<i<<"."<<trys<<")] userId=" << userId
						<< " historyLoginDays=" << historyLoginDays
						<< " continueLoginDays=" << continueLoginDays
						<< " lastLoginTime=" << lastLoginTime
						<< " loginType=" << loginType
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::setLoginDays("<<i<<"."<<trys<<")] userId=" << userId
						<< " historyLoginDays=" << historyLoginDays
						<< " continueLoginDays=" << continueLoginDays
						<< " lastLoginTime=" << lastLoginTime
						<< " loginType=" << loginType
						<< " found unknown exception" );
			}
		} while( --trys>0 );
		if( trys==0 ){
			succ = false;
		}
	}
	MCE_INFO("[ScoreCacheNAdapter::setLoginDays"<<log.str()<<"] userId=" << userId );
	return datas;
}
Ice::ObjectSeq ScoreCacheNAdapter::updateLoginType( int userId, int loginType, bool& succ ){
	succ = true;
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	if( prxs.size() <= 0 ){
		succ = false;
		return Ice::ObjectSeq();
	}
	Ice::ObjectSeq datas;
	bool done = false;
	for( size_t i=0; i<prxs.size(); ++i ){
		int trys = 3;
		do{
			try {
				Ice::ObjectSeq tmp = prxs[i]->updateLoginType( userId, loginType );
				if( !done && tmp.size()==2 ){
					datas = tmp;
					done = true;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::updateLoginType("<<i<<"."<<trys<<")] userId=" << userId
						<< " loginType=" << loginType
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::updateLoginType("<<i<<"."<<trys<<")] userId=" << userId
						<< " loginType=" << loginType
						<< " found unknown exception" );
			}
		} while( --trys>0 );
		if( trys==0 ){
			succ = false;
		}
	}
	return datas;
}
Ice::ObjectSeq ScoreCacheNAdapter::setScore(int userId, int score, bool& succ){
	succ = true;
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	if( prxs.size() <= 0 ){
		succ = false;
		return Ice::ObjectSeq();
	}
	Ice::ObjectSeq datas;
	bool done = false;
	for( size_t i=0; i<prxs.size(); ++i ){
		int trys = 3;
		do{
			try {
				Ice::ObjectSeq tmp = prxs[i]->setScore( userId, score );
				if( !done && tmp.size()==2 ){
					datas = tmp;
					done = true;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::setScore("<<i<<"."<<trys<<")] userId=" << userId
						<< " score=" << score
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::setScore("<<i<<"."<<trys<<")] userId=" << userId
						<< " score=" << score
						<< " found unknown exception" );
			}
		} while( --trys>0 );
		if( trys==0 ){
			succ = false;
		}
	}
	return datas;
}
Ice::ObjectSeq ScoreCacheNAdapter::incScore(int userId, int inc, bool& succ){
	succ = true;
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	if( prxs.size() <= 0 ){
		succ = false;
		return Ice::ObjectSeq();
	}
	Ice::ObjectSeq datas;
	bool done = false;
	for( size_t i=0; i<prxs.size(); ++i ){
		int trys = 3;
		do{
			try {
				Ice::ObjectSeq tmp = prxs[i]->incScore( userId, inc );
				if( !done && tmp.size()==2 ){
					datas = tmp;
					done = true;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::incScore("<<i<<"."<<trys<<")] userId=" << userId
						<< " inc=" << inc
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::incScore("<<i<<"."<<trys<<")] userId=" << userId
						<< " inc=" << inc
						<< " found unknown exception" );
			}
		} while( --trys>0 );
		if( trys==0 ){
			succ = false;
		}
	}
	return datas;
}
Ice::ObjectSeq ScoreCacheNAdapter::decScore(int userId, int dec, bool& succ){
	succ = true;
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	if( prxs.size() <= 0 ){
		succ = false;
		return Ice::ObjectSeq();
	}
	Ice::ObjectSeq datas;
	bool done = false;
	for( size_t i=0; i<prxs.size(); ++i ){
		int trys = 3;
		do{
			try {
				Ice::ObjectSeq tmp = prxs[i]->decScore( userId, dec );
				if( !done && tmp.size()==2 ){
					datas = tmp;
					done = true;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::decScore("<<i<<"."<<trys<<")] userId=" << userId
						<< " dec=" << dec
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::decScore("<<i<<"."<<trys<<")] userId=" << userId
						<< " dec=" << dec
						<< " found unknown exception" );
			}
		} while( --trys>0 );
		if( trys==0 ){
			succ = false;
		}
	}
	return datas;
}
Ice::ObjectSeq ScoreCacheNAdapter::setAwardCount(int userId, int awardCount, bool& succ){
	succ = true;
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	if( prxs.size() <= 0 ){
		succ = false;
		return Ice::ObjectSeq();
	}
	Ice::ObjectSeq datas;
	bool done = false;
	for( size_t i=0; i<prxs.size(); ++i ){
		int trys = 3;
		do{
			try {
				Ice::ObjectSeq tmp = prxs[i]->setAwardCount( userId, awardCount );
				if( !done && tmp.size()==2 ){
					datas = tmp;
					done = true;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::setAwardCount("<<i<<"."<<trys<<")] userId=" << userId
						<< " awardCount=" << awardCount
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::setAwardCount("<<i<<"."<<trys<<")] userId=" << userId
						<< " awardCount=" << awardCount
						<< " found unknown exception" );
			}
		} while( --trys>0 );
		if( trys==0 ){
			succ = false;
		}
	}
	return datas;
}
Ice::ObjectSeq ScoreCacheNAdapter::incAwardCount(int userId, int inc, bool& succ){
	succ = true;
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	if( prxs.size() <= 0 ){
		succ = false;
		return Ice::ObjectSeq();
	}
	Ice::ObjectSeq datas;
	bool done = false;
	for( size_t i=0; i<prxs.size(); ++i ){
		int trys = 3;
		do{
			try {
				Ice::ObjectSeq tmp = prxs[i]->incAwardCount( userId, inc );
				if( !done && tmp.size()==2 ){
					datas = tmp;
					done = true;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::incAwardCount("<<i<<"."<<trys<<")] userId=" << userId
						<< " inc=" << inc
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::incAwardCount("<<i<<"."<<trys<<")] userId=" << userId
						<< " inc=" << inc
						<< " found unknown exception" );
			}
		} while( --trys>0 );
		if( trys==0 ){
			succ = false;
		}
	}
	return datas;
}
Ice::ObjectSeq ScoreCacheNAdapter::decAwardCount(int userId, int dec, bool& succ){
	succ = true;
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	if( prxs.size() <= 0 ){
		succ = false;
		return Ice::ObjectSeq();
	}
	Ice::ObjectSeq datas;
	bool done = false;
	for( size_t i=0; i<prxs.size(); ++i ){
		int trys = 3;
		do{
			try {
				Ice::ObjectSeq tmp = prxs[i]->decAwardCount( userId, dec );
				if( !done && tmp.size()==2 ){
					datas = tmp;
					done = true;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::decAwardCount("<<i<<"."<<trys<<")] userId=" << userId
						<< " dec=" << dec
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::decAwardCount("<<i<<"."<<trys<<")] userId=" << userId
						<< " dec=" << dec
						<< " found unknown exception" );
			}
		} while( --trys>0 );
		if( trys==0 ){
			succ = false;
		}
	}
	return datas;
}

Ice::ObjectSeq ScoreCacheNAdapter::setOn(int userId, int pos, bool& succ){
	succ = true;
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	if( prxs.size() <= 0 ){
		succ = false;
		return Ice::ObjectSeq();
	}
	Ice::ObjectSeq datas;
	bool done = false;
	for( size_t i=0; i<prxs.size(); ++i ){
		int trys = 3;
		do{
			try {
				Ice::ObjectSeq tmp = prxs[i]->setOn( userId, pos );
				if( !done && tmp.size()==2 ){
					datas = tmp;
					done = true;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::setOn("<<i<<"."<<trys<<")] userId=" << userId
						<< " pos=" << pos
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::setOn("<<i<<"."<<trys<<")] userId=" << userId
						<< " pos=" << pos
						<< " found unknown exception" );
			}
		} while( --trys>0 );
		if( trys==0 ){
			succ = false;
		}
	}
	return datas;
}
Ice::ObjectSeq ScoreCacheNAdapter::setOff(int userId, int pos, bool& succ){
	succ = true;
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	if( prxs.size() <= 0 ){
		succ = false;
		return Ice::ObjectSeq();
	}
	Ice::ObjectSeq datas;
	bool done = false;
	for( size_t i=0; i<prxs.size(); ++i ){
		int trys = 3;
		do{
			try {
				Ice::ObjectSeq tmp = prxs[i]->setOff( userId, pos );
				if( !done && tmp.size()==2 ){
					datas = tmp;
					done = true;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::setOff("<<i<<"."<<trys<<")] userId=" << userId
						<< " pos=" << pos
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::setOff("<<i<<"."<<trys<<")] userId=" << userId
						<< " pos=" << pos
						<< " found unknown exception" );
			}
		} while( --trys>0 );
		if( trys==0 ){
			succ = false;
		}
	}
	return datas;
}

Ice::ObjectSeq ScoreCacheNAdapter::updateVipState(int userId, int state, bool& succ){
	succ = true;
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	if( prxs.size() <= 0 ){
		succ = false;
		return Ice::ObjectSeq();
	}
	Ice::ObjectSeq datas;
	bool done = false;
	for( size_t i=0; i<prxs.size(); ++i ){
		int trys = 3;
		do{
			try {
				Ice::ObjectSeq tmp = prxs[i]->updateVipState( userId, state );
				if( !done && tmp.size()==2 ){
					datas = tmp;
					done = true;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::updateVipState("<<i<<"."<<trys<<")] userId=" << userId
						<< " state=" << state
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::updateVipState("<<i<<"."<<trys<<")] userId=" << userId
						<< " state=" << state
						<< " found unknown exception" );
			}
		} while( --trys>0 );
		if( trys==0 ){
			succ = false;
		}
	}
	return datas;
}

Ice::ObjectSeq ScoreCacheNAdapter::updateVipLevel(int userId, int level, bool& succ){
	succ = true;
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	if( prxs.size() <= 0 ){
		succ = false;
		return Ice::ObjectSeq();
	}
	Ice::ObjectSeq datas;
	bool done = false;
	for( size_t i=0; i<prxs.size(); ++i ){
		int trys = 3;
		do{
			try {
				Ice::ObjectSeq tmp = prxs[i]->updateVipLevel( userId, level );
				if( !done && tmp.size()==2 ){
					datas = tmp;
					done = true;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::updateVipLevel("<<i<<"."<<trys<<")] userId=" << userId
						<< " level=" << level
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::updateVipLevel("<<i<<"."<<trys<<")] userId=" << userId
						<< " level=" << level
						<< " found unknown exception" );
			}
		} while( --trys>0 );
		if( trys==0 ){
			succ = false;
		}
	}
	return datas;
}

void ScoreCacheNAdapter::load( int userId, const Ice::ObjectPtr& o ){
	vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( userId );
	for( vector<ScoreCacheNManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
		int trys = 3;
		do{
			try {
				(*itPrx)->load( userId, o );
				break;
			} catch(Ice::TimeoutException e){ 
				MCE_WARN("[ScoreCacheNAdapter::load] userId=" << userId << " found Ice::TimeoutException : " << e.what() );
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::load] userId=" << userId << " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::load] userId=" << userId << " found unkown exception : " );
			}
		} while( --trys>0 );
	}
}

void ScoreCacheNAdapter::setData( const MyUtil::ObjectResultPtr& datas ){
	if (datas->data.empty()){
		return;
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[ScoreCacheNAdapter::setData] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return;
	}
	if (numCluster == 0 || numCluster == 1) {
		try {
			getProxy(0)->setData(datas);
		} catch(Ice::TimeoutException e){
			MCE_WARN("[ScoreCacheNAdapter::setData] Proxy(0) handle Ice::TimeoutException : " << e.what() );
		} catch( Ice::Exception e ){
			MCE_WARN("[ScoreCacheNAdapter::setData] Proxy(0) handle Ice::Exception : " << e.what() );
		} catch( ... ){
			MCE_WARN("[ScoreCacheNAdapter::setData] Proxy(0) handle unkown exception : " );
		}
	} else {
		vector< MyUtil::ObjectResultPtr > seperatedObjs;
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			seperatedObjs.push_back( new MyUtil::ObjectResult() );
		}
		for ( std::map<Ice::Long, Ice::ObjectPtr>::const_iterator it = datas->data.begin();
				it!= datas->data.end(); ++it ){
			seperatedObjs.at(it->first % numCluster)->data.insert( make_pair<Ice::Long, Ice::ObjectPtr>(it->first,it->second) );
		}
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			if( seperatedObjs.at(posCluster)->data.size() > 0 ){
				vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( posCluster );
				for( vector<ScoreCacheNManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
					try {
						(*itPrx)->setData( seperatedObjs.at(posCluster) );
					} catch(Ice::TimeoutException e){
						MCE_WARN("[ScoreCacheNAdapter::setData] Proxy(" << posCluster << ") handle Ice::TimeoutException : " << e.what() );
						continue;
					} catch( Ice::Exception e ){
						MCE_WARN("[ScoreCacheNAdapter::setData] Proxy(" << posCluster << ") handle Ice::Exception : " << e.what() );
						continue;
					} catch( ... ){
						MCE_WARN("[ScoreCacheNAdapter::setData] Proxy(" << posCluster << ") handle unkown exception : " );
						continue;
					}
				}
			}
		}
	}
}

void ScoreCacheNAdapter::setMask(long mask){
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[ScoreCacheNAdapter::setMask] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return;
	}
	if (numCluster == 0 || numCluster == 1) {
		vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( 0 );
		for( vector<ScoreCacheNManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
			try {
				(*itPrx)->setMask( mask );
			} catch(Ice::TimeoutException e){ 
				MCE_WARN("[ScoreCacheNAdapter::setMask] found Ice::TimeoutException : " << e.what() );
				continue;
			} catch( Ice::Exception e ){
				MCE_WARN("[ScoreCacheNAdapter::setMask] found Ice::Exception : " << e.what() );
				continue;
			} catch( ... ){
				MCE_WARN("[ScoreCacheNAdapter::setMask] found unkown exception : " );
				continue;
			}
		}
	} else {
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			vector<ScoreCacheNManagerPrx> prxs = getAllProxySeq( posCluster );
			for( vector<ScoreCacheNManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
				try {
					(*itPrx)->setMask( mask );
				} catch(Ice::TimeoutException e){ 
					MCE_WARN("[ScoreCacheNAdapter::setMask] found Ice::TimeoutException : " << e.what() );
					continue;
				} catch( Ice::Exception e ){
					MCE_WARN("[ScoreCacheNAdapter::setMask] found Ice::Exception : " << e.what() );
					continue;
				} catch( ... ){
					MCE_WARN("[ScoreCacheNAdapter::setMask] found unkown exception : " );
					continue;
				}
			}
		}
	}
}

int ScoreCacheNAdapter::getPrxCount(){
	return getAllProxySeq( 0 ).size();
}

// ========= ScoreLoaderNAdapter =================================

void ScoreLoaderNAdapter::setLoginDays(int userId, int historyLoginDays, int continueLoginDays, long lastLoginTime, int loginType ){
	getProxy( userId )->setLoginDays(userId,historyLoginDays,continueLoginDays,lastLoginTime,loginType);
}
void ScoreLoaderNAdapter::updateLoginType(int userId, int loginType ){
	getProxy( userId )->updateLoginType(userId,loginType);
}
ScoreDataN ScoreLoaderNAdapter::setLoginDaysWithRes(int userId, int historyLoginDays, int continueLoginDays, long lastLoginTime, int loginType ){
	return getProxy( userId )->setLoginDaysWithRes(userId,historyLoginDays,continueLoginDays,lastLoginTime,loginType);
}
ScoreDataN ScoreLoaderNAdapter::updateLoginTypeWithRes(int userId, int loginType ){
	return getProxy( userId )->updateLoginTypeWithRes(userId,loginType);
}
void ScoreLoaderNAdapter::setScore( int userId, int score ){
	getProxy( userId )->setScore( userId,score );
}
void ScoreLoaderNAdapter::incScore( int userId, int inc ){
	getProxy( userId )->incScore( userId,inc );
}
void ScoreLoaderNAdapter::decScore( int userId, int dec ){
	getProxy( userId )->decScore( userId,dec );
}
void ScoreLoaderNAdapter::setAwardCount( int userId, int awardCount ){
	getProxy( userId )->setAwardCount( userId,awardCount );
}
void ScoreLoaderNAdapter::incAwardCount( int userId, int inc ){
	getProxy( userId )->incAwardCount( userId,inc );
}
void ScoreLoaderNAdapter::decAwardCount( int userId, int dec ){
	getProxy( userId )->decAwardCount( userId,dec );
}
void ScoreLoaderNAdapter::setOn( int userId, int pos ){
	getProxy( userId )->setOn( userId,pos );
}
void ScoreLoaderNAdapter::setOff( int userId, int pos ){
	getProxy( userId )->setOff( userId,pos );
}
void ScoreLoaderNAdapter::updateVipState(int userId, int state){
	getProxy( userId )->updateVipState( userId,state );
}
void ScoreLoaderNAdapter::updateVipLevel(int userId, int level){
	getProxy( userId )->updateVipLevel( userId,level );
}
void ScoreLoaderNAdapter::reload( int userId ){
	getProxy( userId )->reload( userId );
}
void ScoreLoaderNAdapter::reloadBatch( const MyUtil::IntSeq& userIds ){
	getProxy( 0 )->reloadBatch( userIds );
}
void ScoreLoaderNAdapter::setMask( long mask ){
	getProxy( 0 )->setMask( mask );
}
