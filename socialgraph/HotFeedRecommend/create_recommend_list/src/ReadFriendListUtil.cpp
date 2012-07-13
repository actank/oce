#include "ReadFriendListUtil.h"

using namespace std;
using namespace readfriend;
using namespace MyUtil;
using namespace com::xiaonei::xce;

void ReadFriendListUtil::GetFriendList(const std::vector<int>& idList, std::map<int, std::vector<int> >& result) {
	cout << "[ReadFriendListUtil] GetFriendList" << endl;
	time_t phrase_start = 0, phrase_end = 0;
	phrase_start = time(NULL);

	TaskManager::instance().config(TASK_LEVEL_READ_FRIEND, ThreadPoolConfig(0, 100));

	int task_size = 0;
	long wait = 0;
	long count = 0;

	for (vector<int>::const_iterator iter = idList.begin();
			iter != idList.end(); ++iter) {
		++count;
		pair<map<int, vector<int> >::iterator, bool> tmp =
			result.insert(make_pair<int, vector<int> >(*iter, vector<int>()));
		
		TaskManager::instance().execute(new ReadFriendTask(*iter, tmp.first->second));	

		while ((task_size = TaskManager::instance().size(TASK_LEVEL_READ_FRIEND)) > 10000) {
			wait += 2;
			sleep(2);
			fprintf(stderr, "TASK_LEVEL_READ_FRIEND: sleep %ld second, %ld / %ld.\r", wait, count, idList.size());
		}
	}
	//sleep(2);
	fprintf(stderr, "\nRead user friendlist completed\n");

	while (TaskManager::instance().size(TASK_LEVEL_READ_FRIEND) > 0) {
		fprintf(stderr, "final left TASK_LEVEL_READ_FRIEND size = %d.\n", task_size);
		sleep(2);
	}

	phrase_end = time(NULL);

	fprintf(stderr, "size of idList = %ld, size of result = %ld, using %.1fm\n", 
		idList.size(), result.size(), (phrase_end - phrase_start)/60.0);
}

void ReadFriendListUtil::GetFriendListSynchronism(const std::vector<int>& idList, std::map<int, std::vector<int> >& result) {
	long count = 0;

	for (vector<int>::const_iterator iter = idList.begin();
			iter != idList.end(); ++iter) {
		if (++count % 1000 == 0) {
			fprintf(stderr, "GetFriendListSynchronism %ld / %ld.\r", count, idList.size());
		}

		pair<map<int, vector<int> >::iterator, bool> tmp =
			result.insert(make_pair<int, vector<int> >(*iter, vector<int>()));

		string tableName = TableName::RelationTableName(*iter);
		string pattern = TableName::RelationPattern(*iter);

		Statement sql;
		sql
			<< "SELECT * FROM " << tableName 
			<< " WHERE host=" << *iter;

		try {
			QueryRunner("relation_friendrank", CDbRServer, pattern).query(sql, FriendRelationReadHandler(tmp.first->second));
		} catch (Ice::Exception& e) {
			cout << "[ReadFriendTask] Connection Exception: " << e.what();	
		} catch (std::exception& e) {
			cout << "[ReadFriendTask] Std Exception: " << e.what();
		} catch (...) {
			cout << "[ReadFriendTask] Unknown Exception" << endl;
		}

		sort(tmp.first->second.begin(), tmp.first->second.end());       //好友列表按id递增排序

		vector<int>(tmp.first->second).swap(tmp.first->second);
	}
}

//---------------------------------------------------------------

void ReadFriendTask::handle() {
	string tableName = TableName::RelationTableName(_userId);
	string pattern = TableName::RelationPattern(_userId);

	Statement sql;
	sql
		<< "SELECT * FROM " << tableName 
		<< " WHERE host=" << _userId;

	try {
		QueryRunner("relation_friendrank", CDbRServer, pattern).query(sql, FriendRelationReadHandler(_friendList));
	} catch (Ice::Exception& e) {
		cout << "[ReadFriendTask] Connection Exception: " << e.what();	
	} catch (std::exception& e) {
		cout << "[ReadFriendTask] Std Exception: " << e.what();
	} catch (...) {
		cout << "[ReadFriendTask] Unknown Exception" << endl;
	}

	sort(_friendList.begin(), _friendList.end());       //好友列表按id递增排序
	//cout << _friendList.size() << endl;
}

//---------------------------------------------------------------

bool FriendRelationReadHandler::handle(mysqlpp::Row& row) const {
	if ((int)_friendList.size() < 2000) {
		//cout << row["guest"] << endl;
		_friendList.push_back((int)row["guest"]);
	}
	return true;
}
