#include "KeySearchI.h"
#include "QueryRunner.h"
#include "SearchStat.h"
using namespace demo;
using namespace MyUtil;
using namespace com::xiaonei::xce;
void MyUtil::initialize()  {
        ServiceI& service = ServiceI::instance();
        service.getAdapter()->add(&KeySearchManagerI::instance(), service.createIdentity("M", ""));
        TaskManager::instance().scheduleRepeated(KeySearchManagerI::instance().getTimer());
}

KeySearchManagerI::KeySearchManagerI()
{
	_scheduleTimer = new SearchStat(3);
}

KeySearchManagerI::~KeySearchManagerI()
{}

Ice::Int KeySearchManagerI::size(const Ice::Current&)
{
    if(_scheduleTimer)
	return _scheduleTimer->readEntryNum();
    return 0;
}

Ice::Int KeySearchManagerI::lookup(const string& Key,const Ice::Current&)
{

    IceUtil::RWRecMutex::RLock lock(_dataMutex);
    std::map<string,Ice::Int>::iterator p = _dataStorage.find(Key);
    if( p != _dataStorage.end())
        return p->second;
    Statement sql;
    sql << "select * from weight_factor where name = " << mysqlpp::quote << Key ;
    mysqlpp::StoreQueryResult res =  QueryRunner("feed_db", CDbRServer,"weight_factor").store(sql);
    if (!res || res.num_rows()<=0) {
        MCE_WARN("load factor error name="<<Key);
        return -1;
    }
    if(res.num_rows()>1){
        MCE_WARN("the name" << Key << "has reduplicate values");
    }
    mysqlpp::Row row = res.at(0);
    return (Ice::Int)row["factor"];
}

bool KeySearchManagerI::insert(const string& Key, Ice::Int Value,const Ice::Current&)
{
    IceUtil::RWRecMutex::WLock lock(_dataMutex);
    std::map<string,Ice::Int>::iterator p = _dataStorage.find(Key);
    if( p != _dataStorage.end())
    {
        if(p->second == Value)
            return true;
    } 
    _dataStorage[Key] = Value;
    Statement sql;
    sql << "delete from weight_factor where name=" << mysqlpp::quote << Key ;
    QueryRunner("feed_db", CDbWServer,"weight_factor").store(sql);
    sql.clear();
    sql<<"insert into  weight_factor values(" << mysqlpp::quote << Key << "," << Value << ")" ;
    QueryRunner("feed_db", CDbWServer,"weight_factor").store(sql);
    return true;	
}
 
bool KeySearchManagerI::remove(const string& Key,const Ice::Current& )
{
    IceUtil::RWRecMutex::WLock lock(_dataMutex);
    Statement sql;
    sql << "delete  from  weight_factor where name =  " << mysqlpp::quote << Key  ;
    QueryRunner("feed_db", CDbWServer,"weight_factor").store(sql);
    return _dataStorage.erase(Key);
}
