/**
 * @file
 * @author  yang.yu@opi-corp.com
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Sequence.h： class Sequence: 记录各个备份的执行序列信息 
 *              class SequenceCheckMap: 执行序列检测操作的封装类
 */

#ifndef __SEQUENCE_H__
#define __SEQUENCE_H__

#include "Checker/share/OperaData.h"
#include "Result.h"

namespace xce{
  namespace check{

    class Sequence : virtual public IceUtil::Shared {
      public:
        std::map<std::string, std::string> replicamap;//map<replica, sequencevalue>
        bool checked;

      public:
        Sequence():
          checked(false){
          }

    };
    typedef IceUtil::Handle<Sequence> SequencePtr;

    typedef std::map<int,SequencePtr > SequenceMap; //map<id,replicaMap>
    typedef std::map<int,SequencePtr >::iterator SequenceMapIter;

    class SequenceCheckMap : virtual public IceUtil::Mutex {
      public:
        void add(const std::string& key, const std::string& replica){
          OperaData data(key);
          IceUtil::Mutex::Lock lock(*this);
          _sequenceMap[data.getId()]->replicamap[replica]+=key;
        }
        Result check(const std::string& key){
          OperaData data(key);
          IceUtil::Mutex::Lock lock(*this);
          Result result;
          if(_sequenceMap[data.getId()]->checked){
            return result; 
          }
          std::string checkvalue = "";
          std::string checkreplica = "";
          for(std::map<std::string, std::string>::const_iterator it= _sequenceMap[data.getId()]->replicamap.begin(); 
              it!= _sequenceMap[data.getId()]->replicamap.end(); ++it){
            if("" == checkvalue){
              checkvalue = it->second;
              checkreplica = it->first;
              continue;
            }
            if( checkvalue != it->second){
              result.type = SEQUENCEERROR;
              ostringstream os;
              os << " id:" << data.getId() << " check[ " << checkreplica << " " << checkvalue
                << " ] compare[ " << it->first << " " <<it->second << " ]";
              result.content = os.str(); 
              return result;
            }
          }
          return result;
        }

        void erase(const std::string& key){
          OperaData data(key);
          IceUtil::Mutex::Lock lock(*this);
          SequenceMapIter it = _sequenceMap.find(data.getId());
          if( it != _sequenceMap.end()){
            _sequenceMap.erase(it);
          }
        }

      private:

        SequenceMap _sequenceMap;
    };
  };
};

#endif
