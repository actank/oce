/*
 * =====================================================================================
 *
 *       Filename:  UserActive.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年12月08日 17时34分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */
#include "getopt.h"
#include "UserActive.h"
#include "UserStateAdapter.h"
#include "util/cpp/String.h"

using namespace std;
using namespace MyUtil;
using namespace xce::useractive;
using namespace com::xiaonei::xce;
using namespace xce::adapter::userstate;

//**************************************************LoginAnalyzer**************************************************************
void LoginAnalyzer::analyze(std::map<int, int>& grade, time_t now) {
	string TableName = "user_login_";
	char year[10];
	char month[10];
	char date[10];

	strftime(year, sizeof(year), "%Y", localtime(&now));
	strftime(month, sizeof(month), "%m", localtime(&now));
	strftime(date, sizeof(date), "%d", localtime(&now));

	string currentFile = TableName + string(year) + string(month);

	time_t last = now - (atoi(date) + 3) * 24 * 3600;        //上个月份数据表
	strftime(year, sizeof(year), "%Y", localtime(&last));
	strftime(month, sizeof(month), "%m", localtime(&last));
	strftime(date, sizeof(date), "%d", localtime(&last));

	string lastFile = TableName + string(year) + string(month);

	cout << "[LoginAnalyzer] analyze firstFile(" << currentFile << ") secondFile(" << lastFile << ")" << endl;
	
	loadCurrentTable( currentFile );
	loadLastTable( lastFile );

	buildBitMap();               //拼接最近30天数据
	buildUserActive(LOGIN_SCOPE, _loginDays, grade);
}


void LoginAnalyzer::loadCurrentTable(const std::string& tableName) {
	cout << "[LoginAnalyzer] loadCurrentTable(" << tableName << ")" << endl;
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
			queryCount = QueryRunner("friendfinder_userlogin", CDbRServer).query( sql, CurrentLoginResultHandler(last, _userLoginMap));
		} catch (Ice::Exception& e) {
			cout << time(NULL) << "[LoginAnalyzer] loadCurrentTable " << tableName << " Connection Exception : " << e.what() << "\n" << endl;
		} catch (...) {
			cout << time(NULL) << "[LoginAnalyzer] loadCurrentTable DB unknow Exception" << endl;
		}
		//cout << "[UserActive] loadCurrentTable queryCount " << queryCount << endl;
		if(last > lowboundary) {
			lowboundary = last;
		} else if (queryCount <= 0) {
			cout << "[LoginAnalyzer] loadCurrentTable lowboundary(" << lowboundary << ") at end of " << tableName << endl;
			break;
		}
	} while(true);
	cout << "1 _userLoginMap size : " << _userLoginMap.size() << endl;
}

void LoginAnalyzer::loadLastTable(const std::string& tableName) {
	cout << "[LoginAnalyzer] loadLastTable(" << tableName << ")" << endl;
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
			queryCount = QueryRunner("friendfinder_userlogin", CDbRServer).query( sql, LastLoginResultHandler(last, _userLoginMap) );
		} catch (Ice::Exception& e) {
			cout << time(NULL) << "[LoginAnalyzer] loadLastTable " << tableName << " Connection Exception : " << e.what() << endl;
		} catch (...) {
			cout << time(NULL) << "[LoginAnalyzer] loadLastTable DB unknow Exception" << endl;
		}
		
		if(last > lowboundary) {
			lowboundary = last;
		} else if (queryCount <= 0) {
			cout << "[LoginAnalyzer] loadLastTable lowboundary(" << lowboundary << ") at end of " << tableName << endl;
			break;
		}
	} while(true);
	cout << "2 _userLoginMap size : " << _userLoginMap.size() << endl;
}

bool CurrentLoginResultHandler::handle(mysqlpp::Row& row) const {
	_last = _last > (int)row["id"] ? _last : (int)row["id"];

	int id = (int)row["id"];
	int frequence = (int)row["frequence"];

	_userLoginMap[id].push_back(frequence);
	_userLoginMap[id].push_back(0);
	return true;
}

bool LastLoginResultHandler::handle(mysqlpp::Row& row) const {
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

void LoginAnalyzer::buildBitMap() {
	cout << "[LoginAnalyzer] buildBitMap" << endl;

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

void LoginAnalyzer::buildUserActive(int limit_days, int active_days, map<int, int>& grade) {
	cout << "[LoginAnalyzer] buildUserActive begin _userLoginBitMap size : " << _userLoginBitMap.size() << " grade size: " << grade.size() << endl;
	if( _userLoginBitMap.empty() ) {
		return;
	}

	for(map< int, bitset<32> >::iterator iter=_userLoginBitMap.begin(); iter!=_userLoginBitMap.end(); ++iter) {
		int count = 0;
		for(int index=0; index <= limit_days; ++index) {
			if( iter->second.test(index) ) {
				count ++;
			}
		}
		if (count >= active_days) {
			grade[iter->first] = 0;
		} 
	}
	cout << "[LoginAnalyzer] buildUserActive end _userLoginBitMap size : " << _userLoginBitMap.size() << " grade size: " << grade.size() << endl;
}
//**************************************************UgcAnalyzer**************************************************************
void UgcAnalyzer::analyze(map<int, int>& grade, time_t now) {
	cout << "[UgcAnalyzer] analyze, grade size: " << grade.size() << endl;
	fstream file;
	file.open(getDataFilePath().c_str(),ios::in);
	if (!file) {
		cout << "[UgcAnalyzer] analyze, dateFilePath " << getDataFilePath() << " is not find!" << endl;
		for (int day = 1; day <= UGC_SCOPE; day ++) {
			time_t time = now - day * 24 * 3600;
			string fileDate = getFileDate(time);

			string filePath = getLogFilePath(fileDate);
			cout << "[UgcAnalyzer] analyze, logFilePath " << filePath << endl;
			loadFromLogFile(filePath, grade, false);
		}
	} else {
		cout << "[UgcAnalyzer] analyze, dateFilePath " << getDataFilePath() << " is find!" << endl;
		loadDataFromFile(grade);

		map<int, int> todayGrade;
		map<int, int> lastGrade;

		time_t time;
		string fileDate;

		time = now - 1 * 24 * 3600;
		fileDate = getFileDate(time);

		string todayFilePath = getLogFilePath(fileDate);
		cout << "[UgcAnalyzer] analyze, todayFilePath " << todayFilePath << endl;
		loadFromLogFile(todayFilePath, todayGrade, true);

		time = now - (UGC_SCOPE + 1) * 24 * 3600;
		fileDate = getFileDate(time);

		string lastFilePath = getLogFilePath(fileDate);
		cout << "[UgcAnalyzer] analyze, lastFilePath" << lastFilePath << endl;
		loadFromLogFile(lastFilePath, lastGrade, true);

		for(map<int, int>::iterator it = todayGrade.begin(); it != todayGrade.end(); ++it) {
			map<int, int>::iterator ig = grade.find(it->first);
			
			if (ig == grade.end()) {
				grade[it->first] = it->second;
			} else {
				grade[it->first] += it->second;
			}
		}

		for(map<int, int>::iterator il = lastGrade.begin(); il != lastGrade.end(); ++il) {
			map<int, int>::iterator ig = grade.find(il->first);
			
			if (ig != grade.end()) {
				grade[il->first] -= il->second;
			} else {
				cout << "WARN id: " << il->first << " not find in file: " << lastFilePath << endl;
			}
		}

	}


	dumpDataToFile(grade);
}

void UgcAnalyzer::loadDataFromFile(map<int, int>& lastGrade) {
	ifstream ifile(getDataFilePath().c_str());
	if(!ifile)
		return;

	string log;
	vector<string> split;
	while (getline(ifile, log)) {
		split = MyUtil::StrUtil::split(log, " ");
		if (split.size() != 2) continue;
		int id = atoi(split[0].c_str());
		int score = atoi(split[1].c_str());
		lastGrade[id] = score;
	}

	ifile.close();
}

void UgcAnalyzer::dumpDataToFile(map<int, int>& currentGrade) {
	cout << "[UgcAnalyzer] dumpDataToFile, currentGrade " << currentGrade.size() << endl;
	ofstream ofile(getDataFilePath().c_str());
	if(!ofile) return;

	for(map<int, int>::iterator it = currentGrade.begin(); it != currentGrade.end(); ++it) {
		if (0 < it->second){
			ofile << it->first << " " << it->second << endl;
		}
	}
	ofile << "end!" << endl;
	ofile.close();
}

string UgcAnalyzer::getFileDate(time_t time) {
		char year[10];
		char month[10];
		char date[10];

		strftime(year, sizeof(year), "%Y", localtime(&time));
		strftime(month, sizeof(month), "%m", localtime(&time));
		strftime(date, sizeof(date), "%d", localtime(&time));
		string fileDate = string(year) + string(month) + string(date);
		return fileDate;
}

string UgcAnalyzer::getLogFilePath(string fileDate) {
	return "../data/ugc_cnt_daily_" + fileDate;
}

string UgcAnalyzer::getDataFilePath() {
	return "../log/ugc_data.log";
}

void UgcAnalyzer::loadFromLogFile(string filePath, map<int, int>& grade, bool ignore) {
	cout << "[UgcAnalyzer] loadGradeFromLogFile filePath " << filePath << endl;
	ifstream ifile(filePath.c_str());
	if(!ifile) {
		cout << "[UgcAnalyzer] loadGradeFromLogFile filePath " << filePath << " is not find!" << endl;
		return;
	}

	string log;
	vector<string> split;
	while(getline(ifile, log)) {
		split = MyUtil::StrUtil::split(log, "'\t'");
		if (split.size() != 3) continue;
		int id = atoi(split[0].c_str());
		map<int, int>::iterator it = grade.find(id);
		string type;
		int count;
		if (ignore || it != grade.end()) {
			type = split[1];
			count = atoi(split[2].c_str());

			int score = getScore(type, count);
			grade[id] += score;
		}
	}
	ifile.close();
}

int UgcAnalyzer::getScore(string type, int count) {
	if ("blog" == type) {
		return BLOG_WEIGHT * count;
	} else if ("doing" == type) {
		return DOING_WEIGHT * count;
	} else if ("gossip" == type) {
		return GOSSIP_WEIGHT * count;
	} else if ("photo" == type) {
		return PHOTO_WEIGHT * count;
	} else if ("share" == type) {
		return SHARE_WEIGHT * count;
	}

	return 0;
}
//**************************************************UserActive**************************************************************
void UserActive::setActive(map<int, int>& grade, float percent) {
	map<int, bool> currentActive;
	map<int, bool> lastActive;

	getCurrentMap(grade, percent, currentActive);
	fileToMap(lastActive);
	cout << "[UserActive] setActive, currentActive.size() " << currentActive.size() << " lastActive.size() " << lastActive.size() << endl;
	
	for(map<int, bool>::iterator ic = currentActive.begin(); ic != currentActive.end(); ++ic) {
		try {
			map<int, bool>::iterator il = lastActive.find(ic->first);
			if ((il != lastActive.end() && ic->second != il->second) || il == lastActive.end()) {
				if (ic->second) {
					UserStateAdapter::instance().setActiveOn(ic->first);
					cout << "[UserActive] setActiveOn " << ic->first << endl;
				} else {
					UserStateAdapter::instance().setActiveOff(ic->first);
					cout << "[UserActive] setActiveOff " << ic->first << endl;
				}
			}
		} catch (Ice::Exception& e) {
			cout << "UserStateAdapter, id " << ic->first  << " exception " << e.what() << endl;
			continue;
		}
	}

	for(map<int, bool>::iterator il = lastActive.begin(); il != lastActive.end(); ++il) {
		try {
			map<int, bool>::iterator ic = currentActive.find(il->first);
			if (il == lastActive.end()) {
				UserStateAdapter::instance().setActiveOff(ic->first);
				cout << "[UserActive] setActiveOff " << ic->first << endl;
			}
		} catch (Ice::Exception& e) {
			cout << "UserStateAdapter, id " << il->first << " exception " << e.what() << endl;
			continue;
		}
	}

	cout << "[UserActive] setActive end, currentActive.size() " << currentActive.size() << " lastActive.size() " << lastActive.size() << endl;
	mapToFile(currentActive);
}

void UserActive::getCurrentMap(map<int, int>& grade, float percent, map<int, bool>& currentActive) {
	cout << "[UserActive] getCurrentMap, grade.size() " << grade.size() << " percent " << 
		percent << " currentActive.size() " << currentActive.size() << endl;

	vector<Entry> list;
	for(map<int, int>::iterator it = grade.begin(); it != grade.end(); ++it) {
		Entry entry(it->second);
		vector<Entry>::iterator ie = lower_bound(list.begin(), list.end(), entry, greater_entry());
		if (ie != list.end() && it->second == ie->grade) {
			ie->ids.push_back(it->first);
		} else {
			entry.ids.push_back(it->first);
			list.insert(ie, entry);
		}
	}

	int enrollNum = (int)grade.size() * percent;
	int enrollCount = 0;
	for(vector<Entry>::iterator iv = list.begin(); iv != list.end(); ++iv) {
	//	cout << "[UserActive] getCurrentMap, setMap, grade: " << iv->grade << " ids.size: " << iv->ids.size() << endl;
		for(deque<int>::iterator id = iv->ids.begin(); id != iv->ids.end(); ++ id) {
			if (enrollCount <= enrollNum) {
				//cout << "[UserActive] getCurrentMap setMap, grade: " << iv->grade << " id: " << *id << endl;
				currentActive[*id] = true;
				enrollCount ++;
			} else {
				currentActive[*id] = false;
			}
		}
	}
}

void UserActive::fileToMap(map<int, bool>& lastActive) {

	ifstream ifile(getFilePath().c_str());
	if(!ifile) {
		return;
	}

	string log;
	vector<string> split;
	while (getline(ifile, log)) {
		
		split = MyUtil::StrUtil::split(log, " ");
		if (split.size() != 2) continue;
		string active = split[0];
		int id = atoi(split[1].c_str());

		if (LOG_ACTIVE_ON == active) {
			lastActive[id] = true;
		} else if (LOG_ACTIVE_OFF == active) {
			lastActive[id] = false;
		}
	}

	ifile.close();
}

void UserActive::mapToFile(map<int, bool>& active) {
	cout << "[UserActive] mapToFile, file " << getFilePath() << " active.size() " << active.size() << endl;

	ofstream ofile(getFilePath().c_str());
	if(!ofile) return;

	for(map<int, bool>::iterator it = active.begin(); it != active.end(); ++it) {
		if(it->second) {
			ofile << LOG_ACTIVE_ON << " " << it->first << endl;
		} else {
			ofile << LOG_ACTIVE_OFF << " " << it->first << endl;
		}
	}

	ofile.close();
}

string UserActive::getFilePath() {
	return "../log/user_active.log";
}
//**************************************************main**************************************************************

int main(int argc, char* argv[]) {

	const char *short_opts = "l:u:";
	const struct option long_opts[] = {
		{"loginDays", required_argument, NULL, 'l'},
		{"ugcPercent", required_argument, NULL, 'u'},
		{NULL, 0, NULL, 0}
	};
	
	int c;
	int loginDays;
	float ugcPercent = 0;
	
	while ((c = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1) {
		switch(c) {
			case 'l':
				loginDays = atoi(optarg);
				break;
			case 'u':
				ugcPercent = atof(optarg);
				break;
			default:
				break;
		}
	}
	
	cout << time(0) << " UserActive analyze begin!" << " login_days " << loginDays << " ugc_percent " <<  ugcPercent << endl;
	ConnectionPoolManager::instance().initialize();
	
	std::map<int, int> grade;
	time_t now = time(0);
	AnalyzerPtr loginAnalyzer = new LoginAnalyzer(loginDays);
	loginAnalyzer->analyze(grade, now);

	
	AnalyzerPtr ugcAnalyzer = new UgcAnalyzer(ugcPercent);
	ugcAnalyzer->analyze(grade, now);
	
	float gradePercent = ugcPercent;
	UserActivePtr active = new UserActive();
	active->setActive(grade, gradePercent);
	
	cout << time(0) << " UserActive analyze end!" << " login_days " << loginDays << " ugc_percent " <<  ugcPercent << endl;
	
	exit(0);
}
