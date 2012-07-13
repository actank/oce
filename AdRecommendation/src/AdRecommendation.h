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
#include "AdIndex.h"
#include"AdRecommendationHandlerI.h"
 
using namespace std;
using namespace boost::multi_index;
using namespace MyUtil;


const size_t kUserSize = 40000;
const float probability = 0.16;//已点击过的广告降低到原来的百分比
const size_t RecommendNum= 10;



//const size_t ItemSize = 1500;
const size_t kBitSetSize = ItemSize * 2;//广告数*占的位数
class UserItem : public IceUtil::Shared {
private:

public:
  int userid_;
  bitset<kBitSetSize> itemset_;
  int GetBitValue(int pos) {
    //  	cout << "GetBitValue bit pos:" << pos << endl;

    try {
      if (itemset_.test(pos) == false)
        return 0;
      else
        return 1;
    } catch (std::exception & e) {
      	MCE_WARN("AdIndex-->GetBitValue::bitset error :-->"<< e.what());
    }
  }
  void SetBitValue(int pos) {
    //  	cout << "SetBitValue  bit pos:" << pos << endl;
    try{
      itemset_.set(pos);
    }catch(std::exception & e){
      MCE_WARN("AdIndex-->SetBitValue::bitset error :-->"<< e.what());
    }

  }
  void ResetBitValue(int pos) {
    itemset_.reset(pos);
  }
  int GetCount() {
    return itemset_.count();
  }
  int GetUserid() {
    return userid_;
  }
  float GetItemValue(int pos) {
    // 	MCE_DEBUG("GetItemValue:pos "<<pos);
    return GetBitValue(pos * 2);
  }
  void SetItemValue(int pos) {
    SetBitValue(pos * 2);
  }
  void ResetItemValue(int pos) {
    ResetBitValue(pos * 2);
  }
  void SetAdItemValue(long aid) {
    //   int index = IndexCache::instance().GetIndex(aid);
    //   SetItemValue(index);//目前只考虑偶数位置，偶数位置表示用户是否看过广告
  }

};
typedef IceUtil::Handle<UserItem> UserItemPtr;
class UserItemEvictor : public Singleton<UserItemEvictor> {
private:
  typedef boost::multi_index_container<UserItemPtr, indexed_by<sequenced<> ,
      ordered_unique<BOOST_MULTI_INDEX_MEMBER(UserItem, int, userid_)> > >
      UserItemEvictorSet;

  typedef UserItemEvictorSet::nth_index<0>::type SeqIndex;
  typedef UserItemEvictorSet::nth_index<1>::type UidIndex;
  UserItemEvictorSet container_;
  IndexCache index_cache_;
  static const int KNNCOUNT = 200; // k:最近邻居个数
//  IceUtil::Mutex mutex_;
  IceUtil::RWRecMutex mutex_;

public:
  //   UserItemPtr GetUserItemPtr(int userid);
  vector<long> GetUserItemValue(int userid);//为了测试
  vector<int> GetGidUser(long groupid);//为了测试
  void SetUserItemValue(int userid, long adid);
  float GetUserSimValue(int userid1, int userid2);
  float GetUserSimValue(UserItemPtr userptr1, UserItemPtr userptr2);
  void Click(int userid, long groupid);
  map<int, float> GetKNNeighbour(UserItemEvictorSet & container,IndexCachePtr & indexcache, int userid1);
  map<long, float> GetRecommendation(int userid);
  bool NeedLoad(int userid);
  void Kick();
  bool Load(int userid);
  vector<long> LoadDB(int userid);
  void Init();
  int GetIndex(UserItemEvictorSet & container,int userid, long groupid);
  vector<int> LoadGidUidDB(int userid, long groupid);
};





#endif /* ADRECOMMENDATION_H_ */
