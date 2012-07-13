/*
 * =====================================================================================
 *
 *       Filename:  MenuTripod.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年12月14日 16时44分00秒
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#ifndef __MENU_TRIPOD_H__
#define __MENU_TRIPOD_H__

#include <Ice/Ice.h>
#include "MenuCache.h"
#include "MenuTripodShare.h"
#include "Singleton.h"
#include "tripod-new/src/cpp/include/TripodClient.h"
#include <IceUtil/IceUtil.h>

namespace xce {
namespace menutripod {

using namespace com::renren::tripod;
using namespace xce::menu;
using namespace MyUtil;

enum OpType {Add, Delete};

class ReloadFromTripodThread : public IceUtil::Thread {
public:
  ReloadFromTripodThread();
  void add(const KeySeq& keys);
  virtual void run();
private:
  IceUtil::Monitor<IceUtil::Mutex> mutex_;
  KeySeq keysToReload_;
  TripodClient* tripodClient_;
};

typedef IceUtil::Handle<ReloadFromTripodThread> ReloadFromTripodThreadPtr;

class WriteRecordToDBThread : public IceUtil::Thread {
public:
  WriteRecordToDBThread(const ReloadFromTripodThreadPtr& ptr);
  virtual void run();
  void add(const vector<MenuTripodDataPtr>& menuTripodDataPtrs);
private:
  IceUtil::Monitor<IceUtil::Mutex> mutex_;
  vector<MenuTripodDataPtr> menuTripodDataPtrs_;
  void write(const vector<MenuTripodDataPtr>& menuTripodDataPtrs);
  void write(const map<int, vector<MenuTripodDataPtr> >& modToMenuTripodDataPtrs, const string& dbSource, const string tablePrefix);
  map<CacheType, map<int, vector<MenuTripodDataPtr> > > seperate(const vector<MenuTripodDataPtr>& menuTripodDataPtrs, int mod);
  ReloadFromTripodThreadPtr reloadThreadPtr_;
};

typedef IceUtil::Handle<WriteRecordToDBThread> WriteRecordToDBThreadPtr;

class MenuCacheManagerI : virtual public MenuCacheManager, public MyUtil::Singleton<MenuCacheManagerI> {
public:
  void init();
  //类型数组typeArray，0返回ALL数组，1返回REC数组，2返回FAV数组
  virtual CacheType2AppIdSeqMap get(::Ice::Int, const CacheTypeSeq&, const Ice::Current& current = Ice::Current());

  virtual CacheType2AppIdSeqMap getAll(::Ice::Int, const Ice::Current&);

  virtual void set(::Ice::Int, CacheType, const AppIdSeq&, const Ice::Current&);

  virtual void setBatch(const MenuCacheRecordSeq& records, const Ice::Current&);
  virtual void addBatch(const MenuCacheRecordSeq& records, const Ice::Current& current = Ice::Current());
  virtual void deleteBatch(const MenuCacheRecordSeq& records, const Ice::Current& current = Ice::Current());

  virtual void load(const UserIdSeq&, const Ice::Current& current = Ice::Current());

  virtual bool isValid(const Ice::Current& = Ice::Current());

  virtual void setValid(bool newState, const Ice::Current& = Ice::Current());
  virtual void setData(const MyUtil::ObjectResultPtr& objects, CacheType type,
      const Ice::Current& = Ice::Current());
private:
  void operate(const MenuCacheRecordSeq& records, OpType type);
  bool isValid_;
  void addKeysToLoad(const KeySeq& keys);
  map<string, MenuTripodDataPtr> getWithKeys(const KeySeq& keys);
  map<string, MenuTripodDataPtr> getFromDB(const KeySeq& keys);
  IceUtil::RWRecMutex validMutex_;
  TripodClient* tripodClient_;
  CacheTypeSeq allCacheTypes_;
  WriteRecordToDBThreadPtr writeThread_;
  ReloadFromTripodThreadPtr loadThread_;
};

}
}
#endif
