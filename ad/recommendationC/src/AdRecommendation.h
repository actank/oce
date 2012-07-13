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
#include"AdRecommendItemI.h"
#include "AdReConfig.h"
#include <boost/lexical_cast.hpp>
using namespace std;
using namespace boost::multi_index;
using namespace boost;
using namespace MyUtil;
using namespace xce::ad;

const double probability = 0.16;//已点击过的广告降低到原来的百分比
const size_t RecommendNum= 30;
const int GidType63 = 0;
const int GidTypeBrand = 1;


//const size_t ItemSize = 1500;
//const size_t kBitSetSize = ItemSize * 2;//广告数*占的位数

class UserUgc: public IceUtil::Shared {
private:
public:
  int userid;
  int total_ugc;
  map<int,int> ugc;
  map<int,int> pos;
};
typedef IceUtil::Handle<UserUgc> UserUgcPtr;

class UserUgcPool : public Singleton<UserUgcPool> {
private:
  typedef map<int , UserUgcPtr> UserUgcEvictor;
  UserUgcEvictor user_ugc_map_;
  map<int, vector<Ice::Long> > gidmap_;
  IceUtil::Mutex mutex_;
  /////////////////////////////////////////////////////////
  static const size_t RecommendNum= 16;
  //////////////////////////////////////////////////////////////////////////
  void InitUserUgcFromFile(UserUgcEvictor &umap);
  void InitGidMapFromFile(map<int, vector<Ice::Long> > &gmap);
public:
  map<Ice::Long, double> GetRecommendation(int userid);
  void Init();

};





#endif /* ADRECOMMENDATION_H_ */
