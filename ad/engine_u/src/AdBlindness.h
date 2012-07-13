/*
 * AdBlindness.h
 *
 *  Created on: 2011-4-18
 *      Author: sunzz
 */

#ifndef ADBLINDNESS_H_
#define ADBLINDNESS_H_
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <Ice/Ice.h>
#include <map>
#include "Singleton.h"

using boost::multi_index_container;

using namespace boost::multi_index;
using namespace MyUtil;
namespace xce{
namespace ad{
class User : public IceUtil::Shared{
public:
  int user_id_;
  map <Ice::Long, pair<int, int> > pv_click_;
  void AddPv(Ice::Long group_id) {
    map<Ice::Long, pair<int, int> >::iterator it = pv_click_.find(group_id);
    if(it != pv_click_.end()) {
      it->second.first += 1;
    } else {
      pv_click_[group_id] = pair<int,int> (1,0);
    }
  }
  void AddClick(Ice::Long group_id) {
    map<Ice::Long, pair<int, int> >::iterator it = pv_click_.find(group_id);
    if(it != pv_click_.end()) {
      it->second.second += 1;
    } else {
      pv_click_[group_id] = pair<int,int> (0,1);
    }
  }
  int GetPv(Ice::Long group_id){
    map<Ice::Long, pair<int, int> >::iterator it = pv_click_.find(group_id);
    if(it != pv_click_.end()) {
      return it->second.first;
    }
    return 0;
  }
  int GetClick(Ice::Long group_id) {
    map<Ice::Long, pair<int, int> >::iterator it = pv_click_.find(group_id);
    if(it != pv_click_.end()) {
      return it->second.second;
    }
    return 0;
  }
};

typedef IceUtil::Handle<User> UserPtr;

class AdBlindness : public MyUtil::Singleton<AdBlindness> {
private:
  typedef boost::multi_index_container<UserPtr, indexed_by<sequenced<> ,
      ordered_unique<BOOST_MULTI_INDEX_MEMBER(User, int, user_id_)> > >
  UserCacheEvictor;

  typedef UserCacheEvictor::nth_index<0>::type SeqIndex;
  typedef UserCacheEvictor::nth_index<1>::type UidIndex;
  IceUtil::Mutex mutex_;
  UserCacheEvictor evictor_;
  static const int kUserSize = 300000; //内存中的用户数
  //static const int pv_threshold = 3;
  void AddUserData(int userid, Ice::Long group_id, bool pv_flag = true);
public:
  void AddClick(int userid, Ice::Long group_id);
  void AddPv(int userid,Ice::Long group_id);
  bool IsDisplay(int userid, Ice::Long group_id);
};

}
}
#endif /* ADBLINDNESS_H_ */
