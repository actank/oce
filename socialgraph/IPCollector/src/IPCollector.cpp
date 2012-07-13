#include "IPCollector.h"
#include "IPRecordReplicaAdapter.h"
#include "FriendFinderByIPReplicaAdapter.h"
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::iprecord::adapter;
using namespace xce::friendfinder::adapter;

IPCollector::IPCollector(const string& date) {
	cout<<"[IPCollector::IPCollector] ConnectionPoolManager initializing" << endl;
	ConnectionPoolManager::instance().initialize();
	cout<<"[IPCollector::IPCollector] ConnectionPoolManager initialize done" << endl;

	string fixName = "iplog_";

	if(date==""){
		char year[10];
		char month[10];
		char date[10];

		time_t t = time(0); 
		t = t - 24*3600;	 //回退24小时
		strftime(year, sizeof(year), "%Y", localtime(&t));
		strftime(month, sizeof(month), "%m", localtime(&t));
		strftime(date, sizeof(date), "%d", localtime(&t));

		_tableName = fixName + string(year) + string(month) + string(date);
	} else{
		_tableName = fixName + date;
	}
}

IPCollector::~IPCollector(){
}

void IPCollector::collectForFBI(){
	cout<<"[IPCollector::collectForFBI] start with tableName : " << _tableName << endl;

	int max_id = 0;
	int last_id = 0;
	do {
		Statement sql;
		sql	<< "SELECT id,type,ip_num FROM " << _tableName
			<< " WHERE id >= " << max_id
			<< " LIMIT 10000";
		size_t count = QueryRunner("ip_log", CDbRServer).query(sql,
				IPDataByIDHandler(last_id,_ipdata_byid));
		for (map<int, set<long> >::iterator itMap = _ipdata_byid.begin(); itMap
				!= _ipdata_byid.end(); ++itMap) {
			cout<<"     key:" << itMap->first << endl;
			MyUtil::LongSeq ips;
			for (set<long>::iterator itSet = itMap->second.begin(); itSet
					!= itMap->second.end(); ++itSet) {
				cout<<"          value:" << *itSet << endl;
				ips.push_back( *itSet );
			}
			try {
				FriendFinderByIPAdapter::instance().accessIPs( itMap->first, ips );
			} catch( Ice::Exception e ){
				cout << "[IPCollector::collectForFBI] accessIPs(" << itMap->first << ",ips) found Ice::Exception: " << e.what() << endl;
				continue;
			} catch( ... ){
				cout << "[IPCollector::collectForFBI] accessIPs(" << itMap->first << ",ips) found unknown exception: " << endl;
				continue;
			}
		}
		_ipdata_byid.clear();

		if (last_id > max_id) {
			max_id = last_id;
		} else if( count > 0) {
			++max_id;
			++last_id;
		} else{
			break;
		}
		cout << "[IPCollector::collectForFBI] collecting to " << last_id << endl;
	} while (true);
}

void IPCollector::collectForIR(){
	cout<<"[IPCollector::collectForIR] start with tableName : " << _tableName << endl;

	int max_id = 0;
	int last_id = 0;
	do {
		Statement sql;
		sql	<< "SELECT id,type,ip_num FROM " << _tableName
			<< " WHERE id >= " << max_id
			<< " LIMIT 10000";
		size_t count = QueryRunner("ip_log", CDbRServer).query(sql,
				IPDataByIDHandler(last_id,_ipdata_byid));
		for (map<int, set<long> >::iterator itMap = _ipdata_byid.begin(); itMap
				!= _ipdata_byid.end(); ++itMap) {
			cout<<"     key:" << itMap->first << endl;
			MyUtil::LongSeq ips;
			for (set<long>::iterator itSet = itMap->second.begin(); itSet
					!= itMap->second.end(); ++itSet) {
				cout<<"          value:" << *itSet << endl;
				ips.push_back( *itSet );
				try {
					IPRecordAdapter::instance().addUser( *itSet, itMap->first );
				} catch( Ice::Exception e ){
					cout << "[IPCollector::collectForIR] addUser(" << *itSet << ", " << itMap->first << ") found Ice::Exception: " << e.what() << endl;
					continue;
				} catch( ... ){
					cout << "[IPCollector::collectForIR] addUser(" << *itSet << ", " << itMap->first << ") found unknown exception: " << endl;
					continue;
				}
			}
		}
		_ipdata_byid.clear();

		if (last_id > max_id) {
			max_id = last_id;
		} else if( count > 0) {
			++max_id;
			++last_id;
		} else{
			break;
		}
		cout << "[IPCollector::collectForIR] collecting to " << last_id << endl;
	} while (true);
}

bool IPDataByIDHandler::handle(mysqlpp::Row& row) const{
	int id = row["id"];
	int type = row["type"];
	long ip = (long)row["ip_num"];

	_last_id = id > _last_id ? id : _last_id;

	if( type != 0 ){
		return true;
	}

	if( ip > 0 ){
		_results[id].insert( ip );
	}
	return true;
}

