#include "ExpressionCache.h"
#include "FeedDispatcher.h"
#include "Util.h"
#include "QueryRunner.h"
#include "IceLogger.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <ostream>

using namespace MyUtil;
using namespace std;
using namespace xce::feed;

//解除对某个表达式的引用
void FactorEntry::DereferExprEntry(const ExprEntryPtr & expr) {
  expr_set_.erase(expr);
  if (expr_set_.empty()) {
    ExprCache::instance().DeleteFactorEntry(name_);
  }
}

//---------------------------------------------------------------------------

void ExprEntry::NotifyRemove() {
  //通知与自己相关的所有factor，把factor中关于自己的条目删除，
  for (vector<FactorEntryPtr>::iterator itr = factors_.begin(); itr
      != factors_.end(); ++itr) {
    (*itr)->DereferExprEntry(this);
  }
}
//---------------------------------------------------------------------------

//根据表达式计算id
set<int> ExprCache::Expr2Ids(string & inputExpr, int actor) {

  //删除空格
  if (inputExpr.find(" ") != inputExpr.npos) {
    MCE_WARN("ExprCache::Expr2Ids --> The expression contains whitespace: " << inputExpr);
    inputExpr.erase(remove(inputExpr.begin(), inputExpr.end(), ' '),
        inputExpr.end());
  }
  set<int> result;
  bool in_cache = false;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    ExprIndex & index = expr_evictor_.get<1> ();
    ExprIndex::iterator eit = index.find(inputExpr);//用表达式来查找
    if (eit != index.end()) {
      in_cache = true;
      SeqIndex::iterator sit = expr_evictor_.project<0> (eit);//转换到顺序排序
      expr_evictor_.relocate(expr_evictor_.get<0> ().begin(), sit);
      ++hit_count_;
      ++find_count_;
      result = (*eit)->ids();
      MCE_DEBUG("ExprCache::Expr2Ids --> this expression = " << inputExpr << " is in expression-cache. ids size = " << result.size() );
    }
    MCE_DEBUG("ExprCache::Expr2Ids --> this expression = " << inputExpr << " is Not in expression-cache");
  }
  if(in_cache && result.size()>0){
   FiltFriend(actor, result);//过滤不接受的id
   return result;
  }


  //cache中没有这个表达式，计算id。
  Parser parser;
  parser.Init(inputExpr);
  vector<FactorPtr> factors = parser.Tokenize(); //解析成Factor
  result = parser.CalculateIds(); //计算表达式

  MCE_DEBUG("ExprCache::Expr2Ids --> After parser.CalculatedIds, the target size = " << result.size());

  //将表达式和因子加入各自的cache
  {
    IceUtil::Mutex::Lock lock(mutex_);
    //判断表达式含有的factor是否全部是user factor
    bool AllU = true;
    for (vector<FactorPtr>::iterator itr = factors.begin(); itr
        != factors.end(); ++itr) {
      if ((*itr)->type() != 'u') {
        AllU = false;
      }
    }
    if (AllU == false) {//不全为user，要cache
      ++expr_total_count;
      ExprEntryPtr exprEntry = new ExprEntry(inputExpr, result); //构造表达式entry
      for (vector<FactorPtr>::iterator itr = factors.begin(); itr
          != factors.end(); ++itr) {
        if ((*itr)->type() != 'u') { //如果不是ufactor，则存入cache
          FactorEntryPtr fp = FactorFactory(*itr);//由factor构造factorEntry,这个factorEntry有可能已经存在
          fp->AddExprEntry(exprEntry);//在factor中加入对应的表达式entry
          exprEntry->AddFactorEntry(fp); //将因子entry的引用加入其属于的表达式entry
        }
      }
      AddExprToEvictor(exprEntry);//将计算的新表达式放入Evictor
      ++find_count_;
    }
  }
  FiltFriend(actor, result);//过滤不接受的id
  return result;
}

bool ExprCache::InvalidateFactor(const string & inputName) {
  MCE_DEBUG("ExprCache::InvalidateFactor --> factor name: " << inputName);
  {
    IceUtil::Mutex::Lock lock(mutex_);
    map<string, FactorEntryPtr>::iterator fitr =
        str_factor_map_.find(inputName);
    if (fitr == str_factor_map_.end()) { //是否有这个factor
      MCE_DEBUG("ExprCache::InvalidateFactor --> There is no factor named : " << inputName);

      return false;
    }
    FactorEntryPtr fp = fitr->second; //建立一个临时指针
    set<ExprEntryPtr> tmpset = fp->expr_set(); //tmpset保存包含该factor的所有表达式
    for (set<ExprEntryPtr>::iterator itr = tmpset.begin(); itr != tmpset.end(); ++itr) { //取到这个FactorEntry里面包含的所有表达式
      DeleteExprEntry((*itr)->expr());//删除这个表达式
      /*      ExprIndex& index = expr_evictor_.get<1> ();//删除evictor中的表达式。
       index.erase((*itr)->expr());*/
    }

    return true;
  }
}

void ExprCache::AddExprToEvictor(const ExprEntryPtr & ep) {

  std::pair<ExprEntryList::iterator, bool> p = expr_evictor_.push_front(ep);
  if (!p.second) {
    expr_evictor_.relocate(expr_evictor_.begin(), p.first);//
  } else {
    Kick();
  }
}

void ExprCache::Kick() {
  while (expr_evictor_.size() > kEvictorSize) {//表达式个数超过evictor大小

    ExprEntryPtr p = expr_evictor_.back();//找到evictor最后一个表达式
    expr_evictor_.pop_back();
    p->NotifyRemove();
    MCE_DEBUG("ExprCache::Kick --> evictor size " << expr_evictor_.size() << ", exceed kEvictorSize --> kick expr:"<< p->expr())
    //    DeleteExprEntry(p->expr());//删除这个表达式,并且更新表达式相关factor;不需要 pop_back，因为DeleteExprEntry执行做了类似操作。
  }
}

//这个函数有两个作用：1.通知与表达式相关的factor与自己解引用，2.把自己从evictor中清除
bool ExprCache::DeleteExprEntry(const string & inputExpr) {
  ExprIndex & index = expr_evictor_.get<1> ();
  ExprIndex::iterator eit = index.find(inputExpr);//用表达式来查找
  if (eit == index.end()) { //evictor中没有这个expr
    return false;
  }
  ExprEntryPtr p = *eit;
  index.erase(eit);//删除inputExpr
  p->NotifyRemove();//通知与inputExpr相关的factor

  return true;

}

void ExprCache::DeleteFactorEntry(const string & inputFactor) {
  str_factor_map_.erase(inputFactor);
}

FactorEntryPtr ExprCache::FactorFactory(const FactorPtr & inputFactor) {
  char type = inputFactor->type(); //用factor的类型和id生成字符串作为索引
  int param_id = inputFactor->param_id();
  ostringstream ios;
  ios << type << '(' << param_id << ')';
  string key = ios.str();
  map<string, FactorEntryPtr>::iterator itr = str_factor_map_.find(key);
  if (itr != str_factor_map_.end()) {
    return itr->second;
  }
  FactorEntryPtr fp = new FactorEntry(key);
  str_factor_map_.insert(make_pair(key, fp));
  ++factor_total_count;

  return fp;
}

//过滤的好友的id
void ExprCache::FiltFriend(int actor, set<int> & result) {
  //  ostringstream oss;
  //  oss << "select source from relation_feed_low where target= " << actor_;
  Statement sql;
  sql << "select source from relation_feed_low where target= " << actor;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner("feedwb", CDbRServer).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" ExprCache::FiltFriend --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" ExprCache::FiltFriend --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("ExprCache::FiltFriend --> StoryQueryResult wrong");
    return;
  }
  MCE_INFO("ExprCache::FiltFriend --> actor : " << actor<< ", has been blocked by : " << res.size() << " people" );
  for (size_t i = 0; i < res.size(); ++i) {
    Ice::Int userId = res.at(i).at(0);
    result.erase(userId);
  }
}
//---------------------------------------------------------------------------
void SpecialFilter::Filt(set<int> & targets) {
  set<int> tmpset;

  IceUtil::Mutex::Lock lock(mutex_);
  //  set_intersection(targets.begin(),targets.end(), ids_.begin(), ids_.end(),std::inserter(tmpset,tmpset.begin()));
  for (set<int>::iterator sit = targets.begin(); sit != targets.end(); ++sit) {
    if (ids_.find(*sit) != ids_.end()) {
      tmpset.insert(*sit);
    }
  }
  targets.swap(tmpset);
}

void SpecialFilter::Load() {
  Statement sql;
  sql << "SELECT id from newsfeed_specific_accepter";
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner("feed_db", CDbRServer, "newsfeed_specific_accepter").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" SpecialFilter::Load --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" SpecialFilter::Load --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("SpecialFilter::Load --> StoryQueryResult wrong");
    return;
  }
  set<int> tmpset;
  for (size_t i = 0; i < res.size(); ++i) {
    tmpset.insert(res.at(i).at(0));
  }
  IceUtil::Mutex::Lock lock(mutex_);
  {
    ids_.swap(tmpset);
  }
  MCE_DEBUG("SpecialFilter::Load --> result size = " << ids_.size());
}

//---------------------------------------------------------------------------
void LoadTask::handle() {
  SpecialFilter::instance().Load();
}
//---------------------------------------------------------------------------
//统计factor中包含的expr和expr中包含的factor是否有数量上不同，如果不同，说明有内存泄露
void PrintTask::handle() {

  IceUtil::Mutex::Lock lock(ExprCache::instance().mutex_);
  set<string> factors, exprs_in_factors, exprs, factors_in_exprs;
  for (map<string, FactorEntryPtr>::iterator mit =
      ExprCache::instance().str_factor_map_.begin(); mit
      != ExprCache::instance().str_factor_map_.end(); ++mit) {
    factors.insert(mit->first);
    const set<ExprEntryPtr>& exprset = mit->second->expr_set();
    for (set<ExprEntryPtr>::const_iterator sit = exprset.begin(); sit
        != exprset.end(); ++sit) {
      exprs_in_factors.insert((*sit)->expr_);
    }
  }
  size_t total_ids_count = 0;
  SeqIndex & sIndex = ExprCache::instance().expr_evictor_.get<0> ();
  for (SeqIndex::iterator itr = sIndex.begin(); itr != sIndex.end(); ++itr) {
    ExprEntryPtr e = *itr;
    total_ids_count += (*itr)->size();
    exprs.insert(e->expr_);
    vector<FactorEntryPtr> vvv = e->factors();
    for (size_t i = 0; i < vvv.size(); ++i) {
      string name = vvv[i]->name();
      factors_in_exprs.insert(name);
    }
  }

  MCE_DEBUG("PrintTask::handle--> factors size = "<< factors.size()
      << ",factors_in_exprs size = " << factors_in_exprs.size()
      << ",exprs size = " << exprs.size()
      << ",exprs_in_factors = " << exprs_in_factors.size()
      << ",hit = " << ExprCache::instance().hit_count_
      << ",find = " << ExprCache::instance().find_count_ //查询次数，不包括只含U的表达式查询
      << ",hit rate = " << 1.0 * 100 * ExprCache::instance().hit_count_ / ExprCache::instance().find_count_ << "%"
      << ",Total cached ids size = " << total_ids_count
      << ",factor_destructed_count = " << factor_destructed_count
      << ",factor_total_count = " << factor_total_count
      << ",expr_destructed_count = " << expr_destructed_count
      << ",expr_total_count = " << expr_total_count
      << ",str_factor_map_ size = " << ExprCache::instance().FactorMapSize()
      << ",expr_evictor_ size = " << ExprCache::instance().ExprEvictorSize()
  );
  ExprCache::instance().hit_count_ = 0;
  ExprCache::instance().find_count_ = 0;
  if (factors.size() != factors_in_exprs.size()) {
    MCE_WARN("PrintTask::handle--> factors.size() != factors_in_exprs.size() !!!!!!");
  }
  if (exprs.size() != exprs_in_factors.size()) {
    MCE_WARN("PrintTask::handle--> exprs.size() != exprs_in_factors.size() !!!!!!");
  }
}
