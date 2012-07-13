#ifndef _REPLYNOTEPOOL_H_
#define _REPLYNOTEPOOL_H_

#include "Singleton.h"
#include "Notify.h"
#include "QueryRunner.h"
#include "ObjectCache.h"
#include "ServiceI.h"
#include "NotifyContent.h"

namespace xce {
namespace notify {


using namespace MyUtil;
using namespace com::xiaonei::xce;

const int REPLY_CACHE_SIZE = 10000000;
const int LOCK_POOL_SIZE   = 2000000;
const int CLUSTER          = 5; // 服务散的份数

class NotifyCache: public  MyUtil::Singleton<NotifyCache>
{
public:
	//在cahce中查找, 如果已经删除，返回-id，如果没有找到返回NULL
	NotifyContentPtr getNotifyContent(Ice::Long replyId);
	void putNotifyContent(const NotifyContentPtr& reply);//尝试压缩
	void addNotifyContent(const NotifyContentPtr& reply);//不尝试压缩
	bool removeNotifyContent(Ice::Long replyId);
	void cleanCacheByTime();
	//如果不能压缩。返回空字符串，否则返回压缩后的字符串
	string compress(const string& data) const;
private:
	int _getNotifyContentIndex(Ice::Long replyId){
		return (int)((replyId/CLUSTER)%REPLY_CACHE_SIZE);
	}
	int _getMutexIndex(Ice::Long replyId){
		return (int)((replyId/CLUSTER)%LOCK_POOL_SIZE);
	}
	NotifyContentPtr _cache[REPLY_CACHE_SIZE];
	time_t    _loadTime[REPLY_CACHE_SIZE];
	IceUtil::Mutex _mutexPool[LOCK_POOL_SIZE];
};

class NotifyFactory: public MyUtil::Singleton<NotifyFactory>{
public:
	ObjectResultPtr createNotifyContent( const MyUtil::LongSeq& replyIds );
	NotifyContentDict createNotifyContentParal( const MyUtil::LongSeq& replyIds );
	void deleteNotifyContent( const MyUtil::LongSeq& replyIds );
private:
	
	int _getTableCount(){
		return 100;
	}
	const string _getTableName( int tableIndex ){
		ostringstream tableName;
		tableName << "content_" << tableIndex % 100;
		return tableName.str();
	}
	const string _getInstance() {
		return "notify";
	}
	const string _getExpr( int tableIndex ) {
		ostringstream expr;
		expr << "content_" << tableIndex % 100;
		return expr.str();
	}
	class ResultHandlerI: public com::xiaonei::xce::ResultHandler {
	public:
		ResultHandlerI(ObjectResultPtr result):_result(result){;}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		ObjectResultPtr _result;
	};
};


class ParaLoader;
typedef IceUtil::Handle<ParaLoader> ParaLoaderPtr;


class ParaLoader: public IceUtil::Shared, public IceUtil::Monitor<IceUtil::Mutex> {
public:
  ParaLoader() :  _count(0) ,not_exist_(0){ }

  NotifyContentDict query(int & not_exist);

  void addQuery(const string& dbInstance, const string& sql, const string& pattern,const vector<Ice::Long> & fids);

  void finish(NotifyContentDict & ndict,int  not_exist) {
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    ndict_.insert(ndict.begin(), ndict.end());
    not_exist_ += not_exist;
    if (--_count == 0) { notify();  };
  }
  void finish(int  not_exist) {
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    not_exist_ += not_exist;
    if (--_count == 0) {       notify();    };
  }
private:
  int _count;
  int not_exist_;
  vector<MyUtil::TaskPtr> _taskSeq;
  NotifyContentDict ndict_;

  class SqlExecuteTask: public MyUtil::Task {
  public:
    SqlExecuteTask(const ParaLoaderPtr& loader) :
      Task(), _loader(loader) {//TODO
    }

    void init(const string& dbInstance, const string& sql,
        const string& pattern,vector<Ice::Long> fids) {
      _sql << sql;
      _dbInstance = dbInstance;
      _pattern = pattern;
      fids_.insert(fids.begin(),fids.end());
    }

    virtual void handle();

  private:
    ParaLoaderPtr _loader;
    Statement _sql;
    string _dbInstance;
    string _pattern;
    set<Ice::Long> fids_;
  };
};


}
}

#endif /* _REPLYCONTENTPOOL_H_ */
