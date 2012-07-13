/*
 * IdCache.h
 *
 *  Created on: Apr 14, 2010
 *      Author: yejingwei
 */

#ifndef __HEADER_EXPRESSIONCACHE__H__
#define __HEADER_EXPRESSIONCACHE__H__


#include "Singleton.h"
#include "ExpressionParser.h"
#include "TaskManager.h"
#include <IceUtil/Mutex.h>
#include <algorithm>
#include <map>
#include <set>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include "LogWrapper.h"

namespace xce {
namespace feed {

using namespace MyUtil;
using namespace std;
using namespace boost::multi_index;

static int expr_destructed_count;
static int expr_total_count;
static int factor_destructed_count;
static int factor_total_count;

const size_t kEvictorSize = 10000 ; //表达式cache大小
class ExprCache;

class FactorEntry;
typedef IceUtil::Handle<FactorEntry> FactorEntryPtr;

class ExprEntry: public IceUtil::Shared {
public:

  ExprEntry(const string& expr, const set<int>& ids) :
    expr_(expr), ids_(ids) {
  }

  set<int> ids() {
    return ids_;
  }

  size_t size(){
    return ids_.size();
  }
  void AddFactorEntry(const FactorEntryPtr& f) {
    factors_.push_back(f);
  }

  void NotifyRemove();

  string expr() {
    return expr_;
  }

  ~ExprEntry() {
    MCE_DEBUG("ExprEntry:" << expr_ << " destructed " );
    ++expr_destructed_count;
  }

  bool operator<(const ExprEntry & lhs) const {
    if (expr_ < lhs.expr_) {
      return true;
    } else {
      return false;
    }
  }
  vector<FactorEntryPtr> factors(){
    return factors_;
  }
  string expr_;
private:

  vector<FactorEntryPtr> factors_;
  set<int> ids_;
};

typedef IceUtil::Handle<ExprEntry> ExprEntryPtr;

/*namespace std {

 template<> struct less<ExprEntryPtr> : public binary_function<ExprEntryPtr,
 ExprEntryPtr, bool> {
 bool operator()(const ExprEntryPtr& e1, const ExprEntryPtr& e2) const {
 return e1.get() < e2.get();
 }
 };
 }*/
//---------------------------------------------------------------------------
class FactorEntry: public IceUtil::Shared {
public:
  FactorEntry(const string & name) :
    name_(name) {
  }
  void DereferExprEntry(const ExprEntryPtr & expr);

  void AddExprEntry(const ExprEntryPtr& expr) {
    expr_set_.insert(expr);
  }

  string name() {
    return name_;
  }
  set<ExprEntryPtr> expr_set() {
    return expr_set_;
  }

  int size() {
    return expr_set_.size();
  }
  ~FactorEntry() {
    MCE_DEBUG("FactorEntry:" << name_ << " destructed " );
    ++factor_destructed_count;
  }
  set<ExprEntryPtr> expr_set_;

private:
  string name_;

};

//---------------------------------------------------------------------------

typedef boost::multi_index_container<ExprEntryPtr, indexed_by<sequenced<> ,
    hashed_unique<BOOST_MULTI_INDEX_MEMBER(ExprEntry,string,expr_)
    > > > ExprEntryList;
typedef ExprEntryList::nth_index<0>::type SeqIndex;
typedef ExprEntryList::nth_index<1>::type ExprIndex;

//---------------------------------------------------------------------------

class ExprCache: public Singleton<ExprCache> {
  friend class FactorEntry;
  friend class PrintTask;
public:
  ExprCache(){
    find_count_ = 0;
    hit_count_ = 0;
  }
  bool InvalidateFactor(const string &);
  set<int> Expr2Ids(string & expr, int actor);//计算id


  void DeleteFactorEntry(const string &);//删除factor，有可能被FactorEntry::DereferExprEntry调用，设为公有


  size_t FactorMapSize(){
    return str_factor_map_.size();
  }
  size_t ExprEvictorSize(){
    return expr_evictor_.size();
  }
private:
  void FiltFriend(int actor, set<int> & result);
  void AddExprToEvictor(const ExprEntryPtr&);
  bool DeleteExprEntry(const string &);

  void Kick();
  FactorEntryPtr FactorFactory(const FactorPtr&);
  map<string, FactorEntryPtr> str_factor_map_;//保存所有的factor，以factor对应的字符串为key

  ExprEntryList expr_evictor_;//保存所有的表达式
  IceUtil::Mutex mutex_;

  size_t find_count_;//记录查询次数，不包括全u的表达式
  size_t hit_count_;//命中的次数

};


//---------------------------------------------------------------------------
class SpecialFilter: public Singleton<SpecialFilter> {
public:
  void Filt(set<int> & targets);
  void Load();
private:
  IceUtil::Mutex mutex_;
  set<int> ids_;
};

//---------------------------------------------------------------------------
//定时加载
class LoadTask: public Timer, public Singleton<LoadTask> {
public:
  LoadTask() :
    Timer( 10 * 60 * 1000) { //执行一次
    ;
  }
  virtual void handle();
};

//---------------------------------------------------------------------------

class PrintTask: public Timer {
public:
  PrintTask() :
    Timer(30 * 1000) { //执行一次
    ;
  }
  virtual void handle();
};
}
;
}
;
#endif
