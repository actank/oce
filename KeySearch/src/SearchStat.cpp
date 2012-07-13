#include "SearchStat.h"
#include "QueryRunner.h"
//#include "ServiceI.h"

using namespace demo;
using namespace MyUtil;
using namespace com::xiaonei::xce;
void SearchStat::handle()
{
    MCE_INFO("SearchStatr::handle() started.");
    try {
	setEntryNum();
    } catch (...) {
        MCE_INFO("Set Entry Num error");
    }
    MCE_DEBUG("Entry num is "<<_entryNum);
}
void SearchStat::setEntryNum()
{
    Ice::Int num = 0;
    Statement sql;
    sql << "select count(*) as entryNum from weight_factor" ;
    mysqlpp::StoreQueryResult res =  QueryRunner("feed_db", CDbRServer,"weight_factor").store(sql);
    if (!res || res.num_rows()<=0) {
        MCE_WARN("db search error");
        num = 0;
    }
    mysqlpp::Row row = res.at(0);
    num = (Ice::Int)row["entryNum"];
    IceUtil::RWRecMutex::WLock lock(_mutex);
    _entryNum = num;
}
Ice::Int SearchStat::readEntryNum()
{
    IceUtil::RWRecMutex::RLock lock(_mutex);
    return _entryNum;
}

