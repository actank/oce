/*
 * =====================================================================================
 *
 * Filename:  FriendsRecentPhotosManagerAdapter.cpp
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
#include "FriendsRecentPhotosManagerAdapter.h"

using namespace xce::friendsphotos;
using namespace friendsrecentphotos;

PhotoResultPtr FriendsRecentPhotosManagerAdapter::getFriendsRecentPhotos(int userId, int begin, int limit) {
  FriendsRecentPhotosManagerPrx prx = NULL;
  PhotoResultPtr result = new PhotoResult;
  result->total = 0;
  try {
    prx = getFriendsRecentPhotosManagerPrx();
    result = prx->getFriendsRecentPhotos(userId, begin, limit);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  return result;
}

UserPhotosPtr FriendsRecentPhotosManagerAdapter::getUserRecentPhotos(int userId) {
  FriendsRecentPhotosManagerPrx prx = NULL;
  UserPhotosPtr result = new UserPhotos;
  try {
    prx = getFriendsRecentPhotosManagerPrx();
    result = prx->getUserRecentPhotos(userId);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  return result;
}

void FriendsRecentPhotosManagerAdapter::addRecentPhotos(const UserPhotosSeq& userPhotos) {
  try {
    vector<FriendsRecentPhotosManagerPrx> prxs = getAllFriendsRecentPhotosManagerPrx();
    for (vector<FriendsRecentPhotosManagerPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it) {
      (*it)->addRecentPhotos(userPhotos);
    }
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
}

void FriendsRecentPhotosManagerAdapter::deleteUserPhotos(const UserPhotosSeq& userPhotos) {
  try {
    vector<FriendsRecentPhotosManagerPrx> prxs = getAllFriendsRecentPhotosManagerPrx();
    for (vector<FriendsRecentPhotosManagerPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it) {
      (*it)->deleteUserPhotos(userPhotos);
    }
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
}

void FriendsRecentPhotosManagerAdapter::setData(const MyUtil::ObjectResultPtr& userPhotoData) {
  try {
    vector<FriendsRecentPhotosManagerPrx> prxs = getAllFriendsRecentPhotosManagerPrx();
    for (vector<FriendsRecentPhotosManagerPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it) {
      (*it)->setData(userPhotoData);
    }
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
}

FriendsRecentPhotosManagerPrx FriendsRecentPhotosManagerAdapter::getFriendsRecentPhotosManagerPrx(int id) {
  return getProxy(id);
}

vector<FriendsRecentPhotosManagerPrx> FriendsRecentPhotosManagerAdapter::getAllFriendsRecentPhotosManagerPrx(int id) {
  return getAllProxySeq(id);
}
