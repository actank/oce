/**
 * @file
 * @author  yang.yu@opi-corp.com
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Entry.h：
 *             class Entry:调用信息实体，记录路径，步骤，备份，键值，数据 
 */

#ifndef __ENTRY_H__
#define __ENTRY_H__

namespace xce{
  namespace check{

    class Entry : virtual public IceUtil::Shared {
      public:

        Entry():
          path(""),
          step(""),
          replica(""),
          key(""),
          value(""){
          }

        Entry(	std::string pathstr,
            std::string stepstr,
            std::string replicastr,
            std::string keystr,
            std::string valuestr):
          path(pathstr),
          step(stepstr),
          replica(replicastr),
          key(keystr),
          value(valuestr){
          }

        std::string path;
        std::string step;
        std::string replica;
        std::string key;
        std::string value;

        friend std::ostream& operator<<(std::ostream& os, const Entry& entry) {
          os << "Entry[ path:" << entry.path << " step:" << entry.step << " replica:" << entry.replica 
            << " key:" << entry.key << " value:" << entry.value << " ]";
          return os;
        }
    };
    typedef IceUtil::Handle<Entry> EntryPtr;


  };
};

#endif
