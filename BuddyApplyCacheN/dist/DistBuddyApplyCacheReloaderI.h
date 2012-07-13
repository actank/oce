#ifndef __DIST_BUDDYAPPLY_CACHE_RELOADERI_H__
#define __DIST_BUDDYAPPLY_CACHE_RELOADERI_H__

#include <ServiceI.h>
#include "BuddyApplyCache.h"
#include "DistBuddyApplyCacheAdapter.h"

namespace xce {
  namespace buddyapplycache {

    const std::string DIST_BUDDYAPPLY_CACHE_RELOADER = "M";
    const int TASK_LEVEL_RELOAD = 1;
    const int TASK_LEVEL_ADD = 2;
    const int TASK_LEVEL_REMOVE = 3;

    class DistBuddyApplyCacheReloaderI : 
      virtual public xce::buddyapplycache::DistBuddyApplyCacheReloader, 
      virtual public MyUtil::Singleton<DistBuddyApplyCacheReloaderI>
    {
      public:
        virtual bool isValid(const Ice::Current& = Ice::Current());
        virtual void setValid(bool valid, const Ice::Current& = Ice::Current());

        virtual void reload(const MyUtil::IntSeq& ids, const Ice::Current&);

        virtual void add(int accepter, const BuddyApplyCacheDataN& applicant,const Ice::Current& current);
        virtual void remove(int accepter, int applicant, const Ice::Current& current);
      
      private:
        DistBuddyApplyCacheReloaderI(){
        }
        friend class MyUtil::Singleton<DistBuddyApplyCacheReloaderI>;

        bool valid_;
        IceUtil::RWRecMutex validmutex_;
    };

    class ReloadTask : public MyUtil::Task {
      public:
        ReloadTask(const MyUtil::IntSeq&, const Ice::Current&);
        virtual void handle();
      private:
        void DistributeIds(std::vector<std::vector<int> >& ids);
        MyUtil::IntSeq ids_;
        Ice::Current current_;
    };

  }
}

#endif
