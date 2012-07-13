#include "socialgraph/socialgraphutil/calculator/component/preloadlogindata.h"

namespace xce {
namespace socialgraph {

void PreloadLoginData::LoadFromDB() {
	std::string TableName = "user_login_";
	char year[10];
	char month[10];
	char date[10];

	time_t now = time(0);         //当前月份数据表
	strftime(year, sizeof(year), "%Y", localtime(&now));
	strftime(month, sizeof(month), "%m", localtime(&now));
	strftime(date, sizeof(date), "%d", localtime(&now));

	std::string firstFile = TableName + string(year) + string(month);

	time_t last = now - (atoi(date) + 3) * 24 * 3600;        //上个月份数据表
	strftime(year, sizeof(year), "%Y", localtime(&last));
	strftime(month, sizeof(month), "%m", localtime(&last));
	strftime(date, sizeof(date), "%d", localtime(&last));

	string secondFile = TableName + string(year) + string(month);

	std::cout << "[PreloadLoginData] loadFromDB firstFile(" << firstFile << ") secondFile(" << secondFile << ")" << std::endl;
	
	LoadFirstTable( firstFile );
	LoadSecondTable( secondFile );

	BuildBitMap();               //拼接最近30天数据
	BuildNegtiveUserList();		 //找出不活跃列表
}

void PreloadLoginData::GetNegtiveList(std::vector<int>& negtive_list) {
	std::cout << "[PreloadLoginData] GetNegtiveList _negtiveUserList size : " << _negtiveUserList.size() << std::endl;
	negtive_list.swap(_negtiveUserList);
}

std::map< int, std::bitset<32> > PreloadLoginData::GetUserLogin(const std::vector<int>& uids) {
	std::cout << "[PreloadLoginData] GetUserLogin uis size : " << uids.size() << std::endl;
	IceUtil::RWRecMutex::RLock lock(init_flag_mutex_);
	std::map< int, std::bitset<32> > login_map;
	for (std::vector<int>::const_iterator iter = uids.begin();
			iter != uids.end(); ++iter) {
		std::map< int, std::bitset<32> >::iterator fit = _userLoginBitMap.find(*iter);
		if (fit != _userLoginBitMap.end()) {
			login_map[fit->first] = fit->second;
		} else {
			login_map[fit->first]	= std::bitset<32>(0);
		}
	}
	return login_map;
}

void PreloadLoginData::LoadFirstTable(const std::string& tableName) {
	using namespace com::xiaonei::xce;
	std::cout << "[PreloadLoginData] loadFirstTable(" << tableName << ")" << std::endl;
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
			std::cout << "[PreloadLoginData] loadFirstTable " << tableName << " Connection Exception : " << e.what() << std::endl;
		} catch (...) {
			std::cout << "[PreloadLoginData] loadFirstTable DB unknow Exception" << std::endl;
		}

		if(last > lowboundary) {
			lowboundary = last;
		} else if (queryCount <= 0) {
			std::cout << "[PreloadLoginData] loadFirstTable lowboundary(" << lowboundary << ") at end of " << tableName << std::endl;
			break;
		}
	} while(true);
	std::cout << "1 _userLoginMap size : " << _userLoginMap.size() << std::endl;
}

void PreloadLoginData::LoadSecondTable(const std::string& tableName) {
	using namespace com::xiaonei::xce;
	std::cout << "[PreloadLoginData] loadSecondTable(" << tableName << ")" << std::endl;
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
			std::cout << "[PreloadLoginData] loadSecondTable " << tableName << " Connection Exception : " << e.what() << std::endl;
		} catch (...) {
			std::cout << "[PreloadLoginData] loadSecondTable " << tableName << " unknown Exception : " << std::endl;
		}
		
		if(last > lowboundary) {
			lowboundary = last;
		} else if (queryCount <= 0) {
			std::cout << "[PreloadLoginData] loadSecondTable lowboundary(" << lowboundary << ") at end of " << tableName << std::endl;
			break;
		}
	} while(true);
	cout << "2 _userLoginMap size : " << _userLoginMap.size() << endl;
}

void PreloadLoginData::BuildBitMap() {
	std::cout << "[PreloadLoginData] trickPreprocess" << std::endl;
	char date[10];
  time_t t = time(0);
  strftime( date, sizeof(date), "%d", localtime(&t) );
  int today = atoi(date);
	std::cout << "today : " << today << std::endl;

	for(map< int, vector<int> >::iterator iter=_userLoginMap.begin(); iter!=_userLoginMap.end(); ++iter) {
		if( iter->second.size() < 2 ) {
			continue;
		}
		std::bitset<32> bitvec1( iter->second.at(0) );
    std::bitset<32> bitvec2( iter->second.at(1) );
		std::bitset<32> result;
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

void PreloadLoginData::BuildNegtiveUserList() {
	std::cout << "[PreloadLoginData] buildNegtiveUserList _userLoginBitMap size : " << _userLoginBitMap.size() << std::endl;
	if( _userLoginBitMap.empty() ) {
		return;
	}
	for(std::map< int, std::bitset<32> >::iterator iter=_userLoginBitMap.begin(); iter!=_userLoginBitMap.end(); ++iter) {
		std::bitset<NEGTIVEDAYS> tmp1;
		for(int index=0; index<NEGTIVEDAYS; ++index) {
			if( iter->second.test(index) ) {
				tmp1.set( index );
			} else {
				tmp1.reset( index );
			}
		}

		std::bitset<NEGTIVEDAYS> tmp2;
		for(int index=NEGTIVEDAYS; index<2*NEGTIVEDAYS && index<32; ++index) {
			if( iter->second.test(index) ) {
				tmp2.set( index - NEGTIVEDAYS );
			} else {
				tmp2.reset( index - NEGTIVEDAYS );
			}
		}

		std::bitset<NOLOGINDAYS> tmp3;
		for (int index = 0; index < NOLOGINDAYS; ++index) {
			if (iter->second.test(index)) {
				tmp3.set(index);
			} else {
				tmp3.reset(index);
			}
		}
		if( tmp1.count()<2 && tmp2.count()<2 && tmp3.count() == 0) {
			_negtiveUserList.push_back( iter->first );
		}
	}
	std::cout << "[PreloadLoginData] buildNegtiveUserList _negtiveUserList size " << _negtiveUserList.size() << std::endl;
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

	std::map< int, std::vector<int> >::iterator fIt = _userLoginMap.find( id );
	if(fIt != _userLoginMap.end()) {
		fIt->second.at(1) = frequence;
	} else {
		_userLoginMap[id].push_back(0);         //default
		_userLoginMap[id].push_back(frequence);
	}
	return true;
}

}
}
