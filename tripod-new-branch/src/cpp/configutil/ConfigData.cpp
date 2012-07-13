#include "ConfigData.h"

namespace com {
namespace renren {
namespace tripod {

ConfigData::ConfigData() {
    name_ == "";
    value_ == "";
    version_ == "";
}

ConfigDataPtr ConfigData::getData() {
    ConfigDataPtr newData(new ConfigData);
    newData->name_ = name_;
    newData->value_ = value_;
    newData->version_ = version_;

    for(std::map<std::string, ConfigDataPtr>::const_iterator it = childrenDatas_.begin(); it != childrenDatas_.end(); ++it) {
        newData->childrenDatas_.insert(make_pair(it->first, it->second->getData()));
    }

    return newData;
}

std::string ConfigData::str() {
    std::ostringstream buff;
    buff << "ConfigData [";
    buff << "name=" << name_ << ";";
    buff << "value=" << value_ << ";";
    buff << "version=" << version_ << ";";
    buff << "children size=" << childrenDatas_.size() << ";";

    for (std::map<std::string, ConfigDataPtr>::iterator it = childrenDatas_.begin(); it != childrenDatas_.end(); ++it) {
        buff << "child data [" << it->second->str();
    }

    buff << " ]";

    return buff.str();
}

};
};
};
