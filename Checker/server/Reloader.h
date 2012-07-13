/**
 * @file
 * @author  yang.yu@opi-corp.com
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Reloader.h： class Reloader: 
 *              class ReloaderFactory:
 */

#ifndef __RELOADER_H__
#define __RELOADER_H__

#include "UtilCxx/src/Singleton.h"
#include "BuddyByIdReplicaAdapter.h"
#include "BuddyApplyCacheAdapter.h"

namespace xce{
  namespace check{

    const static int RELOADERWARNSIZE = 3000;

    class Reloader : virtual public IceUtil::Shared {
      public:
        Reloader(){
        }

        Reloader(const std::string& name):
          _name(name){
          }

        virtual bool reload(const int id){ 
          MCE_WARN("[CHECKRELOAD] reload " << id << ", but do nothing!");
          return true;
        }

        virtual std::string getName(){
          return _name;
        }

        virtual void setName(const std::string& name){
          _name = name;
        }
      private:
        std::string _name;
    };
    typedef IceUtil::Handle<Reloader> ReloaderPtr;

    class CheckerBuddyReloader : virtual public Reloader {
      public:
        CheckerBuddyReloader(const std::string& name):
          Reloader(name){
          }

        virtual bool reload(const int id){
          MCE_INFO("[CHECKRELOAD] reload " << id);
          xce::buddy::adapter::BuddyByIdLoaderAdapter::instance().reload(id);			
          return true;
        }
    };
    typedef IceUtil::Handle<CheckerBuddyReloader> CheckerBuddyReloaderPtr;

    class CheckerApplyReloader : virtual public Reloader {
      public:
        CheckerApplyReloader(const std::string& name):
          Reloader(name){
          }
        virtual bool reload(const int id){
          MCE_INFO("[CHECKRELOAD] reload " << id);
          xce::buddyapplycache::BuddyApplyCacheAdapterN::instance().reload(id);
          return true;
        }
    };
    typedef IceUtil::Handle<CheckerApplyReloader> CheckerApplyReloaderPtr;

    class ReloaderFactory : virtual public IceUtil::Shared, virtual public MyUtil::Singleton<ReloaderFactory> {
      public:
        ReloaderPtr getReloader( const std::string& servicename ){
          IceUtil::RWRecMutex::RLock lock(_rwmutex);
          if(_reloadermap.end() == _reloadermap.find(servicename)){
            ReloaderPtr pReloader;
            if(servicename == "CheckerBuddy"){
              pReloader = new CheckerBuddyReloader("CheckerBuddyReloader");
              MCE_INFO("ReloaderFactory::getReloader(" << servicename <<") init CheckerBuddyReloader.");
            }else if(servicename == "CheckerApply"){
              const std::string name("CheckerApplyReloader");
              pReloader = new CheckerApplyReloader(name);
              MCE_INFO("ReloaderFactory::getReloader(" << servicename <<") init CheckerApplyReloader.");
            }else{
              const std::string name("Reloader");
              pReloader = new Reloader(name);
              MCE_INFO("ReloaderFactory::getReloader(" << servicename <<") init Reloader.");
            }
            lock.upgrade();
            _reloadermap[servicename] = pReloader;
          }
          return _reloadermap[servicename];

        }

      private:
        std::map<std::string,ReloaderPtr> _reloadermap;
        IceUtil::RWRecMutex _rwmutex; 
    };
    typedef IceUtil::Handle<ReloaderFactory> ReloaderFactoryPtr;

  };
};

#endif//__RELOADER_H__
