#include "ScoreChecker.h"
#include "ScoreCacheAdapter.h"
#include "../share/ScoreCacheData.h"
#include "../share/ScoreCacheDataFactory.h"

using namespace com::xiaonei::xce;
using namespace xce::scorecache;
using namespace xce::scorecache::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

/*****************************************************************************/

void CheckerHelper::initialize(){
	checker_ = new CheckerThread(*this);
	checker_->start(1024*1024).detach();
}

void CheckerHelper::invokeDelay( Ice::Int id ){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
	datas_.push_back( id );
	if (datas_.size() >= 100) mutex_.notify();
}
void CheckerHelper::invokeNow( Ice::Int id ){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
	datas_.push_back( id );
	mutex_.notify();
}
void CheckerHelper::swap(vector<int>& empty_vector){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	if (datas_.size() < 100) mutex_.timedWait(IceUtil::Time::seconds(30));
	datas_.swap(empty_vector);
}

void CheckerHelper::CheckerThread::run() {
	while (true){

		_ok_stat = 0;
		_allmiss_stat = 0;
		_somemiss_stat = 0;
		_login_err_stat = 0;
		_score_err_stat = 0;
		_award_err_stat = 0;
		_vip_err_stat = 0;
		_sign_err_stat = 0;

		std::vector<int> id_list;
		helper_.swap( id_list );
		if( id_list.empty() ){
			MCE_INFO("[CheckerHelper::CheckerThread::run] check 0 ids" );
			continue;
		}
		ostringstream idstr;
		for ( std::vector<int>::iterator it = id_list.begin();
				it != id_list.end(); ++it ){
			idstr << " " << *it;
		}

		ostringstream createLog;

		struct timeval tvStart, tvDB, tvCH, tvCK;
		gettimeofday(&tvStart, NULL);
		double linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);

		ObjectResultPtr scoreobj_db = getScoreDataFromDB(id_list,2);
		if( !scoreobj_db )continue;
		vector<int> ids;
		for( std::map<long, Ice::ObjectPtr>::const_iterator it=scoreobj_db->data.begin(); it!=scoreobj_db->data.end(); ++it ){
			ids.push_back( it->first );
		}

		gettimeofday(&tvDB, NULL);
		double linDBEnd = ((double)tvDB.tv_sec * 1000000 + (double)tvDB.tv_usec);
		double linDBTime = linDBEnd - linStart;
		linDBTime = linDBTime/1000000;
		createLog       << "(" << linDBTime << "s/";

		vector<ScoreDataNMap> scoredatas_cache = getScoreDataFromAllPrx( ids );

		gettimeofday(&tvCH, NULL);
		double linCHEnd = ((double)tvCH.tv_sec * 1000000 + (double)tvCH.tv_usec);
		double linCHTime = linCHEnd - linDBEnd;
		linCHTime = linCHTime/1000000;
		createLog       << linCHTime << "s/";

		verify( scoreobj_db, scoredatas_cache, true, false );

		gettimeofday(&tvCK, NULL);
		double linCKEnd = ((double)tvCK.tv_sec * 1000000 + (double)tvCK.tv_usec);
		double linCKTime = linCKEnd - linCHEnd;
		linCKTime = linCKTime/1000000;
		createLog       << linCKTime << "s)";

		MCE_INFO( "[CheckerHelper::CheckerThread::run] " << createLog.str()
				<< " count:" << id_list.size()
				<< " ok:" << _ok_stat
				<< " allmiss:" << _allmiss_stat
				<< " somemiss:" << _somemiss_stat
				<< " loginerr:" << _login_err_stat
				<< " scoreerr:" << _score_err_stat
				<< " awarderr:" << _award_err_stat
				<< " viperr:" << _vip_err_stat
				<< " signerr:" << _sign_err_stat
				<< " ids:" << idstr.str()
		);
	}
}

ObjectResultPtr CheckerHelper::CheckerThread::getScoreDataFromDB(const vector<int>& ids,int days){
	ObjectResultPtr dbres;
	try {
		dbres = ScoreCacheDataNFactory::instance().createByDays(ids,days,_zerotime);
	} catch (Ice::Exception& e) {
		MCE_WARN("[getScoreDataFromDB] Ice::Exception (" << ids.size() << " objs) : " << e.what());
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[getScoreDataFromDB] mysqlpp::Exception (" << ids.size() << " objs) : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[getScoreDataFromDB] std::excetpion (" << ids.size() << " objs) : " << ": " << e.what());
	} catch (...) {
		MCE_WARN("[getScoreDataFromDB] unknown exception (" << ids.size() << " objs) ." );
	}
	return dbres;
}

vector<ScoreDataNMap> CheckerHelper::CheckerThread::getScoreDataFromAllPrx(const vector<int>& ids){
	return ScoreCacheNAdapter::instance().getScoreDataNMapAllPrx( ids );
}

//-------------------------------------------------------------

void CheckerHelper::CheckerThread::verify(
		ObjectResultPtr scoreobj_db,
		const vector<ScoreDataNMap>& scoredata_cache,
		bool tc, bool detail ){

	//! 首先是判断有没有CACHE缺失,有,则reload,结束;
	//@ 验证一,其次是判断各CACHE间和DB是否一致,如一致,得到基准,跳过下一步,如不一致走下一步:
	//# CACHE和DB数据不一致修正, 取数据最大者,得到基准,走下一步:
	//$ 验证二,再次是CACHE合理性验证,目前是SIGN判断
	//% 如果在验证一和验证二中有没通过的,用基准先写DB后reload

	ScoreDataNMap scoredata_db;
	for ( std::map<long, Ice::ObjectPtr>::const_iterator it=scoreobj_db->data.begin();
			it!=scoreobj_db->data.end(); ++it ){
		ScoreCacheDataNIPtr obj = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( it->second ) );
		scoredata_db.insert( make_pair<int,ScoreDataN>( it->first,obj->getScoreDataN(it->first,(long)0) ) );
	}

	for( ScoreDataNMap::iterator itdb=scoredata_db.begin(); itdb!=scoredata_db.end(); ++itdb ){
		bool somecachemiss=false;
		bool allcachemiss=true;
		vector<ScoreDataN> tofindone;
		tofindone.push_back( itdb->second );
		for( vector<ScoreDataNMap>::const_iterator itcaches=scoredata_cache.begin(); itcaches!=scoredata_cache.end(); ++itcaches ){
			ScoreDataNMap::const_iterator itcache=itcaches->find( itdb->first );
			if( itcache==(*itcaches).end() ){
				somecachemiss=true;
				continue;
			}else{
				allcachemiss=false;
				tofindone.push_back( itcache->second );
			}
		}
		if( allcachemiss ){
			cout << "[CheckerHelper::CheckerThread::verify] " << itdb->first << " allcachemiss(" << (int)scoredata_cache.size() << ")" << endl;
			_allmiss_stat++;
			ScoreCacheNAdapter::instance().load( itdb->first,ScoreCacheDataNPtr::dynamicCast( scoreobj_db->data[itdb->first] ) );
		}else{
			bool login_ok = true;
			bool score_ok = true;
			bool award_ok = true;
			bool vip_ok = true;
			bool sign_ok = true;
			ScoreCacheDataNPtr theone = findone( tofindone, detail, login_ok, score_ok, award_ok, vip_ok, sign_ok );
			if( theone ){
				if(!login_ok)_login_err_stat++;
				if(!score_ok)_score_err_stat++;
				if(!award_ok)_award_err_stat++;
				if(!vip_ok)_vip_err_stat++;
				if(!sign_ok)_sign_err_stat++;
				if( tc )savetheone( itdb->first, theone, login_ok, score_ok, award_ok, vip_ok, sign_ok );
			}else{
				if( somecachemiss ){
					cout << "[CheckerHelper::CheckerThread::verify] " << itdb->first << " somecachemiss." << endl;
					_somemiss_stat++;
					ScoreCacheNAdapter::instance().load( itdb->first,ScoreCacheDataNPtr::dynamicCast( scoreobj_db->data[itdb->first] ) );
				}else{
					_ok_stat++;
				}
			}
		}
	}
}

ScoreCacheDataNPtr CheckerHelper::CheckerThread::findone(const vector<ScoreDataN>& datas, bool detail, bool& login_ok, bool& score_ok, bool& award_ok, bool& vip_ok, bool& sign_ok ){
	vector<ScoreDataN>::const_iterator itcache=datas.begin();

	ScoreDataN data;

	data.id = itcache->id;
	data.historyLoginDays = itcache->historyLoginDays ;
	data.continueLoginDays = itcache->continueLoginDays ;
	data.activeDays = itcache->activeDays ;
	data.lastLoginTime = itcache->lastLoginTime ;
	data.loginType = itcache->loginType ;
	data.score = itcache->score ;
	data.level = itcache->level;
	data.awardCount = itcache->awardCount ;
	data.vip = itcache->vip ;
	data.sign = itcache->sign ;

	if( detail ){
		cout << "[CheckerHelper::CheckerThread::find] " << data.id << " db:" << endl;
		cout << "[CheckerHelper::CheckerThread::find] historyLoginDays: " << data.historyLoginDays << endl;
		cout << "[CheckerHelper::CheckerThread::find] continueLoginDays: " << data.continueLoginDays << endl;
		cout << "[CheckerHelper::CheckerThread::find] activeDays: " << data.activeDays << endl;
		cout << "[CheckerHelper::CheckerThread::find] lastLoginTime: " << data.lastLoginTime << endl;
		cout << "[CheckerHelper::CheckerThread::find] loginType: " << data.loginType << endl;
		cout << "[CheckerHelper::CheckerThread::find] score: " << data.score << endl;
		cout << "[CheckerHelper::CheckerThread::find] level: " << data.level << endl;
		cout << "[CheckerHelper::CheckerThread::find] awardCount: " << data.awardCount << endl;
		cout << "[CheckerHelper::CheckerThread::find] vip: " << data.vip << endl;
		cout << "[CheckerHelper::CheckerThread::find] sign: " << data.sign << endl;
		cout << endl;
	}

	ostringstream body;
	int index = 1;
	for( ++itcache; itcache!=datas.end(); ++itcache ){
		if( detail ){
			cout << "[CheckerHelper::CheckerThread::find] " << itcache->id << " db:" << endl;
			cout << "[CheckerHelper::CheckerThread::find] historyLoginDays: " << itcache->historyLoginDays << endl;
			cout << "[CheckerHelper::CheckerThread::find] continueLoginDays: " << itcache->continueLoginDays << endl;
			cout << "[CheckerHelper::CheckerThread::find] activeDays: " << itcache->activeDays << endl;
			cout << "[CheckerHelper::CheckerThread::find] lastLoginTime: " << itcache->lastLoginTime << endl;
			cout << "[CheckerHelper::CheckerThread::find] loginType: " << itcache->loginType << endl;
			cout << "[CheckerHelper::CheckerThread::find] score: " << itcache->score << endl;
			cout << "[CheckerHelper::CheckerThread::find] level: " << itcache->level << endl;
			cout << "[CheckerHelper::CheckerThread::find] awardCount: " << itcache->awardCount << endl;
			cout << "[CheckerHelper::CheckerThread::find] vip: " << itcache->vip << endl;
			cout << "[CheckerHelper::CheckerThread::find] sign: " << itcache->sign << endl;
			cout << endl;
		}

		if( data.historyLoginDays != itcache->historyLoginDays ){
			body	<< " hist_" << index << "(" << data.historyLoginDays << "!=" << itcache->historyLoginDays << ")";
			data.historyLoginDays = data.historyLoginDays>itcache->historyLoginDays?data.historyLoginDays:itcache->historyLoginDays;
			login_ok=false;
		}
		if( data.continueLoginDays != itcache->continueLoginDays ){
			body	<< " cont_" << index << "(" << data.continueLoginDays << "!=" << itcache->continueLoginDays << ")";
			data.continueLoginDays = data.continueLoginDays>itcache->continueLoginDays?data.continueLoginDays:itcache->continueLoginDays;
			login_ok=false;
		}
		if( data.lastLoginTime/1000 != itcache->lastLoginTime/1000 ){
			body	<< " last_" << index << "(" << data.lastLoginTime << "!=" << itcache->lastLoginTime << ")";
			data.lastLoginTime = data.lastLoginTime>itcache->lastLoginTime?data.lastLoginTime:itcache->lastLoginTime;
			login_ok=false;
		}
		if( data.activeDays!=itcache->activeDays ){
			body	<< " activeDays_" << index << "(" << data.activeDays << "!=" << itcache->activeDays << ")";
			data.activeDays = data.activeDays|itcache->activeDays;
		}

		if( (index==1&&data.score<itcache->score) || (index>1&&data.score!=itcache->score) ){
			body	<< " score_" << index << "(" << data.score << "!=" << itcache->score << ")";
			data.score = data.score>itcache->score?data.score:itcache->score;
			data.level = data.level>itcache->level?data.level:itcache->level;
			score_ok=false;
		}

		if( data.awardCount!=itcache->awardCount ){
			body	<< " award_" << index << "(" << data.awardCount << "!=" << itcache->awardCount << ")";
			data.awardCount = data.awardCount<itcache->awardCount?data.awardCount:itcache->awardCount;
			award_ok=false;
		}

		if( index==1 && data.vip!=itcache->vip ){
			body	<< " vip_db(" << data.vip << "!=" << itcache->vip << ")";
			data.vip = data.vip>itcache->vip?data.vip:itcache->vip;
		}else if( index>1 && data.vip!=itcache->vip ){
			body	<< " vip_"<<index<<"(" << data.vip << "!=" << itcache->vip << ")";
			data.vip = data.vip>itcache->vip?data.vip:itcache->vip;
			vip_ok=false;
		}

		if( data.sign!=itcache->sign ){
			body	<< " sign_" << index << "(" << data.sign << "!=" << itcache->sign << ")";
			data.sign = data.sign|itcache->sign;
			sign_ok=false;
		}
		++index;
	}

	long new_sign = data.sign;
	if(data.level>=15||data.vip>=3){
		new_sign |= (((long)1)<<2);
	}else{
		new_sign &= (~(((long)1)<<2));
	}
	if( new_sign!=data.sign ){
		body	<< " f:" << new_sign;
	}
	if(data.level>=7||data.vip>=1){
		new_sign |= (((long)1)<<3);
	}else{
		new_sign &= (~(((long)1)<<3));
	}
	if( new_sign!=data.sign ){
		body	<< " c:" << new_sign;
	}
	if(data.level>=9||data.vip>=1){
		new_sign |= (((long)1)<<4);
	}else{
		new_sign &= (~(((long)1)<<4));
	}
	if( new_sign!=data.sign ){
		body	<< " s:" << new_sign;
	}
	if(data.level>=11||data.vip>=1){
		new_sign |= (((long)1)<<5);
	}else{
		new_sign &= (~(((long)1)<<5));
	}
	if( new_sign!=data.sign ){
		body	<< " h:" << new_sign;
	}
	if( new_sign!=data.sign ){
		body	<< " sign(" << data.sign << "->" << new_sign << ")";
		data.sign = new_sign;
		sign_ok=false;
	}

	if( !login_ok||!score_ok||!award_ok||!vip_ok||!sign_ok ){
		cout << "[CheckerHelper::CheckerThread::find] " << data.id << body.str() << endl;
		ScoreCacheDataNPtr res = new ScoreCacheDataN;
		res->historyLoginDays = data.historyLoginDays ;
		res->continueLoginDays = data.continueLoginDays ;
		res->activeDays = data.activeDays ;
		res->lastLoginTime = data.lastLoginTime ;
		res->loginType = data.loginType ;
		res->score = data.score ;
		res->awardCount = data.awardCount ;
		res->vip = data.vip ;
		res->sign = data.sign ;
		return res;
	} else{
		return 0;
	}
}

void CheckerHelper::CheckerThread::savetheone( Ice::Int userId, const ScoreCacheDataNPtr& datan, bool login_ok, bool score_ok, bool award_ok, bool vip_ok, bool sign_ok ){
	ostringstream pattern;
	pattern << "user_score_" << (userId%100);
	if( !login_ok ){
		ostringstream sql;
		sql     << "INSERT INTO score_login_" << userId%100 << " (user_id,history_days,continue_days,last_time,login_type,active_days)"
			<< " VALUES(" << userId << "," << datan->historyLoginDays << "," << datan->continueLoginDays << ",FROM_UNIXTIME(" << datan->lastLoginTime/1000 << ")," << datan->loginType << "," << datan->activeDays << ")"
			<< " ON DUPLICATE KEY UPDATE history_days=" << datan->historyLoginDays << ",continue_days=" << datan->continueLoginDays << ",last_time=FROM_UNIXTIME(" << datan->lastLoginTime/1000 << "),login_type=" << datan->loginType << ",active_days=" << datan->activeDays;
		cout << "[CheckerHelper::CheckerThread::save] " << sql.str() << endl;
		Statement sqlreal;
		sqlreal << sql.str();
		QueryRunner( "user_score", CDbWServer, pattern.str() ).execute(sqlreal);
	}
	if( !score_ok ){
		ostringstream sql;
		sql     << "INSERT INTO score_" << userId%100 << " (user_id,score)"
			<< " VALUES(" << userId << "," << datan->score << ")"
			<< " ON DUPLICATE KEY UPDATE score=" << datan->score;
		cout << "[CheckerHelper::CheckerThread::save] " << sql.str() << endl;
		Statement sqlreal;
		sqlreal << sql.str();
		QueryRunner( "user_score", CDbWServer, pattern.str() ).execute(sqlreal);
	}
	if( !award_ok ){
		ostringstream sql;
		sql     << "INSERT INTO score_award_count_" << userId%100 << " (user_id,award_count)"
			<< " VALUES(" << userId << "," << datan->awardCount << ")"
			<< " ON DUPLICATE KEY UPDATE award_count=" << datan->awardCount;
		cout << "[CheckerHelper::CheckerThread::save] " << sql.str() << endl;
		Statement sqlreal;
		sqlreal << sql.str();
		QueryRunner( "user_score", CDbWServer, pattern.str() ).execute(sqlreal);
	}
	if( !sign_ok ){
		ostringstream sql;
		sql     << "INSERT INTO score_sign_" << userId%100 << " (user_id,sign)"
			<< " VALUES(" << userId << "," << datan->sign << ")"
			<< " ON DUPLICATE KEY UPDATE sign=" << datan->sign;
		cout << "[CheckerHelper::CheckerThread::save] " << sql.str() << endl;
		Statement sqlreal;
		sqlreal << sql.str();
		QueryRunner( "user_score", CDbWServer, pattern.str() ).execute(sqlreal);
	}
	ScoreCacheNAdapter::instance().load( userId,datan );
}

