/*
 * AdRecommendation.h
 *
 *  Created on: 2010-11-21
 *      Author: sunzz
 */

#ifndef ADRECOMMENDATION_H_
#define ADRECOMMENDATION_H_
#include <bitset>
#include <map>
#include <vector>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <iostream>
#include "Singleton.h"
#include "AdReConfig.h"
#include <string>
//#include "AdIndex.h"
#include"AdRecommendationHandlerI.h"
#include <boost/lexical_cast.hpp>
 
using namespace std;
using namespace boost::multi_index;
using namespace boost;
using namespace MyUtil;
using namespace xce::ad;


//const size_t kUserSize = 2000000;
//const size_t kActiveUserSize = 40000;
const double probability = 0.16;//已点击过的广告降低到原来的百分比
//const size_t RecommendNum= 16;
//const size_t KNNCount = 200;
//const double SimThreshold = 0.1;

const size_t WaitListNum = 100;
class UserItem : public IceUtil::Shared {
public:
  int userid;
  set<Ice::Long> gids;
  int gender;
  int age;
  void FillUserData();
};
typedef IceUtil::Handle<UserItem> UserItemPtr;
class UserItemPool : public Singleton<UserItemPool> {
private:
  typedef boost::multi_index_container<UserItemPtr, indexed_by<sequenced<> ,
      ordered_unique<BOOST_MULTI_INDEX_MEMBER(UserItem, int, userid)> > >
      UserItemEvictor;

  typedef UserItemEvictor::nth_index<0>::type SeqIndex;
  typedef UserItemEvictor::nth_index<1>::type UidIndex;
  UserItemEvictor evictor_;
  static const int kUserSize = 2000000; //内存中的用户数
  static const size_t kActiveUserSize = 100000;
//  static const string GroupidType = "100000000001";
  static const string TableName;
  IceUtil::Mutex mutex_;

  int GetSetSize() {
    int setsize = kUserSize;
    string s = RecommendConfig::instance().GetValue("kUserSize");
    try {
      if (!s.empty()) {
        setsize = lexical_cast<int> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("UserItemPool::GetSetSize --> sting cast error :-" << s <<"-"<< e.what());
    }
 //   MCE_DEBUG("UserItemPool::--> GetSetSize = " << setsize);
    return setsize;
  }
  size_t GetActiveUserSize() {
    int setsize = kActiveUserSize;
    string s = RecommendConfig::instance().GetValue("kActiveUserSize");
    try {
      if (!s.empty()) {
        setsize = lexical_cast<int> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("UserItemPool::GetActiveUserSize --> sting cast error :-" << s <<"-"<< e.what());
    }
 //   MCE_DEBUG("UserItemPool::--> GetActiveUserSize = " << setsize);
    return setsize;
  }

public:
  string GetTableName() {
    string value = TableName;
    string s = RecommendConfig::instance().GetValue("DBTableName");
    if (!s.empty()) {
      value = s;
    }

    return value;
  }



public:
  void Init();
  UserItemPtr Locate(int userid);
  void GetActiveUsers(vector<UserItemPtr> & list);
  UserItemPtr Load(int userid);
  void UpdateClick(int userid, Ice::Long groupid);

//  void Click(int userid, Ice::Long groupid);
//  vector<UserItemPtr> GetActiveUsers();

  //////////////////////////////////////////////////////////测试接口
  void CleanClick(int userid);
  vector<int> GetGidUser(Ice::Long groupid);
  vector<Ice::Long> GetUserItemValue(int userid);
  //double GetUserDistance(int userid1, int userid2);
//  int GetIndex(UserItemEvictor & container,int userid, long groupid);
  vector<int> LoadGidUsersDB(Ice::Long groupid, int userid);
};

class  UpdateTask: public MyUtil::Task {
public:
  UpdateTask(UserItemPtr u):upt(u){
  }
  virtual void handle() {
    MCE_DEBUG("UpdateUidGidTask:start: userid:" << upt->userid);
    UpdateUidGid();
  }
  void UpdateUidGid();
  UserItemPtr upt;
};

class UserItemList : public IceUtil::Shared{
public:
  vector<UserItemPtr> list;
};
typedef IceUtil::Handle<UserItemList> UserItemListPtr;


class AdRecommend : public Singleton<AdRecommend> {
private:

  UserItemListPtr list_;
  static const int KNNCOUNT = 200; // k:最近邻居个数
  static const size_t RecommendNum= 16;
  static const double SimThreshold = 0.1;
  static const double ReThreshold = 1.0;

  static const int Strategy = 0;
  static const double PropThreshold = 1.0;
  static const double ReThresholdEmpty = 1.0;
  static const double GenderPercent = 0;
  static const double AgePercent = 0;
  int GetKNNCount() {
    int value = KNNCOUNT;
    string s = RecommendConfig::instance().GetValue("KNNCount");
    try {
      if (!s.empty()) {
        value = lexical_cast<int> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetKNNCount --> sting cast error :-" << s <<"-"<< e.what());
    }

//    MCE_DEBUG("AdRecommend::--> GetKNNCount = " << value);
    return value;
  }
  int GetMaxReCount() {
    int value = RecommendNum;
    string s = RecommendConfig::instance().GetValue("MaxReCount");
    try {
      if (!s.empty()) {
        value = lexical_cast<int> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetMaxReCount --> sting cast error :-" << s <<"-"<< e.what());
    }
 //   MCE_DEBUG("AdRecommend::--> GetMaxReCount = " << value);
    return value;
  }
  double GetSimThreshold() {
    double value = SimThreshold;
    string s = RecommendConfig::instance().GetValue("SimThreshold");
    try {
      if (!s.empty()) {
        value = lexical_cast<double> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetSimThreshold --> sting cast error :-" << s <<"-"<< e.what());
    }
  //  MCE_DEBUG("AdRecommend::--> GetSimThreshold = " << value);
    return value;
  }
  double GetReThreshold() {
    double value = ReThreshold;
    string s = RecommendConfig::instance().GetValue("ReThreshold");
    try {
      if (!s.empty()) {
        value = lexical_cast<double> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetReThreshold --> sting cast error :-" << s <<"-"<< e.what());
    }
   // MCE_DEBUG("AdRecommend::--> GetReThreshold = " << value);
    return value;
  }

  double GetGenderPercent() {
    double value = GenderPercent;
    string s = RecommendConfig::instance().GetValue("GenderPercent");
    try {
      if (!s.empty()) {
        value = lexical_cast<double> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetGenderPercent --> sting cast error :-" << s <<"-"<< e.what());
    }
   // MCE_DEBUG("AdRecommend::--> GetReThreshold = " << value);
    return value;
  }
  double GetAgePercent() {
    double value = AgePercent;
    string s = RecommendConfig::instance().GetValue("AgePercent");
    try {
      if (!s.empty()) {
        value = lexical_cast<double> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetAgePercent --> sting cast error :-" << s <<"-"<< e.what());
    }
   // MCE_DEBUG("AdRecommend::--> GetReThreshold = " << value);
    return value;
  }
  double GetPropThreshold() {
    double value = PropThreshold;
    string s = RecommendConfig::instance().GetValue("PropThreshold");
    try {
      if (!s.empty()) {
        value = lexical_cast<double> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetPropThreshold --> sting cast error :-" << s <<"-"<< e.what());
    }
   // MCE_DEBUG("AdRecommend::--> GetReThreshold = " << value);
    return value;
  }
  double GetReThresholdEmpty() {
    double value = ReThresholdEmpty;
    string s = RecommendConfig::instance().GetValue("ReThresholdEmpty");
    try {
      if (!s.empty()) {
        value = lexical_cast<double> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetReThresholdEmpty --> sting cast error :-" << s <<"-"<< e.what());
    }
   // MCE_DEBUG("AdRecommend::--> GetReThreshold = " << value);
    return value;
  }
  int GetStrategy() {
    int value = Strategy;
    string s = RecommendConfig::instance().GetValue("Strategy");
    try {
      if (!s.empty()) {
        value = lexical_cast<int> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetStrategy --> sting cast error :-" << s <<"-"<< e.what());
    }

//    MCE_DEBUG("AdRecommend::--> GetKNNCount = " << value);
    return value;
  }

public:
  AdRecommend() {
    list_ = new UserItemList();
    UpdateSet();
    TaskManager::instance().scheduleRepeated(new UpdateSetTimer());
  }
  double GetPropertyDistance(UserItemPtr&, UserItemPtr&, double, double);
  double GetUserDistance(UserItemPtr& userptr1, UserItemPtr& userptr2);
  multimap<double, UserItemPtr> GetKNNeighbour(UserItemPtr& u);
  map<Ice::Long, double> GetRecommendation(int userid);
  void UpdateSet();

  class UpdateSetTimer : public MyUtil::Timer {
  public:
    UpdateSetTimer() :
      Timer(10 * 1000) {
    }
    virtual void handle() {
      try {
        AdRecommend::instance().UpdateSet();
      } catch (Ice::Exception& e) {
        MCE_WARN("AdRecommend --> UpdateSet err:" << e << ", line:"
            << __LINE__);
      } catch (...) {
        MCE_WARN("AdRecommend --> UpdateSet error, line:" << __LINE__);
      }
    }
  };
////////////////////////////测试接口
  double GetUserDistance(int userid1, int userid2);

};





#endif /* ADRECOMMENDATION_H_ */
