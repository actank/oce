#include <fstream>
#include <stdio.h> 
#include <time.h>
#include <util/cpp/String.h>
#include "SetFpt.h"
#include "DbCxxPool/src/ConnectionPoolManager.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "FootprintNew/src/FootprintNewI.h"
#include "OceCxxAdapter/src/FootprintNAdapter.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::fptn;
using namespace xce::fptn::adapter;

class FootprintHandler : public com::xiaonei::xce::ResultHandler{
public:
	FootprintHandler(const FootprintSetPtr& fsp):_fsp(fsp) 	{
	};
	virtual bool handle(mysqlpp::Row& row) const {
		
		string str = (string) row[COLUMN_GUESTS];
		_fsp->getFootprints() = IntSeqSerialize::unserialize(row[COLUMN_GUESTS]);
		return true;
	};
private:
	friend class FootprintSet;
	FootprintSetPtr _fsp;
};

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

	// 读写参数
	int host = 0;
	string guests = "";
	string type = "";
	if ( (argc >= 3) && (string("get") == argv[1]) ) {
		host = atoi(argv[2]);
		type = "get";
	} else if ( (argc >= 4) ) {
		host = atoi(argv[2]);
		guests = argv[3];
		type = "set";
	} else {
		cout << "setfptn [set] [host] [guestslist] \nor\nsetfptn [get] [host]\n";
		exit(1);
	} 

	// 初始化DB连接
	ConnectionPoolManager::instance().initialize();

	if ( type == "get" ) {
		Statement sql;
		FootprintSetPtr st = new FootprintSet;
		ostringstream table;
		FootprintSeq fpts =  FootprintNAdapter::instance().getAll(host, 0, 20)->fpts;
		/* table << "footprintn_" << host % 100;
		sql << "SELECT * FROM footprintn_" << host%100 << " WHERE host=" << host;
		QueryRunner("footprintn", CDbRServer, table.str()).query(sql, FootprintHandler(st)); 
		FootprintSeq fpts = st->getFootprints(); */
		cout << "footprint of " << host << " :\n"; 
		for ( int i = 0; i < fpts.size(); ++i ) {
			cout << fpts[i]->guest << "," << fpts[i]->timestamp << ",";
		}
		cout << "\n";
	} else if ( type == "set" ) {
		// 取得id
		vector<string> segs = StrUtil::split(guests, ",");
		FootprintSeq fpts;
		for ( int i = 0; i < segs.size() / 2; ++i ) {
			FootprintTinyPtr tiny = new FootprintTiny();
			tiny->guest = atoi(segs[2*i].c_str());
			tiny->timestamp = atoi(segs[2*i+1].c_str());
			fpts.push_back(tiny);
		}
		Statement sql;
		sql << "UPDATE footprintn_" << host%100 << " SET guests = "
		       << mysqlpp::quote << IntSeqSerialize::serialize(fpts) << " WHERE host = " << host;
		QueryRunner("user_footprint", CDbWServer).execute(sql);
		FootprintNAdapter::instance().reloadCache(FOOTPRINT_SET, host);
		cout << "Set hosts of " << host << " complete!\n";

	}
}
