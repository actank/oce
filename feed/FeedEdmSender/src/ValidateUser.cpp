#include "ValidateUser.h"
#include "LogWrapper.h"
#include "UserStateAdapter.h"

namespace xce {
namespace edm {

bool DuplicateCache::Insert(int user_id) {
  IceUtil::Mutex::Lock lock(mutex_);
  user_login_.insert(user_id);
  return true;
}
bool DuplicateCache::Find(int user_id) {
  IceUtil::Mutex::Lock lock(mutex_);
  std::set<int>::iterator ite = user_login_.find(user_id);
  if (ite != user_login_.end()) {
    return true;
  }
  return false;
}
bool DuplicateCache::FindAndInsert(int user_id) {
  IceUtil::Mutex::Lock lock(mutex_);
  std::set<int>::iterator ite = user_login_.find(user_id);
  if (ite != user_login_.end()) {
    return true;
  } 
  user_login_.insert(user_id);
  return false;
  
}
bool DuplicateCache::Remove(int user_id) {
  IceUtil::Mutex::Lock lock(mutex_);
  std::set<int>::iterator ite = user_login_.find(user_id);
  if (ite != user_login_.end()) {
    user_login_.erase(ite);
  }  
  return true;
}
bool DuplicateCache::Clear() {
  IceUtil::Mutex::Lock lock(mutex_);
  user_login_.clear();
  return true;
}

bool ValidateUser::CheckDuplicateAndInsert(int user_id) {
  return DuplicateCache::instance().DuplicateCache::FindAndInsert(user_id);
}
bool ValidateUser::InsertIntoDuplicateCache(int user_id) {
  return DuplicateCache::instance().Insert(user_id);
}
bool ValidateUser::RemoveFromDuplicateCache(int user_id) {
  return DuplicateCache::instance().Remove(user_id);
}
bool ValidateUser::CheckUserStatus(int user_id) {
  xce::adapter::userstate::UserStateInfoPtr user_state_ptr = xce::adapter::userstate::UserStateAdapter::instance().getUserState(user_id);
  return user_state_ptr->isGuideDone();
}
}
}
