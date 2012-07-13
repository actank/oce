#ifndef _BUDDYAPPLYCACHE_ICE_
#define _BUDDYAPPLYCACHE_ICE_

#include <Util.ice>
#include <ObjectCache.ice>
module xce {
  module buddyapplycache {
/*
    struct BuddyApplyCacheData
    {
      int applicant;
      int time;
      string applicantName;
      int enrollmentYear;
      string schoolid;
      string dorm;
      string headUrl;
      string why;
      string schoolName;
      int selected;
      int star;
    };
    sequence<BuddyApplyCacheData> BuddyApplyCacheDataSeq;

    struct BuddyApplyCacheWithTotal{
      BuddyApplyCacheDataSeq caches;
      int totalCount;
    };

    interface BuddyApplyCacheManager {
      BuddyApplyCacheDataSeq getBuddyApplyCache(int userId,int begin,int limit);
      BuddyApplyCacheWithTotal getBuddyApplyCacheWithTotal(int userId,int begin,int limit);
      void removeBuddyApplyCache(int userId,int applicant);
      void clear(int userId);
      void rebuildBuddyApplyCache(int userId);
      int getHardLimit();
    };
*/
    struct BuddyApplyCacheDataN
    {
      int applicant;
      int time;
      string why;
      string statfrom;
      string groupname;
    };
    sequence<BuddyApplyCacheDataN> BuddyApplyCacheDataSeqN;

    struct BuddyApplyCacheWithTotalN{
      BuddyApplyCacheDataSeqN caches;
      int totalCount;
    };


    class BuddyApplyCacheN{
      BuddyApplyCacheDataSeqN buddyApplyCaches;
      int  ownerId;
    };

    interface BuddyApplyCacheManagerN {
      BuddyApplyCacheDataSeqN getBuddyApplyCache(int userId,int begin,int limit);
      BuddyApplyCacheWithTotalN getBuddyApplyCacheWithTotal(int userId,int begin,int limit);
      void removeBuddyApplyCache(int userId,int applicant);
      void clear(int userId);
      void rebuildBuddyApplyCache(int userId);
      int getHardLimit();
      void reload(int userId);
      void setValid(bool valid);
      void setData(MyUtil::ObjectResult data);
      bool isValid();

    };

    interface DistBuddyApplyCacheReloader
    {
      void reload(MyUtil::IntSeq userIds);
    
      void add(int accepter, BuddyApplyCacheDataN applicant);
      void remove(int accepter, int applicant); 
 
      bool isValid();
      void setValid(bool newState);
    };
  };
};

#endif
