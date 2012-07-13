#ifndef __OPERADATA_H__
#define __OPERADATA_H__

#include <stdlib.h>
#include "util/cpp/String.h"

namespace xce{
  namespace check{

    const std::string KEY_MAP_PREFIX = "CHECKER_KEY";

    class OperaData : virtual public IceUtil::Shared {
      public:
        OperaData(const long id, const std::string& doStr, const std::string& sthStr) :
          _id(id),_doStr(doStr),_sthStr(sthStr) {
          }

        OperaData(const Ice::Current& crt) {
          std::map<std::string, std::string>::const_iterator it = crt.ctx.find(KEY_MAP_PREFIX);
          if( it == crt.ctx.end() ) {
            _id = -1;
            _doStr = "";
            _sthStr = "";
          }else {
            std::vector<std::string> parts = MyUtil::StrUtil::split(it->second, ":");
            if( parts.size() != 3 ) {
              _id = -1;
              _doStr = "";
              _sthStr = "";
            }else {
              _id = atoi(parts.at(0).c_str());
              _doStr = parts.at(1);
              _sthStr = parts.at(2);
            }
          }
        }

        OperaData(const std::string& key) {
          std::vector<std::string> parts = MyUtil::StrUtil::split(key, ":");
          if( parts.size() != 3 ) {
            _id = -1;
            _doStr = "";
            _sthStr = "";
          }else {
            _id = atoi(parts.at(0).c_str());
            _doStr = parts.at(1);
            _sthStr = parts.at(2);
          }
        }

        void insertToContext(Ice::Context& ctx) {
          ctx.insert(make_pair(KEY_MAP_PREFIX, toString()));
        }

        std::string toString() {
          std::ostringstream os;
          os<<_id<<":"<<_doStr<<":"<<_sthStr;
          return os.str();
        }

        int getId(){
          return _id;
        }

        std::string getDoStr(){
          return _doStr;
        }

        std::string getSthStr(){
          return _sthStr;
        }

      private:

        OperaData(){}
      private:
        int _id;
        std::string _doStr;
        std::string _sthStr;
    };
    typedef IceUtil::Handle<OperaData> OperaDataPtr;

  };
};

#endif
