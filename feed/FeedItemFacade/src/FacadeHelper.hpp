/*
 * =====================================================================================
 *
 *       Filename:  FacadeHelper.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月29日 11时10分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __FACADEHELPER_HPP__
#define __FACADEHELPER_HPP__

#include <map>
#include "QueryRunner.h"

namespace xce {
namespace feed {

using namespace MyUtil;
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

class FacadeHelper {
public:
	static bool is_exist(const std::map<string, string>& context, const string& prefix) {

		if (context.find(prefix) != context.end()) {
			return true;
		}
		
		return false;
	}

	static bool write_db(const FeedMeta& meta, const std::map<string, string>& context) {
		Statement sql;
		std::string table = table_name(meta.feedid);
		sql << "INSERT INTO " << table << " (" << DB_COLUMN_FEEDID << ", " << DB_COLUMN_USERID << ", " << DB_COLUMN_TYPE << ", "
		   <<DB_COLUMN_TIME << ", " << DB_COLUMN_WEIGHT << ", " << DB_COLUMN_MERGEID << ", " << DB_COLUMN_CONTEXT << ") values (" 
		   << meta.feedid << "," << meta.userid << "," << meta.type << "," << meta.time << "," << meta.weight << "," << meta.mergeid
		   << "," << mysqlpp::quote << context_to_string(context)
		   << ") ON DUPLICATE KEY UPDATE " << DB_COLUMN_FEEDID << "=values(" << DB_COLUMN_FEEDID << ")"; 

		try {
			QueryRunner(DB_INSTANCE_NAME, CDbWServer, table).schedule(sql);
		} catch (mysqlpp::Exception& e) {
			MCE_WARN("PersistenceHelper::write, feedid = " << meta.feedid << " userid = " << meta.userid  << e.what());
		} catch (...) {
			MCE_WARN("PersistenceHelper::write, feedid = " << meta.feedid << " userid = " << meta.userid << " UNKNOWN EXCEPTION!");
		}
		
		return true;
	}
private:
	static std::string table_name(long index) {
		ostringstream oss;
		oss << DB_TABLE_PREFIX << index%DB_TABLE_COUNT;
		return oss.str();
	}

	static std::string context_to_string(const std::map<string, string>& context) {
		ostringstream oss;
		uint count = 0;

		for (std::map<string, string>::const_iterator it = context.begin();
				it != context.end(); ++it) {
			if (0 == strcmp(it->first.c_str(), AppidContextPrefix.c_str())) {
				if (count != 0) {
					oss << ";";
				}
				oss << it->first << "=" << it->second;
				count ++;
			}
		}
		return oss.str();
	}

};

}
}


#endif

