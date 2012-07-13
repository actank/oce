/*
 * =====================================================================================
 *
 *       Filename:  FansPageReloaderAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2010 03:40:57 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#include "FansPageReloaderAdapter.h"
#include "FansPage.h"

using namespace xce::buddy::adapter;
using namespace xce::buddy;

FansPageReloaderAdapter::FansPageReloaderAdapter() : MyUtil::ReplicatedClusterAdapterI <FansPageReloaderPrx>("FansPageReloader", 120, 300) {
}

void FansPageReloaderAdapter::updateState(int pageId, int userId, short state) {
  try {
    getProxy(0)->updateState(pageId, userId, state);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageReloaderAdapter::updateState] Ice::Exception " << e);
  }
}

void FansPageReloaderAdapter::insert(PageEntryPtr page) {
  try {
    getProxy(0)->insert(page);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageReloaderAdapter::insert] Ice::Exception " << e);
    //cout << "[FansPageReloaderAdapter::insert] Ice::Exception " << e.what() << endl;
  } catch(...) {
    //cout << "[FansPageReloaderAdapter::insert] Ice::Exception " << endl;
  }
}
//插入一批PageEntry
void FansPageReloaderAdapter::insertBatch(PageEntryList pageList) {
  try {
    getProxy(0)->insertBatch(pageList);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageReloaderAdapter::insertBatch] Ice::Exception " << e);
    //cout << "[FansPageReloaderAdapter::insertBatch] Ice::Exception " << e.what() << endl;
  } catch(...) {
    //cout << "[FansPageReloaderAdapter::insertBatch] Ice::Exception " << endl;
  }
}
void FansPageReloaderAdapter::deleteByUidPid(int pageId, int userId) {
  try {
    getProxy(0)->deleteByUidPid(pageId, userId);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageReloaderAdapter::deleteByUidPid] Ice::Exception " << e);
    //cout << "[FansPageReloaderAdapter::deleteByUidPid] Ice::Exception " << e.what() << endl;
  } catch (...) {
    //cout << "[FansPageReloaderAdapter::deleteByUidPid] Unknown::Exception" << endl;
  }
}
//删除用户userId的所有PageEntry
void FansPageReloaderAdapter::deleteByUid(int userId) {
  try {
    getProxy(0)->deleteByUid(userId);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageReloaderAdapter::deleteByUid] Ice::Exception " << e);
    //cout << "[FansPageReloaderAdapter::deleteByUid] Ice::Exception " << e.what() << endl;
  } catch(...) {
    //cout << "[FansPageReloaderAdapter::deleteByUid] Ice::Exception" << endl;
  }
}
//更新userId关注的所有PageEntry
void FansPageReloaderAdapter::loadFansPageByUid(int userId) {
  try {
    getProxy(0)->loadFansPageByUid(userId);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageReloaderAdapter::loadFansPageByUid] Ice::Exception " << e);
  }
}
//更新userId,pageId对应的PageEntry
void FansPageReloaderAdapter::loadFansPageByUidPid(int pageId, int userId) {
  try {
    getProxy(0)->loadFansPageByUidPid(pageId, userId);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageReloaderAdapter::loadFansPageByUidPid] Ice::Exception " << e);
  }
}

//更新userId和state对应的所有PageEntry
void FansPageReloaderAdapter::loadFansPageByUidState(int userId, short state) {
  try {
    getProxy(0)->loadFansPageByUidState(userId, state);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageReloaderAdapter::loadFansPageByUidState] Ice::Exception " << e);
  }
}
bool FansPageReloaderAdapter::isValid() {
  try {
    return getProxy(0)->isValid();
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageReloaderAdapter::isValid] Ice::Exception " << e);
    return false;
  }
}

void FansPageReloaderAdapter::setValid(bool valid) {
  try {
    getProxy(0)->setValid(valid);
  } catch (const Ice::Exception& e) {
    MCE_WARN("[FansPageReloaderAdapter::setValid] Ice::Exception " << e);
  }
}
