#ifndef __PAGEFANSCOMMON_H__
#define __PAGEFANSCOMMON_H__

#include "PageCommon.h"
#include <string>
#include <QueryRunner.h>

using namespace ::com::xiaonei::xce;
using namespace xce::buddy;
using namespace std;
namespace xce {
namespace buddy {


const static string DB_PAGE_FANS = "xce_page_fans";
const static string DB_PAGE_FANS_BLOCK = "xce_page_fans_block";
const static string TABLE_PAGE_FANS = "page_fans_";
const static string TABLE_PAGE_FANS_BLOCK = "page_fans_blacklist";
const static int TABLE_PAGE_FANS_COUNT = 100;

const static int NORMAL = 0;
const static int BLOCK = 2;

class PageFansHelper {
	public:
			static string getTableName(const string & table, int userId) {
					ostringstream tableName;
					tableName<<table<<"_"<<(userId%100);
					return tableName.str();
			}
};


class PageFansResultHandler : public com::xiaonei::xce::ResultHandler {
public:
  PageFansResultHandler(const MyUtil::ObjectResultPtr& results) : _results(results){};
    virtual bool handle(mysqlpp::Row& row) const {
    
	int userId = (int)row["user_id"];
    int id = (int)row["id"];
   
    int state = (int)row["state"];
    int addTime = (int)row["add_time"];
    int oper = (int)row["operator"];
    int pageId = (int)row["page_id"];


    PageEntryPtr ptr = new PageEntry(id, pageId, userId, state, addTime, oper);

	MyUtil::ObjectMap::iterator it = _results->data.find(pageId);
	if (it == _results->data.end())
	{}
	else
	{
			if(state == NORMAL)
			{
					PageFansObjPtr::dynamicCast(it->second)->realfans.push_back(ptr); 
			}
			else if( state == BLOCK)
			{
					PageFansObjPtr::dynamicCast(it->second)->blockfans.push_back(ptr); 
			}else
			{
					PageFansObjPtr::dynamicCast(it->second)->otherfans.push_back(ptr); 
			}

	}

    return true;
  }

private:

  MyUtil::ObjectResultPtr _results;
};

class PageEntryBlockResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	PageEntryBlockResultHandler(const MyUtil::ObjectResultPtr& results) : _results(results){};
    virtual bool handle(mysqlpp::Row& row) const {

	int userId = (int)row["userid"];
    int id = (int)row["id"];
    int state = BLOCK;
    int addTime = (int)row["timestamp"];
    int operatorId = (int)row["executor"];
    int pageId = (int)row["pageid"];


    PageEntryPtr ptr = new PageEntry(id, pageId, userId, state, addTime, operatorId);

	MyUtil::ObjectMap::iterator it = _results->data.find(pageId);
	if (it == _results->data.end())
	{}
	else{
			PageFansObjPtr::dynamicCast(it->second)->blockfans.push_back(ptr); 
	}

    return true;
  }

private:

  MyUtil::ObjectResultPtr _results;
};



}
}
#endif
