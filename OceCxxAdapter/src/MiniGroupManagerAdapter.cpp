/*
 * =====================================================================================
 *
 *       Filename:  MiniGroupManagerAdapter.cpp
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

#include "MiniGroupManagerAdapter.h"
using namespace xce::mngp;
using namespace minigroup;

MiniGroupManagerPrx MiniGroupManagerAdapter::getMiniGroupManagerPrx(int id) {
  return getProxy(id);
}

vector<MiniGroupManagerPrx> MiniGroupManagerAdapter::getAllMiniGroupManagerPrx(int id) {
  return getAllProxySeq(0);
}

MiniGroupSeq MiniGroupManagerAdapter::getMiniGroups(int user_id) {
  MiniGroupManagerPrx prx = NULL;
  MiniGroupSeq result;
  try {
    prx = getMiniGroupManagerPrx();
    result = prx->getMiniGroups(user_id);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  return result;
}

MiniGroupSeq MiniGroupManagerAdapter::getMiniGroupsWithIds(const MyUtil::LongSeq& miniGroupIds) {
  MiniGroupManagerPrx prx = NULL;
  MiniGroupSeq result;
  try {
    prx = getMiniGroupManagerPrx();
    result = prx->getMiniGroupsWithIds(miniGroupIds);
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
  return result;
}

void MiniGroupManagerAdapter::addMiniGroups(const MiniGroupSeq& minigroups) {
  try {
    vector<MiniGroupManagerPrx> prxs = getAllMiniGroupManagerPrx();
    for (vector<MiniGroupManagerPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it) {
      (*it)->addMiniGroups(minigroups);
    }
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
}

void MiniGroupManagerAdapter::setJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids) {
  try {
    vector<MiniGroupManagerPrx> prxs = getAllMiniGroupManagerPrx();
    for (vector<MiniGroupManagerPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it) {
      (*it)->setJoinedMiniGroups(user_id, minigroup_ids);
    }
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
}

void MiniGroupManagerAdapter::addJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids) {
  try {
    vector<MiniGroupManagerPrx> prxs = getAllMiniGroupManagerPrx();
    for (vector<MiniGroupManagerPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it) {
      (*it)->addJoinedMiniGroups(user_id, minigroup_ids);
    }
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
}

void MiniGroupManagerAdapter::removeJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids) {
  try {
    vector<MiniGroupManagerPrx> prxs = getAllMiniGroupManagerPrx();
    for (vector<MiniGroupManagerPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it) {
      (*it)->removeJoinedMiniGroups(user_id, minigroup_ids);
    }
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
}

void MiniGroupManagerAdapter::removeAllJoinedMiniGroups(int user_id) {
  try {
    vector<MiniGroupManagerPrx> prxs = getAllMiniGroupManagerPrx();
    for (vector<MiniGroupManagerPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it) {
      (*it)->removeAllJoinedMiniGroups(user_id);
    }
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
}

void MiniGroupManagerAdapter::setValid(bool valid) {
  try {
    vector<MiniGroupManagerPrx> prxs = getAllMiniGroupManagerPrx();
    for (vector<MiniGroupManagerPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it) {
      (*it)->setValid(valid);
    }
  } catch (const Ice::ConnectTimeoutException& e) {
    throw e;
  }
}

