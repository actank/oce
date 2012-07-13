#include "OfferCacheDataFactory.h"
#include "OfferCacheDataI.h"

using namespace std;
using namespace IceUtil;
using namespace MyUtil;
using namespace mysqlpp;
using namespace com::xiaonei::xce;
using namespace xce::offerfriends;
using namespace xce::cacheloader;

Ice::ObjectPtr OfferCacheDataFactory::create(Ice::Int userId ){
	MyUtil::ObjectResultPtr result = 0;
	return 0;
}

/*****************************************************************************/

ObjectResultPtr OfferCacheDataFactory::create(Ice::Int minId,Ice::Int maxId) {
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
/*
	for( int i=0; i<100; ++i ){
		{
			Statement sql;
			sql << "SELECT user_id, history_days, continue_days, UNIX_TIMESTAMP(last_time) AS sec_time, login_type, active_days FROM score_login_" << i << " WHERE user_id >= " << minId << " AND user_id <= " << maxId;
			ostringstream pattern;
			pattern << "user_score_" << (i%100);
			QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, OfferResultHandler(result));
		}
	}
        {
                Statement sql;
                sql << "SELECT id, state, level FROM user_state WHERE id >= " << minId << " AND id <= " << maxId;
                QueryRunner("user_state",CDbRServer).query(sql, OfferResultHandler(result));
        }
*/
	return result;
}

ObjectResultPtr OfferCacheDataFactory::create(const MyUtil::IntSeq& ids) {
	ObjectResultPtr result = new ObjectResult;
/*
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
			   QueryRunner("user_score",CDbRServer, pattern.str()).query(sql, OfferResultHandler(result));
		}
	}
        {
                Statement sql;
                sql << "SELECT id, state, level FROM user_state WHERE id IN (" << all_idstream.str() << ")";
                QueryRunner("user_state",CDbRServer).query(sql, OfferResultHandler(result));
        }
*/
	return result;
}

/*****************************************************************************/

bool OfferResultHandler::handle(Row& row) const {
/*
        int id = (int) row["user_id"];
        int historyLoginDays = (int) row["history_days"];
        int continueLoginDays = (int) row["continue_days"];
        long lastLoginTime = (long) row["sec_time"];
        lastLoginTime *= 1000;
        int loginType = (int) row["login_type"];
        int activeDays = (int) row["active_days"];
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it != _result->data.end()) {
                MCE_WARN("[OfferResultHandler::handle] duplicated userId=" <<  id);
        } else {
		OfferCacheDataPtr data = new OfferCacheData;
		//TODO
		_result->data.insert(make_pair(id,data));
        }
*/
	return true;
}

/*****************************************************************************/

