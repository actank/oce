#ifndef __QUEST_UTIL_H__
#define __QUEST_UTIL_H__

#include <string>
#include <vector>
#include <map>
#include "Quest.h"
#include "UtilCxx/src/Singleton.h"
#include "util/cpp/String.h"

using std::string;
using std::map;

namespace xce {
namespace quest {

/** ------------------------------------------------
 *
 * I. 常量定义部分
 *
 -------------------------------------------------*/
// 1. ObjectCache标识
const int QUEST_CACHE = 0;

// 2. DB相关定义
const string DESC_QUEST_TIPFLOW = "user_quest";
const string TABLE_TIP = "tip";
const string FIELD_TIP_TIPID = "tipid";
const string FIELD_TIP_TITLE = "title";
const string FIELD_TIP_CONTENT = "content";
const string FIELD_TIP_URL = "url";
const string TABLE_FLOW = "flow";
const string FIELD_FLOW_FLOWID = "flowid";
const string FIELD_FLOW_TIPS = "tips";
const string FIELD_FLOW_ONLINETIME = "onlinetime";
const string DESC_QUEST_PROCESS = "user_quest";
const string TABLE_PROCESS = "process_";
const int TABLE_PROCESS_CLUSTER = 100;
const string FIELD_PROCESS_USERID = "userid";
const string FIELD_PROCESS_FLOWID = "flowid";
const string FIELD_PROCESS_CURRENTPOS = "current_tipid";
const string FIELD_PROCESS_REMAINCOUNT = "remain_count";

// 3. 其他常量
// TODO: 其他常量定义在这里

/** ------------------------------------------------
 *
 * II. DB结构
 *
 ------------------------------------------------ */
// database : quest

// table 1 : tip
/* table name: tip
 * table num : 1
 * +---------+---------------+------+-----+---------+----------------+
   | Field   | Type          | Null | Key | Default | Extra          |
   +---------+---------------+------+-----+---------+----------------+
   | tipid   | int(4)        | NO   | PRI | NULL    | auto_increment | 
   | title   | varchar(128)  | NO   |     |         |                | 
   | content | varchar(1024) | NO   |     |         |                | 
   | url     | varchar(512)  | NO   |     |         |                |
  +---------+---------------+------+-----+---------+----------------+ */

// table 2 : flow
/* table name : flow
 * talbe num : 1
 * +------------+---------------+------+-----+---------+----------------+
 * | Field      | Type          | Null | Key | Default | Extra          |
 * +------------+---------------+------+-----+---------+----------------+
 * | flowid     | int(4)        | NO   | PRI | NULL    | auto_increment | 
 * | tips       | varchar(1024) | NO   |     |         |                | 
 * | onlinetime | datetime      | YES  |     | NULL    |                | 
 * +------------+---------------+------+-----+---------+----------------+ 
 * 注: 一条flow的所有tip都组合成字符串写在tips这个字段里, 格式为:
 *     tipid1:showCount1-tipid2:showCount2-....
 *     例 1-5;2-1;3-10;4-5 */

// table 3 : process
/* table name : process_N (N=0~99)
 * table name : 100
 * +---------------+--------+------+-----+---------+-------+
 * | Field         | Type   | Null | Key | Default | Extra |
 * +---------------+--------+------+-----+---------+-------+
 * | userid        | int(4) | NO   | PRI | 0       |       | 
 * | flowid        | int(4) | YES  |     | NULL    |       | 
 * | current_tippos| int(4) | YES  |     | NULL    |       | 
 * | remain_count  | int(4) | YES  |     | NULL    |       | 
 * +---------------+--------+------+-----+---------+-------+ */


/** ------------------------------------------------
 *
 * II. 用户信息结构类
 *
 -------------------------------------------------*/ 
static TipPtr emptyTip() {
	TipPtr tip = new Tip;
	tip->id = -1;
	tip->title = "";
	tip->content = "";
	tip->url = "";
	return tip;
}

static bool startsWith(const std::string& url, const std::string& prefix) {
	int uc = url.size();
	int pc = prefix.size();
	int uo = 0;
	int po = 0;
	if (uc - pc < 0) {
		return false;
	}
	while (--pc >= 0) {
		if (url[uo++] != prefix[po++]) {
			return false;
		}
	}
	return true;
}

}
}






#endif
