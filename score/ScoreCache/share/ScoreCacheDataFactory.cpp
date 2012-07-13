#include "ScoreCacheDataFactory.h"
#include "ScoreCacheData.h"

using namespace std;
using namespace IceUtil;
using namespace MyUtil;
using namespace mysqlpp;
using namespace com::xiaonei::xce;
using namespace xce::scorecache;
using namespace xce::cacheloader;

Ice::ObjectPtr ScoreCacheDataNFactory::create(Ice::Int userId ){
	ostringstream log;
	log << "[create] userId=" << userId;
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	{
		Statement sql;
		sql << "SELECT user_id, history_days, continue_days, active_days, UNIX_TIMESTAMP(last_time) AS sec_time, login_type FROM score_login_" << userId%100 << " WHERE user_id=" << userId;
		ostringstream pattern;
		pattern << "user_score_" << (userId%100);
		QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreLoginResultHandler(result));
	}

	map<long, Ice::ObjectPtr>::const_iterator it = result->data.find(userId);
	if( it==result->data.end() ){
		log << "(NEW_OBJ)";
		ScoreCacheDataNPtr obj = new ScoreCacheDataN;
		obj->historyLoginDays = 1;
		obj->continueLoginDays = 1;
		obj->activeDays = 2;
		obj->lastLoginTime = 1000;
		obj->loginType = 4;
		obj->score = 0;
		obj->awardCount = 0;
		obj->vip = -1;
		obj->sign = 0;
		result->data.insert(make_pair(userId,obj));
	}

	{
		Statement sql;
		sql << "SELECT user_id, score FROM score_" << userId%100 << " WHERE user_id=" << userId;
		ostringstream pattern;
		pattern << "user_score_" << (userId%100);
		int count = QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreResultHandler(result));
		if( !count ){
			log << " score_" << userId%100 << "(Nmiss)";
		}
	}
	{
		Statement sql;
		sql << "SELECT user_id, award_count FROM score_award_count_" << userId%100 << " WHERE user_id=" << userId;
		ostringstream pattern;
		pattern << "user_score_" << (userId%100);
		int count = QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreAwardCountResultHandler(result));
		if( !count ){
			log << " score_award_count_" << userId%100 << "(Nmiss)";
		}
	}
	{
		Statement sql;
		sql << "SELECT user_id, sign FROM score_sign_" << userId%100 << " WHERE user_id=" << userId;
		ostringstream pattern;
		pattern << "user_score_" << (userId%100);
		int count = QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreSignResultHandler(result));
		if( !count ){
			log << " score_sign_" << userId%100 << "(Nmiss)";
		}
	}
        {
                Statement sql;
                sql << "SELECT id, state, level FROM user_state WHERE id=" << userId;
                int count = QueryRunner("user_state",CDbRServer).query(sql, ScoreVipResultHandler(result));
		if( !count ){
			log << " score_state" << "(Nmiss)";
		}
        }

	it = result->data.find(userId);
	ScoreCacheDataNPtr res = ScoreCacheDataNPtr::dynamicCast(it->second);
	MCE_INFO( log.str() );
	return res;
}

/*****************************************************************************/

ObjectResultPtr ScoreCacheDataNFactory::create(Ice::Int minId,Ice::Int maxId) {
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	for( int i=0; i<100; ++i ){
		{
			Statement sql;
			sql << "SELECT user_id, history_days, continue_days, UNIX_TIMESTAMP(last_time) AS sec_time, login_type, active_days FROM score_login_" << i << " WHERE user_id >= " << minId << " AND user_id <= " << maxId;
			ostringstream pattern;
			pattern << "user_score_" << (i%100);
			QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreLoginResultHandler(result));
		}
		{
			Statement sql;
			sql << "SELECT user_id, score FROM score_" << i << " WHERE user_id >= "<< minId << " AND user_id <= "<< maxId;
			ostringstream pattern;
			pattern << "user_score_" << (i%100);
			QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreResultHandler(result));
		}
		{
			Statement sql;
			sql << "SELECT user_id, sign FROM score_sign_" << i << " WHERE user_id >= " << minId << " AND user_id <= " << maxId;
			ostringstream pattern;
			pattern << "user_score_" << (i%100);
			QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreSignResultHandler(result));
		}
		{
			   Statement sql;
			   sql << "SELECT user_id, award_count FROM score_award_count_" << i << " WHERE user_id >= " << minId << " AND user_id <= " << maxId;
			   ostringstream pattern;
			   pattern << "user_score_" << (i%100);
			   QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreAwardCountResultHandler(result));
		}
	}
        {
                Statement sql;
                sql << "SELECT id, state, level FROM user_state WHERE id >= " << minId << " AND id <= " << maxId;
                QueryRunner("user_state",CDbRServer).query(sql, ScoreVipResultHandler(result));
        }

	return result;
}

ObjectResultPtr ScoreCacheDataNFactory::create(const MyUtil::IntSeq& ids) {
	ObjectResultPtr result = new ObjectResult;
	if( ids.empty() ) return result;
	vector< string > split_idstring;
	vector<bool> has_id;
	for( int i=0; i<100; ++i ){
		split_idstring.push_back( string() );
		has_id.push_back( false );
	}
	for ( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		ostringstream tmp;
		if( has_id[*it%100] ){
			tmp << ", " << *it;
		} else {
			tmp << *it;
			has_id[*it%100] = true;
		}
		split_idstring[*it%100] += tmp.str();
	}
	ostringstream all_idstream;
	bool all_idstream_started = false;

	for( int i=0; i<100; ++i ){
		if( !has_id[i] ){
			continue;
		}
		if( all_idstream_started ){
			all_idstream << ",";
		} else{
			all_idstream_started  = true;
		}
		all_idstream << split_idstring[i];

		{
			   Statement sql;
			   sql << "SELECT user_id, history_days, continue_days, UNIX_TIMESTAMP(last_time) AS sec_time, login_type, active_days FROM score_login_" << i << " WHERE user_id IN (" << split_idstring[i] << ")";
			   ostringstream pattern;
			   pattern << "user_score_" << (i%100);
			   QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreLoginResultHandler(result));
		}
		{
			   Statement sql;
			   sql << "SELECT user_id, score FROM score_" << i << " WHERE user_id IN (" << split_idstring[i] << ")";
			   ostringstream pattern;
			   pattern << "user_score_" << (i%100);
			   QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreResultHandler(result));
		}
		{
			Statement sql;
			sql << "SELECT user_id, sign FROM score_sign_" << i << " WHERE user_id IN (" << split_idstring[i] << ")";
			ostringstream pattern;
			pattern << "user_score_" << (i%100);
			QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreSignResultHandler(result));
		}
		{
			   Statement sql;
			   sql << "SELECT user_id, award_count FROM score_award_count_" << i << " WHERE user_id IN (" << split_idstring[i] << ")";
			   ostringstream pattern;
			   pattern << "user_score_" << (i%100);
			   QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreAwardCountResultHandler(result));
		}
	}
        {
                Statement sql;
                sql << "SELECT id, state, level FROM user_state WHERE id IN (" << all_idstream.str() << ")";
                QueryRunner("user_state",CDbRServer).query(sql, ScoreVipResultHandler(result));
        }

	return result;
}

ObjectResultPtr ScoreCacheDataNFactory::createByDays(const vector<int>& ids,int days,long zerotime) {
	ObjectResultPtr result = new ObjectResult;
	if( ids.empty() ) return result;
	vector< string > split_idstring;
	vector<bool> has_id;
	for( int i=0; i<100; ++i ){
		split_idstring.push_back( string() );
		has_id.push_back( false );
	}
	for ( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		ostringstream tmp;
		if( has_id[*it%100] ){
			tmp << ", " << *it;
		} else {
			tmp << *it;
			has_id[*it%100] = true;
		}
		split_idstring[*it%100] += tmp.str();
	}
	ostringstream all_idstream;
	bool all_idstream_started = false;

	for( int i=0; i<100; ++i ){
		if( !has_id[i] ){
			continue;
		}
		if( all_idstream_started ){
			all_idstream << ",";
		} else{
			all_idstream_started  = true;
		}
		all_idstream << split_idstring[i];

		IntSeq goon_ids;
		{
			   Statement sql;
			   sql << "SELECT user_id, history_days, continue_days, UNIX_TIMESTAMP(last_time) AS sec_time, login_type, active_days FROM score_login_" << i << " WHERE user_id IN (" << split_idstring[i] << ")";
			   ostringstream pattern;
			   pattern << "user_score_" << (i%100);
			   QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreLoginByDaysResultHandler(result,goon_ids,days,zerotime));
		}

		if( goon_ids.empty() ){
			continue;
		}else if( goon_ids.size() < split_idstring[i].size() ){
			ostringstream new_ids;
			MyUtil::IntSeq::const_iterator it = goon_ids.begin();
			new_ids << *it;
			for ( ++it; it != goon_ids.end(); ++it) {
				new_ids << ", " << *it;
			}
			split_idstring[i] = new_ids.str();
		}
		{
			   Statement sql;
			   sql << "SELECT user_id, score FROM score_" << i << " WHERE user_id IN (" << split_idstring[i] << ")";
			   ostringstream pattern;
			   pattern << "user_score_" << (i%100);
			   QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreResultHandler(result));
		}
		{
			Statement sql;
			sql << "SELECT user_id, sign FROM score_sign_" << i << " WHERE user_id IN (" << split_idstring[i] << ")";
			ostringstream pattern;
			pattern << "user_score_" << (i%100);
			QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreSignResultHandler(result));
		}
		{
			   Statement sql;
			   sql << "SELECT user_id, award_count FROM score_award_count_" << i << " WHERE user_id IN (" << split_idstring[i] << ")";
			   ostringstream pattern;
			   pattern << "user_score_" << (i%100);
			   QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, ScoreAwardCountResultHandler(result));
		}
	}
        {
                Statement sql;
                sql << "SELECT id, state, level FROM user_state WHERE id IN (" << all_idstream.str() << ")";
                QueryRunner("user_state",CDbRServer).query(sql, ScoreVipResultHandler(result));
        }

	return result;
}

/*****************************************************************************/

bool ScoreLoginResultHandler::handle(Row& row) const {
        int id = (int) row["user_id"];
        int historyLoginDays = (int) row["history_days"];
        int continueLoginDays = (int) row["continue_days"];
        long lastLoginTime = (long) row["sec_time"];
        lastLoginTime *= 1000;
        int loginType = (int) row["login_type"];
        int activeDays = (int) row["active_days"];
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it != _result->data.end()) {
                MCE_WARN("[ScoreLoginResultHandler::handle] duplicated userId=" <<  id);
        } else {
		ScoreCacheDataNPtr data = new ScoreCacheDataN;
		data->historyLoginDays = historyLoginDays<=0?1:historyLoginDays;
		data->continueLoginDays = continueLoginDays<=0?1:continueLoginDays;
		data->lastLoginTime = lastLoginTime<1000?1000:lastLoginTime;
		data->loginType = loginType==0?4:loginType;
		if( activeDays == 0 ){
			int abs_count = 2;
			for(int i=1; i<=continueLoginDays; ++i){
				activeDays = activeDays|abs_count;
				abs_count *= 2;
				if( i==30 ) break;
			}
		}
		data->activeDays = activeDays;
		data->score = 0;
		data->awardCount = 0;
		data->vip = -1;
		data->sign = 0;
		_result->data.insert(make_pair(id,data));
        }
	return true;
}

bool ScoreLoginByDaysResultHandler::handle(Row& row) const {
        int id = (int) row["user_id"];
        int historyLoginDays = (int) row["history_days"];
        int continueLoginDays = (int) row["continue_days"];
        long lastLoginTime = (long) row["sec_time"];
        int loginType = (int) row["login_type"];
        int activeDays = (int) row["active_days"];
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it != _result->data.end()) {
                MCE_WARN("[ScoreLoginByDaysResultHandler::handle] duplicated userId=" << id);
        } else {
		if( _zerotime>lastLoginTime && (_zerotime-lastLoginTime)/(long)(24*3600)+1>_days ){
			return true;
		}
		
		lastLoginTime *= 1000;
		lastLoginTime = lastLoginTime<1000?1000:lastLoginTime;
		ScoreCacheDataNPtr data = new ScoreCacheDataN;
		data->historyLoginDays = historyLoginDays<=0?1:historyLoginDays;
		data->continueLoginDays = continueLoginDays<=0?1:continueLoginDays;
		data->lastLoginTime = lastLoginTime<1000?1000:lastLoginTime;
		data->loginType = loginType==0?4:loginType;
		if( activeDays == 0 ){
			int abs_count = 2;
			for(int i=1; i<=continueLoginDays; ++i){
				activeDays = activeDays|abs_count;
				abs_count *= 2;
				if( i==30 ) break;
			}
		}
		data->activeDays = activeDays;
		data->score = 0;
		data->awardCount = 0;
		data->vip = -1;
		data->sign = 0;
		_result->data.insert(make_pair(id,data));
		_ids.push_back(id);
        }
	return true;
}

bool ScoreResultHandler::handle(Row& row) const {
        int id = (int) row["user_id"];
        int score = (int) row["score"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
	if( it == _result->data.end() ){
                MCE_WARN("[ScoreResultHandler::handle] score_" << id%100 << " has " << id << " but score_login_" << id%100 << "|user_login_days_" << id%100 << " not" );
		return true;
	}
	ScoreCacheDataNPtr data = ScoreCacheDataNPtr::dynamicCast(it->second);
	data->score = score;
        return true;
}

bool ScoreAwardCountResultHandler::handle(Row& row) const {
        int id = (int) row["user_id"];
        int awardCount = (int) row["award_count"];
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
	if( it == _result->data.end() ){
                MCE_WARN("[ScoreAwardCountResultHandler::handle] score_award_count_" << id%100 << " has " << id << " but score_login_" << id%100 << "|user_login_days_" << id%100 << " not" );
		return true;
	}
	ScoreCacheDataNPtr data = ScoreCacheDataNPtr::dynamicCast(it->second);
	data->awardCount = awardCount;
	return true;
}

bool ScoreSignResultHandler::handle(Row& row) const {
        int id = (int) row["user_id"];
        long sign = (long) row["sign"];
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
	if( it == _result->data.end() ){
                MCE_WARN("[ScoreSignResultHandler::handle] score_sign_" << id%100 << " has " << id << " but score_login_" << id%100 << "|user_login_days_" << id%100 << " not" );
		return true;
	}
	ScoreCacheDataNPtr data = ScoreCacheDataNPtr::dynamicCast(it->second);
	data->sign = sign;
	return true;
}

bool ScoreVipResultHandler::handle(Row& row) const {
        int id = (int) row["id"];
        int state = (int) row["state"];
        int level = (int) row["level"];
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
	if( it == _result->data.end() ){
                MCE_WARN("[ScoreVipResultHandler::handle] user_state has " << id << " but score_login_" << id%100 << "|user_login_days_" << id%100 << " not" );
		return true;
	}
	ScoreCacheDataNPtr data = ScoreCacheDataNPtr::dynamicCast(it->second);
	if( level == 0 ){
		level = 1;
	}
	data->vip = (0!=(state&(1<<1))) ? level : -level;
	return true;
}

/*****************************************************************************/

