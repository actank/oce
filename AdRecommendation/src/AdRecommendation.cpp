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

void UserItemEvictor::Init() {//
  UserItemEvictorSet container;
  set<long> adset;
  Statement sql;
  sql << "SELECT * FROM ad_uid_gid limit 40000";
  mysqlpp::StoreQueryResult res;
  MCE_INFO("UserItemEvictor::Init");
  try {
    res = QueryRunner("ads_db", CDbRServer, "ad_uid_gid").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("UserItemEvictor::Init --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("UserItemEvictor::Init --> error:" << e.what());
  }


  int maxuserid = 0;
  long usercount = 0;

  if (res && res.num_rows() > 0) {
    for (int i = 0; i < res.num_rows(); i++) {
      int userid = boost::lexical_cast<int>(res[i]["userid"]);
      MCE_DEBUG("UserItemEvictor::Init insert DB userid:" << userid);
      if (userid <= 0)
        continue;
      if (userid > maxuserid)
        maxuserid = userid;
      usercount++;
      string result = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)(
          res[i]["gids"]);

      vector < string > group_ids;
      boost::algorithm::split_regex(group_ids, result, regex("\\,"));
      if(group_ids.empty()) //没有点击行为不插入
        continue;

      UserItemPtr upt = new UserItem();  //userid uniq
      upt->userid_ = userid;

      for (vector<string>::iterator it = group_ids.begin(); it
          != group_ids.end(); it++) {
        MCE_DEBUG("UserItemEvictor::Init-->userid:"
            << userid << ", this groupid = " << *it);
        long adid = 0;
        try {
          adid = boost::lexical_cast<long>(*it);
        } catch (std::exception & e) {
          MCE_WARN("UserItemEvictor::Init --> sting2 cast error :-"
              << e.what());
        }
        if (adid > 0) {
 //         UserItemPtr upt = new UserItem();   //bug
 //         upt->userid_ = userid;
          int pos =  GetIndex(container, userid, adid);
          upt->SetItemValue(pos);
//          container.push_front(upt);
          adset.insert(adid);
        }
      }
      container.push_front(upt); //用户提前

    }
  }
 /* long groupid[10] = {10000,10001,10002,10003,10004,10000,10007,10003,10007,10000};
  int user[10]= {100,101,102,103,100,100,104,102,100,102};
  vector<int> u(user, user+10);
  vector<long> g(groupid, groupid+10);
  for(int i = 0; i < 10; i++) {
    UserItemPtr upt = new UserItem();
    upt->userid_ = u[i];
    int pos =  GetIndex(container, u[i], g[i]);
    upt->SetItemValue(pos);
    container.push_front(upt);
    adset.insert(g[i]);
  }*/


  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    container_.swap(container);
  }
  MCE_INFO("UserItemEvictor::Init-->ad size: " << adset.size() << " usercount:" << usercount <<"max userid:" << maxuserid << ",ad indexsize:" << index_cache_.size());
//  MCE_DEBUG("UserItemEvictor::Init-->ad size: " << adset.size() << ",ad indexsize:" << index_cache_.size());
  {
    string result;
    int i = 0;
    int maxitem = index_cache_.GetMaxIndex();
    for(; i <= maxitem; ++i) {
      string str = boost::lexical_cast<string>(index_cache_.GetAdId(i));
      result += ",";
      result += str;
    }
    MCE_DEBUG("UserItemEvictor::Init-->done maxitem:"<<maxitem << "groupid:" << result);
  }


}
bool UserItemEvictor::NeedLoad(int userid) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  UidIndex & index = container_.get<1> ();
  UidIndex::iterator uit = index.find(userid);
  if (uit != index.end()) {
    return false;
  }
  return true;
}
vector<long> UserItemEvictor::LoadDB(int userid) {
  vector<long> vec;
  Statement sql;
  sql << "SELECT * FROM ad_uid_gid where userid = " << userid;
  mysqlpp::StoreQueryResult res;
  MCE_DEBUG("UserItemEvictor::LoadDB userid: "<<userid);
  try {
    res = QueryRunner("ads_db", CDbRServer, "ad_uid_gid").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("UserItemEvictor::LoadDB --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("UserItemEvictor::LoadDB --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("UserItemEvictor::LoadDB --> StoryQueryResult wrong");
  }

  if (res && res.num_rows() > 0) {
    string result = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)(
        res[0]["gids"]);

    vector < string > group_ids;
    boost::algorithm::split_regex(group_ids, result, regex("\\,"));
    for (vector<string>::iterator it = group_ids.begin(); it != group_ids.end(); it++) {
      MCE_DEBUG("[INDEXLOG] UserItemEvictor::LoadGidUidDB-->userid:" << userid
          << ", this groupid = " << *it);
      long id = 0;
      try {
        id = boost::lexical_cast<long>(*it);
      } catch (std::exception & e) {
        MCE_WARN("UserItemEvictor::LoadGidUidDB --> sting2 cast error :-"
            << e.what());
      }
      if (id > 0) {
        vec.push_back(id);
      }
    }
  }

  return vec;
}
vector<int> UserItemEvictor::LoadGidUidDB(int userid, long groupid) {
  vector<int> vec;
  MCE_DEBUG("UserItemEvictor::LoadGidUidDB-->gids:" << groupid);

  Statement sql;
  sql << "SELECT * FROM ad_gid_uid where gids = " << groupid;
  mysqlpp::StoreQueryResult res;

  try {
    res = QueryRunner("ads_db", CDbRServer, "ad_gid_uid").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("UserItemEvictor::LoadGidUidDB --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("UserItemEvictor::LoadGidUidDB --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("UserItemEvictor::LoadGidUidDB --> StoryQueryResult wrong");
  }

  if (res && res.num_rows() > 0) {
    string result = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)(
        res[0]["userid"]);

    vector < string > user_ids;
    boost::algorithm::split_regex(user_ids, result, regex("\\,"));
    for (vector<string>::iterator it = user_ids.begin(); it != user_ids.end(); it++) {
    //  MCE_DEBUG("[INDEXLOG] UserItemEvictor::LoadGidUidDB--> new userid = "
    //      << userid << ", gids:" << groupid << ", this userid = " << *it);
      int id = 0;
      try {
        id = boost::lexical_cast<int>(*it);
      } catch (std::exception & e) {
        MCE_WARN("UserItemEvictor::LoadGidUidDB --> sting2 cast error :-"
            << e.what());
      }
      if (id > 0) {
        vec.push_back(id);
      }
    }
  }

  return vec;
}
bool UserItemEvictor::Load(int userid) {//如果用户不在cache中，重新从数据库加载点击行为
  vector<long> result = LoadDB(userid);
  if (result.empty()) {
    MCE_DEBUG("UserItemEvictor::Load-->not exist in DB,userid: " << userid);
    return false;
  }
  MCE_DEBUG("UserItemEvictor::Load-->exist in DB,userid: " << userid);
  //插入容器
  {
    vector<int> indexseq;
    vector<long>::iterator it = result.begin();
    for (; it != result.end(); ++it) {
      int index = GetIndex(container_, userid, *it);
      if (index >= 0) {
        indexseq.push_back(index);
      } else {
        MCE_DEBUG("UserItemEvictor::Load-->groupid index not exist userid:" << userid << " groupid:" << *it);
      }
    }

    UserItemPtr upt = new UserItem();
    upt->userid_ = userid;

    vector<int>::iterator itseq = indexseq.begin();
    for (; itseq != indexseq.end(); ++itseq) {
      //int index = IndexCache::instance().GetIndex(aid);
      int index = *itseq;
      upt->SetItemValue(index);
    }

    {
      TimeStat ts;
      IceUtil::RWRecMutex::WLock lock(mutex_);
      MCE_DEBUG("[USEDTIME] UserItemEvictor::Load--> wlock used time = " << ts.getTime());
      SeqIndex &sindex = container_.get<0> ();
      pair<SeqIndex::iterator, bool> p = container_.push_front(upt);
      if (!p.second) {
        //   MCE_DEBUG("[INDEXLOG] UserItemEvictor::Load--> userid: " << userid << ", already exist!");
        sindex.relocate(sindex.begin(), p.first);
      } else {
        //   MCE_DEBUG("[INDEXLOG] UserItemEvictor::Load--> userid: " << userid << ", didn't exist!,now exist in cache");
        Kick();
        ////////////////just for debug
        //      string result;
        //      int itemsize = IndexCache::instance().GetMaxIndex();
        //      UidIndex & index = container_.get<1> ();
        //      UidIndex::iterator uit = index.find(userid);
        //      for(int i=0; i < itemsize; i++) {
        //        if((*uit)->GetItemValue(i)) {
        //          long groupid = IndexCache::instance().GetAdId(i);
        //          string str = boost::lexical_cast<string>(groupid);
        //          result += str;
        //          result += ",";
        //        }
        //      }
        //      MCE_DEBUG("[INDEXLOG] UserItemEvictor::Load--> userid: " << userid << ", click:" <<result);
      }
    }

  }
  return true;
}

int UserItemEvictor::GetIndex(UserItemEvictorSet & container, int userid,
    long groupid) {//里面有锁，注意
//  TimeStat ts;
  int pos = -1;
  pos = index_cache_.GetIndex(groupid);
  if (pos >= 0) {
    return pos;
  }

  vector<int> userset;

  //
  ////////////////////////////////////debug
  //    {
  //      int itemsize = IndexCache::instance().GetMaxIndex();
  //      string result;
  //      for (int i = 0; i < itemsize; i++) {
  //        long groupid = IndexCache::instance().GetAdId(i);
  //        string str = boost::lexical_cast<string>(groupid);
  //        result += ",";
  //        result += str;
  //        // MCE_DEBUG("ReGetIndex before:gids:" <<groupid);
  //      }
  //     // MCE_DEBUG("[INDEXLOG] ReGetIndex before:gids:" <<result.c_str());
  //
  //    }
  //////////////////////////////////////////////////

  pos = index_cache_.ReGetIndex(groupid); //索引值超过最大数目，只能替换旧有的过期groupid；
  ////////////////////////////////////debug
  //    {
  //      int itemsize = IndexCache::instance().GetMaxIndex();
  //      string result;
  //      for (int i = 0; i < itemsize; i++) {
  //        long groupid = IndexCache::instance().GetAdId(i);
  //        string str = boost::lexical_cast<string>(groupid);
  //        result += ",";
  //        result += str;
  //      }
  //     // MCE_DEBUG("[INDEXLOG] ReGetIndex after:gids:" <<result.c_str());
  //
  //    }
  //////////////////////////////////////////////////
  if (pos < 0) {
    MCE_WARN("[INDEXLOG] UserItemEvictor::GetIndex-->ReGetIndex not success,maxsize:"
        << index_cache_.GetMaxIndex());
    string result;
    int i = 0;
    int maxitem = index_cache_.GetMaxIndex();
    for(; i <= maxitem; ++i) {
      string str = boost::lexical_cast<string>(index_cache_.GetAdId(i));
      result += ",";
      result += str;
    }
    MCE_WARN("[INDEXLOG] UserItemEvictor::GetIndex-->ReGetIndex not success,groupid:" << result);

  }
  MCE_INFO("[INDEXLOG] UserItemEvictor::GetIndex--> exceed max itemsize, new pos:"
          << pos << ", maxsize" << index_cache_.GetMaxIndex());

  //load from ads_gid_uid;索引值超过最大数目，需要重新加载将要新分配的index的点击行为
  userset = LoadGidUidDB(userid, groupid);
  if(pos >= 0) {
    {
      IceUtil::RWRecMutex::WLock lock(mutex_);
      UidIndex & index = container.get<1> ();
      UidIndex::iterator uit = index.begin();
      for (; uit != index.end(); ++uit) {//需要更新旧有的groupid的item项，需要都清0，包括cache
        (*uit)->ResetItemValue(pos);
      }
      vector<int>::iterator it = userset.begin();
      for (; it != userset.end(); ++it) {//从DB中重新更新用户的行为
        uit = index.find(*it);
        if (uit != index.end()) {
          (*uit)->SetItemValue(pos);
          //   MCE_DEBUG("[INDEXLOG] UserItemEvictor::GetIndex--> SetItemValue groupid = "
          //       << groupid <<", userid = " << (*uit)->userid_ <<", pos:" <<pos);
        }
      }
    }
  }
  ///////////////////////////////debug
  //    {
  //      UidIndex & index = container.get<1> ();
  //      UidIndex::iterator uit = index.begin();
  //      string result;
  //      for (; uit != index.end(); ++uit) {
  //        if ((*uit)->GetItemValue(pos)) {
  //          string str = boost::lexical_cast<string>((*uit)->userid_);
  //          result += ",";
  //          result += str;
  //        }
  //
  //      }
  //      //  MCE_DEBUG("[INDEXLOG] ReGetIndex after  this userid = " << userid
  //      //      << ", new pos:" <<pos <<" groupid:" << IndexCache::instance().GetAdId(pos) << " userid:"<<result.c_str());
  //    }

  ///////////////////////////////

//  MCE_DEBUG("[USEDTIME] UserItemEvictor::GetIndex --> used time = "
//      << ts.getTime());
  return pos;
}

/*void UserItemEvictor::SetUserItemValue(int userid, long adid) {
  int pos = GetIndex(container_, userid, adid);//如果不存在，需要新分配index 里面有锁，不能放在锁里
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    UidIndex & index = container_.get<1> ();
    UidIndex::iterator uit = index.find(userid);
    //int pos = IndexCache::instance().GetIndex(adid);//如果不存在，需要新分配index

    if (uit != index.end()) {
      (*uit)->SetItemValue(pos);
      //  cout << "UserItemEvictor::SetUserItemValue insert pos:" << pos << "userid: " << userid <<"adid: " << adid<< endl;
      return;
    }
  }

  UserItemPtr upt = new UserItem();
  //  cout << "UserItemEvictor::SetUserItemValue insert pos:" << pos << "userid: " << userid <<"adid: " << adid<< endl;
  upt->userid_ = userid;
  upt->SetItemValue(pos);
  //  container_.push_front(upt);
  //  MCE_DEBUG("UserItemEvictor::SetUserItemValue-->user not exist in cache userid: "<<userid << "adid: " <<adid);
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    SeqIndex &sindex = container_.get<0> ();
    pair<SeqIndex::iterator, bool> p = container_.push_front(upt);
    if (!p.second) {
      sindex.relocate(sindex.begin(), p.first);
    } else {
      Kick();
    }
  }

}*/

void UserItemEvictor::SetUserItemValue(int userid, long adid) {
  int pos = GetIndex(container_, userid, adid);//如果不存在，需要新分配index 里面有锁，不能放在锁里

  if (NeedLoad(userid)) {
    MCE_DEBUG("UserItemEvictor::SetUserItemValue need load from DB-->userid: " <<userid );
    Load(userid);
  }
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    UidIndex & index = container_.get<1> ();
    UidIndex::iterator uit = index.find(userid);
    //int pos = IndexCache::instance().GetIndex(adid);//如果不存在，需要新分配index

    if (uit != index.end()) {
      (*uit)->SetItemValue(pos);

      SeqIndex::iterator sit = container_.project<0> (uit);//当前用户为活跃用户，提前
      container_.relocate(container_.get<0> ().begin(), sit);
      //  cout << "UserItemEvictor::SetUserItemValue insert pos:" << pos << "userid: " << userid <<"adid: " << adid<< endl;
      return;
    }
  }

  UserItemPtr upt = new UserItem();
  //  cout << "UserItemEvictor::SetUserItemValue insert pos:" << pos << "userid: " << userid <<"adid: " << adid<< endl;
  upt->userid_ = userid;
  upt->SetItemValue(pos);
  //  container_.push_front(upt);
  //  MCE_DEBUG("UserItemEvictor::SetUserItemValue-->user not exist in cache userid: "<<userid << "adid: " <<adid);
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    SeqIndex &sindex = container_.get<0> ();
    pair<SeqIndex::iterator, bool> p = container_.push_front(upt);
    if (!p.second) {
      sindex.relocate(sindex.begin(), p.first);
    } else {
      Kick();
    }
  }

}

void UserItemEvictor::Click(int userid, long groupid) {
  //更新Cache
  SetUserItemValue(userid, groupid);
}
/*UserItemPtr UserItemEvictor::GetUserItemPtr(int userid) {
 UidIndex & index = container_.get<1>();
 UidIndex::iterator uit = index.find(userid);
 if(uit != index.end()) {
 cout << "exist" << endl;
 //  cout << (*uit)->GetItemValue(2);
 return (*uit);
 }
 cout << "not exist" << endl;
 return NULL;
 }*/
vector<long> UserItemEvictor::GetUserItemValue(int userid) {//为了测试
//  UserItemPtr upt = GetUserItemPtr(userid);
  MCE_DEBUG("UserItemEvictor::GetUserItemValue-->userid: " << userid );
  vector<long> vec;
  int itemsize = index_cache_.GetMaxIndex();
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    UidIndex & index = container_.get<1> ();
    UidIndex::iterator uit = index.find(userid);
    if (uit != index.end()) {
      for (int i = 0; i <= itemsize; i++) {
        if ((*uit)->GetItemValue(i)) {
          long gid = index_cache_.GetAdId(i);
          vec.push_back(gid);
        }
      }
    }
  }
  return vec;

}



vector<int> UserItemEvictor::GetGidUser(long groupid) {//为了测试
  vector<int> result;
  int pos = GetIndex(container_, 0, groupid);
//  IceUtil::Mutex::Lock lock(mutex_);
  IceUtil::RWRecMutex::RLock lock(mutex_);

  UidIndex & index = container_.get<1> ();
  UidIndex::iterator uit = index.begin();
  for (; uit != index.end(); ++uit) {
    if ((*uit)->GetItemValue(pos)) {
      result.push_back((*uit)->userid_);
    }
  }

  return result;

}
float UserItemEvictor::GetUserSimValue(int userid1, int userid2) {//
//  IceUtil::RWRecMutex::RLock lock(mutex_);
  UidIndex & index = container_.get<1> ();
  UidIndex::iterator uit1 = index.find(userid1);
  UidIndex::iterator uit2 = index.find(userid2);
  float result = 0;
  if ((uit1 != index.end()) && (uit2 != index.end())) {
    //    cout << "UserItemEvictor::GetUserSimValue: find" <<endl;
    result = GetUserSimValue(*uit1, *uit2);
    return result;
  }
  // cout << "UserItemEvictor::GetUserSimValue: not find" <<endl;
  return result;

}
///这个函数可以适合于其他item项。item不一定是0、1
/*float UserItemEvictor::GetUserSimValue(UserItemPtr userptrx,
    UserItemPtr userptry) {
  if (!(userptrx && userptry)) {
    //cout << "wrong" << endl;
    return 0;
  }
  float sumxy = 0;
  float sumxsq = 0;
  float sumysq = 0;
  int itemsize = index_cache_.GetMaxIndex();
  int comcount = 0;
  // MCE_DEBUG("UserItemEvictor::GetUserSimValue-->itemsize:" << itemsize);
  for (int i = 0; i <= itemsize; i++) {
    float x = userptrx->GetItemValue(i);
    float y = userptry->GetItemValue(i);

    sumxy += x * y;
    sumxsq += x * x;
    sumysq += y * y;
  }

  if (sumxsq == 0 || sumysq == 0) {
    return 0;
  }

  if((userptrx->itemset_ & userptry->itemset_) == (userptry->itemset_)) {//delete subset//test

    MCE_DEBUG("UserItemEvictor::GetUserSimValue-->inter userid:"<<userptrx->userid_
            <<" has subset:"<<",userid:"<<userptry->userid_ << ",comcount:" << userptry->GetCount());
    return 0;
  }

  return sumxy / sqrt(sumxsq * sumysq);

}*/

float UserItemEvictor::GetUserSimValue(UserItemPtr userptrx, //just for item is 0 or 1
    UserItemPtr userptry) {
  bitset<kBitSetSize> useritemx = userptrx->itemset_;
  bitset<kBitSetSize> useritemy = userptry->itemset_;
  float sumxy = 0;
  float sumxsq = 0;
  float sumysq = 0;
  bitset<kBitSetSize> useriter;
  useriter = (useritemx & useritemy);
  sumxy = useriter.count();
  sumxsq = useritemx.count();
  sumysq = useritemy.count();
  if (sumxsq == 0 || sumysq == 0) {
    return 0;
  }

  if(useriter == useritemy) {//delete subset//test

//    MCE_DEBUG("UserItemEvictor::new GetUserSimValue-->inter userid:"<<userptrx->userid_
//            <<" has subset:"<<",userid:"<<userptry->userid_ << ",comcount:" << userptry->GetCount());
    return 0;
  }

  return sumxy / sqrt(sumxsq * sumysq);

}

map<int, float> UserItemEvictor::GetKNNeighbour(UserItemEvictorSet & container,
    IndexCachePtr & indexcache, int userid) {//可以考虑设置门限，不一定要是指定的k
  TimeStat ts;
  map<int, float> result;
  multimap<float, int> tmp;
  if (NeedLoad(userid)) {
    MCE_INFO("UserItemEvictor::GetKNNeighbour need load from DB-->userid: " <<userid );
    ts.reset();
    if (!Load(userid)) {
      return result;
    }
  //  MCE_DEBUG("[USEDTIME] UserItemEvictor::GetKNNeighbour --> Load used time = " << ts.getTime());
  }

 /*   IceUtil::RWRecMutex::WLock lock(mutex_);
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    UidIndex & index = container_.get<1> ();
    UidIndex::iterator uit = index.find(userid);
    if (uit != index.end()) {
      SeqIndex::iterator sit = container_.project<0> (uit);//当前用户为活跃用户，提前
      container_.relocate(container_.get<0> ().begin(), sit);
      UidIndex::iterator otherit = index.begin();

      ts.reset();
      for (; otherit != index.end(); ++otherit) {
        if (uit == otherit)
          continue;//是自己不参与计算
        float distance = GetUserSimValue((*uit), (*otherit));
        if ((distance != 1) && (distance > 0.1)) { //和自己行为一样的用户可以不考虑？just for testing
          tmp.insert(pair<float, int> (distance, (*otherit)->GetUserid()));
        }
      }
      MCE_DEBUG("[USEDTIME] UserItemEvictor::GetKNNeighbour --> jisuan time = " << ts.getTime());
    }
    //cout << "UserItemEvictor::GetKNNeighbour: multimapsize:" << tmp.size() << endl;
  }*/


  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    UidIndex & index = container_.get<1> ();
    UidIndex::iterator uit = index.find(userid);
    if (uit != index.end()) {

      SeqIndex::iterator sit = container_.project<0> (uit);//当前用户为活跃用户，提前
      container_.relocate(container_.get<0> ().begin(), sit);
    }
//    MCE_DEBUG("UserItemEvictor::GetKNNeighbour-->swap before: container size:"
//        << container_.size() << ",indexcache size:" << index_cache_.size());
//    ts.reset();
    container = container_;
    indexcache = new IndexCache(index_cache_);
//    MCE_DEBUG("UserItemEvictor::GetKNNeighbour-->swap after: container size:"
//        << container.size() << ",indexcache size:"
//        << indexcache->size() << ", used time = " << ts.getTime());

  }

  ts.reset();
  UidIndex & index = container.get<1> ();
  UidIndex::iterator uit = index.find(userid);
  if(uit != index.end()) {

    UidIndex::iterator otherit = index.begin();
    for (; otherit != index.end(); ++otherit) {
      if (uit == otherit)
        continue;//是自己不参与计算
      float distance = GetUserSimValue((*uit), (*otherit));
      if ((distance != 1) && (distance > 0.1)) { //和自己行为一样的用户可以不考虑？just for testing
        tmp.insert(pair<float, int> (distance, (*otherit)->GetUserid()));
      }

    }
    MCE_DEBUG("[USEDTIME] UserItemEvictor::GetKNNeighbour copy--> jisuan time = " << ts.getTime());
  }
    //cout << "UserItemEvictor::GetKNNeighbour: multimapsize:" << tmp.size() << endl;


  //将排在最后的最相似的k个用户取到
  int i = 0;
  for (multimap<float, int>::reverse_iterator mit = tmp.rbegin(); (mit
      != tmp.rend()) && (i < KNNCOUNT); ++mit, i++) {
    result.insert(pair<int, float> (mit->second, mit->first));
    //  cout << "UserItemEvictor::GetKNNeighbour:transform map : " << "userid:" << mit->second  <<"distance: "<<mit->first <<endl;
  }
  //cout << "UserItemEvictor::GetKNNeighbour: mapsize:" << result.size()  <<" k: " << k << endl;

  return result;

}
map<long, float> UserItemEvictor::GetRecommendation(int userid) {//该算法没有去除均值（只有0、1项）中，均值在后续会考虑去均值的效果.

//  TimeStat subts;
  MCE_DEBUG("UserItemEvictor::GetRecommendation-->userid" << userid);
  map<long, float> result;
  multimap<float, long> result_tmp;

  UserItemEvictorSet container;
  IndexCachePtr indexcache;

  map<int, float> knn = GetKNNeighbour(container,indexcache, userid);//获取k个最近邻居

  if (knn.empty()) {
    MCE_INFO("UserItemEvictor::GetRecommendation-->result size:" << result.size() <<", no friend, userid:" <<userid);
    return result;
  }

  TimeStat ts;
//  MCE_DEBUG("[USEDTIME] UserItemEvictor::GetRecommendation --> GetKNNeighbour used time = " << ts.getTime());
///////////////////////////////////////////////////////////////用户历史行为
  bitset<kBitSetSize> useritem;
  vector<long> clickAd;
  {

      UidIndex & index = container.get<1> ();
      UidIndex::iterator uit = index.find(userid);
      if (uit != index.end()) {
        useritem = (*uit)->itemset_;
      }
  }
///////////////////////////////////////////////////////////////
  int itemsize = indexcache->GetMaxIndex();
/*  map<int, float>::iterator it = knn.begin(); ///找到的邻居
  for (; it != knn.end(); ++it) {
    //   cout << "userid: " << it->first << "distance:" << it->second <<endl;
    MCE_DEBUG("GetKNNeighbour::userid:"<<userid <<" Neighbour:" <<it->first << " distance: " << it->second);
  }*/

  MCE_INFO("UserItemEvictor::GetRecommendation max -->itemsize:" << itemsize);
  for (int i = 0; i <= itemsize; i++) {
    float total = 0;
    float sumsim = 0;
    map<int, float>::iterator it = knn.begin();


    for (; it != knn.end(); ++it) {
      float sim = it->second;
      //     UserItemPtr userptr = GetUserItemPtr(it->first);
      //     float tmp = userptr->GetItemValue(i);
      //     cout << "i: " << i << "userid: " << it->first << "simvalue: " << sim << endl;
      float tmp = 0;
      {

        UidIndex & index = container.get<1> ();
        UidIndex::iterator uit = index.find(it->first);
        if (uit != index.end()) {
          tmp = (*uit)->GetItemValue(i);
        }
      }


      // if(tmp != 0) {
      total += sim * tmp;//
      //  sumsim += sim;  //放里面计算还是外面？这个需要考虑下差别
      // }
      sumsim += sim;
    }

    ///////////////////////////////////////
    bool isclick = useritem.test(i*2);

    //////////////////////////////////////
    //index-->ADid
    // cout << "item index: " << i << "recommend total" << total << "sumsim" << sumsim <<endl;
    long id = indexcache->GetAdId(i);

    if (sumsim == 0) {
      // result.insert(pair<long,float>(id,0));//sumsim 可能为0 需考虑处理
    } else {
      float tmp = total / sumsim;
      if (tmp > 0) {
        //  MCE_DEBUG("UserItemEvictor::GetRecommendation index: " << i);
        if(isclick) {
          MCE_DEBUG("UserItemEvictor::GetRecommendation-->userid:"<<userid<<",decrease already click:" <<id);

          /////////////////////////////////////test dispose already read
          clickAd.push_back(id);
          tmp *= 0.1;
          ///////////////////////////////////////
        }
    //    result.insert(pair<long, float> (id, tmp));//
        result_tmp.insert(pair<float, long> (tmp, id));
      }
      // result.insert(pair<long,float>(id,tmp));//

    }

  }
  //需要考虑用户已经看过的影响，为了测试，先不考虑//重要调试
/*  {
    MCE_DEBUG("UserItemEvictor::GetRecommendation-->before filt result size:" << result.size() <<" userid:" <<userid);
    for (multimap<float, long>::reverse_iterator itm = result_tmp.rbegin(); (itm
        != result_tmp.rend()); ++itm) {
      MCE_DEBUG("UserItemEvictor::GetRecommendation-->before filt userid:"<<userid<<" Adid:" << itm->second << " value:" << itm->first);
    }
  }*/

  //debug just top10
  ////////////////////////////////////////
  int num = 0;
  int invalidcount = 0;
  float value;
  result.clear();
  vector<long>::iterator itc = clickAd.begin();
  string invalidAd;
  for (multimap<float, long>::reverse_iterator itm = result_tmp.rbegin(); (itm
      != result_tmp.rend()) && (num < RecommendNum); ++itm) {
    long groupid = itm->second;

    if ((AdGroupPool::instance().HasAdGroup(groupid))) {//找到有效广告// 还没有对已看广告进行处理

      //////////对已看过的广告进行简单处理
     // itc = clickAd.find(groupid);
      itc = find(clickAd.begin(), clickAd.end(), groupid);
      value = itm->first;
      if(itc != clickAd.end()) {
        MCE_INFO("UserItemEvictor::GetRecommendation-->userid:"<<userid<<",dispose already click:" <<groupid);
        value *= probability;
      }
      if(value == 1) { //如果推荐值大于一定的阈值，应该惩罚用户的行为，因为这个行为是热门，但是用户不一定是最喜欢的，有待于考虑
        value *= 0.1;
      }
      //////////////

    //  result.insert(pair<long, float> (itm->second, itm->first));
      result.insert(pair<long, float> (itm->second, value));
      ++num;
    } else {
      invalidAd += "gid:";
      invalidAd += boost::lexical_cast<string>(groupid);
      invalidAd += ",value:";
      invalidAd += boost::lexical_cast<string>(itm->first);
      invalidAd += ";";
   //   MCE_DEBUG("UserItemEvictor::GetRecommendation-->userid:"<< userid<<" invalid groupid:" << groupid <<" value:" <<itm->first);
      ++invalidcount;
    }
  }

  MCE_INFO("UserItemEvictor::GetRecommendation-->userid:"<< userid<<",invalid gid num:" << invalidcount << ",groupid:" << invalidAd);
  ////////////////////////////////////////
  MCE_INFO("UserItemEvictor::GetRecommendation-->after filt recom size:" << result.size() <<" userid:" <<userid);
  for (map<long, float>::iterator it = result.begin(); it != result.end(); ++it) {
    MCE_INFO("UserItemEvictor::GetRecommendation-->after filt recom userid:"<<userid <<" result Adid:" << it->first << " value:" << it->second);
  }
 // MCE_DEBUG("UserItemEvictor::GetRecommendation--> userid = " << userid <<", used time = " << ts.getTime());
  return result;
}
void UserItemEvictor::Kick() {
  while (container_.size() > kUserSize) {
    MCE_INFO("[INDEXLOG] UserItemEvictor::Kick-->container size:" << container_.size() <<" kicking userid:" << (*(container_.rbegin()))->userid_);
    container_.pop_back();
  }
}
