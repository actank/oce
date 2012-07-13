/*
 * DBHelper.h
 *
 *  Created on: Dec 24, 2009
 *      Author: antonio
 */

#ifndef DBHELPER_H_
#define DBHELPER_H_

#include "QueryRunner.h"
#include "Singleton.h"
#include "RFeed.h"
#include "IceLogger.h"
namespace xce {
namespace feed {

class TimeStat{
  timeval _begin, _end;
  bool _log;
public:
  TimeStat(bool log=false){
    _log = log;
    reset();
  }
  ~TimeStat(){
    if(_log){
      MCE_INFO("######### " << getTime());
    }
  }
  void reset(){
    gettimeofday(&_begin, NULL);
  }
  float getTime(){
    gettimeofday(&_end, NULL);
    float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
    -_begin.tv_usec;
    timeuse/=1000;
    return timeuse;
  }
  
};

using namespace com::xiaonei::xce;
using namespace MyUtil;

static string DB_INSTANCE = "feed_db";
const string GROUP_DB_INSTANCE = "feed_minigroup";
const string TAB_ID_SEQ = "feed_id_seq";

#define COL_FEED "feed"
#define COL_SOURCE "source"
#define COL_SMALLTYPE "small_type"
#define COL_PSOURCE "psource"
#define COL_PTYPE "ptype"
#define COL_ACTOR "actor"
#define COL_NEWS_MERGE "news_merge"
#define COL_MINI_MERGE "mini_merge"
#define COL_TIME "time"

const int LOAD_TIMEOUT = 1000; //200ms
const int ID_SEQ_STEP=1000;

class DbHelper: public MyUtil::Singleton<DbHelper> {
public:

	DbHelper():_curIndex(-1), _topIndex(-1) {};

	void addFeedIndex(const FeedIndexPtr& itemPtr);
	FeedIndexSeq getFeedIndex(int stype, Ice::Long source, int actor);
	FeedIndexSeq getFeedIndexBySource(int stype, Ice::Long source);
	FeedIndexSeq getFeedIndexByPsource(int ptype, Ice::Long psource);
	FeedIndexPtr getFeedIndex(Ice::Long feed);
	void removeFeedIndex(const MyUtil::LongSeq& feeds);

	void addFeedContent(const FeedSeedPtr& content);

	map<string,string> getFeedContent(Ice::Long feed_id);
	bool copyFeedContent(Ice::Long old_feed_id, Ice::Long new_feed_id, int new_type, const string& new_props);

	void removeFeedContent(const MyUtil::LongSeq& feeds);
	map<string,string> getFeedConfigProp(Ice::Long fid);
	void addFeedMini(const FeedSeedPtr& content);
	void addFeedSchool(int schoolid, const FeedSeedPtr& content);
	void addFeedGroup(int groupid, const FeedSeedPtr& content);
	void removeAllFeedMini(int user);
	void removeFeedMini(int user, int stype, Ice::Long miniMerge);

	void ReplaceXML(const  MyUtil::LongSeq & fids , const string & xml );
//	Ice::Long createId();
  void UpdateReply(const map<Ice::Long, FeedReplyPtr>& buffer);

private:
	string getIndexTab(Ice::Long feed) {
		ostringstream tab;
		tab << "feed_index_" << feed % 10;
		return tab.str();
	}
	string getContentTab(Ice::Long feed) {
		ostringstream tab;
		tab << "feed_content_" << feed % 100;
		return tab.str();
	}
	string getMiniTab(int user) {
		ostringstream tab;
		tab << "feed_mini_" << user % 100;
		return tab.str();
	}
  string getSchoolTab(int schoolid) {
		ostringstream tab;
		tab << "feed_school_" << schoolid % 10;
		return tab.str();
	}
  string getGroupTab(int groupid) {
		ostringstream tab;
		tab << "feed_group_" << groupid % 100;
		return tab.str();
	}
	Ice::Long _curIndex;
	Ice::Long _topIndex;
	IceUtil::Mutex _mutex;
};

class IndexLoader;
typedef IceUtil::Handle<IndexLoader> IndexLoaderPtr;

class IndexLoader: public IceUtil::Shared, public IceUtil::Monitor<
		IceUtil::Mutex> {
public:
	IndexLoader() :
		_count(0) {
	}
	;

	FeedIndexSeq query();

	void addQuery(const string& dbInstance, const string& sql,
			const string& pattern, bool isSelect = true);

	void finish(FeedIndexSeq& indexSeq) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
		_indexSeq.insert(_indexSeq.end(), indexSeq.begin(), indexSeq.end());
		if (--_count == 0) {
			//MCE_INFO("IndexLoader --> finish  res.size:"<<_indexSeq.size());
			notify();
		};
	}
	void finish() {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
		if (--_count == 0) {
			notify();
		};
	}
private:
	int _count;
	vector<MyUtil::TaskPtr> _taskSeq;
	FeedIndexSeq _indexSeq;

	class SqlExecuteTask: public MyUtil::Task {
	public:
		SqlExecuteTask(const IndexLoaderPtr& loader, bool isSelect) :
			Task(), _loader(loader), _isSelect(isSelect) {
		}
		;

		void init(const string& dbInstance, const string& sql,
				const string& pattern) {
			_sql << sql;
			_dbInstance = dbInstance;
			_pattern = pattern;
		}
		;

		virtual void handle();

	private:
		IndexLoaderPtr _loader;
		bool _isSelect;
		Statement _sql;
		string _dbInstance;
		string _pattern;
	};
};

}
;
}
;

#endif /* DBHELPER_H_ */
