#include "LoginRecordWorker.h"
#include "UserStateAdapter.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::userbase;
using namespace xce::adapter::userstate;

LoginRecordWorker::LoginRecordWorker() {
	cout<<"[LoginRecordWorker::LoginRecordWorker] ConnectionPoolManager initializing" << endl;
	ConnectionPoolManager::instance().initialize();
	cout<<"[LoginRecordWorker::LoginRecordWorker] ConnectionPoolManager initialize done" << endl;

        tzset();
        _time = time(NULL);

        struct tm *local;
        local = localtime(&_time);

        local->tm_hour = 0;
        local->tm_min = 0;
        local->tm_sec = 0;

        _today = timelocal(local);

	cout<<"[LoginRecordWorker::LoginRecordWorker] today : " << _today << "(" << ((_time-_today)/3600) << "h ago) Zero Time is: " << asctime(local) << endl;

        _yesterday_login_stat = 0;
        _yesterday_login_continue_stat = 0;
        _clean_stat = 0;
        _orange_err_stat = 0;
        _black_err_stat = 0;
}

LoginRecordWorker::~LoginRecordWorker(){
}

void LoginRecordWorker::work(bool o_,bool oc_,bool r_,bool w_,const string& wf_){
	cout <<  "[LoginRecordWorker::work] starting ... orange_check=" << o_ << " to_correct=" << oc_ << endl;
	int idcount = 0;
	int logincount = 0;

        try {
                int maxId_ = 0;
                int large_id = 0;
                do {
			vector<int> id_list;
			{
				Statement sql;
				sql << "SELECT id FROM user_passport WHERE id > " << maxId_ << " LIMIT 10000";
				com::xiaonei::xce::QueryRunner("user_passport_status",com::xiaonei::xce::CDbRServer ).query(
						sql, IdListResultHandler( large_id, id_list )
						);
			}
			vector< vector<int> > seperatedIds;
			seperatedIds.resize(DB_CLUSTER);
			for (vector<int>::iterator it = id_list.begin(); it
					!= id_list.end(); ++it) {
				seperatedIds.at(*it % DB_CLUSTER).push_back(*it);
			}
			for (int posCluster = 0; posCluster < DB_CLUSTER; ++posCluster) {
				if( seperatedIds.at(posCluster).size() > 0 ){
					idcount += seperatedIds.size();
					vector<LoginData> data = getLoginRecordData(seperatedIds.at(posCluster),posCluster);
					logincount += data.size();
					if ( o_ ){
						doCheckOrange( data,oc_,w_,wf_ );
					}
					/*
					if ( a_ ){
						//checkawardcount += doCheckAward( data, ids, award_record_file );
					}
					*/
				}
			}
                        if (large_id > maxId_) {
                                maxId_ = large_id;
                        } else {
                                break;
                        }
                        cout << "[LoginRecordWorker::work] searching to " << large_id
				<< " (cl)" << _clean_stat
				<< " (co)" << _orange_err_stat
				<< " (cb)" << _black_err_stat
				//<< " (ca)" << checkawardcount
				<< " (logincount)" << logincount
				<< " (idcount)" << idcount << endl;
                } while (true);
        } catch (mysqlpp::Exception e) {
                cerr << "[LoginRecordWorker::work] loading exit with mysqlpp::Exception." << e.what() << endl;
        } catch (std::exception e) {
                cerr << "[LoginRecordWorker::work] loading exit with std::exception." << e.what() << endl;
        } catch (...) {
                cerr << "[LoginRecordWorker::work] loading exit with unknown exception." << endl;
        }
	time_t now = time(NULL);
	cout << "[LoginRecordWorker::work] finished!" << endl;
	cout << "[LoginRecordWorker::work] (time_cost)" << (now-_time)/3600 << "h" << (now-_time)%3600/60 << "m" << (now-_time)%3600%60 << "s" << endl;
	cout << "[LoginRecordWorker::work] (cl)" << _clean_stat << endl;
	cout << "[LoginRecordWorker::work] (co)" << _orange_err_stat << endl;
	cout << "[LoginRecordWorker::work] (cb)" << _black_err_stat << endl;
	//cout << "[LoginRecordWorker::work] (ca)" << checkawardcount << endl;
	cout << "[LoginRecordWorker::work] (yesterday_login && continue>=" << GAP << ")" << _yesterday_login_continue_stat << endl;
	cout << "[LoginRecordWorker::work] (yesterday_login)" << _yesterday_login_stat << endl;
	cout << "[LoginRecordWorker::work] (logincount)" << logincount << endl;
	cout << "[LoginRecordWorker::work] (idcount)" << idcount << endl << endl;

	recording( r_ );
	cout << endl;
}

//-------------------------------------------------------------

void LoginRecordWorker::doCheckOrange(const vector<LoginData>& data,bool correct,bool writefile,const string& writefilename ){
	vector<int> ids;
	for( vector<LoginData>::const_iterator it = data.begin(); it != data.end(); ++it ){
		ids.push_back( it->user_id );
	}
	map<int,bool> states = getUserState( ids );
	map<int,bool> configs = getKeepUserConfig( ids );

	map<int,bool> hitones;

	ofstream ofs;
	if( writefile ){
		ofs.open(writefilename.c_str(), ios::out | ios::app );
	}
	for( vector<LoginData>::const_iterator it = data.begin(); it != data.end(); ++it ){
		///////////////////////////////////////
		int stat_tmp = it->continue_days;
		if( _today < it->last_day ){
			stat_tmp -= 1;
			if( stat_tmp>=1 ){
				++_yesterday_login_stat;
			}
			if( stat_tmp>=GAP ){
				++_yesterday_login_continue_stat;
			}
		} else if( (_today-it->last_day) <= (24*3600) ){
			++_yesterday_login_stat;
			if( stat_tmp>=GAP ){
				++_yesterday_login_continue_stat;
				if( writefile ){
					ofs << it->user_id << " " << stat_tmp << endl;
				}
			}
		} else{
			stat_tmp = 0;
		}
		map<int,int>::iterator itStat = _continue_stat.find( stat_tmp );
		if( itStat == _continue_stat.end() ){
			_continue_stat[stat_tmp] = 0;
			itStat = _continue_stat.find( stat_tmp );
		}
		itStat->second++;
		///////////////////////////////////////
		map<int,bool>::iterator itStates =  states.find( it->user_id );
		if( itStates == states.end() ){
			cout << "[LoginRecordWorker::doCheckOrange] WARN why there is no " << it->user_id << " in user_state??? " << endl;
			continue;
		}
		bool isOrange = itStates->second;

		bool noOrange = false;
		map<int,bool>::iterator itConfig =  configs.find( it->user_id );
		if( itConfig != configs.end() ){
			noOrange = true;
		}

		ostringstream log_days;
		log_days << "[LoginRecordWorker::doCheckOrange] id(" << it->user_id << ")"
			<< " continue(" << it->continue_days << ")"
			<< " lastday(" << it->last_day << ")";
		it->last_day > _today ? ( log_days << ":today" ) : ( log_days << ":" << ((_today-it->last_day)/(86400)+1) << "_days_ago");
		log_days << " isOrange=" << isOrange;
		if( (it->continue_days>=GAP) ){
			if( (_today-it->last_day) > (24*3600) ){ // 昨天0点之前
				if( isOrange ){
					cout << log_days.str()
						<< " cleaning ..." << endl;
					hitones.insert( make_pair<int,bool>(it->user_id,false) );
					++_clean_stat;
				}
			}else{  // 昨天0点之后(昨天或者今天登录了)
				if( (!isOrange) && (!noOrange) ){
					cout << log_days.str()
						<< " noOrange=" << noOrange
						<< " oranging ..." << endl;
					hitones.insert( make_pair<int,bool>(it->user_id,true) );
					++_black_err_stat; 
				}else if( (isOrange) && (noOrange) ){
					cout << log_days.str()
						<< " noOrange=" << noOrange
						<< " unoranging ..." << endl;
					hitones.insert( make_pair<int,bool>(it->user_id,false) );
					++_orange_err_stat; 
				}
			}
		} else if( isOrange ){
			cout << log_days.str()
				<< " unoranging ..."<< endl;
			hitones.insert( make_pair<int,bool>(it->user_id,false) );
			++_orange_err_stat; 
		}
	}
	if( correct ){
		oranging( hitones );
	}
	ofs.close();
}

//-------------------------------------------------------------

void LoginRecordWorker::oranging(const map<int,bool>& ids){
	for(map<int,bool>::const_iterator it = ids.begin(); it
			!= ids.end(); ++it) {
		if( it->second ){
			try {
				UserStateAdapter::instance().setKeepUseOn( it->first );
			} catch (Ice::Exception e) {
				cerr << "[LoginRecordWorker::doClean] setKeepUseOn(" << it->first << ")  failed with Ice::Exception." << e.what() << endl;
			} catch (std::exception e) {
				cerr << "[LoginRecordWorker::doClean] setKeepUseOn(" << it->first << ")  failed with std::exception." << e.what() << endl;
			} catch (...) {
				cerr << "[LoginRecordWorker::doClean] setKeepUseOn(" << it->first << ")  failed with unknown exception." << endl;
			}
		} else{
			try {
				UserStateAdapter::instance().setKeepUseOff( it->first );
			} catch (Ice::Exception e) {
				cerr << "[LoginRecordWorker::doClean] setKeepUseOff(" << it->first << ")  failed with Ice::Exception." << e.what() << endl;
			} catch (std::exception e) {
				cerr << "[LoginRecordWorker::doClean] setKeepUseOff(" << it->first << ")  failed with std::exception." << e.what() << endl;
			} catch (...) {
				cerr << "[LoginRecordWorker::doClean] setKeepUseOff(" << it->first << ")  failed with unknown exception." << endl;
			}
		}
		usleep(5000);
	}
}

void LoginRecordWorker::recording(bool r_){
        char year[10];
        char month[10];
        char day[10];
	time_t yesterday = _time - 24*3600;
        strftime(year, sizeof(year), "%Y", localtime(&yesterday));
        strftime(month, sizeof(month), "%m", localtime(&yesterday));
        strftime(day, sizeof(day), "%d", localtime(&yesterday));
        string date = string(year) + "-" + string(month) + "-" + string(day);

	cout << "[LoginRecordWorker::recording] -------stat-result-------" << endl;
	map<int,int>::iterator itStat = _continue_stat.begin();
	if( itStat == _continue_stat.end() ){
		cout << "[LoginRecordWorker::recording] ---------no-stat---------" << endl;
		return;
	}
	Statement sqlw;
	sqlw << "INSERT INTO user_loginday_stat (date,logindays,users) VALUES "
		<< "(" << mysqlpp::quote << date << "," << itStat->first << "," << itStat->second << ")";
	cout << "[LoginRecordWorker::recording] continue\t" << itStat->first << "\t=>\t" << itStat->second << endl;
	for( ++itStat; itStat != _continue_stat.end(); ++itStat ){
		sqlw << ",(" << mysqlpp::quote << date << "," << itStat->first << "," << itStat->second << ")";
		cout << "[LoginRecordWorker::recording] continue\t" << itStat->first << "\t=>\t" << itStat->second << endl;
	}
	if( r_ ){
		cout << "[LoginRecordWorker::recording] --------recording--------" << endl;
		try {
			QueryRunner("user_loginday_stat",CDbWServer).execute(sqlw);
		} catch (mysqlpp::Exception e) {
			cerr << "[LoginRecordWorker::recording] insert db failed with mysqlpp::Exception." << e.what() << endl;
		} catch (std::exception e) {
			cerr << "[LoginRecordWorker::recording] insert db failed with std::exception." << e.what() << endl;
		} catch (...) {
			cerr << "[LoginRecordWorker::recording] insert db failed with unknown exception." << endl;
		}
	}
}
//-------------------------------------------------------------

vector<LoginData> LoginRecordWorker::getLoginRecordData(const vector<int>& ids,int cluster){
	Statement sql;
	ostringstream pattern;
	pattern << DB_NAME << "_" << cluster;
	sql	<< "SELECT user_id,continue_days,UNIX_TIMESTAMP(last_time) AS last_day FROM " << TABLE_NAME << cluster << " WHERE user_id IN (";
	vector<int>::const_iterator it = ids.begin();
	sql	<< *it;
	for( ++it;it!=ids.end();++it){
		sql << "," << *it;
	}
	sql << ")";

	vector<LoginData> data;

        QueryRunner(DB_NAME,CDbRServer,pattern.str()).query(sql,
			LoginRecordDataHandler(data) );

	return data;
}

map<int,bool> LoginRecordWorker::getUserState(const vector<int>& ids){
	map<int,bool> states;
	if( ids.empty() ){
		return states;
	}
        Statement sql;
        sql << "SELECT id, state FROM user_state WHERE id IN (";
	MyUtil::IntSeq::const_iterator it = ids.begin();
	sql << *it;
        for (++it; it != ids.end(); ++it) {
		sql << "," << *it;
        }
        sql << ")";
        QueryRunner("user_state_preloader",CDbRServer).query(sql,
		UserStateHandler(states) );

	return states;
}

map<int,bool> LoginRecordWorker::getKeepUserConfig(const vector<int>& ids){
	map<int,bool> configs;
	if( ids.empty() ){
		return configs;
	}
        Statement sql;
        sql << "SELECT id FROM keep_user_config WHERE id IN (";
	MyUtil::IntSeq::const_iterator it = ids.begin();
	sql << *it;
        for (++it; it != ids.end(); ++it) {
		sql << "," << *it;
        }
        sql << ")";
        QueryRunner("user_config",CDbRServer).query(sql,
		KeepUserConfigHandler(configs) );

	return configs;
}

//-------------------------------------------------------------

bool IdListResultHandler::handle(mysqlpp::Row& row) const {
        int uid = row["id"];
        _large_id = uid > _large_id ? uid : _large_id;
        _result.push_back(uid);
        return true;
}

bool LoginRecordDataHandler::handle(mysqlpp::Row& row) const{
	LoginData data;
	data.user_id = row["user_id"];
	data.continue_days = row["continue_days"];
	data.last_day = ((long)row["last_day"]);

	_data.push_back( data );

	return true;
}

bool UserStateHandler::handle(mysqlpp::Row& row) const{
        int uid = row["id"];
	int state = row["state"];
	_data[uid] = state&128;
	return true;
}

bool KeepUserConfigHandler::handle(mysqlpp::Row& row) const{
        int uid = row["id"];
	_data[uid] = true;
	return true;
}

//-------------------------------------------------------------
/*
void LoginRecordWorker::cleaning(const vector<int>& ids){
	for (vector<int>::const_iterator it = ids.begin(); it
			!= ids.end(); ++it) {
		try {
			UserStateAdapter::instance().setKeepUseOff( *it );
		} catch (Ice::Exception e) {
			cerr << "[LoginRecordWorker::clean] doClean setKeepUseOff failed with Ice::Exception." << e.what() << endl;
		} catch (std::exception e) {
			cerr << "[LoginRecordWorker::clean] doClean setKeepUseOff failed with std::exception." << e.what() << endl;
		} catch (...) {
			cerr << "[LoginRecordWorker::clean] doClean setKeepUseOff failed with unknown exception." << endl;
		}
		try {
			ostringstream pattern;
			pattern << DB_NAME << "_" << *it%DB_CLUSTER;
			Statement sqlw;
			sqlw    << "UPDATE " << TABLE_NAME << (*it%DB_CLUSTER)
				<< " SET continue_days=1 WHERE user_id=" << *it;
			QueryRunner(DB_NAME,CDbWServer,pattern.str()).execute(sqlw);
		} catch (mysqlpp::Exception e) {
			cerr << "[LoginRecordWorker::clean] doClean update db failed with mysqlpp::Exception." << e.what() << endl;
		} catch (std::exception e) {
			cerr << "[LoginRecordWorker::clean] doClean update db failed with std::exception." << e.what() << endl;
		} catch (...) {
			cerr << "[LoginRecordWorker::clean] doClean update db failed with unknown exception." << endl;
		}
	}
}
*/

