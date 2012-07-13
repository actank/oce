#include "socialgraph/socialgraphutil/calculator/component/readfriendlistutil.h"

namespace readfriend {
//批量计算
void ReadFriendListUtil::GetFriendList(const std::vector<int>& idList, std::map<int, std::vector<int> >& result) {
	time_t phrase_start = 0, phrase_end = 0;
	phrase_start = time(NULL);

	MyUtil::TaskManager::instance().config(TASK_LEVEL_READ_FRIEND, MyUtil::ThreadPoolConfig(0, 100));
	int task_size = 0;
	long wait = 0;
	long count = 0;
	for (std::vector<int>::const_iterator iter = idList.begin();
			iter != idList.end(); ++iter) {
		++count;
		std::pair<std::map<int, std::vector<int> >::iterator, bool> tmp =
			result.insert(make_pair<int, vector<int> >(*iter, vector<int>()));
		
		MyUtil::TaskManager::instance().execute(new ReadFriendTask(*iter, tmp.first->second));	
		while ((task_size = MyUtil::TaskManager::instance().size(TASK_LEVEL_READ_FRIEND)) > 10000) {
			wait += 2;
			sleep(2);
			fprintf(stderr, "TASK_LEVEL_READ_FRIEND: sleep %ld second, %ld / %ld.\r", wait, count, idList.size());
		}
	}
	fprintf(stderr, "\nRead user friendlist completed\n");
	//等待所有线程运行结束
	while (MyUtil::TaskManager::instance().size(TASK_LEVEL_READ_FRIEND) > 0) {
		fprintf(stderr, "final left TASK_LEVEL_READ_FRIEND size = %d.\n", task_size);
		sleep(2);
	}
	phrase_end = time(NULL);
	fprintf(stderr, "size of idList = %ld, size of result = %ld, using %.1fm\n", 
		idList.size(), result.size(), (phrase_end - phrase_start)/60.0);
}

//同步获取接口
void ReadFriendListUtil::GetFriendListSynchronism(const std::vector<int>& idList, std::map<int, std::vector<int> >& result) {
	using namespace com::xiaonei::xce;
	long count = 0;
	for (std::vector<int>::const_iterator iter = idList.begin();
			iter != idList.end(); ++iter) {
		if (++count % 1000 == 0) {
			fprintf(stderr, "GetFriendListSynchronism %ld / %ld.\r", count, idList.size());
		}

		std::pair<std::map<int, std::vector<int> >::iterator, bool> tmp =
			result.insert(std::make_pair<int, std::vector<int> >(*iter, std::vector<int>()));
		string tableName = TableName::RelationTableName(*iter);
		string pattern = TableName::RelationPattern(*iter);

		Statement sql;
		sql
			<< "SELECT * FROM " << tableName 
			<< " WHERE host=" << *iter;
		try {
			QueryRunner("relation_friendrank", CDbRServer, pattern).query(sql, FriendRelationReadHandler(tmp.first->second));
		} catch (Ice::Exception& e) {
			std::cout << "[ReadFriendTask] Connection Exception: " << e.what() << std::endl;;	
		} catch (std::exception& e) {
			std::cout << "[ReadFriendTask] Std Exception: " << e.what() << std::endl;
		} catch (...) {
			std::cout << "[ReadFriendTask] Unknown Exception" << std::endl;
		}

		//好友列表按id递增排序
		std::sort(tmp.first->second.begin(), tmp.first->second.end());       
		std::vector<int>(tmp.first->second).swap(tmp.first->second);
	}
}

//---------------------------------------------------------------

void ReadFriendTask::handle() {
	using namespace com::xiaonei::xce;
	std::string tableName = TableName::RelationTableName(_userId);
	std::string pattern = TableName::RelationPattern(_userId);

	Statement sql;
	sql
		<< "SELECT * FROM " << tableName 
		<< " WHERE host=" << _userId;
	try {
		QueryRunner("relation_friendrank", CDbRServer, pattern).query(sql, FriendRelationReadHandler(_friendList));
	} catch (Ice::Exception& e) {
		std::cout << "[ReadFriendTask] Connection Exception: " << e.what() << std::endl;	
	} catch (std::exception& e) {
		std::cout << "[ReadFriendTask] Std Exception: " << e.what() << std::endl;
	} catch (...) {
		std::cout << "[ReadFriendTask] Unknown Exception" << std::endl;
	}

	//好友列表按id递增排序
	std::sort(_friendList.begin(), _friendList.end());       
}

//---------------------------------------------------------------

bool FriendRelationReadHandler::handle(mysqlpp::Row& row) const {
	if ((int)_friendList.size() < 2000) {
		_friendList.push_back((int)row["guest"]);
	}
	return true;
}

}
