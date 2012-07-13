/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  migrate data from feed_content to feed_item.
 *
 *        Version:  1.0
 *        Created:  2011年10月28日 15时06分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "UserFeeds.h"
#include "QueryRunner.h"
#include "../persistence/PersistenceHelper.h"
#include "Channel.h"

using namespace std;
using namespace xce::feed;
using namespace com::xiaonei::xce;

const int TABLE_COUNT = 1;
const int FEED_BEGIN = 807512969;
const int FEED_INTERVAL = 1000;
const int FEED_END = 807515000;

class FeedContentHandler : public com::xiaonei::xce::ResultHandler {
public:
	FeedContentHandler(FeedItemInfoPtr& items) : items_(items) {}
	virtual bool handle(mysqlpp::Row& row) const {

		FeedMeta meta;
		meta.feedid = (long)row["feed"];
		meta.userid = (int)row["actor"];
		meta.type = (int)row["type"];
		meta.time = (time_t) mysqlpp::DateTime(row["time"]);
		meta.weight = 0;
		meta.mergeid = (long)row["news_merge"];

		std::cout << "FeedContentHandler::handle, feedid: " << meta.feedid << " userid: " << meta.userid << " type: " << meta.type << " time: " << meta.time << " weight: " << meta.weight << " mergeid: " << meta.mergeid << std::endl;
		
		items_->put(meta);
		
		return true;
	}
private:
	FeedItemInfoPtr items_;
};

class MigrateDBHelper : public Ice::Object {
public:
	MigrateDBHelper() {}
	~MigrateDBHelper() {}

	FeedItemInfoPtr readFromContent(int index, int begin, int end) {
		Statement sql;
		std::string table = getContentTable(index);
		sql << "SELECT feed, type, actor, time, news_merge FROM " << table <<
			" WHERE feed > " << begin << " AND feed <= " << end;

		{
			ostringstream oss;
			oss << "SELECT feed, type, actor, time, news_merge FROM " << table <<
				" WHERE feed > " << begin << " AND feed <= " << end;
			std::cout << oss.str() << std::endl;
		}
		
		FeedItemInfoPtr items = new FeedItemInfo;
		try {
			size_t count = QueryRunner("feed_db", CDbRServer, table).query(sql, FeedContentHandler(items));
			std::cout << " count: " << count << std::endl;
			return items;
		} catch (mysqlpp::Exception& e) {
			std::cout << "MigrateDBHelper::readFromContent, index: " << index
				<< " begin: " << begin << " end: " << end << e.what() << std::endl;
		} catch (...) {
			std::cout << "MigrateDBHelper::readFromContent, index: " << index
				<< " begin: " << begin << " end: " << end << std::endl;
		}
		return NULL;
	}

	void writeToItem(const FeedItemInfoPtr& data) {
		std::vector<FeedMeta> metas = data->get_metas();
		for (std::vector<FeedMeta>::const_iterator it = metas.begin();
				it != metas.end(); ++it) {
			PersistenceHelper::write(*it);
		}
	}
private:
	std::string getContentTable(int index) {
		ostringstream oss;
		oss << "feed_content_" << index;
		return oss.str();
	}

	std::string getItemTable(int index) {
		ostringstream oss;
		oss << "feed_item_" << index;
		return oss.str();
	}
};
typedef IceUtil::Handle<MigrateDBHelper> MigrateDBHelperPtr;

class MigrateFeedDbThread : virtual public IceUtil::Thread {
public:
	MigrateFeedDbThread(int index) : index_(index), helper_(new MigrateDBHelper) {}
	virtual ~MigrateFeedDbThread() {}

protected:
	virtual void run() {
		//std::cout << "Thread " << index_ << " is started!" << std::endl;
		int begin = FEED_BEGIN;
		int end = FEED_BEGIN + FEED_INTERVAL;
		while (true) {
			//std::cout << "Thread " << index_ << " begin: " << begin << " end: " << end;
			try {
				FeedItemInfoPtr items = helper_ -> readFromContent(index_, begin, end);
				helper_->writeToItem(items);
			} catch (mysqlpp::Exception& e) {
				std::cout << "Thread " << index_ << " " << e.what() << std::endl;
				continue;
			} catch (...) {
				std::cout << "Thread " << index_ << " Exception!" << std::endl;
				continue;
			}

			begin = end;
			end = begin + FEED_INTERVAL;

			if (end >= FEED_END) {
				break;
			}
		}//end while
		//std::cout << "Thread " << index_ << " is end!" << std::endl;
	}
private:
	int index_;
	MigrateDBHelperPtr helper_;
};
typedef IceUtil::Handle<MigrateFeedDbThread> MigrateFeedDbThreadPtr;

int main(int argc, char ** argv) {
	ConnectionPoolManager::instance().initialize();

	std::vector<MigrateFeedDbThreadPtr> threads;
	threads.reserve(TABLE_COUNT);

	for (int index = 0; index < TABLE_COUNT; index++) {
		MigrateFeedDbThreadPtr thread = new MigrateFeedDbThread(index);
		threads.push_back(thread);
		thread->start();
	}

	for (int index = 0; index < TABLE_COUNT; index++) {
		threads[index]->getThreadControl().join();
	}

	std::cout << "migrate from content to item is done!" << std::endl;
	exit(0);
}
