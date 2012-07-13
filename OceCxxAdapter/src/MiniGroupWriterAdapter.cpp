/*
 * =====================================================================================
 *
 *       Filename:  MiniGroupWriterAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/26/2011 11:35:52 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#include "MiniGroupWriterAdapter.h"
using namespace xce::mngp;
using namespace minigroup;

MiniGroupWriterPrx MiniGroupWriterAdapter::getMiniGroupWriterPrx(int id) {
  return getProxy(id);
}

void MiniGroupWriterAdapter::setJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids) {
  getMiniGroupWriterPrx(0)->setJoinedMiniGroups(user_id, minigroup_ids);
}

void MiniGroupWriterAdapter::addJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids) {
  getMiniGroupWriterPrx(0)->addJoinedMiniGroups(user_id, minigroup_ids);
}

void MiniGroupWriterAdapter::removeJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids) {
  getMiniGroupWriterPrx(0)->removeJoinedMiniGroups(user_id, minigroup_ids);
}

void MiniGroupWriterAdapter::reloadUsers(const MyUtil::IntSeq& userIds) {
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
  _userIdSet.insert(userIds.begin(), userIds.end());
  _mutex.notify();
}

void MiniGroupWriterAdapter::reloadMiniGroups(const MyUtil::LongSeq& miniGroupIds) {
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
  _userIdSet.insert(miniGroupIds.begin(), miniGroupIds.end());
  _mutex.notify();
}

void MiniGroupWriterAdapter::run() {
  while (true) {
    MyUtil::IntSeq userIds;
    MyUtil::LongSeq miniGroupIds;
    {
      IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
      if (_userIdSet.empty() && _miniGroupIdSet.empty()) {
        _mutex.wait();
      }
      userIds.insert(userIds.end(), _userIdSet.begin(), _userIdSet.end());
      _userIdSet.clear();
      miniGroupIds.insert(miniGroupIds.end(), _miniGroupIdSet.begin(), _miniGroupIdSet.end());
      _miniGroupIdSet.clear();
    }
    if (!userIds.empty()) {
      try {
        getMiniGroupWriterPrx(0)->reloadUsers(userIds);
      } catch(Ice::Exception& e) {
        MCE_WARN("[MiniGroupWriterAdapter::run] reloadUsers size = " << userIds.size());
      }
    }
    if (!miniGroupIds.empty()) {
      try {
        getMiniGroupWriterPrx(0)->reloadMiniGroups(miniGroupIds);
      } catch(Ice::Exception& e) {
        MCE_WARN("[MiniGroupWriterAdapter::run] reloadMiniGroups size = " << miniGroupIds.size());
      }
    }
  }
}
