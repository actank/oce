#ifndef __EDM_VALIDATE_USER_H__
#define __EDM_VALIDATE_USER_H__
#include "Singleton.h"
#include "TaskManager.h"
#include <set>
namespace xce {
namespace edm {
/*
* 防止短时间内重复登录造成的新鲜事重复发送
*/
class DuplicateCache : public MyUtil::Singleton<DuplicateCache> {
public:
  bool Insert(int user_id);
  bool Find(int user_id);
  bool FindAndInsert(int user_id);
  bool Remove(int user_id);
  bool Clear();
private:
  std::set<int> user_login_;
  IceUtil::Mutex mutex_;
};
/*
  合法性检查统计入口
*/

class ValidateUser : public IceUtil::Shared {
public:
  bool CheckDuplicateAndInsert(int user_id);
  bool InsertIntoDuplicateCache(int user_id);
  bool RemoveFromDuplicateCache(int user_id);
  bool CheckUserStatus(int user_id);    
};
typedef IceUtil::Handle<ValidateUser> ValidateUserPtr;

}
}
#endif
