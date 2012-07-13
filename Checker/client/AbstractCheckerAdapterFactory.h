#ifndef _CHECKERADAPTERFACTORY_H_
#define _CHECKERADAPTERFACTORY_H_

#include "AbstractCheckerAdapter.h"
namespace xce{
  namespace check{

    class AbstractCheckerAdapterFactory : virtual public MyUtil::Singleton<AbstractCheckerAdapterFactory> {
      public:
        //adapter must be initalized with endpoint, path, step,replica before using 
        AbstractCheckerAdapter<MyUtil::BuddyChannel>* getBuddyChecker(std::string name, 
            std::string endpoints = "", std::string path = "", std::string step = "", std::string replica = ""){
          IceUtil::RWRecMutex::RLock lock(_buddyMutex);
          if(_buddyCheckerMap.end() == _buddyCheckerMap.find(name)){
            AbstractCheckerAdapter<MyUtil::BuddyChannel>* pBuddyChecker = new AbstractCheckerAdapter<MyUtil::BuddyChannel>;
            pBuddyChecker->configure(new MyUtil::BuddyChannel);
            pBuddyChecker->initialize(endpoints,path,step,replica);
            lock.upgrade();
            _buddyCheckerMap[name] = pBuddyChecker;
          }
          return _buddyCheckerMap[name];	
        }

        AbstractCheckerAdapter<MyUtil::UserChannel>* getUserChecker(std::string name,
            std::string endpoints = "", std::string path = "", std::string step = "", std::string replica = ""){
          IceUtil::RWRecMutex::RLock lock(_userMutex);
          if(_userCheckerMap.end() == _userCheckerMap.find(name)){
            AbstractCheckerAdapter<MyUtil::UserChannel>* pUserChecker = new AbstractCheckerAdapter<MyUtil::UserChannel>;
            pUserChecker->configure(new MyUtil::UserChannel);
            pUserChecker->initialize(endpoints,path,step,replica);
            lock.upgrade();
            _userCheckerMap[name] = pUserChecker;
          }
          return _userCheckerMap[name];
        }

        AbstractCheckerAdapter<MyUtil::OceChannel>* getOceChecker(std::string name,
            std::string endpoints = "", std::string path = "", std::string step = "", std::string replica = ""){
          IceUtil::RWRecMutex::RLock lock(_oceMutex);	
          if(_oceCheckerMap.end() == _oceCheckerMap.find(name)){
            AbstractCheckerAdapter<MyUtil::OceChannel>* pOceChecker = new AbstractCheckerAdapter<MyUtil::OceChannel>;
            pOceChecker->configure(new MyUtil::OceChannel);
            pOceChecker->initialize(endpoints,path,step,replica);
            lock.upgrade();
            _oceCheckerMap[name] = pOceChecker;
          }
          return _oceCheckerMap[name];
        }
      private:

        AbstractCheckerAdapterFactory(){
        }
        friend class MyUtil::Singleton<AbstractCheckerAdapterFactory>;

        std::map<std::string, AbstractCheckerAdapter<MyUtil::BuddyChannel>*>  _buddyCheckerMap;
        IceUtil::RWRecMutex _buddyMutex;
        std::map<std::string, AbstractCheckerAdapter<MyUtil::UserChannel>*> _userCheckerMap;
        IceUtil::RWRecMutex _userMutex;
        std::map<std::string, AbstractCheckerAdapter<MyUtil::OceChannel>*> _oceCheckerMap;
        IceUtil::RWRecMutex _oceMutex;
    };


  };
};
#endif
