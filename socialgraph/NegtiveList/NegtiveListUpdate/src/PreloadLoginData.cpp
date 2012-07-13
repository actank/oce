#include "PreloadLoginData.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::logindata;
//using namespace xce::friendfinder::adapter;
using namespace std;
using namespace readfriend;
using namespace sortbyrank;
using namespace reversemap;
using namespace tasks;

void PreloadLoginData::loadFromDB() {
	string TableName = "user_login_";
	char year[10];
	char month[10];
	char date[10];

	time_t now = time(0);
	strftime(year, sizeof(year), "%Y", localtime(&now));
	strftime(month, sizeof(month), "%m", localtime(&now));
	strftime(date, sizeof(date), "%d", localtime(&now));

	string firstFile = TableName + string(year) + string(month);

	time_t last = now - (atoi(date) + 3) * 24 * 3600;
	strftime(year, sizeof(year), "%Y", localtime(&last));
	strftime(month, sizeof(month), "%m", localtime(&last));
	strftime(date, sizeof(date), "%d", localtime(&last));

	string secondFile = TableName + string(year) + string(month);

	cout << "firstFile(" << firstFile << ") secondFile(" << secondFile << ")" << endl;
	
	loadFirstTable( firstFile );
	loadSecondTable( secondFile );

	buildBitMap();               //拼接最近30天数据
	buildNegtiveUserList();		 //找出不活跃列表

	_negtiveUserList.push_back(239737004);
	_negtiveUserList.push_back(233940269);
	
	cout << "_negtiveUserList size = " << _negtiveUserList.size() << endl;

	map<int, vector<int> > negtiveFriendsMap;
	getFriendsOfNegtiveList(negtiveFriendsMap);            //获取不活跃用户的好友列表

	//cout << negtiveFriendsMap.size() << endl;
	//for (map<int, vector<int> >::iterator iter = negtiveFriendsMap.begin();
	//		iter != negtiveFriendsMap.end(); ++iter) {
	//	cout << iter->first << " : " << endl;
	//	for (vector<int>::iterator it = iter->second.begin();
	//			it != iter->second.end(); ++it) {
	//		cout << *it << " ";
	//	}
	//	cout << endl;
	//}

	map<int, vector<int> > userNegtiveListMap;
	getUserNegtiveListMap(negtiveFriendsMap, userNegtiveListMap);          //获取用户的不活跃列表

	//userNegtiveListMap[238111132].push_back(285340707);
	//userNegtiveListMap[238111132].push_back(222677625);
	//userNegtiveListMap[238111132].push_back(1331);
	//userNegtiveListMap[238111132].push_back(249386140);
	//userNegtiveListMap[238111132].push_back(24496255);

	//cout << "next" << endl;
	//cout << userNegtiveListMap.size() << endl;
	//for (vector<int>::iterator iter = userNegtiveListMap[238111132].begin();
	//		iter != userNegtiveListMap[238111132].end(); ++iter) {
	//	cout << *iter << " ";
	//}
	//cout << endl;
	//for (map<int, vector<int> >::iterator iter = userNegtiveListMap.begin();
	//		iter != userNegtiveListMap.end(); ++iter) {
	//	cout << iter->first << " : " << endl;
	//	for (vector<int>::iterator it = iter->second.begin();
	//			it != iter->second.end(); ++it) {
	//		cout << *it << " ";
	//	}
	//	cout << endl;
	//}

	sortUserNegtiveListByRank(userNegtiveListMap);            //按亲密度排序

	//cout << "next next" << endl;
	//cout << userNegtiveListMap.size() << endl;
	//for (vector<int>::iterator iter = userNegtiveListMap[238111132].begin();
	//		iter != userNegtiveListMap[238111132].end(); ++iter) {
	//	cout << *iter << " ";
	//}
	//cout << endl;

	appendSingleRelationAndsetCache(userNegtiveListMap);
}

void PreloadLoginData::loadFirstTable(const std::string& tableName) {
	cout << "[PreloadLoginData] loadFirstTable(" << tableName << ")";
	int lowboundary = 0;
	int last = 0;

	do {
		size_t queryCount = 0;
		Statement sql;   //查数据库
		sql
			<< "select id, CAST(`login_days` AS signed) as frequence from " << tableName
			<< " where id>" << lowboundary
			<< " limit 100000";
		try {
			queryCount = QueryRunner("friendfinder_userlogin", CDbRServer).query( sql, FirstLoginReadHandler(last, _userLoginMap));
		} catch (Ice::Exception& e) {
			cout << time(NULL) << "[PreloadLoginData] loadFirstTable " << tableName << " Connection Exception : " << e.what() << endl;
		} catch (...) {
			cout << time(NULL) << "[PreloadLoginData] loadFirstTable DB unknow Exception" << endl;
		}

		if(last > lowboundary) {
			lowboundary = last;
		} else if (queryCount <= 0) {
			cout << "[PreloadLoginData] loadFirstTable lowboundary(" << lowboundary << ") at end of " << tableName << endl;
			break;
		}
	} while(true);
	cout << "1 _userLoginMap size : " << _userLoginMap.size() << endl;
}

void PreloadLoginData::loadSecondTable(const std::string& tableName) {
	cout << "[PreloadLoginData] loadSecondTable(" << tableName << ")" << endl;
	int lowboundary = 0;
	int last = 0;
	
	do {
		size_t queryCount = 0;
		Statement sql;   //查数据库
		sql
			<< "select id, CAST(`login_days` AS signed) as frequence from " << tableName
			<< " where id>" << lowboundary
			<< " limit 100000";
		try {
			queryCount = QueryRunner("friendfinder_userlogin", CDbRServer).query( sql, SecondLoginReadHandler(last, _userLoginMap) );
		} catch (Ice::Exception& e) {
			cout << time(NULL) << "[PreloadLoginData] loadSecondTable " << tableName << " Connection Exception : " << e.what() << endl;
		} catch (...) {
			cout << time(NULL) << "[PreloadLoginData] loadSecondTable DB unknow Exception" << endl;
		}
		
		if(last > lowboundary) {
			lowboundary = last;
		} else if (queryCount <= 0) {
			cout << "[PreloadLoginData] loadSecondTable lowboundary(" << lowboundary << ") at end of " << tableName << endl;
			break;
		}
	} while(true);
	cout << "2 _userLoginMap size : " << _userLoginMap.size() << endl;
}

void PreloadLoginData::buildBitMap() {
	cout << "[PreloadLoginData] trickPreprocess" << endl;

	char date[10];
    time_t t = time(0);
    strftime( date, sizeof(date), "%d", localtime(&t) );
    int today = atoi(date);
	cout << "today : " << today << endl;

	for(map< int, vector<int> >::iterator iter=_userLoginMap.begin(); iter!=_userLoginMap.end(); ++iter) {
		if( iter->second.size() < 2 ) {
			continue;
		}

		bitset<32> bitvec1( iter->second.at(0) );
        bitset<32> bitvec2( iter->second.at(1) );
		
		bitset<32> result;
        result.reset();

		int pos = 0;
        for(int i=today-1; i>=0; --i) {
            if( bitvec1.test(i) ) {
                result.set(pos);
            } else {
                result.reset(pos);
            }
            ++pos;
        }

		int endTime = pos;
        for(int j=31; j>endTime; --j) {
            if( bitvec2.test(j) ) {
                result.set(pos);
            } else {
                result.reset(pos);
            }
            ++pos;
        }
		_userLoginBitMap[iter->first] = result;
	}
}

void PreloadLoginData::buildNegtiveUserList() {
	cout << "[PreloadLoginData] buildNegtiveUserList _userLoginBitMap size : " << _userLoginBitMap.size() << endl;
	if( _userLoginBitMap.empty() ) {
		return;
	}

	for(map< int, bitset<32> >::iterator iter=_userLoginBitMap.begin(); iter!=_userLoginBitMap.end(); ++iter) {
		bitset<NEGTIVEDAYS> tmp1;
		for(int index=0; index<NEGTIVEDAYS; ++index) {
			if( iter->second.test(index) ) {
				tmp1.set( index );
			} else {
				tmp1.reset( index );
			}
		}

		bitset<NEGTIVEDAYS> tmp2;
		for(int index=NEGTIVEDAYS; index<2*NEGTIVEDAYS && index<32; ++index) {
			if( iter->second.test(index) ) {
				tmp2.set( index - NEGTIVEDAYS );
			} else {
				tmp2.reset( index - NEGTIVEDAYS );
			}
		}

		if( tmp1.count()<2 && tmp2.count()<2 ) {
			_negtiveUserList.push_back( iter->first );
		}
	}
	cout << "[PreloadLoginData] buildNegtiveUserList _negtiveUserList size " << _negtiveUserList.size() << endl;
}

void PreloadLoginData::getFriendsOfNegtiveList(std::map<int, std::vector<int> >& negtiveFriendsMap) {
	cout << "[PreloadLoginData] getFriendsOfNegtiveList negtiveFriendsMap size(" << negtiveFriendsMap.size() << ")" << endl;
	g_readfriend_ptr->GetFriendList(_negtiveUserList, negtiveFriendsMap);
}

void PreloadLoginData::getUserNegtiveListMap(const std::map<int, std::vector<int> >& negtiveFriendsMap, 
		std::map<int, std::vector<int> >& userNegtiveListMap) {
	cout << "[PreloadLoginData] getUserNegtiveListMap negtiveFriendsMap size(" << negtiveFriendsMap.size() << 
			")  userNegtiveListMap size(" << userNegtiveListMap.size() << ")" << endl;
	ReverseMapUtil::ReverseMap(negtiveFriendsMap, userNegtiveListMap);
}

void PreloadLoginData::sortUserNegtiveListByRank(std::map<int, std::vector<int> >& userNegtiveListMap) {
	cout << "[PreloadLoginData] sortUserNegtiveListByRank userNegtiveListMap size(" << userNegtiveListMap.size() << ")" << endl;
	g_sortbyrank_ptr->SortTargetByFriendRank(userNegtiveListMap);

	//cout << "presss" << endl;
	//cout << userNegtiveListMap.size() << endl;
	//for (vector<int>::iterator iter = userNegtiveListMap[238111132].begin();
	//		iter != userNegtiveListMap[238111132].end(); ++iter) {
	//	cout << *iter << " ";
	//}
	//cout << endl;
}

void PreloadLoginData::appendSingleRelationAndsetCache(const std::map<int, std::vector<int> >& userNegtiveListMap) {
    TaskManager::instance().config(TASK_LEVEL_SET_CACHE, ThreadPoolConfig(0, 100));                                                                                            
    int task_size = 0;   
    long wait = 0;       
    long count = 0;      
      
	for (map<int, vector<int> >::const_iterator iter = userNegtiveListMap.begin();
			iter != userNegtiveListMap.end(); ++iter) {

        //if (iter->first % 10 != 2) {          //尾号控制
		//	continue;    
        //}

        if (++count % 1000 == 0) {    
			fprintf(stderr, "appendSingleRelationAndsetCache %ld / %ld \r", count, userNegtiveListMap.size());	
		}
		TaskManager::instance().execute(new AppendSingleRelationAndSetCacheTask(iter->first, iter->second));	

		while ((task_size = TaskManager::instance().size(TASK_LEVEL_SET_CACHE)) > 10000) {
			wait += 2;
			sleep(2);
		}
	}
 
    //sleep(5);
    fprintf(stderr, "\n appendSingleRelation set cache completed.\n");                                                                                                         
    while (TaskManager::instance().size(TASK_LEVEL_SET_CACHE) > 0) {
        sleep(2);
    }
}

//********************************************************************************************

bool FirstLoginReadHandler::handle(mysqlpp::Row& row) const {
	_last = _last > (int)row["id"] ? _last : (int)row["id"];

	int id = (int)row["id"];
	int frequence = (int)row["frequence"];

	_userLoginMap[id].push_back(frequence);
	_userLoginMap[id].push_back(0);
	return true;
}

//********************************************************************************************

bool SecondLoginReadHandler::handle(mysqlpp::Row& row) const {
	_last = _last > (int)row["id"] ? _last : (int)row["id"];

	int id = (int)row["id"];
	int frequence = (int)row["frequence"];

	map< int, vector<int> >::iterator fIt = _userLoginMap.find( id );
	if(fIt != _userLoginMap.end()) {
		fIt->second.at(1) = frequence;
	} else {
		_userLoginMap[id].push_back(0);         //default
		_userLoginMap[id].push_back(frequence);
	}
	return true;
}
