#include "FriendFinderNewComerCollector.h"
#include "FriendFinderNewComerWorkerAdapter.h"
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::socialgraph::adapter;

FriendFinderNewComerCollector::FriendFinderNewComerCollector() {
	cout<<"[FriendFinderNewComerCollector::FriendFinderNewComerCollector] ConnectionPoolManager initializing" << endl;
	ConnectionPoolManager::instance().initialize();
	cout<<"[FriendFinderNewComerCollector::FriendFinderNewComerCollector] ConnectionPoolManager initialize done" << endl;
}

FriendFinderNewComerCollector::~FriendFinderNewComerCollector(){
}

void FriendFinderNewComerCollector::deployByDays(int inDays){
	cout<<"[FriendFinderNewComerCollector::deployByDays(" << inDays << ")] " << endl;

	int minTimeInt = 2147483647;
	string minTimeStr = "2099-12-03 22:31:50";

        struct tm *tm_local_now;
        time_t now = time(NULL);
        tm_local_now = localtime(&now);
        tm_local_now->tm_hour = 0;
        tm_local_now->tm_min = 0;
        tm_local_now->tm_sec = 0;
        time_t time_zero = timelocal(tm_local_now);
        int deadline = time_zero-inDays*86400;

	do {
		int minTimeIntTmp = minTimeInt;
		string minTimeStrTmp = minTimeStr;

		Statement sql;
		sql	<< "SELECT id, registertime, UNIX_TIMESTAMP(registertime) as rt "
			<< " FROM user_time"
			<< " WHERE registertime < " << mysqlpp::quote << minTimeStr
			<< " ORDER BY registertime DESC"
			<< " LIMIT 10000";

		vector<registerNode> nodes;
		QueryRunner("user_time", CDbRServer).query(sql,
				NewComerCollectorHandler(deadline,minTimeInt,minTimeStr,nodes));

		for (vector<registerNode>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
			try {
				FriendFinderNewComerWorkerAdapter::instance().addNewComerWithTime( it->userId,it->timeInt );
				cout << "[deployByDays] addNewComerWithTime(" << it->userId << ",\"" << it->timeStr << "\")" << endl;
			} catch( Ice::Exception e ){
				cout << "[deployByDays] addNewComerWithTime(" << it->userId << ",\"" << it->timeStr << "\") found Ice::Exception: " << e.what() << endl;
				continue;
			} catch( ... ){
				cout << "[deployByDays] addNewComerWithTime(" << it->userId << ",\"" << it->timeStr << "\") found unknown exception: " << endl;
				continue;
			}
		}

		if ( minTimeInt < minTimeIntTmp ){
			minTimeIntTmp = minTimeInt;
			minTimeStrTmp = minTimeStr;
		} else if( nodes.size() > 0) {
			//TODO:
			break;
		} else{
			break;
		}

		nodes.clear();

		cout << "[deployByDays] collecting to " << minTimeInt << endl;
	} while (true);
}

void FriendFinderNewComerCollector::deployByUser(int userId){
	try {
		FriendFinderNewComerWorkerAdapter::instance().addNewComer( userId );
		cout << "[deployByDays] addNewComer(" << userId << ")" << endl;
	} catch( Ice::Exception e ){
		cout << "[deployByDays] addNewComer(" << userId << ") found Ice::Exception: " << e.what() << endl;
	} catch( ... ){
		cout << "[deployByDays] addNewComer(" << userId << ") found unknown exception: " << endl;
	}
}

bool NewComerCollectorHandler::handle(mysqlpp::Row& row) const{
	int id = (int)row["id"];
	string registertime= (string)row["registertime"];
	int rt = (int)row["rt"];

	if( rt < _deadline ){
		return false;
	}

	if( rt < _minTimeInt ){
		_minTimeStr = registertime;
		_minTimeInt = rt;
	}

	registerNode node;
	node.userId = id;
	node.timeStr = registertime;
	node.timeInt = rt;
	_results.push_back( node );
	return true;
}

