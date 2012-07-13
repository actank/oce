/*
 * =====================================================================================
 *
 *       Filename:  FansPageReloader.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/16/2010 02:02:02 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <QueryRunner.h>
#include "FansPageReloaderI.h"
#include "FansPageCacheAdapter.h"
#include "buddy/page/share/common.h"

using namespace MyUtil;
using xce::buddy::adapter::FansPageCacheAdapter;
using namespace xce::serverstate;
using namespace ::com::xiaonei::xce;
using namespace xce::buddy;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FansPageReloaderI::instance(), service.createIdentity("FPR", ""));
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FansPageReloader.Mod");
  string endpoint = service.getCommunicator()->getProperties()->getProperty("FansPageReloader.Endpoint");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FansPageReloader.Interval", 5);
  service.registerXceObserver(new ServiceSchemaI);

  // 向Controller注册
  ServerStateSubscriber::instance().initialize("ControllerFansPageReloader", &FansPageReloaderI::instance(), mod, interval);
}

void FansPageReloaderI::updateState(int pageId, int userId, short state, const Ice::Current& current) {
  MCE_INFO("[FansPageReloaderI::updateState] pageId = " << pageId << ", userId = " << userId << ", state = " << state);
  MyUtil::TaskManager::instance().execute(new UpdateStateTask(pageId, userId, state));
}

void FansPageReloaderI::insert(const PageEntryPtr& page, const Ice::Current& current) {
  MCE_INFO("[FansPageReloaderI::insert] id = " << page->id << ", pageId = " << page->pageId << ", userId = " << page->userId << ", state = " << page->state
      << ", oper = " << page->operatorId << ", addTime = " << page->addTime);
  PageEntryPtr temp_page = new PageEntry;
  assignPageEntry(temp_page, page);
  MCE_INFO("[FansPageReloaderI::insert] id = " << temp_page->id << ", pageId = " << temp_page->pageId << ", userId = " << temp_page->userId << ", state = " 
          << temp_page->state << ", oper = " << temp_page->operatorId << ", addTime = " << temp_page->addTime);
  MyUtil::TaskManager::instance().execute(new InsertTask(temp_page));
}

void FansPageReloaderI::insertBatch(const PageEntryList& list, const Ice::Current& current) {
  MCE_INFO("[FansPageReloaderI::insertBatch] size = " << list.size());
  for(PageEntryList::const_iterator it = list.begin(); it != list.end(); ++it) {
    insert(*it);
  }
}

void FansPageReloaderI::deleteByUidPid(int pageId, int userId, const Ice::Current& current) {
  MCE_INFO("[FansPageReloaderI::deleteByUidPid] pageId = " << pageId << ", userId = " << userId);
  MyUtil::TaskManager::instance().execute(new DeleteByUidPidTask(pageId, userId));
}
void FansPageReloaderI::deleteByUid(int userId, const Ice::Current& current) {
  MCE_INFO("[FansPageReloaderI::deleteByUid] userId = " << userId);
  MyUtil::TaskManager::instance().execute(new DeleteByUidTask(userId));
}

void FansPageReloaderI::loadFansPageByUid(int userId, const Ice::Current& current) {
  MCE_INFO("[FansPageReloaderI::loadFansPageByUid] userId = " << userId);
  MyUtil::TaskManager::instance().execute(new LoadPageByUidTask(userId));
}

void FansPageReloaderI::loadFansPageByUidPid(int pageId, int userId, const Ice::Current& current) {
  MCE_INFO("[FansPageReloaderI::loadFansPageByUidPid] pageId = " << pageId << ", userId = " << userId);
  MyUtil::TaskManager::instance().execute(new LoadPageByUidPidTask(pageId, userId));
}

void FansPageReloaderI::loadFansPageByUidState(int userId, short state, const Ice::Current& current) {
  MCE_INFO("[FansPageReloaderI::loadFansPageByUidState] userId = " << userId << ", state = " << state);
  MyUtil::TaskManager::instance().execute(new LoadPageByUidStateTask(userId, state));
}

void FansPageReloaderI::assignPageEntry(const PageEntryPtr& ptr1, const PageEntryPtr& ptr2) {
  ptr1->id = ptr2->id;
  ptr1->pageId = ptr2->pageId;
  ptr1->userId = ptr2->userId;
  ptr1->addTime = ptr2->addTime;
  ptr1->operatorId = ptr2->operatorId;
  ptr1->state = ptr2->state;
}

bool FansPageReloaderI::isValid(const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::isValid] " << isValid_);
  IceUtil::RWRecMutex::RLock lock(validMutex_);
  return isValid_;
}

void FansPageReloaderI::setValid(bool valid, const Ice::Current& current) {
  MCE_INFO("[FansPageCacheI::setValid] " << valid);
  IceUtil::RWRecMutex::WLock lock(validMutex_);
  isValid_ = valid;
  if(valid) {
    ServerStateSubscriber::instance().setStatus(1);
  } else{
    ServerStateSubscriber::instance().setStatus(0);
  }
}

LoadPageByUidPidTask::LoadPageByUidPidTask(int pageId, int userId, int level) : Task(level), userId_(userId), pageId_(pageId) {
}

void LoadPageByUidPidTask::handle() {
  PageEntryList list;
  int maxId = 0;
  int maxPageId = -1;
  BatchPageEntryResultHandlerI handler(maxId, maxPageId, list);
  Statement sql;
  ostringstream str_sql;
  str_sql << "select id, page_id, user_id, state, UNIX_TIMESTAMP(add_time), operator from " << PageHelper::getTableName(userId_) << " where user_id = " << userId_ 
      << " and page_id = " << pageId_;
  sql << str_sql.str();
  MCE_INFO("[LoadPagesByUidPidTask::handle] sql = " << str_sql.str());

  QueryRunner(DATA_SOURCE, CDbRServer).query(sql, handler);
  FansPageCacheAdapter::instance().updatePageEntryBatch(list);
}

LoadPageByUidTask::LoadPageByUidTask(int userId, int level) : Task(level), userId_(userId) {
}

void LoadPageByUidTask::handle() {
  PageEntryList list;
  int maxId = 0;
  int maxPageId = 0;
  BatchPageEntryResultHandlerI handler(maxId, maxPageId, list);
  Statement sql;
  ostringstream str_sql;
  str_sql << "select id, page_id, user_id, state, UNIX_TIMESTAMP(add_time), operator from " << PageHelper::getTableName(userId_) << " where user_id = " << userId_;
  sql << str_sql.str();
  MCE_INFO("[LoadPagesByUidTask::handle] sql = " << str_sql.str());
  QueryRunner(DATA_SOURCE, CDbRServer).query(sql, handler);
  FansPageCacheAdapter::instance().addPageEntryToUser(userId_, list);
}

LoadPageByUidStateTask::LoadPageByUidStateTask(int userId, int state, int level) : Task(level), userId_(userId), state_(state) {
}

void LoadPageByUidStateTask::handle() {
  PageEntryList list;
  int maxId = 0;
  int maxPageId = 0;
  BatchPageEntryResultHandlerI handler(maxId, maxPageId, list);
  Statement sql;
  ostringstream str_sql; 
  str_sql << "select id, page_id, user_id, state, UNIX_TIMESTAMP(add_time), operator from " << PageHelper::getTableName(userId_) << " where user_id = " << userId_ 
      << " and state = " << state_;
  sql << str_sql.str();
  MCE_INFO("[LoadPageByUidStateTask::handle] sql = " << str_sql.str());
  QueryRunner(DATA_SOURCE, CDbRServer).query(sql, handler);
  FansPageCacheAdapter::instance().updatePageEntryBatch(list);
}

UpdateStateTask::UpdateStateTask(int pageId, int userId, short state, int level) : Task(level), userId_(userId), pageId_(pageId), state_(state) {
}

void UpdateStateTask::handle() {
  //写入数据库
  Statement sql;
  ostringstream str_sql;
  str_sql << "update " << PageHelper::getTableName(userId_) << " set state = " << state_ << " where user_id = " << userId_ << " and page_id = " << pageId_;
  sql << str_sql.str();
  MCE_INFO("[UpdateStateTask::handle] sql = " << str_sql.str());
  QueryRunner(DATA_SOURCE, CDbWServer).schedule(sql);
  FansPageCacheAdapter::instance().updateState(pageId_, userId_, state_);
}

InsertTask::InsertTask(PageEntryPtr& page, int level) : Task(level), page_(page) {
}

void InsertTask::handle() {
  Statement sql;
  ostringstream str_sql;
  str_sql << "insert into " << PageHelper::getTableName(page_->userId) << "(page_id, user_id, state, p_type, operator, add_time) values(" 
          << page_->pageId << ", " << page_->userId << ", " << page_->state << ", 10, " << page_->operatorId << ", '" << MyUtil::Date(time(0)).str() << "')";
  sql << str_sql.str();
  MCE_INFO("[InsertTask::handle] sql = " << str_sql.str());
  QueryRunner(DATA_SOURCE, CDbWServer).schedule(sql);
  FansPageCacheAdapter::instance().insert(page_);
}

DeleteByUidPidTask::DeleteByUidPidTask(int pageId, int userId, int level) : Task(level), userId_(userId), pageId_(pageId) {
}

void DeleteByUidPidTask::handle() {
  Statement sql;
  ostringstream str_sql;
  str_sql << "delete from " << PageHelper::getTableName(userId_) << " where user_id = " << userId_ << " and page_id = " << pageId_;
  sql << str_sql.str();
  MCE_INFO("[DeleteByUidPidTask::handle] sql = " << str_sql.str());
  QueryRunner(DATA_SOURCE, CDbWServer).schedule(sql);
  MCE_INFO("[DeleteByUidPidTask::handle] call FansPageCacheAdapter");
  FansPageCacheAdapter::instance().deleteByUidPid(userId_, pageId_);
}

DeleteByUidTask::DeleteByUidTask(int userId, int level) : Task(level), userId_(userId) {
}

void DeleteByUidTask::handle() {
  Statement sql;
  ostringstream str_sql;
  str_sql << "delete from " << PageHelper::getTableName(userId_) << " where user_id = " << userId_;
  sql << str_sql.str();
  MCE_INFO("[DeleteByUidTask::handle] sql = " << str_sql.str());
  QueryRunner(DATA_SOURCE, CDbWServer).schedule(sql);
  FansPageCacheAdapter::instance().deleteByUid(userId_);
}

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
  TaskManager::instance().config(props, "LoadByUidState", LOADPAGE_BYUIDSTATE_TASK_LEVEL, ThreadPoolConfig(1, 5));
  TaskManager::instance().config(props, "LoadByUidPid", LOADPAGE_BYUIDPID_TASK_LEVEL, ThreadPoolConfig(1, 5));
  TaskManager::instance().config(props, "LoadByUid", LOADPAGE_BYUID_TASK_LEVEL, ThreadPoolConfig(1, 5));
  TaskManager::instance().config(props, "LoadAll", LOADALLPAGE_TASK_LEVEL, ThreadPoolConfig(1, 5));
  
  TaskManager::instance().config(props, "DeleteByUidPid", DELETE_BYUIDPID_TASK_LEVEL, ThreadPoolConfig(1, 5));
  TaskManager::instance().config(props, "DeleteByUid", DELETE_BYUID_TASK_LEVEL, ThreadPoolConfig(1, 5));
  TaskManager::instance().config(props, "Insert", INSERT_TASK_LEVEL, ThreadPoolConfig(1, 5));
  TaskManager::instance().config(props, "InsertBatch", INSERT_BATCH_TASK_LEVEL, ThreadPoolConfig(1, 5));
  TaskManager::instance().config(props, "UpdateState", UPDATE_STATE_TASK_LEVEL, ThreadPoolConfig(1, 5));
}

