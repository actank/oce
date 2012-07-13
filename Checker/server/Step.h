/**
 * @file
 * @author  yang.yu@opi-corp.com
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Step.h： class Step: 检测步骤类，保存该步骤所包含的备份信息 
 */

#ifndef __STEP_H__
#define __STEP_H__

#include "Replica.h"
#include "Result.h"

namespace xce{
  namespace check{

#define MAXSTEP 100

    class Step : virtual public IceUtil::Shared{
      public:
        bool checksumValid;
        ReplicaMap replicaMap;
      public:
        Step():checksumValid(false){
        }
        Result check(ReplicaValue& replicaValue){
          Result result;
          replicaValue.check = checksumValid;
          if(!replicaMap.empty() && checksumValid){
            replicaValue.checksum = replicaMap.begin()->second->checksum;
          }
          for(ReplicaMapIter it = replicaMap.begin(); it != replicaMap.end(); it++){
            if(!it->second->success){
              result.type = CALLERROR;
              result.content = "replica:" + it->first;
            }
            if(checksumValid && replicaValue.checksum != it->second->checksum){
              result.type = REPLICAVALUEERROR;
              result.content = "replica:" + it->first;
            }
          }
          return result;
        }
    };
    typedef IceUtil::Handle<Step> StepPtr;

    typedef std::map<std::string, StepPtr> StepMap; //map< StepName, Step >
    typedef std::map<std::string, StepPtr>::iterator StepMapIter;

  };
};
#endif
