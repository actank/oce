#include <fstream>
#include <boost/algorithm/string.hpp>
#include "FptnComple.h"
#include "OceCxxAdapter/src/FootprintNAdapter.h"

using namespace std;
using namespace com::xiaonei::xce;
using namespace mop::hi::svc::fptn::dbtrans;
using namespace xce::fptn::adapter;

int main(int argc, char** argv) {


	// 初始化DB连接
	ConnectionPoolManager::instance().initialize();

	// log文件
	ofstream result(RESULT_FILE.c_str(), ios::app);

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

	if ( beginTable >= TABLE_NUM ) {
		cout << time(NULL) << "\tThe begin table " << beginTable << " is invalid\n";
		return 0;
	}

	// 依次取出所有不到6个脚印的用户
	for ( int a1 = 0; a1 < 10; ++a1 ) {
	   for ( int a10 = 0; a10 < 10; ++a10) {
		int tableNum = a10 * 10 + a1;
		int maxId = 0;
		int lastId = 0;
		int totalcount = 0;
		if ( tableNum == beginTable) {
			maxId = beginId;
		}

		do {
			map<int, xce::fptn::FootprintSeq> mapFptn;
			lastId = maxId;

			// 拼凑sql
			Statement sql;
			ostringstream tableStr;
			tableStr << TABLE_FPT << tableNum;
			sql << "SELECT * FROM " << tableStr.str()
				<< " WHERE " << FIELD_HOST << " >= " << maxId << " limit " << RECORD_LIMIT; 

			// 查DB
			try {
				QueryRunner(DESC_FPT, CDbRServer).query(sql, 
					FptnReadHandler(mapFptn, maxId) );
			} catch (Ice::Exception& e) {
				cout << time(NULL) << " [main]DB Connection Exception (" << DESC_FPT << "): " << e.what() << "\n";
			}
			vector<long> reloadHosts;
			for ( map<int, xce::fptn::FootprintSeq>::const_iterator it = mapFptn.begin(); it != mapFptn.end(); ++it ) {
				if ( (*it).second.size() < 6 ) {
					// 查人气
					int visitcount = FootprintNAdapter::instance().getUserVisitCount((*it).first);	
					if ( visitcount <= 20 ) {
						continue;
					} else if ( (visitcount > 0) && (visitcount <= (*it).second.size()) ) {
						continue;
					}
					result << (*it).first << " " << (*it).second.size() << " " << visitcount;
					
					// 记录所有内容
					for ( size_t i = 0; i < (*it).second.size(); ++i ) {
						result << " " << (*it).second[i]->guest << " " << (*it).second[i]->timestamp;
					}
					result << "\n";
				}			
				totalcount++;
			}		

		} while ( maxId > lastId );
	   }
	}
}

// -------------------------------
bool FptnReadHandler::handle(mysqlpp::Row& row) const {

	int host = (int) row[FIELD_HOST.c_str()];
	xce::fptn::FootprintSeq guests = xce::fptn::IntSeqSerialize::unserialize(row[FIELD_GUESTS.c_str()]);
	_mapData.insert(pair<int, xce::fptn::FootprintSeq>(host, guests));
	if ( host > _maxId ) {
		_maxId = host;
	}
	return true;
}
