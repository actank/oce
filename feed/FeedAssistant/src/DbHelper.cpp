#include "DbHelper.h"
#include "QueryRunner.h"
#include "IceLogger.h"

using namespace xce::feed;
using namespace com::xiaonei::xce;
using namespace MyUtil;

void DbHelper::addFeedIndex(const FeedIndexPtr& itemPtr) {
	if (!itemPtr) {
		return;
	}
	Statement sql;
	sql << "INSERT INTO " << getIndexTab(itemPtr->feed)
			<< " (feed,source,psource,small_type,big_type,ptype,actor,time,mini_merge,news_merge)"
			<< " VALUES (" << itemPtr->feed << "," << itemPtr->source << ","
			<< itemPtr->psource << ", " << itemPtr->stype << ","
			<< int(itemPtr->stype / 100) * 100 << "," << itemPtr->ptype << ","
			<< itemPtr->actor << ",from_unixtime(" << itemPtr->time << "),"
			<< itemPtr->miniMerge << "," << itemPtr->newsMerge << ")";

	QueryRunner(DB_INSTANCE, CDbWServer, getIndexTab(itemPtr->feed)).store(sql);
}
FeedIndexSeq DbHelper::getFeedIndex(int stype, Ice::Long source, int actor) {

	IndexLoaderPtr loader = new IndexLoader();
	for (size_t i = 0; i < 10; ++i) {
		ostringstream sql;
		sql << "select * from " << getIndexTab(i) << " where " << COL_SMALLTYPE
				<< " = " << stype << " and " << COL_SOURCE << " = " << source
				<< " and " << COL_ACTOR << " = " << actor << " limit 10";
		loader->addQuery(DB_INSTANCE, sql.str(), getIndexTab(i), true);
	}
	return loader->query();
}
FeedIndexSeq DbHelper::getFeedIndexBySource(int stype, Ice::Long source) {
	IndexLoaderPtr loader = new IndexLoader();
	for (size_t i = 0; i < 10; ++i) {
		ostringstream sql;
		sql << "select * from " << getIndexTab(i) << " where " << COL_SMALLTYPE
				<< " = " << stype << " and " << COL_SOURCE << " = " << source
				<< " order by time desc limit 30";
		loader->addQuery(DB_INSTANCE, sql.str(), getIndexTab(i), true);
	}
	return loader->query();
}
FeedIndexSeq DbHelper::getFeedIndexByPsource(int ptype, Ice::Long psource) {
	IndexLoaderPtr loader = new IndexLoader();
	for (size_t i = 0; i < 10; ++i) {
		ostringstream sql;
		sql << "select * from " << getIndexTab(i) << " where " << COL_PTYPE
				<< " = " << ptype << " and " << COL_PSOURCE << " = " << psource
				<< " order by time desc limit 30";
		loader->addQuery(DB_INSTANCE, sql.str(), getIndexTab(i), true);
	}
	return loader->query();
}
FeedIndexPtr DbHelper::getFeedIndex(Ice::Long feed) {
	Statement sql;
	sql << "SELECT * FROM " << getIndexTab(feed) << " WHERE " << COL_FEED
			<< " = " << feed;
	mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer,
			getIndexTab(feed)).store(sql);
	if (!res) {
		return 0;
	}
	FeedIndexPtr index = new FeedIndex;
	for (size_t i = 0; i < res.num_rows(); ++i) {
		mysqlpp::Row& row = res.at(i);
		index->feed = row[COL_FEED];
		index->newsMerge = row[COL_NEWS_MERGE];
		index->miniMerge = row[COL_MINI_MERGE];
		index->source = row[COL_SOURCE];
		index->psource = row[COL_PSOURCE];
		index->stype = row[COL_SMALLTYPE];
		index->ptype = row[COL_PTYPE];
		index->actor = row[COL_ACTOR];
		index->time = (time_t) mysqlpp::DateTime(row[COL_TIME]);
	}
	return index;
}
void DbHelper::removeFeedIndex(const MyUtil::LongSeq& feeds) {
	map<int, MyUtil::LongSeq> tmp;
	for (size_t i = 0; i < feeds.size(); ++i) {
		tmp[feeds.at(i) % 10].push_back(feeds.at(i));
	}
	IndexLoaderPtr loader = new IndexLoader();
	for (map<int, MyUtil::LongSeq>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
		ostringstream sql;
		sql << "delete from " << getIndexTab(it->first) << " where "
				<< COL_FEED << " in (";
		for (size_t i = 0; i < it->second.size(); ++i) {
			if (i) {
				sql << ",";
			}
			sql << it->second.at(i);
		}
		sql << ")";
		loader->addQuery(DB_INSTANCE, sql.str(), getIndexTab(it->first), false);
	}
	loader->query();
}

void DbHelper::addFeedContent(const FeedSeedPtr& seed) {
	if (!seed) {
		return;
	}
	ostringstream os;
	for (MyUtil::Str2StrMap::iterator it = seed->extraProps.begin(); it
			!= seed->extraProps.end(); ++it) {
		os << it->first << '=' << it->second << ',';
	}
	Statement sql;
	sql << "insert into " << DbHelper::instance().getContentTab(seed->feed)
			<< " (feed,source,type,big_type,small_type,actor,time,prop,xml,psource,ptype,mini_merge,news_merge) values ("
			<< seed->feed << "," << seed->source << "," << seed->type << ","
			<< int((seed->type & 0xffff) / 100) * 100 << "," << (seed->type
			& 0xffff) << "," << seed->actor << "," << "from_unixtime("
			<< seed->time << ")," << mysqlpp::quote << os.str() << ","
			<< mysqlpp::quote << seed->xml << "," << seed->psource << ","
			<< seed->ptype << "," << seed->miniMerge << "," << seed->newsMerge
			<< ")";

	QueryRunner(DB_INSTANCE, CDbWServer, getContentTab(seed->feed)).store(sql);
}
void DbHelper::removeFeedContent(const MyUtil::LongSeq& feeds) {
	if (feeds.empty()) {
		return;
	}
	map<int, MyUtil::LongSeq> ids;
	for (size_t i = 0; i < feeds.size(); ++i) {
		ids[feeds.at(i) % 100].push_back(feeds.at(i));
	}
	for (map<int, MyUtil::LongSeq>::iterator it = ids.begin(); it != ids.end(); ++it) {
		Statement sql;
		sql << "delete from " << getContentTab(it->first) << " where feed in (";
		for (size_t i = 0; i < it->second.size(); ++i) {
			if (i) {
				sql << ",";
			}
			sql << it->second.at(i);
		}
		sql << ")";

		QueryRunner(DB_INSTANCE, CDbWServer, getContentTab(it->first)).store(
				sql);

	}
}

map<string, string> DbHelper::getFeedConfigProp(Ice::Long fid) {
	map<string, string> prop;
	Statement sql;
	sql << "SELECT prop FROM " << DbHelper::instance().getContentTab(fid)
			<< " WHERE feed=" << fid;
	mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer,
			getContentTab(fid)).store(sql);
	string propStr;
	if (!res) {
		return prop;
	}
	for (size_t i = 0; i < res.num_rows(); ++i) {
		mysqlpp::Row row = res.at(i);
		propStr = row["prop"].data();
		size_t pos = 0;
		string::size_type idx;
		string key;
		string val;

		while (pos < propStr.size()) {

			idx = propStr.find('=', pos);
			if (idx == string::npos) {
				return prop;
			}

			key = propStr.substr(pos, idx - pos);
			pos = idx + 1;
			idx = propStr.find(',', pos);
			if (idx == string::npos) {
				return prop;
			}

			val = propStr.substr(pos, idx - pos);
			pos = idx + 1;

			prop[key] = val;

		}

		return prop;
	}
	return prop;

}

void DbHelper::addFeedMini(const FeedSeedPtr& seed) {
	if (!seed) {
		return;
	}
	if ((seed->type & 0x30000000) >> 28 == FeedMergeReplace) {
		Statement sql;
		sql << " delete from " << getMiniTab(seed->actor)
				<< " where mini_merge=" << seed->miniMerge
				<< " AND  small_type=" << int(seed->type & 0xffff)
				<< " AND user=" << seed->actor;
		QueryRunner(DB_INSTANCE, CDbWServer, getMiniTab(seed->actor)).store(sql);
	}
	{
		Statement sql;
		sql << "insert into " << getMiniTab(seed->actor)
				<< " (user, feed, big_type,small_type,source,time,unread,mini_merge,type,actor) values ("
				<< seed->actor << "," << seed->feed << "," << int((seed->type
				& 0xffff) / 100) * 100 << "," << int(seed->type
				& 0xffff) << "," << seed->source << ",from_unixtime("
				<< seed->time << "), 1 ," << seed->miniMerge << " , "
				<< seed->type << "," << seed->actor << " ) ";
		QueryRunner(DB_INSTANCE, CDbWServer, getMiniTab(seed->actor)).store(sql);
	}
}

void DbHelper::removeFeedMini(int user, int stype, Ice::Long miniMerge) {
	Statement sql;
	sql << "delete from " << getMiniTab(user) << " where user=" << user
			<< " and small_type=" << stype << " and mini_merge=" << miniMerge;
	QueryRunner(DB_INSTANCE, CDbWServer, getMiniTab(user)).store(sql);
}

void DbHelper::removeAllFeedMini(int user) {
	Statement sql;
	sql << "delete from " << getMiniTab(user) << " where user=" << user;
	QueryRunner(DB_INSTANCE, CDbWServer, getMiniTab(user)).store(sql);
}

void DbHelper::loadDangerUserList(set<int>& blockUsers) {
	Statement sql;
	sql << "select user_id from " << getDangeruserListTab()
			<< " where biz_type=1 and is_validate=1";
	mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer,
			getDangeruserListTab()).store(sql);
	if (!res) {
		return;
	}
	MCE_INFO("DbHelper::loadDangerUserList --> loading block users, size:"<<res.num_rows());
	for (size_t i = 0; i < res.num_rows(); ++i) {
		mysqlpp::Row row = res.at(i);
		int uid = row["user_id"];
		blockUsers.insert(uid);
	}
}

Ice::Long DbHelper::createId() {

	{
		IceUtil::Mutex::Lock lock(_mutex);
		if(++_count > 1000){
			MCE_WARN("DbHelper::createId --> IdSeqDb is blocked");
			--_count;
			return -1;
		}

	}

	{
		IceUtil::Mutex::Lock lock(_mutex);
		while (1) {
			MCE_DEBUG("_curIndex:"<<_curIndex << ", _topIndex:"<<_topIndex << ", count:"<< _count);
			if (_curIndex != -1 && _curIndex < _topIndex) {
				--_count;
				return _curIndex++;
			}
			{
				Statement sql;
				sql << "update " << TAB_ID_SEQ << " SET id = id +"
						<< ID_SEQ_STEP;
				QueryRunner(DB_INSTANCE, CDbWServer, TAB_ID_SEQ).store(sql);
			}
			{
				Statement sql;
				sql << "SELECT * FROM " << TAB_ID_SEQ;
				mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE,
						CDbWServer, TAB_ID_SEQ).store(sql);
				if (!res) {
					--_count;
					return -1;
				}
				for (size_t i = 0; i < res.num_rows(); ++i) {
					mysqlpp::Row row = res.at(i);
					_topIndex = (Ice::Long) row["ID"];
					_curIndex = _topIndex - ID_SEQ_STEP;
					MCE_DEBUG("topIndex:"<<_topIndex);
					break;
				}
			}

		}
	}

}

//---------------------------------------------------------------------------

FeedIndexSeq IndexLoader::query() {
	for (size_t i = 0; i < _taskSeq.size(); ++i) {
		MyUtil::TaskManager::instance().execute(_taskSeq.at(i));
	}
	_taskSeq.clear();

	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
	if (!timedWait(IceUtil::Time::milliSeconds(LOAD_TIMEOUT))) {
		MCE_WARN("IndexLoader::query -> count: " << _count << " missing");
	} else {
		MCE_INFO("IndexLoader::query -> success");
	}
	return _indexSeq;
}
void IndexLoader::addQuery(const string& dbInstance, const string& sql,
		const string& pattern, bool isSelect) {
	SqlExecuteTask* t = new SqlExecuteTask(this, isSelect);
	t->init(dbInstance, sql, pattern);
	_taskSeq.push_back(t);
	++_count;
}

void IndexLoader::SqlExecuteTask::handle() {
	//	MCE_DEBUG("IndexLoader --> SqlExecuteTask  seq:"<<_sql);
	if (_isSelect) {
		mysqlpp::StoreQueryResult res = QueryRunner(_dbInstance, CDbRServer,
				_pattern).store(_sql);
		if (!res) {
			return;
		}
		FeedIndexSeq seq;
		for (size_t i = 0; i < res.num_rows(); ++i) {
			mysqlpp::Row row = res.at(i);

			FeedIndexPtr index = new FeedIndex;
			index->feed = row[COL_FEED];
			index->newsMerge = row[COL_NEWS_MERGE];
			index->miniMerge = row[COL_MINI_MERGE];
			index->source = row[COL_SOURCE];
			index->psource = row[COL_PSOURCE];
			index->stype = row[COL_SMALLTYPE];
			index->ptype = row[COL_PTYPE];
			index->actor = row[COL_ACTOR];
			index->time = (time_t) mysqlpp::DateTime(row[COL_TIME]);
			seq.push_back(index);

		}
		_loader->finish(seq);

	} else {
		QueryRunner(_dbInstance, CDbWServer, _pattern).store(_sql);
		_loader->finish();
	}

}
