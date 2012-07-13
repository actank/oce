/*
 * =====================================================================================
 *
 *       Filename:  DBHelper.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月23日 14时49分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __DBHELPER_HPP__
#define __DBHELPER_HPP__

#include <vector>
#include <deque>
#include <map>
#include <UserFeeds.h>
#include <QueryRunner.h>
#include "FeedStream.hpp"

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
const string DB_COLUMN_CONTEXT = "context";

class FeedItemInfo : public Ice::Object {
public:
	FeedItemInfo() {}
	~FeedItemInfo() {}

	void put(int id, FeedMeta meta) {
		std::map<int, std::vector<FeedMeta> >::iterator it = metas_.find(id);
		if (it == metas_.end()) {
			std::vector<FeedMeta> items;
			metas_[id] = items;
		}
		metas_[id].push_back(meta);
	}

	std::map<int, std::vector<FeedMeta> >& get_metas() {
		return metas_;
	}
private:
	std::map<int, std::vector<FeedMeta> > metas_;
};
typedef IceUtil::Handle<FeedItemInfo> FeedItemInfoPtr;

class ContextParser {
public:
	static int get_id(string context, string context_prefix) {
		std::vector<string> msg = MyUtil::StrUtil::split(context, ";");

		if (!msg.empty()) {
			for (std::vector<string>::iterator it = msg.begin();
					it != msg.end(); ++it) {
				std::vector<string> item = MyUtil::StrUtil::split((*it), "=");
				if (2 == item.size() && 0 == strcmp(item[0].c_str(), context_prefix.c_str())) {
					return boost::lexical_cast<int>(item[1]);
					
				}
			}
		}

		return -1;
	}
};

class FeedItemHandler : public com::xiaonei::xce::ResultHandler {
public:
	FeedItemHandler(FeedItemInfoPtr& item, string context_prefix_) : item_(item), context_prefix_(context_prefix_) {}
	virtual bool handle(mysqlpp::Row& row) const {
		
		FeedMeta meta;
		meta.feedid = (long)row[DB_COLUMN_FEEDID.c_str()];
		meta.userid = (int)row[DB_COLUMN_USERID.c_str()];
		meta.type = (int)row[DB_COLUMN_TYPE.c_str()];
		meta.time = (int)row[DB_COLUMN_TIME.c_str()];
		meta.weight = (int)row[DB_COLUMN_WEIGHT.c_str()];
		meta.mergeid = (long)row[DB_COLUMN_MERGEID.c_str()];

		string context = (string)row[DB_COLUMN_CONTEXT.c_str()];

		//MCE_DEBUG("FeedItemHandler::handle, meta.feedid: " << meta.feedid << " meta.userid: " << meta.userid
		//		<< " meta.type: " << meta.type << " meta.time: " << meta.time);
		//
		int id = 0;
		if (0 == strcmp(context_prefix_.c_str(), UseridContextPrefix.c_str())) {
			id = meta.userid;
		} else {
			id = ContextParser::get_id(context, context_prefix_);
		}
		
		if (id >= 0) {
			item_->put(id, meta);
		}
		
		return true;
	}
private:
	FeedItemInfoPtr item_;
	string context_prefix_;
};


class PersistenceHelper {
public:
	static FeedItemInfoPtr read(int index, int begin, int end, string context_prefix) {
		Statement sql;
		std::string table = table_name(index);
		sql << "SELECT * FROM " << table << " WHERE time < " << begin << " AND time >= " << end << " AND " << DB_COLUMN_CONTEXT << "!='' ";
		FeedItemInfoPtr item = new FeedItemInfo;

		try {
			QueryRunner(DB_INSTANCE_NAME, CDbRServer, table).query(sql, FeedItemHandler(item, context_prefix));
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
