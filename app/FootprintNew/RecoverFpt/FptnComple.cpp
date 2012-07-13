#include <fstream>
#include <stdio.h> 
#include <time.h>
#include <util/cpp/String.h>
#include "FptnComple.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "FootprintNew/src/FootprintNewI.h"
#include "OceCxxAdapter/src/FootprintNAdapter.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::fptn;
using namespace xce::fptn::adapter;

time_t timeTrans(const string& date, const string& time) {
	
	vector<string> dateSeg = StrUtil::split(date, "-");
	vector<string> timeSeg = StrUtil::split(time, ":");
	if ( (dateSeg.size() < 3) || (timeSeg.size() < 3) ) {
		return 0;
	}
	struct tm time_s;
	time_s.tm_year = atoi(dateSeg[0].c_str())-1900;
	time_s.tm_mon = atoi(dateSeg[1].c_str()) - 1;
	time_s.tm_mday = atoi(dateSeg[2].c_str());
	time_s.tm_hour = atoi(timeSeg[0].c_str());
	time_s.tm_min = atoi(timeSeg[1].c_str());
	time_s.tm_sec = atoi(timeSeg[2].c_str());
	time_t t = mktime(&time_s);
	return t;
}

bool descByTime(const FootprintTinyPtr& fpt1, const FootprintTinyPtr& fpt2) {
	return ( fpt1->timestamp > fpt2->timestamp);
}

FootprintSeq topSix(FootprintSeq& fptAll) {


	// 排序
	sort(fptAll.begin(), fptAll.end(), descByTime);

	// 去重/选取
	FootprintSeq result;
	set<int> guests;
	for ( size_t i = 0; i < fptAll.size(); ++i ) {

		if ( result.size() >= 6 ) {
			break;
		}
		if ( guests.find(fptAll[i]->guest) != guests.end() ) {
			continue;
		}
		result.push_back(fptAll[i]);
		guests.insert(fptAll[i]->guest);
	}
	return result;
}

int main(int argc, char** argv) {

	// 初始化DB连接
	ConnectionPoolManager::instance().initialize();

	// 读取列表
	ifstream ifsList("merge.08");
	string line;
	while ( getline(ifsList, line) ) {

		// 列表
		FootprintSeq fptAll, fptSix;

		// 取得id
		vector<string> segs = StrUtil::split(line, "\t");
		if ( segs.size() <=0 ) {
			cout << "Ivalid line : " << line << endl;
			continue;
		}
		int host = atoi(segs[0].c_str());

		// 取得log中的脚印列表
		if ( segs.size() <= 1 ) {
			cout << "host " << host << " with no fpt in log" << endl;
			continue;
		}
		for ( size_t i = 1; i < segs.size(); ++i ) {
			vector<string> params = StrUtil::split(segs[i], " ");
			if ( params.size() < 4 ) {
				continue;
			}
			FootprintTinyPtr fpt = new FootprintTiny;
			fpt->guest = atoi(params[2].c_str());
			fpt->timestamp = timeTrans(params[0], params[1]);
			fptAll.push_back(fpt);
			
		}

		// 取得现在服务中的脚印列表		
		FootprintSeq fptCur = xce::fptn::adapter::FootprintNAdapter::instance().getAll(host, 0, 6)->fpts;
		for ( size_t i = 0; i < fptCur.size(); ++i ) {
			fptAll.push_back(fptCur[i]);
		}

		// 排序&找出前6个
		fptSix = topSix(fptAll);

		// 序列化
		string serialStr = IntSeqSerialize::serialize(fptSix);

		// 写库
		Statement sql;
		sql << "UPDATE footprintn_" << host%100 << " SET guests = "
		       << mysqlpp::quote << serialStr.data() << " WHERE host = " << host;
		try {
			QueryRunner("user_footprint", CDbWServer).execute(sql);
		} catch (Ice::Exception& e) {
			cout << host << "\t" << time(NULL) << " [write_sql] Ice::Exception : " << e.what() << "\n";
			continue;
		} catch (mysqlpp::Exception& e) {
			cout << host << "\t" << time(NULL) << " [write_sql] mysqlpp::exception : " << e.what() << endl;
			continue;
		} catch (...) {
			cout << host << "\t" << time(NULL) << " [write_sql] UNKNOWN exception" << endl;
			continue;
		}

		// reload ObjectCache
		try {
			FootprintNAdapter::instance().reloadCache(FOOTPRINT_SET, host);
			cout << host << " reload ok " << endl;
		} catch (Ice::Exception& e ) {
			cout << "[WARN] reload " << host << " Ice::Exception " << e.what() << endl;
		} catch (...) {
			cout << "[WARN] reload " << host << " UNKNOWN exception" << endl;
		}
	}
}
