#include <fstream>
#include <boost/algorithm/string.hpp>
#include "FptnDbtrans.h"

using namespace std;
using namespace com::xiaonei::xce;
using namespace mop::hi::svc::fptn::dbtrans;

int main(int argc, char** argv) {


	// 初始化DB连接
	ConnectionPoolManager::instance().initialize();

	// log文件
	ofstream log(LOG_FILE.c_str(), ios::app);
	// ofstream log2("a.txt");
	// log2.close();
	ofstream loge("e.txt", ios::app);
	loge << "Table\t\tmaxId\n";
	loge.close();

	// 解析起始点
	int beginTable = 0;
	int beginId = 0;
	ifstream ifsBreak;
	ifsBreak.open(BREAK_POINT_FILE.c_str());
	if ( ifsBreak.is_open() ) {
		string tmp;
		vector<string> param;
		while (getline(ifsBreak, tmp, '\t')) {
			param.push_back(tmp);
		}
		if ( param.size() >= 2 ) {
			try {
				beginTable = atoi(param[0].c_str());
				beginId = atoi(param[1].c_str());
			} catch (std::exception& e) {
				cout << "[main]breakpoint excepiton : " << e.what() << endl;
			}
		} else {
			cout << "[main] no breakpoint file : " << tmp << "\n";
		}
		ifsBreak.close();
	}

	if ( beginTable >= TABLE_NUM_OLD ) {
		log << time(NULL) << "\tThe begin table " << beginTable << " is invalid\n";
		return 0;
	}

	// 依次转换、导库
	// for ( int tableNum = 0; tableNum < 1; ++tableNum ) {
	for ( int tableNum = beginTable; tableNum < TABLE_NUM_OLD; ++tableNum ) {

		map<int, FptSetPtr> mapData;
		int maxId = 0;
		int getNum = 0;
		int totalcount = 0;
		if ( tableNum == beginTable) {
			maxId = beginId;
		}

		do {

			// 拼凑sql
			Statement sql;
			ostringstream sqltmp;
			ostringstream tableStr;
			tableStr << TABLE_OLD << tableNum;
			sqltmp << "SELECT " << FIELD_HOST_OLD << "," << FIELD_GUEST_OLD << "," << FIELD_TIME_OLD 
				<< " FROM " << tableStr.str()
				<< " WHERE " << FIELD_HOST_OLD << " >= " << maxId << " limit " << RECORD_LIMIT; 
			sql << sqltmp.str();
			getNum = 0;
			log << time(NULL) << " [read_sql]" << sqltmp.str() << "\n";

			// 查旧数据库
			try {
				QueryRunner("user_footprint", CDbRServer).query(sql, 
					FootprintReadHandler(mapData, maxId, getNum) );
			} catch (Ice::Exception& e) {
				log << time(NULL) << " [main]DB Connection Exception : " << e.what() << "\n";
			}
			log << time(NULL) << " [getNum]" << getNum << "\n";


			// 生成写入的mysql
			Statement sqlw;
			ostringstream sqltmpw;
			ostringstream theTable;
			theTable << TABLE_NEW << tableNum;
			sqlw << "REPLACE INTO " << theTable.str() << " (host, guests) values ";
			sqltmpw << "REPLACE INTO " << theTable.str() << " (host, guests) values ";
			int count = 0;
			for ( map<int, FptSetPtr>::iterator it = mapData.begin(); it != mapData.end(); ++it ) {

				if ( ((*it).first == maxId) && (getNum > 0) ) {
					continue;
				}
				vector<FptDataPtr> fpts((*it).second->fptDatas.begin(), (*it).second->fptDatas.end());
				sort(fpts.begin(), fpts.end(), reverseByTime);
				// 生成FootprintSeq
				mop::hi::svc::fptn::FootprintSeq fptnSeq;
				log << time(NULL) << " [" << (*it).first << "]\t";
				for ( size_t i = 0; i < fpts.size(); ++i ) {
					log << fpts[i]->guest << " ";
					fptnSeq.push_back(fpts[i]->guest);
				}
				// 写入sql;
				if ( count > 0 ) {
					sqlw << ",";
					sqltmpw << ",";
				}
				sqlw << "(" << (*it).first << ", " << mysqlpp::quote << mop::hi::svc::fptn::IntSeqSerialize::serialize(fptnSeq)<<")";
				sqltmpw << "(" << (*it).first << ", " << mysqlpp::quote << mop::hi::svc::fptn::IntSeqSerialize::serialize(fptnSeq)<<")";
				log << "\n";
				count ++;
				totalcount ++;

			}
			// log << time(NULL) << " [write_sql]" << sqltmpw.str() << "\n";
			// 写新数据库
			log << time(NULL) << " [Table] " << theTable.str() << "\n";
			if ( count > 0 ) {
				// QueryRunner("footprintnew", CDbWServer, theTable.str()).execute(sqlw);
			
				try {
					QueryRunner("footprintnew", CDbWServer).execute(sqlw);
				} catch (Ice::Exception& e) {
					log << time(NULL) << " [write_sql] exception : " << e.what() << "\n";
				} catch (...) {
					log << time(NULL) << " [write_sql] exception : " << theTable.str() << " maxId " << maxId << "\n";
				}
			}

			// erase该组数据
			for ( map<int, FptSetPtr>::iterator it = mapData.begin(); it != mapData.end(); ++it ) {

				if ( ((*it).first == maxId) && (getNum > 0) ) {
					continue;
				}
				mapData.erase(it);
			}
			cout << "[table] : " << theTable.str() << " [write count] : " << count << " [mapData] size : " << mapData.size() << "\n";
			if ( count <= 0 ) {
				maxId++;
			}

			// 写入新的断点
			ofstream ofsBreak(BREAK_POINT_FILE.c_str());
			ofsBreak << tableNum << "\t" << maxId;

			// 如果有结束标志,则停止
			ifstream ifsFlag;
			ifsFlag.open(END_FLAG_FILE.c_str());
			if ( ifsFlag.is_open() ) {
				string tmp;
				vector<string> param;
				ifsFlag >> tmp;
				if ( tmp.find("1") != string::npos ) {
					log << time(NULL) << " Finish with " << maxId << "\n";
					ifsFlag.close();
					return 0;
				}
			}
		} while ( getNum > 0 );

		// 记录各表结束点
		ofstream loge2("e.txt", ios::app);
		loge2 << TABLE_OLD << tableNum << "\t" << maxId << "\t" << totalcount << "\n";
		loge2.close();
	}
}

bool FootprintReadHandler::handle(mysqlpp::Row& row) const {

	FptDataPtr data = new FptData;
	int host = (int) row[FIELD_HOST_OLD.c_str()];
	data->guest = (int) row[FIELD_GUEST_OLD.c_str()];
	data->timestamp = (string) row[FIELD_TIME_OLD.c_str()];

	map<int, FptSetPtr>::iterator it = _mapData.find(host);
	if ( it != _mapData.end() ) {
		if ( it->second->guests.find(data->guest) != it->second->guests.end() ) {
			return true;
		} else {
			it->second->fptDatas.insert(data);
			it->second->guests.insert(data->guest);
		}		
	} else {
		FptSetPtr seq = new FptSet;
		seq->host = host;
		seq->fptDatas.insert(data);
		seq->guests.insert(data->guest);
		_mapData.insert( pair<int, FptSetPtr>(host, seq) );
	}
	if ( host > _maxId ) {
		_maxId = host;
	}
	_getNum++;
	/* ofstream log2("a.txt", ios::app);
	log2 << host << "\n";
	log2.close(); */

	return true;
}
