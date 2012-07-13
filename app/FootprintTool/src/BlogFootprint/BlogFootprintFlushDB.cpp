/*
 * =====================================================================================
 *
 *       Filename:  BlogFootprintFlushDB.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年02月24日 11时04分25秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "../FootprintIdProducer.h"
#include "BlogFootprintAdapter.h"
#include "util/cpp/String.h"

using namespace std;
using namespace MyUtil;
using namespace xce::footprint;
using namespace com::xiaonei::xce;
using namespace mop::hi::svc::fpt;

const std::string TABLE_NAME = "blog_footprint";
const int TABLE_COUNT = 100;

int main() {
	ProducerPtr producer_ = new FootprintIdProducer();
	ConnectionPoolManager::instance().initialize();
	int begin_id_ = 0;
	int max_id_ = 0;

	vector<int> ids_;
	do {
		begin_id_ = max_id_;
		ids_ = producer_ -> produce(begin_id_, max_id_);
		std::cout << "ids scope: <" << begin_id_ << " - " << max_id_ << ">" << std::endl;
		for (vector<int>::iterator it = ids_.begin(); it != ids_.end(); ++it) {
			FootprintSeq fpts_ = BlogFootprintAdapter::instance().getAll(*it);

			if (fpts_.empty()) {
				continue;
			}

			for (size_t i = 0; i < fpts_.size(); ++i) {
				FootprintPtr& footprint = fpts_.at(i);

				ostringstream table_name;
				table_name << TABLE_NAME << "_" << (footprint->host % TABLE_COUNT);

				Statement sql;
				sql << "INSERT IGNORE INTO " << table_name.str() << " (host, guest, guest_name, guest_head, time) values";
				sql << "('" << footprint->host << "', '" << footprint->guest << "', '"
					<< StrUtil::fixString(footprint->guestName) << "', '" << StrUtil::fixString(footprint->guestHead)
					<< "', '" << Date(footprint->time/1000).str() << "')";
				QueryRunner("blog_footprint_new", CDbWServer, table_name.str()).execute(sql);
				

				ostringstream oss;
				oss << "INSERT IGNORE INTO " << table_name.str() << " (host, guest, guest_name, guest_head, time) values";
				oss << "('" << footprint->host << "', '" << footprint->guest << "', '"
					<< StrUtil::fixString(footprint->guestName) << "', '" << StrUtil::fixString(footprint->guestHead)
					<< "', '" << Date(footprint->time/1000).str() << "')";
				std::cout << oss.str() << std::endl;
			}

		}
	} while (!ids_.empty());
}
