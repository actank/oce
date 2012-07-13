#ifndef _XCE_FEED_MEMC_LOADER_H_
#define _XCE_FEED_MEMC_LOADER_H_

#include <set>

#include "Singleton.h"
#include "FeedMemcProxy.h"
#include "TaskManager.h"
#include "QueryRunner.h"
#include <boost/foreach.hpp>
#include <iostream>
namespace xce {
namespace feed {

using namespace std;
using namespace MyUtil;
using com::xiaonei::xce::Statement;

class FeedContentDB {
public:
  static const string GetTableName(Ice::Long id) {
    ostringstream table;
    table << "feed_content_" << id % 100;
    return table.str();
  }
  static map<int, vector<Ice::Long> > GetIdMap(const vector<Ice::Long> ids) {
    map<int, vector<Ice::Long> > id_map;
    for (unsigned i = 0; i != ids.size(); ++i) {
      id_map[ids.at(i) % 100].push_back(ids.at(i));
    }
    return id_map;
  }
  static const string GetInstance() {
    return "feed_db";
  }

  static const string GetLoadExpr(Ice::Long id) {
    ostringstream expr;
    expr << "feed_content_" << id % 100;
    return expr.str();
  }
};

class LoadFeedTask: public MyUtil::Task {
public:
  LoadFeedTask(Ice::Long id) :
    id_(id) {
  }
  virtual void handle();

private:
  Ice::Long id_;
};

class FeedMemcLoaderI: public MyUtil::Timer,
    public FeedMemcLoader,
    public Singleton<FeedMemcLoaderI> {
public:
  FeedMemcLoaderI();
  void LoadFeed(Ice::Long id, const Ice::Current& = Ice::Current());
  void LoadFeedSeq(const Ice::LongSeq & ids, const Ice::Current& =
      Ice::Current());

  void LoadFeedByKey(const string & key, const Ice::Current& = Ice::Current());
  void LoadFeedSeqByKey(const MyUtil::StrSeq & keys, const Ice::Current& =
      Ice::Current());

  MyUtil::Str2StrMap GetFeedSeqByKey(const MyUtil::StrSeq & keys,
      const Ice::Current& = Ice::Current());

  virtual FeedContentDict GetFeedDict(const vector<Ice::Long> & ids,
      const Ice::Current& = Ice::Current());

  virtual FeedContentDict GetFeedDictTimeout(int uid,const vector<Ice::Long> & ids,int timeout,
      const Ice::Current& = Ice::Current());
  FeedContentDict GetFeedDictSingleThread(const vector<Ice::Long> & ids);
private:
  set<Ice::Long> recent_loaded_ids_;
  IceUtil::Mutex mutex_;

  virtual void handle();
};

class ParaLoader;
typedef IceUtil::Handle<ParaLoader> ParaLoaderPtr;
class ParaLoader: public IceUtil::Shared, public IceUtil::Monitor<
    IceUtil::Mutex> {
public:
  ParaLoader(int uid,int timeout) :
    uid_(uid), _count(0) ,not_exist_(0),timeout_(timeout){
  }

  FeedContentDict query(int & not_exist);

  void addQuery(const string& dbInstance, const string& sql,
      const string& pattern,const vector<Ice::Long> & fids);

  void finish(FeedContentDict& fdict,int  not_exist) {
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    fdict_.insert(fdict.begin(), fdict.end());
    not_exist_ += not_exist;
    //    _indexSeq.insert(_indexSeq.end(), indexSeq.begin(), indexSeq.end());
    if (--_count == 0) {
      //MCE_INFO("ParaLoader --> finish  res.size:"<<_indexSeq.size());
      notify();
    };
  }
  void finish(int  not_exist) {
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    not_exist_ += not_exist;
    if (--_count == 0) {
      notify();
    };
  }
private:
  int uid_;
  int _count;
  int not_exist_;
  vector<MyUtil::TaskPtr> _taskSeq;
  FeedContentDict fdict_;
  int timeout_;

  class SqlExecuteTask: public MyUtil::Task {
  public:
    SqlExecuteTask(int uid,const ParaLoaderPtr& loader) :
      uid_(uid),Task(), _loader(loader) {//TODO
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
    int uid_;
    ParaLoaderPtr _loader;
    Statement _sql;
    string _dbInstance;
    string _pattern;
    set<Ice::Long> fids_;

  };
};

class MemcTask:public MyUtil::Task{
public:
  MemcTask(const FeedContentDict & contents);
  virtual void handle();
  virtual ~MemcTask(){}
private:
  FeedContentDict fdict_;
};


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
  float getTime(bool clear = true){
    gettimeofday(&_end, NULL);
    float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
    -_begin.tv_usec;
    timeuse/=1000;
    if(clear){
      reset();
    }
    return timeuse;
  }

};

class PrintHelper {
public:
  template<typename T>
  static string Print(const T & container,int limit = 30) {
    ostringstream ios;

    ios << "(";
    int idx = 0;
    typedef typename T::value_type vt;
    BOOST_FOREACH(const vt & v,container){
    	if(idx++ > 0) ios << ",";

    	ios << v;

      if(idx >= limit) break;
    }
    ios << ")";

    return ios.str();
  }
};

}
}

#endif // _XCE_FEED_MEMC_LOADER_H_
