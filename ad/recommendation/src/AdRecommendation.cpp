/*
 * AdRecommendation.cpp
 *
 *  Created on: 2010-11-21
 *      Author: sunzz
 */
#include <QueryRunner.h>
#include <exception>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <Ice/Exception.h>
#include <string>
#include <set>
#include "AdRecommendation.h"
#include "AdCache.h"

using namespace xce::ad;
using namespace com::xiaonei::xce;
using namespace boost;

const string UserItemPool::TableName = "ad_uid_gid_app";
void UserItemPool::Init() {//
  UserItemEvictor evictor;
  set<Ice::Long> adset;
  Statement sql;
  size_t max_active_count = GetActiveUserSize();
  sql << "SELECT * FROM "<< GetTableName() <<" limit " << max_active_count;
  mysqlpp::StoreQueryResult res;
  MCE_INFO("UserItemPool::Init");
  try {
    res = QueryRunner("ads_db", CDbRServer, GetTableName()).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("UserItemPool::Init --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("UserItemPool::Init --> error:" << e.what());
  }

  int maxuserid = 0;
  Ice::Long usercount = 0;

  if (res && res.num_rows() > 0) {
    int evictor_max_size = GetSetSize();
    for (int i = 0; i < res.num_rows(); i++) {
      int userid = boost::lexical_cast<int>(res[i]["userid"]);
      MCE_DEBUG("UserItemPool::Init insert DB userid:" << userid);
      if (userid <= 0)
        continue;
      if (userid > maxuserid)
        maxuserid = userid;
      usercount++;
      string result = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)(
          res[i]["gids"]);

      vector < string > group_ids;
      boost::algorithm::split_regex(group_ids, result, regex("\\,"));
      if (group_ids.empty()) //没有点击行为不插入
        continue;

      UserItemPtr upt = new UserItem(); //userid uniq
      upt->userid = userid;
      set<Ice::Long> gids;
      for (vector<string>::iterator it = group_ids.begin(); it
          != group_ids.end(); it++) {
        MCE_DEBUG("UserItemPool::Init-->userid:"
            << userid << ", this groupid = " << *it);
        Ice::Long groupid = 0;
        try {
          groupid = boost::lexical_cast<Ice::Long>(*it);
        } catch (std::exception & e) {
          MCE_WARN("UserItemPool::Init --> sting2 cast error :-"
              << e.what());
        }
        if ((groupid > 0)) {
          gids.insert(groupid);
          adset.insert(groupid);
        }
      }
      upt->gids = gids;
      SeqIndex & sindex = evictor.get<0> ();
      pair<SeqIndex::iterator, bool> p = evictor.push_front(upt); //用户提前
      if (!p.second) {
        sindex.relocate(sindex.begin(), p.first);
      } else {
        if (evictor.size() > evictor_max_size) {
          evictor.pop_back();
        }
      //  Kick();
      }

    }
  }


  {
    IceUtil::Mutex::Lock  lock(mutex_);
    evictor_.swap(evictor);
  }
  MCE_INFO("UserItemPool::Init-->ad size: " << adset.size() << " usercount:" << usercount <<"max userid:" << maxuserid);
  //  MCE_DEBUG("UserItemPool::Init-->ad size: " << adset.size() << ",ad indexsize:" << index_cache_.size());
  {
    string result;
    int i = 0;
    for (set<Ice::Long>::iterator it = adset.begin(); it != adset.end(); ++it) {
      string str = boost::lexical_cast<string>(*it);
      result += ",";
      result += str;
    }
    MCE_DEBUG("UserItemPool::Init-->done ,groupid:" << result);
  }

}
UserItemPtr UserItemPool::Locate(int userid) {

  {
    IceUtil::Mutex::Lock  lock(mutex_);

  //  UserItemEvictor& evictor = GetEvictorSet(userid);
    UidIndex & index = evictor_.get<1> ();
    UidIndex::iterator uit = index.find(userid);
    if (uit != index.end()) {
      SeqIndex::iterator sit = evictor_.project<0> (uit);//新查的放到前面
      evictor_.relocate(evictor_.get<0> ().begin(), sit);
      MCE_DEBUG("UserItemPool::Locate-->userid:" <<userid << " in the cache");
      return *uit;
    }
  }
  MCE_DEBUG("UserItemPool::Locate-->userid:" <<userid << " not in the cache,need load form DB");
  UserItemPtr u = Load(userid);

  IceUtil::Mutex::Lock  lock(mutex_);

//  UserItemEvictor& evictor = GetEvictorSet(userid);
  SeqIndex & sindex = evictor_.get<0> ();
  pair<SeqIndex::iterator, bool> p = evictor_.push_front(u); //用户提前
  if (!p.second) {
    sindex.relocate(sindex.begin(), p.first);
  } else {
    if (evictor_.size() > GetSetSize()) {
      evictor_.pop_back();
    }
  //  Kick();
  }
//  evictor_.push_front(u);
//  if (evictor_.size() > kUserSize) {
//    evictor_.pop_back();
//  }
  return u;
}

UserItemPtr UserItemPool::Load(int userid) {
  MCE_DEBUG("UserItemPool::Load-->userid:" << userid <<" need load from DB");
  UserItemPtr u = new UserItem();
  u->userid = userid;

  Statement sql;
  sql << "SELECT * FROM " << GetTableName() << " where userid = " << userid;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner("ads_db", CDbRServer, GetTableName()).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("UserItemPool::LoadDB --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("UserItemPool::LoadDB --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("UserItemPool::LoadDB --> StoryQueryResult wrong");
  }

  if (res && res.num_rows() > 0) {
    string result = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)(
        res[0]["gids"]);

    vector < string > group_ids;
    boost::algorithm::split_regex(group_ids, result, regex("\\,"));
    for (vector<string>::iterator it = group_ids.begin(); it
        != group_ids.end(); it++) {
      MCE_DEBUG("[INDEXLOG] UserItemPool::Load-->userid:" << userid
          << ", this groupid = " << *it);
      Ice::Long id = 0;
      try {
        id = boost::lexical_cast<Ice::Long>(*it);
      } catch (std::exception & e) {
        MCE_WARN("UserItemPool::LoadGidUidDB --> sting2 cast error :-"
            << e.what());
      }
      if ((id > 0)) {
        u->gids.insert(id);
      }
    }
  }

  return u;

}

vector<int> UserItemPool::LoadGidUsersDB(Ice::Long groupid, int userid) { //userid for debug
  vector<int> vec;
  MCE_DEBUG("UserItemPool::LoadGidUidDB-->gids:" << groupid);

  Statement sql;
  sql << "SELECT * FROM ad_gid_uid where gids = " << groupid;
  mysqlpp::StoreQueryResult res;

  try {
    res = QueryRunner("ads_db", CDbRServer, "ad_gid_uid").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("UserItemPool::LoadGidUidDB --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("UserItemPool::LoadGidUidDB --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("UserItemPool::LoadGidUidDB --> StoryQueryResult wrong");
  }

  if (res && res.num_rows() > 0) {
    string result = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)(
        res[0]["userid"]);

    vector < string > user_ids;
    boost::algorithm::split_regex(user_ids, result, regex("\\,"));
    for (vector<string>::iterator it = user_ids.begin(); it
        != user_ids.end(); it++) {
      //  MCE_DEBUG("[INDEXLOG] UserItemPool::LoadGidUidDB--> new userid = "
      //      << userid << ", gids:" << groupid << ", this userid = " << *it);
      int id = 0;
      try {
        id = boost::lexical_cast<int>(*it);
      } catch (std::exception & e) {
        MCE_WARN("UserItemPool::LoadGidUidDB --> sting2 cast error :-"
            << e.what());
      }
      if (id > 0) {
        vec.push_back(id);
      }
    }
  }

  return vec;
}

void UserItemPool::UpdateClick(int userid, Ice::Long groupid) {
  MCE_INFO("UserItemPool::UpdateClick: userid:" <<userid << " groupid:" << groupid);
  UserItemPtr u = Locate(userid);

  if (!u->gids.count(groupid)) {

    UserItemPtr tmp = new UserItem();
    tmp -> userid = u->userid;
    tmp->gids = u->gids;
    tmp->gids.insert(groupid);
    {
      IceUtil::Mutex::Lock  lock(mutex_);

//      UserItemEvictor& evictor = GetEvictorSet(userid);
      UidIndex & index = evictor_.get<1> ();
      UidIndex::iterator uit = index.find(userid);
      index.replace(uit, tmp);

      SeqIndex::iterator sit = evictor_.project<0> (uit);//更新之后放到前面
      evictor_.relocate(evictor_.get<0> ().begin(), sit);
    }
    ///TODO to insert db
    ///xxx.push_back(tmp);
    TaskManager::instance().execute(new UpdateTask(tmp));

  }



}

void UpdateTask::UpdateUidGid() {
  MCE_INFO("UpdateTask::UpdateUidGid execute");
  Statement sql;
  ostringstream os;
  for(set<Ice::Long>::iterator it = upt->gids.begin(); it != upt->gids.end(); ++it) {
    if(it != upt->gids.begin()){
      os << ",";
    }
    os << *it;
  }
  //sql <<"update ad_uid_gid set gids = " << mysqlpp::quote << os.str() << "where userid = " << upt->userid;
  string tablename = UserItemPool::instance().GetTableName(); 
  sql <<"insert into " << tablename << " (userid, gids) values(" << upt->userid << ","
      << mysqlpp::quote << os.str() << ")"
      << "on duplicate key update userid=values(userid),gids=values(gids)";
  try {
    QueryRunner("ads_db", CDbWServer,tablename).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("UpdateTask::UpdateGidUid Set--> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("UpdateTask::UpdateGidUid Set--> error:" << e.what());
  }
}

vector<Ice::Long> UserItemPool::GetUserItemValue(int userid) {//为了测试
////  UserItemPtr upt = GetUserItemPtr(userid);
  MCE_DEBUG("UserItemPool::GetUserItemValue-->userid: " << userid );
  vector<Ice::Long> vec;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    UidIndex & index = evictor_.get<1> ();
    UidIndex::iterator uit = index.find(userid);
    if (uit != index.end()) {
      set<Ice::Long>& gids = (*uit)->gids;
      for (set<Ice::Long>::iterator it = gids.begin(); it != gids.end(); ++it) {
        vec.push_back(*it);
      }

    }
  }
  return vec;

}

vector<int> UserItemPool::GetGidUser(Ice::Long groupid) {//为了测试
  vector<int> result;

  IceUtil::Mutex::Lock  lock(mutex_);

  UidIndex & index = evictor_.get<1> ();
  UidIndex::iterator uit = index.begin();
  for (; uit != index.end(); ++uit) {
    set<Ice::Long>& gids = (*uit)->gids;
    set<Ice::Long>::iterator it = gids.find(groupid);
    if (it != gids.end()) {
    //if ((*uit)->IsClickGid(groupid)) {
      result.push_back((*uit)->userid);
    }
  }

  return result;

}
/*double UserItemPool::GetUserDistance(int userid1, int userid2) {//
  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex & index = evictor_.get<1> ();
  UidIndex::iterator uit1 = index.find(userid1);
  UidIndex::iterator uit2 = index.find(userid2);
  double result = 0;
  if ((uit1 != index.end()) && (uit2 != index.end())) {
    //    cout << "UserItemPool::GetUserDistance: find" <<endl;
    result = GetUserDistance(*uit1, *uit2);
    return result;
  }
  // cout << "UserItemPool::GetUserDistance: not find" <<endl;
  return result;

}*/
///这个函数可以适合于其他item项。item不一定是0、1
/*double UserItemPool::GetUserSimValue(UserItemPtr userptrx,
 UserItemPtr userptry) {
 if (!(userptrx && userptry)) {
 //cout << "wrong" << endl;
 return 0;
 }
 double sumxy = 0;
 double sumxsq = 0;
 double sumysq = 0;
 int itemsize = index_cache_.GetMaxIndex();
 int comcount = 0;
 // MCE_DEBUG("UserItemPool::GetUserSimValue-->itemsize:" << itemsize);
 for (int i = 0; i <= itemsize; i++) {
 double x = userptrx->GetItemValue(i);
 double y = userptry->GetItemValue(i);

 sumxy += x * y;
 sumxsq += x * x;
 sumysq += y * y;
 }

 if (sumxsq == 0 || sumysq == 0) {
 return 0;
 }

 if((userptrx->itemset_ & userptry->itemset_) == (userptry->itemset_)) {//delete subset//test

 MCE_DEBUG("UserItemPool::GetUserSimValue-->inter userid:"<<userptrx->userid
 <<" has subset:"<<",userid:"<<userptry->userid << ",comcount:" << userptry->GetCount());
 return 0;
 }

 return sumxy / sqrt(sumxsq * sumysq);

 }*/


void UserItemPool::GetActiveUsers(vector<UserItemPtr> & vec) {
  MCE_INFO("UserItemPool::GetActiveUsers:begin");
  int count = 0;

  IceUtil::Mutex::Lock lock(mutex_);
  SeqIndex& index = evictor_.get<0>();
  MCE_INFO("UserItemPool::GetActiveUsers line:" << __LINE__ << ", evictor_ size = " << evictor_.size());

  size_t max_active_count = GetActiveUserSize();
  for(SeqIndex::iterator it = index.begin(); it != index.end(); ++it){
    if (!(*it)) {
      MCE_WARN("NULL evictor_ item");
      continue;
    } 
    if((*it)->gids.empty()){
      continue;
    }
    vec.push_back(*it);
    if (++count > max_active_count) {
      return;
    }
  }
  MCE_INFO("UserItemPool::GetActiveUsers line:" << __LINE__ << ", vec size = " << vec.size());

}

void UserItemPool::CleanClick(int userid) {
  IceUtil::Mutex::Lock  lock(mutex_);
  MCE_INFO("UserItemPool::CleanClick-->userid:" << userid << "clean his click");

//  UidIndex & uindex = GetEvictor(userid).get<1> ();
  UidIndex & uindex = evictor_.get<1> ();
  UidIndex::iterator uit = uindex.find(userid);
  if (uit != uindex.end()) {
    UserItemPtr u = new UserItem();
    u->userid = userid;
    uindex.replace(uit, u);


  }
}

//-----------------------------------------------------------------------------

void AdRecommend::UpdateSet() {
  MCE_INFO("before AdRecommend::UpdateSet list_ size:" << list_->list.size());
  //  list_ = UserItemPool::instance().GetActiveUsers();
  try {
    UserItemListPtr c = new UserItemList();
    if (!c) {
      MCE_WARN("AdRecommend::UpdateSet c is NULL! line:" << __LINE__);
      return;
    }
    UserItemPool::instance().GetActiveUsers(c->list);
    list_ = c;
  } catch (Ice::Exception& e) {
    MCE_WARN("UserItemPool::instance().GetActiveUsers--> ERR  line:"
        << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("UserItemPool::instance().GetActiveUsers ERR  line:"
        << __LINE__ << " err:" << e.what());
  }

  MCE_INFO("after AdRecommend::UpdateSet list_ size:" << list_->list.size());
}

double AdRecommend::GetUserDistance(int userid1, int userid2) {//

  UserItemListPtr c = list_;
  UserItemPtr upt1;
  UserItemPtr upt2;
  vector<UserItemPtr>::iterator it = c->list.begin();
  for (; it != c->list.end(); ++it) {
    if ((*it)->userid == userid1) {
      upt1 = *it;
    }
    if ((*it)->userid == userid2) {
      upt2 = *it;
    }

  }

  double result = 0;
  if ((!upt1) && (!upt2)) {
    //    cout << "UserItemPool::GetUserDistance: find" <<endl;
    MCE_DEBUG("both users in the computher cache");
    result = GetUserDistance(upt1, upt2);
    return result;
  }
  MCE_DEBUG("not both users in the computher cache");
  // cout << "UserItemPool::GetUserDistance: not find" <<endl;
  return result;

}

double AdRecommend::GetUserDistance(UserItemPtr& userptrx, //just for item is 0 or 1
    UserItemPtr& userptry) {
  set<Ice::Long>& gidsx = userptrx->gids;
  set<Ice::Long>& gidsy = userptry->gids;

  if (gidsx.empty() || gidsy.empty()) {
    return 0.0;
  }

  long sumxy = 0;
  long sumxsq = gidsx.size();
  long sumysq = gidsy.size();
  set<Ice::Long>::iterator itx = gidsx.begin();
  set<Ice::Long>::iterator ity = gidsy.begin();
  for (; (itx != gidsx.end()) && (ity != gidsy.end());) {
    if ((*itx) == (*ity)) {
      sumxy += 1;
      ++itx;
      ++ity;
    } else if ((*itx) > (*ity)) {
      ++ity;
    } else {
      ++itx;
    }
  }

  if (sumxy == sumysq) {//delete subset//test
    MCE_DEBUG("UserItemPool::new GetUserDistance-->inter userid:"<<userptrx->userid
        <<" has subset:"<<",userid:"<<userptry->userid << ",comcount:" << userptry->gids.size());
    return 0.0;
  }
  return (double)sumxy / sqrt((double)sumxsq * (double)sumysq);
}

multimap<double, UserItemPtr> AdRecommend::GetKNNeighbour(UserItemPtr& upt) {//可以考虑设置门限，不一定要是指定的k
  TimeStat ts;
  multimap<double, UserItemPtr> result;
  multimap<double, UserItemPtr> tmp;

  if ((!upt) || (upt->gids.empty())) {
    MCE_INFO("AdRecommend::GetKNNeighbour-->uid:" << upt->userid << "has no click");
    return result;
  }
  UserItemListPtr c = list_;
  vector<UserItemPtr>::iterator otherit = c->list.begin();
  int excellentcount = 0;
  double sim_threshold = GetSimThreshold();
  for (; otherit != c->list.end(); ++otherit) {
    if ((*otherit)->userid == upt->userid) { // 不计算自己和自己的distance
      continue;
    }
    double distance = GetUserDistance(upt, (*otherit));
    if ((distance != 1) && (distance > sim_threshold)) { //和自己行为一样的用户可以不考虑？just for testing
      tmp.insert(make_pair(distance, (*otherit)));
      excellentcount ++;
    }
  }

  int knn_count = GetKNNCount();
  MCE_INFO("AdRecommend::GetKNNeighbour-->uid:" << upt->userid <<",excellent Neighbour num:"
      << excellentcount <<",KNN count:" << knn_count << ",SimThreshold:" << sim_threshold);

  //将排在最后的最相似的k个用户取到
  size_t i = 0;
  for (multimap<double, UserItemPtr>::reverse_iterator mit = tmp.rbegin(); (mit
      != tmp.rend()) && (i < knn_count); ++mit, i++) {
    result.insert(make_pair(mit->first, mit->second));
  }

  return result;
}

map<Ice::Long, double> AdRecommend::GetRecommendation(int userid) {//该算法没有去除均值（只有0、1项）中，均值在后续会考虑去均值的效果.
  //  TimeStat subts;
  MCE_INFO("AdRecommend::GetRecommendation-->userid" << userid);
  TimeStat ts;
  map<Ice::Long, double> result;

  UserItemPtr u = UserItemPool::instance().Locate(userid);
  if (!u) {
    MCE_WARN("null user item. uid=" << userid);
    return result;
  }

  multimap<double, UserItemPtr> knn; // = GetKNNeighbour(u);//获取k个最近邻居
  try {
    knn = GetKNNeighbour(u);
  } catch (std::exception& e) {
    MCE_WARN("get knn ice err:" << e.what());
  }

  if (knn.empty()) {
    MCE_INFO("AdRecommend::GetRecommendation-->after filt recom size:" << result.size() <<", no friend, userid:" <<userid);
    return result;
  }

  //TimeStat ts;
  //  MCE_DEBUG("[USEDTIME] UserItemPool::GetRecommendation --> GetKNNeighbour used time = " << ts.getTime());
  ///////////////////////////////////////////////////////////////用户历史行为


  //  {
  //    IceUtil::RWRecMutex::RLock lock(mutex_);
  //    UidIndex & index = list__.get<1> ();
  //    UidIndex::iterator uit = index.find(userid);
  //    if (uit != index.end()) {
  // /     clickAd = (*uit)->gids_;
  //    }
  //  }
  ///////////////////////////////////////////////////////////////Todo  上线一定要删除这段代码，不然耗时

//  map<double, UserItemPtr>::iterator it = knn.begin(); ///找到的邻居
//  for (; it != knn.end(); ++it) {
//    //   cout << "userid: " << it->first << "distance:" << it->second <<endl;
//    MCE_DEBUG("GetKNNeighbour::userid:"<<userid <<" Neighbour:" <<(it->second)->userid << " distance: " << it->first);
//  }
 ///////////////////////////////////////////////////////////////

  set<Ice::Long> &clickAd = u->gids;

  map<Ice::Long, double> total;
  //  map<Ice::Long, double> sumsim;
  double sumsim = 0.0;
  map<double, UserItemPtr>::iterator itk = knn.begin();
  for (; itk != knn.end(); ++itk) {
    set<Ice::Long>& gids = (itk->second)->gids;
    set<Ice::Long>::iterator itg = gids.begin();
    double sim = itk->first;
    for (; itg != gids.end(); ++itg) {
      map<Ice::Long, double>::iterator itt = total.find(*itg);
      if (itt != total.end()) {
        itt->second += sim;
      } else {
        total.insert(make_pair(*itg, sim));
      }
    }

    sumsim += sim;
  }

  multimap<double, Ice::Long> result_tmp;

  if (sumsim > 0) {
    map<Ice::Long, double>::iterator itt = total.begin();
    for (; itt != total.end(); ++itt) {
      double tmp = (*itt).second / sumsim;
      Ice::Long groupid = (*itt).first;
      set<Ice::Long>::iterator iter = clickAd.find(groupid);

      if (iter != clickAd.end()) {
        MCE_INFO("AdRecommend::GetRecommendation-->userid:"<<userid<<",decrease already click:" <<*iter);
        /////////////////////////////////////test dispose already read
        tmp *= 0.05;
      } else {
        result_tmp.insert(make_pair(tmp, groupid));
      }

    }
  }

  //需要考虑用户已经看过的影响，为了测试，先不考虑//重要调试
  /*  {
   MCE_DEBUG("UserItemPool::GetRecommendation-->before filt result size:" << result.size() <<" userid:" <<userid);
   for (multimap<double, Ice::Long>::reverse_iterator itm = result_tmp.rbegin(); (itm
   != result_tmp.rend()); ++itm) {
   MCE_DEBUG("UserItemPool::GetRecommendation-->before filt userid:"<<userid<<" Adid:" << itm->second << " value:" << itm->first);
   }
   }*/

  //debug just top10
  ////////////////////////////////////////
  int num = 0;
  int invalidcount = 0;
  double value;
  result.clear();
  set<Ice::Long>::iterator itc = clickAd.begin();
  string invalidAd;
  int allreNum = result_tmp.size();
  double rec_threshold = GetReThreshold();
  int max_rec_count = GetMaxReCount();
  for (multimap<double, Ice::Long>::reverse_iterator itm = result_tmp.rbegin(); (itm
      != result_tmp.rend()) && (num < max_rec_count); ++itm) {
    Ice::Long groupid = itm->second;

    if ((AdGroupPool::instance().HasAdGroup(groupid))) {//找到有效广告// 还没有对已看广告进行处理

      //////////对已看过的广告进行简单处理
      // itc = clickAd.find(groupid);
      itc = find(clickAd.begin(), clickAd.end(), groupid);
      value = itm->first;
      //if (itc != clickAd.end()) {
      //  MCE_INFO("AdRecommend::GetRecommendation-->userid:"<<userid<<",dispose already click:" <<groupid);
      //  value *= probability;
      //}
      if (value == 1) { //如果推荐值大于一定的阈值，应该惩罚用户的行为，因为这个行为是热门，但是用户不一定是最喜欢的，有待于考虑
        value *= 0.1;
      }
      //////////////
      if(value < rec_threshold) {
        MCE_INFO("AdRecommend::GetRecommendation--> less than threshold:" << rec_threshold)
        break;
      }
      //  result.insert(pair<Ice::Long, double> (itm->second, itm->first));
      if (itc == clickAd.end()) {
        result.insert(make_pair(itm->second, value));
        ++num;
      }

    } else {
      invalidAd += "gid:";
      invalidAd += boost::lexical_cast<string>(groupid);
      invalidAd += ",value:";
      invalidAd += boost::lexical_cast<string>(itm->first);
      invalidAd += ";";
    //  MCE_DEBUG("UserItemPool::GetRecommendation-->userid:"<< userid<<" invalid groupid:" << groupid <<" value:" <<itm->first);
      ++invalidcount;
    }
  }

  MCE_INFO("AdRecommend::GetRecommendation-->userid:"<< userid<<",invalid gid num:" << invalidcount << ",groupid:" << invalidAd);
  ////////////////////////////////////////
  MCE_INFO("AdRecommend::GetRecommendation-->after filt recom size:" << result.size() << ",allrecom size:" << allreNum
      << ",invalid gid num:" << invalidcount<< ",ReThreshold:" << rec_threshold <<",userid:" <<userid);
  for (map<Ice::Long, double>::iterator it = result.begin(); it != result.end(); ++it) {
    MCE_INFO("AdRecommend::GetRecommendation-->after filt recom userid:"<<userid <<" result Adid:" << it->first << " value:" << it->second);
  }
  MCE_INFO("[USEDTIME] AdRecommend::GetRecommendation --> TIME_TEST ALL END  time = "<< ts.getTime());
  // MCE_DEBUG("UserItemPool::GetRecommendation--> userid = " << userid <<", used time = " << ts.getTime());
  return result;
}
