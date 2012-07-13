/**
 * @file
 * @author  yang.yu@opi-corp.com
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * PathChecker.h：
 * 		class PathChecker: 路径检测类,对相应key的路径进行调用或数据校验
 * 		class SequencePathChecker: 带顺序检测的路径检测类，对相应key的路径进行调用或数据校验并附带调用顺序校验  
 *		class FullPathChecker: 全路径检测类，用于对多条路径进行检测
 */

#ifndef __PATHCHECKER_H__
#define __PATHCHECKER_H__

#include "UtilCxx/src/AtomicSTL.h"
#include "Path.h"
#include "util/cpp/TimeCost.h"
#include "IceLogger.h"
#include "Checker/share/OperaData.h"
#include "Sequence.h"
#include "Reloader.h"
#include "XceAlertAdapter.h"

namespace xce{
  namespace check{

    class PathChecker : virtual public IceUtil::Shared, virtual public IceUtil::Mutex {

      public:
        PathChecker(){};
        virtual ~PathChecker(){};

        PathPtr find(const std::string& key){
          IceUtil::Mutex::Lock lock(*this);
          //Check if we have a object in the map already
          PathMapIter i = _pathMap.find(key);
          if (i != _pathMap.end()) {
            // Got the entry already, dequeue the entry from
            // its current position.
            return i->second;
          } else {
            return NULL;
          }
        }	

        void add(const std::string& key, const std::string& replica,PathPtr& obj){
          IceUtil::Mutex::Lock lock(*this);
          pair<PathMapIter,bool> ret;
          ret = _pathMap.insert(make_pair<std::string, PathPtr>(key, obj));
          _evictor.push_back(key);
        }

        void set(const std::string& key, const std::string& step, const std::string& replica, const std::string& checksum){
          IceUtil::Mutex::Lock lock(*this);
          PathMapIter i = _pathMap.find(key);
          if (i != _pathMap.end()) {
            i->second->set(key,step,replica,checksum);
          } else {
            MCE_WARN("PathChecker::set find no key. key: " << key << " not found,   step:" << step
                << " replica:" << replica << " checksum:" << checksum );
          }
        }

        void check(std::string pathname, ReloaderPtr reloader){
          ostringstream msg;
          msg << " Check Timer PathChecker::check " << pathname << " " ;
          MyUtil::TimeCost tc = MyUtil::TimeCost::create(msg.str(),MyUtil::TimeCost::LOG_LEVEL_INFO);

          MCE_INFO(" Check Timer PathChecker::check " << pathname << "_checkVec.size=" << _checkVec.size());
          IceUtil::Mutex::Lock lock(*this);
          int error = 0;
          int total = 0;
          bool warning = false;	
          std::set<int> reloadset;

          for(std::vector<std::string>::iterator it = _checkVec.begin(); it != _checkVec.end(); it++){
            total++;
            PathMapIter pmit = _pathMap.find(*it);
            if (pmit == _pathMap.end()) {
              continue;
            }
            Result result = pmit->second->check(*it);
            if( SUCCESS != result.type){
              MCE_INFO("[CHECKERROR] " << *it << result << " path:"<< pathname);
              error++;
              if(error < RELOADERWARNSIZE){
                OperaData data(*it);
                reloadset.insert(data.getId());
              }else{
                MCE_WARN("[CHECKNOTREPAIR]" << *it << result << " path:" << pathname);
                if(!warning){
                  std::vector<string> subjects;
                  subjects.push_back("CheckerReloaderWarning");
                  ostringstream os;
                  os<<"Checker reload too many data, error count > " << error;
                  com::xiaonei::xce::XceAlertAdapter::instance().notifyAlertN(subjects, 
                      os.str() , com::xiaonei::xce::SENDALL); 
                  warning = true;
                }
              }
            }else{
              MCE_INFO("[CHECKSUCCESS] " << *it << result << " path:"<< pathname);
            }
          }
          //reload error data
          for(std::set<int>::iterator it = reloadset.begin(); it != reloadset.end();++it){
            reloader->reload(*it);
          }
          MCE_INFO( "[CHECKSTAT] " << "Path:" << pathname << " total:" << total << " success:" << (total-error) 
              << " error:" << error);

        }

        bool checkVecEmpty(std::string pathname){
          MCE_INFO("PathChecker::_checkVecEmpty: " << _checkVec.empty());
          return _checkVec.empty();
        }
        void clearCheckVec(std::string pathname){

          IceUtil::Mutex::Lock lock(*this);
          MCE_INFO(" PathChecker::clearCheckVec " << pathname  << " _checkVec.size=" << _checkVec.size() );
          for(std::vector<std::string>::iterator it = _checkVec.begin(); it != _checkVec.end(); it++){
            PathMapIter pmit = _pathMap.find(*it);
            if (pmit == _pathMap.end()) {
              continue;
            } 
            _pathMap.erase(pmit);
          }
          _checkVec.clear();
        }
        void evict(std::string pathname){
          IceUtil::Mutex::Lock lock(*this);
          if(!_evictor.empty()){
            _evictor.swap(_checkVec);
            MCE_INFO(" Check Timer PathChecker::evict " << pathname << " _checkVec.size=" << _checkVec.size() 
                << " _evictor.size=" << _evictor.size());
          }else{
            MCE_INFO("PathChecker::evict " << pathname << "_evictor is empty ");
          }
        }
      protected:

        PathMap _pathMap;//map<key, path>
        std::vector<std::string> _evictor;
        std::vector<std::string> _checkVec;
    };
    typedef IceUtil::Handle<PathChecker> PathCheckerPtr;

    typedef std::map<std::string, PathCheckerPtr> PathCheckerMap; //map<pathname, pathchecker>
    typedef std::map<std::string, PathCheckerPtr>::iterator PathCheckerMapIter;

    class SequencePathChecker : virtual public PathChecker{
      public:
        SequencePathChecker(){};
        virtual ~SequencePathChecker(){};

        void add(const std::string& key, const std::string& replica,PathPtr& obj){
          {
            IceUtil::Mutex::Lock lock(*this);
            pair<PathMapIter,bool> ret;
            ret = _pathMap.insert(make_pair<std::string, PathPtr>(key, obj));
            _evictor.push_back(key);
          }
          _sequenceCheckMap.add(key,replica);	
        }

        void set(const std::string& key, const std::string& step, const std::string& replica, const std::string& checksum){
          {
            IceUtil::Mutex::Lock lock(*this);
            PathMapIter i = _pathMap.find(key);
            if (i != _pathMap.end()) {
              i->second->set(key,step,replica,checksum);
            } else {
              MCE_WARN("PathChecker::set find no key. key: " << key << " not found,   step:" << step
                  << " replica:" << replica << " checksum:" << checksum );
            }
          }
          _sequenceCheckMap.add(key,replica);
        }

        void check(std::string pathname){
          ostringstream msg;
          msg << " Check Timer SequencePathChecker::check " << pathname << " " ;
          MyUtil::TimeCost tc = MyUtil::TimeCost::create(msg.str(),MyUtil::TimeCost::LOG_LEVEL_INFO);

          MCE_INFO(" Check Timer SequencePathChecker::check " << pathname << "_checkVec.size=" << _checkVec.size());
          {
            IceUtil::Mutex::Lock lock(*this);
            int error = 0;
            int total = 0;

            for(std::vector<std::string>::iterator it = _checkVec.begin(); it != _checkVec.end(); it++){
              total++;
              PathMapIter pmit = _pathMap.find(*it);
              if (pmit == _pathMap.end()) {
                continue;
              }
              Result result = pmit->second->check(*it);
              if( SUCCESS != result.type){
                MCE_INFO("[CHECKERROR] " << *it << result << " path:"<< pathname);
                error++;
              }else{
                result = _sequenceCheckMap.check(*it);
                if( SUCCESS != result.type){
                  MCE_INFO("[CHECKERROR] " << *it << result << " path:"<< pathname);
                  error++;
                }else{
                  MCE_INFO("[CHECKSUCCESS] " << *it << result << " path:"<< pathname);
                }
              }
            }
            MCE_INFO( "[CHECKSTAT] " << "Path:" << pathname << " total:" << total << " success:" << (total-error)
                << " error:" << error);
          }

        }

        bool checkVecEmpty(std::string pathname){
          MCE_INFO("SequencePathChecker::_checkVecEmpty: " << _checkVec.empty());
          return _checkVec.empty();
        }
        void clearCheckVec(std::string pathname){

          IceUtil::Mutex::Lock lock(*this);
          MCE_INFO(" SequencePathChecker::clearCheckVec " << pathname  << " _checkVec.size=" << _checkVec.size() );
          for(std::vector<std::string>::iterator it = _checkVec.begin(); it != _checkVec.end(); it++){
            PathMapIter pmit = _pathMap.find(*it);
            if (pmit == _pathMap.end()) {
              continue;
            } 
            _pathMap.erase(pmit);
            _sequenceCheckMap.erase(*it);
          }
          _checkVec.clear();
        }
        void evict(std::string pathname){
          IceUtil::Mutex::Lock lock(*this);
          if(!_evictor.empty()){
            _evictor.swap(_checkVec);
            MCE_INFO(" SequencePathChecker::evict " << pathname << " _checkVec.size=" << _checkVec.size()
                << " _evictor.size=" << _evictor.size());
          }else{
            MCE_INFO(" SequencePathChecker::evict " << pathname << "_evictor is empty ");
          }
        }

      private:
        SequenceCheckMap _sequenceCheckMap; //map<id,map<replica,squencestr>>
    };

    class FullPathChecker : virtual public Ice::Object , 
    virtual public MyUtil::Singleton<FullPathChecker>{
      public:
        PathCheckerMap pathCheckerMap;

      private:
        FullPathChecker() {
        }
        friend class MyUtil::Singleton<FullPathChecker>;
    };
    typedef IceUtil::Handle<FullPathChecker> FullPathCheckerPtr;

  };
};
#endif
