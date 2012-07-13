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
#include <deque>
#include <map>
#include <UserFeeds.h>
#include <QueryRunner.h>
#include "../src/FeedStream.h"

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

class FeedItemInfo : public Ice::Object {
public:
	FeedItemInfo() {}
	~FeedItemInfo() {}

	void put(FeedMeta meta) {
		std::map<int, std::vector<FeedMeta> >::iterator it = metas_.find(meta.userid%FEEDSTREAM_MUTEX_NUM);
		if (it == metas_.end()) {
			std::vector<FeedMeta> items;
			metas_[meta.userid%FEEDSTREAM_MUTEX_NUM] = items;
		}
		metas_[meta.userid%FEEDSTREAM_MUTEX_NUM].push_back(meta);
	}

	std::map<int, std::vector<FeedMeta> >& get_metas() {
		return metas_;
	}
private:
	std::map<int, std::vector<FeedMeta> > metas_;
};
typedef IceUtil::Handle<FeedItemInfo> FeedItemInfoPtr;

class FeedItemHandler : public com::xiaonei::xce::ResultHandler {
public:
	FeedItemHandler(FeedItemInfoPtr& item) : item_(item) {}
	virtual bool handle(mysqlpp::Row& row) const {
		
		FeedMeta meta;
		meta.feedid = (long)row[DB_COLUMN_FEEDID.c_str()];
		meta.userid = (int)row[DB_COLUMN_USERID.c_str()];
		meta.type = (int)row[DB_COLUMN_TYPE.c_str()];
		meta.time = (int)row[DB_COLUMN_TIME.c_str()];
		meta.weight = (int)row[DB_COLUMN_WEIGHT.c_str()];
		meta.mergeid = (long)row[DB_COLUMN_MERGEID.c_str()];

		//MCE_DEBUG("FeedItemHandler::handle, meta.feedid: " << meta.feedid << " meta.userid: " << meta.userid
		//		<< " meta.type: " << meta.type << " meta.time: " << meta.time);
		
		item_->put(meta);
		
		return true;
	}
private:
	FeedItemInfoPtr item_;
};


class PersistenceHelper {
public:
	static FeedItemInfoPtr read(int index, int begin, int end) {
		Statement sql;
		std::string table = table_name(index);
		sql << "SELECT * FROM " << table << " WHERE time < " << begin << " AND time >= " << end;
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

	static void write(const FeedMeta& meta) {
		Statement sql;
		std::string table = table_name(meta.feedid);
		//sql << "INSERT INTO " << table << " (" << DB_COLUMN_FEEDID << ", " << DB_COLUMN_USERID << ", " << DB_COLUMN_TYPE << ", "
		//   <<DB_COLUMN_TIME << ", " << DB_COLUMN_WEIGHT << ", " << DB_COLUMN_MERGEID << ") values (" 
		//   << meta.feedid << "," << meta.userid << "," << meta.type << "," << meta.time << "," << meta.weight << "," << meta.mergeid
		//   << ") ON DUPLICATE KEY UPDATE " << DB_COLUMN_FEEDID << "=values(" << DB_COLUMN_FEEDID << ")"; 

		try {
		//	QueryRunner(DB_INSTANCE_NAME, CDbWServer, table).execute(sql);
		} catch (mysqlpp::Exception& e) {
			MCE_WARN("PersistenceHelper::write, feedid = " << meta.feedid << " userid = " << meta.userid  << e.what());
		} catch (...) {
			MCE_WARN("PersistenceHelper::write, feedid = " << meta.feedid << " userid = " << meta.userid << " UNKNOWN EXCEPTION!");
		}
		
		return ;
	}
private:
	static std::string table_name(int index) {
		ostringstream oss;
		oss << DB_TABLE_PREFIX << index%DB_TABLE_COUNT;
		return oss.str();
	}

};

}
}

#endif
