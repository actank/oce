#include "PreloadLoginData.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::logindata;
using namespace xce::friendfinder::adapter;
using namespace std;

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

	MCE_DEBUG("[PreloadLoginData] loadFromDB firstFile(" << firstFile << ") secondFile(" << secondFile << ")");
	
	loadFirstTable( firstFile );
	loadSecondTable( secondFile );

	buildBitMap();               //拼接最近30天数据
	buildNegtiveUserList();		 //找出不活跃列表

	clearUserLoginCache();      //清空cache

	//sleep( 10 );    //为清空内存留时间,确保内存被清理

	setBitMap();            //向cache加载数据 为了方便判断有先后顺序，先加载bitMap，后加载negtiveList

	setNegtiveList();		//向cache加载数据
}

void PreloadLoginData::loadFirstTable(const std::string& tableName) {
	MCE_INFO("[PreloadLoginData] loadFirstTable(" << tableName << ")");
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
			MCE_WARN(time(NULL) << "[PreloadLoginData] loadFirstTable " << tableName << " Connection Exception : " << e.what() << "\n");
		} catch (...) {
			MCE_WARN(time(NULL) << "[PreloadLoginData] loadFirstTable DB unknow Exception");
		}

		if(last > lowboundary) {
			lowboundary = last;
		} else if (queryCount <= 0) {
			MCE_INFO("[PreloadLoginData] loadFirstTable lowboundary(" << lowboundary << ") at end of " << tableName);
			break;
		}
	} while(true);
	MCE_INFO("1 _userLoginMap size : " << _userLoginMap.size());
}

void PreloadLoginData::loadSecondTable(const std::string& tableName) {
	MCE_INFO("[PreloadLoginData] loadSecondTable(" << tableName << ")");
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
			MCE_WARN(time(NULL) << "[PreloadLoginData] loadSecondTable " << tableName << " Connection Exception : " << e.what() << "\n");
		} catch (...) {
			MCE_WARN(time(NULL) << "[PreloadLoginData] loadSecondTable DB unknow Exception");
		}
		
		if(last > lowboundary) {
			lowboundary = last;
		} else if (queryCount <= 0) {
			MCE_INFO("[PreloadLoginData] loadSecondTable lowboundary(" << lowboundary << ") at end of " << tableName);
			break;
		}
	} while(true);
	MCE_INFO("2 _userLoginMap size : " << _userLoginMap.size());
}

void PreloadLoginData::buildBitMap() {
	MCE_INFO("[PreloadLoginData] trickPreprocess");

	char date[10];
    time_t t = time(0);
    strftime( date, sizeof(date), "%d", localtime(&t) );
    int today = atoi(date);
    MCE_INFO("today : " << today);

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
	MCE_INFO("[PreloadLoginData] buildNegtiveUserList _userLoginBitMap size : " << _userLoginBitMap.size());
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
	MCE_INFO("[PreloadLoginData] buildNegtiveUserList _negtiveUserList size " << _negtiveUserList.size());
}

void PreloadLoginData::setBitMap() {
	MCE_INFO("[PreloadLoginData] setBitMap");
	map<int, int> tmpData;
	for(map< int,bitset<32> >::iterator iter=_userLoginBitMap.begin(); iter!=_userLoginBitMap.end(); ++iter) {
		tmpData[iter->first] = iter->second.to_ulong();
		if(tmpData.size() >= 100000) {
			for(int cluster = begin; cluster < end; ++cluster) {				//加载10份cache
				try {
					FriendFinderByActiveUserAdapter::instance().setBitMap( cluster, tmpData );
				} catch ( Ice::Exception& e ) {
					MCE_WARN(time(NULL) << "[PreloadLoginData] setBitMap Ice Exception " << e.what() << "\n");
				} catch ( ... ) {
					MCE_WARN(time(NULL) << "[PreloadLoginData] setBitMap Unknow Exception");
				}
			}
			tmpData.clear();
		}
	}

	if(tmpData.size() > 0) {
		for(int cluster = begin; cluster < end; ++cluster) {					//加载10份cache
			try {
				FriendFinderByActiveUserAdapter::instance().setBitMap( cluster, tmpData );
			} catch ( Ice::Exception& e ) {
				MCE_WARN(time(NULL) << "[PreloadLoginData] setBitMap Ice Exception " << e.what() << "\n");
			} catch ( ... ) {
				MCE_WARN(time(NULL) << "[PreloadLoginData] setBitMap Unknow Exception");
			}
		}
	}
}

void PreloadLoginData::setNegtiveList() {
	MCE_INFO("[PreloadLoginData] setNegtiveList");
	IntSeq tmpData;
	for(vector<int>::iterator iter=_negtiveUserList.begin(); iter!=_negtiveUserList.end(); ++iter) {
		tmpData.push_back( *iter );
		if(tmpData.size() >= 100000) {
			for(int cluster = begin; cluster < end; ++cluster) {				//加载10份cache
				try {
					FriendFinderByActiveUserAdapter::instance().setNegtiveList( cluster, tmpData );
				} catch ( Ice::Exception& e ) {
					MCE_WARN(time(NULL) << "[PreloadLoginData] setBitMap Ice Exception " << e.what() << "\n");
				} catch ( ... ) {
					MCE_WARN(time(NULL) << "[PreloadLoginData] setBitMap Unknow Exception");
				}
			}
			tmpData.clear();
		}
	}

	if(tmpData.size() > 0) {
		for(int cluster = begin; cluster < end; ++cluster) {          //加载10份cache
			try {
				FriendFinderByActiveUserAdapter::instance().setNegtiveList( cluster, tmpData );
			} catch ( Ice::Exception& e ) {
				MCE_WARN(time(NULL) << "[PreloadLoginData] setBitMap Ice Exception " << e.what() << "\n");
			} catch ( ... ) {
				MCE_WARN(time(NULL) << "[PreloadLoginData] setBitMap Unknow Exception");
			}
		}
	}
}

void PreloadLoginData::clearUserLoginCache() {
	for(int cluster=0; cluster<10; ++cluster) {				//删除10份cache
		try {
			FriendFinderByActiveUserAdapter::instance().clearUserLoginCache( cluster );
		} catch ( Ice::Exception& e ) {
			MCE_WARN(time(NULL) << "[PreloadLoginData] clearUserLoginCache Ice Exception " << e.what() << " cluster=" << cluster << "\n");
		} catch ( ... ) {
			MCE_WARN(time(NULL) << "[PreloadLoginData] clearUserLoginCache Unknow Exception");
		}

		sleep( 10 );    //为清空内存留时间,确保内存被清理
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
