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
using namespace xce::fptn;

const int TABLE_NUM = 100;
const string TABLE_NEW = "footprintnew_";
const string FIELD_HOST_NEW = "host";
const string FIELD_GUESTS_NEW = "guests";
const int RECORD_LIMIT  = 10000;	// 一次读取的记录数

class FptData : public Ice::Object {
public:
	int guest;
	int timestamp;
};

typedef IceUtil::Handle<FptData> FptDataPtr;

bool reverseByTime(const FptDataPtr& f1, const FptDataPtr& f2) {
	return f1->timestamp > f2->timestamp;
};

// --------------------
class FptnReadHandler : public com::xiaonei::xce::ResultHandler{
public:
	FptnReadHandler(map<int, vector<FootprintTinyPtr> >& mapData, int& maxId):
		_mapData(mapData),
		_maxId(maxId) {
	}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	map<int, vector<FootprintTinyPtr> >& _mapData;
	int& _maxId;
};

}
}
}
}
}

#endif
