#include "Recorder.h"
#include "IceLogger.h"
using namespace std;
using namespace xce::feed;
void Recorder::addDispatch(int stype){
	IceUtil::Mutex::Lock lock(_mutex);
	++(_data[stype].dispatchCount);
}

void Recorder::addReply(int stype){
	IceUtil::Mutex::Lock lock(_mutex);
	++(_data[stype].replyCount);
}

void Recorder::addDeliver(int stype, int count){
	IceUtil::Mutex::Lock lock(_mutex);
	_data[stype].deliverCount += count;
}

void Recorder::handle(){
	Statement sql;
	stype2Object temp;
	stype2Object::iterator rit;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		temp.swap(_data);
	}
	if(temp.empty()){
		return;
	}
	MCE_INFO("Recorder::handle --> insert into db");
	sql << "insert into " << DB_TABLE << "(stype, dispatchcount, replycount, tousercount) values";
        for(rit = temp.begin(); rit != temp.end(); ++rit){
		if(rit != temp.begin()){
			sql << ", ";
		}
                sql << "(" << rit->first << ", " << rit->second.dispatchCount << ", " << rit->second.replyCount << ", " << rit->second.deliverCount << ")";
        }
	sql << ";";
	QueryRunner(DB_INSTANCE, CDbWServer, DB_TABLE).store(sql);
}
