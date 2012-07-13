/**
 * @file
 * @author  yang.yu@opi-corp.com
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Path.h：
 *             class Path: 路径，用来记录一条路径上的调用以及数据校准情况 
 */

#ifndef __PATH_H__
#define __PATH_H__

#include "Replica.h"
#include "Step.h"

namespace xce{
  namespace check{

    class Path : virtual public IceUtil::Shared , virtual public IceUtil::Mutex{
      public:
        Path(){
        }
        Path(StepMap& map):_stepMap(map){
        }
        Path(Path& path){
          for(StepMapIter it = path._stepMap.begin(); it != path._stepMap.end(); it++){
            _stepMap.insert(make_pair<std::string, StepPtr>(it->first, new Step()));
            _stepMap[it->first]->checksumValid = path._stepMap[it->first]->checksumValid;
            for(std::map<std::string, ReplicaPtr>::iterator itv = path._stepMap[it->first]->replicaMap.begin(); 
                itv != path._stepMap[it->first]->replicaMap.end(); itv++){
              _stepMap[it->first]->replicaMap.insert(make_pair<std::string, ReplicaPtr>(itv->first,new Replica));
            }
          }
        }
        Path(std::map<std::string, std::vector<std::string> >& stepStrMap, std::map<std::string, bool>& checksumValidMap){
          if(checksumValidMap.size() != stepStrMap.size()){
            MCE_WARN("Path(stepStrMap, checksumValidMap) checksumValidMap.size != stepStrMap.size!");
          }
          for(std::map<std::string, std::vector<std::string> >::iterator it = stepStrMap.begin(); it!= stepStrMap.end(); it++){
            _stepMap.insert(make_pair<std::string, StepPtr>(it->first, new Step()));	
            _stepMap[it->first]->checksumValid = checksumValidMap[it->first];
            for(std::vector<std::string>::iterator itv = it->second.begin(); itv != it->second.end(); itv++){
              _stepMap[it->first]->replicaMap.insert(make_pair<std::string, ReplicaPtr>(*itv,new Replica));
            }
          }
        }
        virtual ~Path(){}

        Result check(const string& key){
          IceUtil::Mutex::Lock lock(*this);
          std::vector<ReplicaValue> checksumValidVec;
          Result result;
          for(StepMapIter it = _stepMap.begin(); it != _stepMap.end(); it++){
            ReplicaValue checksumValid;
            checksumValid.step = it->first;
            result = it->second->check(checksumValid);
            if(SUCCESS != result.type){
              result.content += " step:" + it->first;
              return result;
            }
            checksumValidVec.push_back(checksumValid);
          }
          string standerdValue = "";
          string standerdStep = "";
          for(std::vector<ReplicaValue>::iterator nit = checksumValidVec.begin(); nit != checksumValidVec.end(); nit++){
            if( nit->check &&  "" == standerdValue){
              standerdValue = nit->checksum;
              standerdStep = nit->step;
              continue;
            }
            if( nit->check && standerdValue != nit->checksum){
              result.type = STEPVALUEERROR;
              result.content = " standerdStep:" + standerdStep + " standerdValue:" + standerdValue 
                + " checkedstep:" + nit->step + " checkedchecksum:" + nit->checksum; 	
              return result;
            }
          }
          result.content += " checksum:" + standerdValue;
          return result;
        }

        bool set(const string& key, const string& step, const string& replica,
            const string checksum     ){
          //MCE_INFO("Path::set key:" << key << " step:" << step << " replica:" << replica
          //		<< " checksum:" << checksum);
          IceUtil::Mutex::Lock lock(*this);
          StepMapIter imit = _stepMap.find(step);
          if(imit == _stepMap.end()){
            MCE_WARN("StepObject::set step " << step << " not found");
            return false;
          }
          ReplicaMapIter nmit = imit->second->replicaMap.find(replica);
          if(nmit == imit->second->replicaMap.end()){
            MCE_WARN("StepObject::set replica " << replica << " not found");
            return false;
          }
          nmit->second->success = true;
          nmit->second->checksum = checksum;
          return true;
        }

        void display(const string& fun, const string& key){
          for(StepMapIter it = _stepMap.begin(); it != _stepMap.end(); it++){		
            MCE_INFO( fun << " " << key << " " << &_stepMap << " " << "	Step:" << it->first);
            MCE_INFO( fun << " " << key << " " << &_stepMap <<" " << "		Step.checksumValid: " << it->second->checksumValid);
            for( ReplicaMapIter rmit = it->second->replicaMap.begin(); rmit != it->second->replicaMap.end(); rmit++){
              MCE_INFO( fun << " " << key << " " << &_stepMap << " " << rmit->first << " it->second->replicaMap.size() " << it->second->replicaMap.size()); 
              MCE_INFO( fun << " " << key << " " << &_stepMap <<"		Step.replica:" << rmit->first << " " << *rmit->second);
            }
          }
        }

        bool addStep(std::string step){
          pair<map<std::string,StepPtr>::iterator,bool> ret;
          ret = _stepMap.insert(make_pair<std::string,StepPtr>(step, new Step));
          return ret.second;
        }

        bool addReplica(std::string step, std::string replica){
          StepMapIter it = _stepMap.find(step);
          if(it!= _stepMap.end()){
            pair<map<std::string,ReplicaPtr>::iterator,bool> ret;	
            ret = it->second->replicaMap.insert(make_pair<std::string,ReplicaPtr>(replica, new Replica));
            return ret.second;
          }
          return false;
        }
      private:

        StepMap _stepMap;
    };
    typedef IceUtil::Handle<Path> PathPtr;

    typedef std::map<std::string, PathPtr> PathMap; //map<key, path>
    typedef std::map<std::string, PathPtr>::iterator PathMapIter;

    class Config : virtual public IceUtil::Shared {
      public:
        Config(){
        }
        Config(PathMap& pathmap){
          _pathmap = pathmap;
        }
        PathMap& getConfig(){
          return _pathmap;
        }

      private:
        PathMap _pathmap;
    };
    typedef IceUtil::Handle<Config> ConfigPtr;

  };
};

#endif
