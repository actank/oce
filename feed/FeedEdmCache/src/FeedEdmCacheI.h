#ifndef __EDM_FEED_EDM_CACHE_I_H__
#define __EDM_FEED_EDM_CACHE_I_H__
#include "FeedEdmCache.h"
#include "FeedEdmModel.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include "IceUtil/RWRecMutex.h"
#include "TaskManager.h"
namespace xce {
namespace edm {
using namespace MyUtil;
class FeedEdmCacheI : public FeedEdmCache, public MyUtil::Singleton<FeedEdmCacheI> {
public:

	//--------------------------------------------------------------------------
  FeedSeedInfoPtr SelectEdmById(int edmid, const Ice::Current& = Ice::Current());
  FeedEdmModelPtr _SelectEdmById(int edmid); 
  FeedSeedInfoPtr SelectEdmByType(int user_id, int type, const Ice::Current& = Ice::Current());
  FeedEdmModelPtr _SelectEdmByType(int user_id, int type); 
	//--------------------------------------------------------------------------

  FeedEdmCacheI();
  FeedSeedInfoPtr SelectEdm(Ice::Int user_id, const MyUtil::Str2StrMap& params, const FeedUserInfoPtr& user, const Ice::Current& = Ice::Current());
  FeedSeedInfoPtr SelectEdmForPhone(Ice::Int user_id, const MyUtil::Str2StrMap& params, const FeedUserInfoPtr& user, const Ice::Current& = Ice::Current());
  void SetSendingFlag(Ice::Int edm_id, const Ice::Current& = Ice::Current());
  void SetSuspendFlag(Ice::Int edm_id, const Ice::Current& = Ice::Current());
  void UpdateEdmInfo(Ice::Int edm_id, const Ice::Current& = Ice::Current());
  void SetForComplete(const Ice::Current& = Ice::Current());
  std::string LookUpEdm(int edmId, const Ice::Current& = Ice::Current());
  void Initialize();
private:
  void _Load();
  FeedEdmModelPtr _SelectEdm(Ice::Int user_id, const MyUtil::Str2StrMap& params, const FeedUserInfoPtr& user); 
  FeedEdmModelPtr _SelectEdmForPhone(Ice::Int user_id, const MyUtil::Str2StrMap& params, const FeedUserInfoPtr& user); 
  void _LoadEdmModelFromDb();
  ConfigureModelPtr _FormatEdmConfigure(const mysqlpp::Row& row);
  void _CheckConsume(FeedEdmModelPtr fit_edm_ptr);
  std::list<FeedEdmModelPtr>& _MappingSeq(int type);
private:
  bool need_init_;
  bool init_complete_;
  std::list<FeedEdmModelPtr> null_type_seq_;
  std::list<FeedEdmModelPtr> orient_seq_;
  std::list<FeedEdmModelPtr> assign_seq_;
private:
  class DbConsumeOperator : public MyUtil::Task {
    public:
      DbConsumeOperator(Ice::Int edm_id, Ice::Int user_id) : Task(TASK_LEVEL_QUERY), edm_id_(edm_id), user_id_(user_id) {}
      virtual void handle();
    private:
      Ice::Int edm_id_;
      Ice::Int user_id_;
  };
  typedef IceUtil::Handle<DbConsumeOperator> DbConsumeOperatorPtr;
  class DbConsumeOperatorUncheck : public MyUtil::Task {
    public:
      DbConsumeOperatorUncheck(Ice::Int edm_id, Ice::Int user_id) : Task(TASK_LEVEL_QUERY), edm_id_(edm_id), user_id_(user_id) {}
      virtual void handle();
    private:
      Ice::Int edm_id_;
      Ice::Int user_id_;
  };
  typedef IceUtil::Handle<DbConsumeOperatorUncheck> DbConsumeOperatorUncheckPtr;
  class DbStatusOperator : public MyUtil::Task {
    public:
      DbStatusOperator(Ice::Int edm_id) : Task(TASK_LEVEL_QUERY), edm_id_(edm_id) {}
      virtual void handle();
    private:
      Ice::Int edm_id_;
  };
  typedef IceUtil::Handle<DbStatusOperator> DbStatusOperatorPtr;
/*
  when reload from db,first reload data to temp list,when swap
  there is some synchronization questions when set edm status at the same time. 
  to resolve this, before setting edm's stutas,lock temp mutex to confirm reload isn't running.

*/
  IceUtil::RWRecMutex mutex_;
  std::list<FeedEdmModelPtr> null_type_seq_temp_;
  std::list<FeedEdmModelPtr> orient_seq_temp_;
  std::list<FeedEdmModelPtr> assign_seq_temp_;
};
}
}
#endif
