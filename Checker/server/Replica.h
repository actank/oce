/**
 * @file
 * @author  yang.yu@opi-corp.com
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Replica.h： class Replica: 保存每个备份信息，用于相同step下不同备份的信息检测
 *             class ReplicaValue: 用于step层的checksum检测，保存是否进行checksum检测的有效信息
 */

#ifndef __REPLICA_H__
#define __REPLICA_H__

namespace xce{
  namespace check{

    class Replica : virtual public IceUtil::Shared{
      public:
        bool success;
        std::string checksum;
      public:
        Replica():success(false),checksum(""){
        }

        friend std::ostream& operator<<(std::ostream& os, const Replica& replica) {
          os << "Replica[ success:" <<replica.success << " checksum:" << replica.checksum << "]";
          return os;
        }
    };
    typedef IceUtil::Handle<Replica> ReplicaPtr;

    typedef std::map<std::string, ReplicaPtr> ReplicaMap; 
    typedef std::map<std::string, ReplicaPtr>::iterator ReplicaMapIter;


    class ReplicaValue{
      public:
        bool check;
        std::string checksum;
        std::string step;
      public:
        ReplicaValue():check(false),checksum(""),step(""){
        }

    };

  };
};

#endif
