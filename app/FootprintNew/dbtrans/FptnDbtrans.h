#ifndef __FPT_DBTRANS_H__
#define __FPT_DBTRANS_H__

#include "FootprintNew/replica/FootprintNewI.h"
#include "IceExt/src/ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"

namespace mop {
namespace hi {
namespace svc {
namespace fptn {
namespace dbtrans {

using namespace std;

const int TABLE_NUM_OLD = 100;
const string TABLE_OLD = "footprint_";
const string FIELD_HOST_OLD = "host";
const string FIELD_GUEST_OLD = "guest";
const string FIELD_TIME_OLD = "time";
const int TABLE_NUM_NEW = 100;
const string TABLE_NEW = "footprintnew_";
const string FIELD_HOST_NEW = "host";
const string FIELD_GUESTS_NEW = "guests";
const int RECORD_LIMIT  = 10000;	// 一次读取的记录数

const string BREAK_POINT_FILE = "breakpoint.dat";
const string END_FLAG_FILE = "endflag.txt";
const string LOG_FILE = "log.txt";

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

class OldNewSerialize {
public:
   static string printByte(const mysqlpp::String& str) {
	   ostringstream os;
	   for ( size_t i = 0; i < str.size(); ++i) {
		   int k = str.at(i);
		   os << k << " ";
	   }
	   return os.str();
   }

   static string printByte(const string& str) {
	   ostringstream os;
	   for ( size_t i = 0; i < str.size(); ++i) {
		   int k = str.at(i);
		   os << k << " ";
	   }
	   return os.str();
   }

};

class FootprintReadHandler : public com::xiaonei::xce::ResultHandler{
public:
	FootprintReadHandler(map<int, FptSetPtr>& mapData, int& maxId, int& getNum):
		_mapData(mapData),
		_maxId(maxId),
		_getNum(getNum)	{
	}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	map<int, FptSetPtr>& _mapData;
	int& _maxId;
	int& _getNum;
};


}
}
}
}
}

#endif
