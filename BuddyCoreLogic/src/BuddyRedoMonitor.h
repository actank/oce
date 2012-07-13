#ifndef __BUDDY_REDO_MONITOR_H__
#define __BUDDY_REDO_MONITOR_H__

#include "ReplicaCluster/CacheLoader/src/RedoMonitor.h"
#include "BuddyApplyCacheAdapter.h"
#include "BuddyByIdReplicaAdapter.h"
#include "DistBuddyApplyCacheReloaderAdapter.h"
#include "DistBuddyByIdCacheReloaderAdapter.h"

namespace xce{

  namespace redomonitor{

    template<class T> class RebuildBuddyApplyCacheRedoMonitor :  virtual public xce::redomonitor::RedoMonitor,virtual public Ice::Object
    {
      public:
        RebuildBuddyApplyCacheRedoMonitor( T& ctx, int id, bool beUseQueue, std::string desc):
          xce::redomonitor::RedoMonitor(beUseQueue,desc),_ctx(ctx),_id(id){
          }
        virtual void handle(){
          xce::buddyapplycache::BuddyApplyCacheAdapterN::instance().rebuildBuddyApplyCache(_id,_ctx);
        }
      private:
        T _ctx;
        int _id;
    };

    template<class T> class RemoveBuddyApplyCacheRedoMonitor :  virtual public xce::redomonitor::RedoMonitor,virtual public Ice::Object
    {
      public:
        RemoveBuddyApplyCacheRedoMonitor( int id, int applicant, bool beUseQueue, std::string desc):
          xce::redomonitor::RedoMonitor(beUseQueue,desc),_id(id),_applicant(applicant){
          }
        virtual void handle(){
          xce::buddyapplycache::BuddyApplyCacheAdapterN::instance().removeBuddyApplyCache(_id,_applicant);
        }
      private:
        int _id;
        int _applicant;
    };

    template<class T> class AddBuddyApplyNotifyReloaderRedoMonitor :  virtual public xce::redomonitor::RedoMonitor,virtual public Ice::Object
    {
      public:
        AddBuddyApplyNotifyReloaderRedoMonitor( T& ctx, int id, bool beUseQueue, std::string desc):
          xce::redomonitor::RedoMonitor(beUseQueue,desc),_ctx(ctx),_id(id){
          }
        virtual void handle(){
          xce::distcache::buddyapplycache::DistBuddyApplyCacheReloaderAdapter::instance().add(_id,_ctx);
        }
      private:
        T _ctx;
        int _id;
    };

    template<class T> class RemoveBuddyApplyNotifyReloaderRedoMonitor :  virtual public xce::redomonitor::RedoMonitor,virtual public Ice::Object
    {
      public:
        RemoveBuddyApplyNotifyReloaderRedoMonitor( int accepter, int applicant, bool beUseQueue, std::string desc):
          xce::redomonitor::RedoMonitor(beUseQueue,desc),_accepter(accepter),_applicant(applicant){
          }
        virtual void handle(){
          xce::distcache::buddyapplycache::DistBuddyApplyCacheReloaderAdapter::instance().remove(_accepter,_applicant);
        }
      private:
        int _accepter;
        int _applicant;
    };

    template<class T> class AddBuddyByIdLoaderRedoMonitor :  virtual public xce::redomonitor::RedoMonitor,virtual public Ice::Object
    {
      public:
        AddBuddyByIdLoaderRedoMonitor( int userid, int friendid, bool beUseQueue, std::string desc):
          xce::redomonitor::RedoMonitor(beUseQueue,desc),_userid(userid),_friendid(friendid){
          }
        virtual void handle(){
          xce::buddy::adapter::BuddyByIdLoaderAdapter::instance().add(_userid,_friendid);
        }
      private:
        int _userid;
        int _friendid;
    };

    template<class T> class BecomeFriendNotifyByIdReloaderRedoMonitor :  virtual public xce::redomonitor::RedoMonitor,virtual public Ice::Object
    {
      public:
        BecomeFriendNotifyByIdReloaderRedoMonitor( int host, int guest, bool beUseQueue, std::string desc):
          xce::redomonitor::RedoMonitor(beUseQueue,desc),_host(host),_guest(guest){
          }
        virtual void handle(){
          xce::distcache::buddybyidcache::DistBuddyByIdCacheReloaderAdapter::instance().add(_host,_guest);
        }
      private:
        int _host;
        int _guest;
    };
 
    template<class T> class RemoveFriendNotifyByIdReloaderRedoMonitor :  virtual public xce::redomonitor::RedoMonitor,virtual public Ice::Object
    {
      public:
        RemoveFriendNotifyByIdReloaderRedoMonitor( int host, int guest, bool beUseQueue, std::string desc):
          xce::redomonitor::RedoMonitor(beUseQueue,desc),_host(host),_guest(guest){
          }
        virtual void handle(){
          xce::distcache::buddybyidcache::DistBuddyByIdCacheReloaderAdapter::instance().remove(_host,_guest);
        }
      private:
        int _host;
        int _guest;
    }; 
  }
}

#endif
