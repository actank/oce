#ifndef __FPT_DBTRANS_H__
#define __FPT_DBTRANS_H__

#include "FootprintNew/src/FootprintNewI.h"
#include "IceExt/src/ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"

namespace mop {
namespace hi {
namespace svc {
namespace fptn {
namespace dbtrans {

using namespace std;

const int TABLE_NUM = 100;
const string DESC_FPT = "user_footprintn";
const string TABLE_FPT = "footprintn_";
const string FIELD_HOST = "host";
const string FIELD_GUESTS = "guests";
const int RECORD_LIMIT  = 10000;	// 一次读取的记录数

const string BREAK_POINT_FILE = "breakpoint.dat";
const string END_FLAG_FILE = "endflag.txt";
const string RESULT_FILE = "result.txt";

class FptData : public Ice::Object {
public:
	int guest;
	string timestamp;
};

typedef IceUtil::Handle<FptData> FptDataPtr;

class FptSet : public Ice::Object {
public:
	int host;
	set<FptDataPtr> fptDatas;
	set<int> guests;
};

typedef IceUtil::Handle<FptSet> FptSetPtr;

bool reverseByTime(const FptDataPtr& f1, const FptDataPtr& f2) {
	return f1->timestamp > f2->timestamp;
};

class FptnReadHandler : public com::xiaonei::xce::ResultHandler{
public:
	FptnReadHandler(map<int, xce::fptn::FootprintSeq>& mapData, int& maxId):
		_mapData(mapData),
		_maxId(maxId) {
	}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	map<int, xce::fptn::FootprintSeq>& _mapData;
	int& _maxId;
};


}
}
}
}
}

#endif
