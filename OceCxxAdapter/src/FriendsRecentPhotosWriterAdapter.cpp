/*
 * =====================================================================================
 *
 * Filename:  FriendsRecentPhotosWriterAdapter.cpp
 *
 *    Description:  
 *
 *  Version:  1.0
 *  Created:  11/02/2011 02:07:48 PM
 * Revision:  none
 * Compiler:  g++
 *
 *   Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *  Company:  renren-inc
 *
 * =====================================================================================
 */
#include "FriendsRecentPhotosWriterAdapter.h"

using namespace xce::friendsphotos;
using namespace friendsrecentphotos;

void FriendsRecentPhotosWriterAdapter::reloadUserPhotos(const MyUtil::IntSeq& userIds) {
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
  _userIdSet.insert(userIds.begin(), userIds.end());
  _mutex.notify();
}

void FriendsRecentPhotosWriterAdapter::run() {
  while (true) {
    MyUtil::IntSeq userIds;
    {
      if (_userIdSet.empty()) {
        _mutex.wait();
      }
      userIds.insert(userIds.end(), _userIdSet.begin(), _userIdSet.end());
      _userIdSet.clear();
    }
    if (!userIds.empty()) {
      try {
        vector<FriendsRecentPhotosWriterPrx> prxs = getAllFriendsRecentPhotosWriterPrx();
        for (vector<FriendsRecentPhotosWriterPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it) {
          (*it)->reloadUserPhotos(userIds);
        }
      } catch (const Ice::ConnectTimeoutException& e) {
        throw e;
      }
    }
  }
}

FriendsRecentPhotosWriterPrx FriendsRecentPhotosWriterAdapter::getFriendsRecentPhotosWriterPrx(int id) {
  return getProxy(id);
}

vector<FriendsRecentPhotosWriterPrx> FriendsRecentPhotosWriterAdapter::getAllFriendsRecentPhotosWriterPrx(int id) {
  return getAllProxySeq(id);
}
