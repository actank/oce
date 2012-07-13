/*
 * ResultCache.h
 *
 *  Created on: Dec 13, 2010
 *      Author: yejingwei
 */

#ifndef RESULTCACHE_H_
#define RESULTCACHE_H_
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

#include "Singleton.h"
#include "TaskManager.h"
#include <map>
#include <vector>
#include <IceUtil/IceUtil.h>
#include "RFeed.h"
#include "Date.h"
#include "IceLogger.h"

using namespace std;
using namespace MyUtil;
using namespace xce::feed;
using namespace boost::multi_index;

namespace xce{
namespace feed{

const size_t kPool = 100;//TODO
const size_t kScanTime = 4 * 60 * 1000;
const size_t kValidTimeInSecond =  5 * 60;

//-----------------------------------------------------
class Result: public IceUtil::Shared{
public:
  Result(FeedDataSeq & data,int uid):data_(data),uid_(uid){
    stamp_ = MyUtil::Date(time(NULL)).toSeconds();
    MCE_DEBUG("Result::Result,stamp_:" << stamp_ << ",data size:"<< data.size() << ",uid:" << uid);
  }
  FeedDataSeq data_;
  Ice::Int uid_;
  IceUtil::Int64 stamp_;
};
typedef IceUtil::Handle<Result> ResultPtr;
//-----------------------------------------------------

class ResultPool: public IceUtil::Shared{
public:
  ResultPtr Get(int uid);
  void Add(const ResultPtr & r) ;
  void Del(int uid);
  void Scan();
  int Size();
private:
  ///////////
  struct uidtag{};
  struct timetag{};
  typedef multi_index_container<
    ResultPtr,
    indexed_by<
          hashed_unique<tag<uidtag>,BOOST_MULTI_INDEX_MEMBER(Result, int,uid_)>,
          ordered_non_unique<tag<timetag>,BOOST_MULTI_INDEX_MEMBER(Result, time_t,stamp_)>
          >
  > ResultMI;
  typedef ResultMI::index<uidtag>::type UidIndex;
  typedef ResultMI::index<timetag>::type TimeIndex;
  ////////////

  ResultMI mi_;
  IceUtil::Mutex mutex_;
};
typedef IceUtil::Handle<ResultPool> ResultPoolPtr;
//-----------------------------------------------------
class ResultCache: public Singleton<ResultCache>{
public:
  void Init();
  ResultPtr Get(int uid,int begin,int limit);
  void Add(const ResultPtr & r);
  void Del(int uid);
  void Scan();
private:
  int GetIndex(int uid);
  vector<ResultPoolPtr> pools_;

  long gets_;
  long hits_;
};

//-----------------------------------------------------
class ScanTask: public Timer {
public:
  ScanTask() : Timer(kScanTime) { }
  virtual void handle();
};

}
}
#endif /* RESULTCACHE_H_ */
