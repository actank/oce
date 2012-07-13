#ifndef _TRIPOD_CONFIGDATA_H__
#define _TRIPOD_CONFIGDATA_H__

#include "Common.h"

namespace com {
namespace renren {
namespace tripod {

class ConfigData {
    public:
        ConfigData();
        ConfigDataPtr getData();
        std::string str();

    public:
        std::string name_;
        std::string value_;
        std::string version_;

        std::map<std::string, ConfigDataPtr> childrenDatas_;
};

}
}
}

#endif
