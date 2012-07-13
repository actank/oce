/*
 * =====================================================================================
 *
 *       Filename:  PersistenceHelper.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年10月27日 15时44分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __PERSISTENCE_HELPER_H__
#define __PERSISTENCE_HELPER_H__

#include <vector>
//#include <UserFeeds.h>
#include "RFeed.h" 
#include "QueryRunner.h"

namespace xce {
namespace feed {

using namespace com::xiaonei::xce;

const string DB_INSTANCE_NAME = "feed_item";
const string DB_TABLE_PREFIX = "feed_item_";
const int DB_TABLE_COUNT = 100;

const string DB_COLUMN_FEEDID = "feed";
const string DB_COLUMN_USERID = "actor";
const string DB_COLUMN_TYPE = "type";
const string DB_COLUMN_TIME = "time";
const string DB_COLUMN_WEIGHT = "weight";
const string DB_COLUMN_MERGEID = "merge";
const string DB_COLUMN_CONTEXT= "context";

class FeedItemInfo : public Ice::Object {
public:
	FeedItemInfo() {}
	~FeedItemInfo() {}

	void put(FeedItem item) {
		items_.push_back(item);
	}

	std::vector<FeedItem> get_items() {
		return items_;
	}
private:
	std::vector<FeedItem> items_;
};
typedef IceUtil::Handle<FeedItemInfo> FeedItemInfoPtr;

class FeedItemHandler : public com::xiaonei::xce::ResultHandler {
public:
	FeedItemHandler(FeedItemInfoPtr& itemInfo) : itemInfo_(itemInfo) {}
	virtual bool handle(mysqlpp::Row& row) const {

		FeedItem item;
		item.feed = (long)row[DB_COLUMN_FEEDID.c_str()];
		item.actor= (int)row[DB_COLUMN_USERID.c_str()];
		item.type = (int)row[DB_COLUMN_TYPE.c_str()];
		item.time = (int)row[DB_COLUMN_TIME.c_str()];
		item.weight = (int)row[DB_COLUMN_WEIGHT.c_str()];
		item.merge = (long)row[DB_COLUMN_MERGEID.c_str()];
		
		itemInfo_->put(item);
		
		return true;
	}
private:
	FeedItemInfoPtr itemInfo_;
};


class PersistenceHelper {
public:
	static FeedItemInfoPtr read(Ice::Long index, int begin, int end) {
		Statement sql;
		std::string table = table_name(index);
		sql << "SELECT * FORM " << table << " WHERE time > " << begin << " AND time <= " << end;
		FeedItemInfoPtr item = new FeedItemInfo;

		try {
			QueryRunner(DB_INSTANCE_NAME, CDbRServer, table).query(sql, FeedItemHandler(item));
			return item;
		} catch (mysqlpp::Exception& e) {
			MCE_WARN("PersistenceHelper::read, index = " << index << " begin = " << begin << " end = " << end << e.what());
		} catch (...) {
			MCE_WARN("PersistenceHelper::read, index = " << index << " begin = " << begin << " end = " << end);
		}
		return new FeedItemInfo;
	}

	static void write(const FeedItem& item, const string& context) {
		Statement sql;
		std::string table = table_name(item.feed);
		sql << "INSERT INTO " << table << " (" << DB_COLUMN_FEEDID << ", " << DB_COLUMN_USERID << ", " << DB_COLUMN_TYPE << ", "
		   <<DB_COLUMN_TIME << ", " << DB_COLUMN_WEIGHT << ", " << DB_COLUMN_MERGEID << ", " << DB_COLUMN_CONTEXT << ") values (" 
		   << item.feed << "," << item.actor << "," << item.type << "," << item.time << "," << item.weight << "," << item.merge << ",\"" << context
		   << "\") ON DUPLICATE KEY UPDATE " << DB_COLUMN_FEEDID << "=values(" << DB_COLUMN_FEEDID << ")"; 

		try {
			QueryRunner(DB_INSTANCE_NAME, CDbWServer, table).execute(sql);
		} catch (mysqlpp::Exception& e) {
			MCE_WARN("PersistenceHelper::write, feed = " << item.feed << " actor = " << item.actor  << e.what());
		} catch (...) {
			MCE_WARN("PersistenceHelper::write, feed = " << item.feed << " actor = " << item.actor << " UNKNOWN EXCEPTION!");
		}
		
		return ;
	}
private:
	static std::string table_name(Ice::Long index) {
		ostringstream oss;
		oss << DB_TABLE_PREFIX << index%DB_TABLE_COUNT;
		return oss.str();
	}

};

}
}

#endif
