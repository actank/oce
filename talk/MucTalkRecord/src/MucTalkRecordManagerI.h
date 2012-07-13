#ifndef __MUCTALKRECORD_H__
#define __MUCTALKRECORD_H__
#include <map>
#include <string>
#include <vector>
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Thread.h>
#include "MucTalkRecord.h"
#include "ServiceI.h"
#include "Singleton.h"

namespace com{
namespace xiaonei{
namespace talk{
namespace muc{

using namespace std;
using namespace MyUtil;

const int poolsize = 100000;
const int AG = 0;
const int MESSAGESIZE = 10;
const string DB_INSTANCE = "minigroup";
const int DBCLUSTERSIZE = 100;

//string GetTableName(int groupid);

class ActiveGroupFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};


class ActiveGroup : public Ice::Object{
public:
	ActiveGroup(int groupid) : _groupid(groupid), _pos(0){
	};
	MucTalkMessageSeq GetMessage();
	void PushMessage(const MucTalkMessagePtr& msg);
private:
	int _groupid;
	int _pos;
	MucTalkMessageSeq _messages;
};
typedef IceUtil::Handle<ActiveGroup> ActiveGroupPtr;


class DBWriter : public MyUtil::Timer{
public:
	DBWriter(int table_index) : _table_index(table_index), Timer(10 * 1000){
	}
	virtual void handle();
	void WriteDB(const MucTalkMessagePtr& msg);
private:
	MucTalkMessageSeq _messages;
	IceUtil::Mutex _mutex;
	int _table_index;
};
typedef IceUtil::Handle<DBWriter> DBWriterPtr;


class MucTalkRecordManagerI: public MucTalkRecordManager, public Singleton<MucTalkRecordManagerI>{
public:
	MucTalkRecordManagerI();
	void init();
	void RecordTalkMsg(int userid, int groupid, const string& msg, const Ice::Current& = Ice::Current());
	MucTalkMessageSeq GetRecentlyMsg(int groupid, const Ice::Current& = Ice::Current());
private:
	IceUtil::Mutex _mutex[poolsize];
	vector<DBWriterPtr> _db_writers;
};

class TimeStat{
	timeval _begin, _end;
	public:
	TimeStat(){
		reset();
	}
	void reset(){
		gettimeofday(&_begin, NULL);
	}
	float getTime(){
		gettimeofday(&_end, NULL);
		float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
			-_begin.tv_usec;
		timeuse/=1000;
		return timeuse;
	}
};


};
};
};
};

#endif
