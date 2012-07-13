/*
 * =====================================================================================
 *
 *       Filename:  FansPageReloader.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/16/2010 02:01:13 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#ifndef __FANS_PAGE_RELOADERI_H__
#define __FANS_PAGE_RELOADERI_H__

#include <Singleton.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"
#include "FansPage.h"

namespace xce {
namespace buddy {

const int LOADPAGE_BYUIDSTATE_TASK_LEVEL = 1;
const int LOADPAGE_BYUIDPID_TASK_LEVEL = 2;
const int LOADPAGE_BYUID_TASK_LEVEL = 3;
const int LOADALLPAGE_TASK_LEVEL = 4;
const int UPDATE_STATE_TASK_LEVEL = 5;
const int INSERT_TASK_LEVEL = 6;
const int INSERT_BATCH_TASK_LEVEL = 6;
const int DELETE_BYUID_TASK_LEVEL = 7;
const int DELETE_BYUIDPID_TASK_LEVEL = 8;

class FansPageReloaderI : virtual public FansPageReloader, virtual public MyUtil::Singleton<FansPageReloaderI> {
public:
  virtual void updateState(int pageId, int userId, short state, const Ice::Current& current = Ice::Current());
  //更新userId关注的所有PageEntry
  virtual void insert(const PageEntryPtr& page, const Ice::Current& current = Ice::Current());
  virtual void insertBatch(const PageEntryList& pageList, const Ice::Current& current = Ice::Current());
  virtual void deleteByUidPid(int pageId, int userId, const Ice::Current& current = Ice::Current());
  virtual void deleteByUid(int userId, const Ice::Current& current = Ice::Current());

  virtual void loadFansPageByUid(int userId, const Ice::Current& current = Ice::Current());
  //更新userId,pageId对应的PageEntry
  virtual void loadFansPageByUidPid(int pageId, int userId, const Ice::Current& current = Ice::Current());
  //更新userId和state对应的所有PageEntry
  virtual void loadFansPageByUidState(int userId, short state, const Ice::Current& current = Ice::Current());
  virtual bool isValid(const Ice::Current& current = Ice::Current());
  virtual void setValid(bool valid, const Ice::Current& current = Ice::Current());
  static string getTableName(int userId);
private:
  void assignPageEntry(const PageEntryPtr& ptr1, const PageEntryPtr& ptr2);
  bool isValid_;
  IceUtil::RWRecMutex validMutex_;
};

class LoadPageByUidPidTask : virtual public MyUtil::Task {
public:
  LoadPageByUidPidTask(int pageId, int userId, int level = LOADPAGE_BYUIDPID_TASK_LEVEL);
protected:
  virtual void handle();
private:
  int userId_;
  int pageId_;
};

class LoadPageByUidTask : virtual public MyUtil::Task {
public:
  LoadPageByUidTask(int userId, int level = LOADPAGE_BYUID_TASK_LEVEL);
protected:
  virtual void handle();
private:
  int userId_;
};

class LoadPageByUidStateTask : virtual public MyUtil::Task {
public:
  LoadPageByUidStateTask(int userId, int state, int level = LOADPAGE_BYUIDSTATE_TASK_LEVEL);
protected:
  virtual void handle();
private:
  int userId_;
  int state_;
};

class UpdateStateTask : virtual public MyUtil::Task {
public:
  UpdateStateTask(int pageId, int userId, short state, int level = UPDATE_STATE_TASK_LEVEL);
protected:
  virtual void handle();
private:
  int userId_;
  int pageId_;
  short state_;
};

class InsertTask : virtual public MyUtil::Task {
public:
  InsertTask(PageEntryPtr& page, int level = INSERT_TASK_LEVEL);
protected:
  virtual void handle();
private:
  PageEntryPtr page_;
};

class DeleteByUidPidTask : virtual public MyUtil::Task {
public:
  DeleteByUidPidTask(int pageId, int userId, int level = DELETE_BYUIDPID_TASK_LEVEL);
protected:
  virtual void handle();
private:
  int userId_;
  int pageId_;
};

class DeleteByUidTask : virtual public MyUtil::Task {
public:
  DeleteByUidTask(int userId, int level = DELETE_BYUID_TASK_LEVEL);
protected:
  virtual void handle();
private:
  int userId_;
};


class ServiceSchemaI: public MyUtil::Descriptor {
public:
  virtual void configure(const Ice::PropertiesPtr& props);
};

}
}
#endif
