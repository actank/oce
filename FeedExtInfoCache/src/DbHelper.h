/*
 * DBHelper.h
 *
 *  Created on: Dec 24, 2009
 *      
 */

#ifndef __DBHELPER_H__
#define __DBHELPER_H__

//#include "ServiceI.h"
#include <iostream>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

//#include "QueryRunner.h"
//#include "Singleton.h"
//#include "RFeed.h"
//#include "IceLogger.h"
namespace xce {
namespace feed {
namespace count {

//using namespace com::xiaonei::xce;
//using namespace MyUtil;

//const string DB_INSTANCE = "feed_db";
//const string TAB_ID_SEQ = "feed_id_seq";

/*#define COL_FEED "feed"
#define COL_SOURCE "source"
#define COL_SMALLTYPE "small_type"
#define COL_PSOURCE "psource"
#define COL_PTYPE "ptype"
#define COL_ACTOR "actor"
#define COL_NEWS_MERGE "news_merge"
#define COL_MINI_MERGE "mini_merge"
#define COL_TIME "time" */

//const int LOAD_TIMEOUT = 120;
//const int ID_SEQ_STEP=1000;

class DbHelper{
public:
  static const int kTableCount = 100;
//  static const int kDbCountStrLength = 150;
  static const std::string kDbTableName; 
  static const std::string kDbLogicName; ///////////////
  static const std::string kColSource;
  static const std::string kColActor;
  static const std::string kColStype;
  static const std::string kColCount;
  static const std::string kColIndex;

  static std::string GetTableName(Ice::Long source){
    std::ostringstream os;
    os << kDbTableName << "_" << source % kTableCount;
    return os.str();
  }
  static std::string GetExpr(Ice::Long source){
    std::ostringstream os;
    os << "feed_count_" << source % kTableCount; ///////////////////
    return os.str();
  }
};


/*class ParallelLoader;
typedef IceUtil::Handle<ParallelLoader> ParallelLoaderPtr;

class ParallelLoader: public IceUtil::Shared, public IceUtil::Monitor<
		IceUtil::Mutex> {
public:
	ParallelLoader() :
		_count(0) {
		_result = new ObjectResult;
	}
	;

	bool query(ObjectResultPtr& res);

	void addQuery(const string& dbInstance, const string& sql,
			const string& pattern, bool isSelect = true);
	void finish() {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
		if (--_count == 0) {
			notify();
		}
	}
	void finish(const ObjectResultPtr& res) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
		_result->data.insert(res->data.begin(), res->data.end());
		if (--_count == 0) {
			notify();
		}
	}
private:
	int _count;
	vector<MyUtil::TaskPtr> _taskSeq;
	FeedIndexSeq _indexSeq;
	ObjectResultPtr _result;
	

	class SqlExecuteTask: public MyUtil::Task {
	public:
		SqlExecuteTask(const ParallelLoaderPtr& loader, bool isSelect) :
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
		ParallelLoaderPtr _loader;
		bool _isSelect;
		Statement _sql;
		string _dbInstance;
		string _pattern;
	};
};
*/

/*class FeedInteractionFactoryI : public MyUtil::Singleton<FeedInteractionFactoryI>{
  public:
    Ice::ObjectPtr create(Ice::Long id);
    MyUtil::ObjectResultPtr create(const MyUtil::LongSeq& ids);
};*/

}
}
}
#endif /* __DBHELPER_H__ */
