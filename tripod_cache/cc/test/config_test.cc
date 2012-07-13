#include "gtest/gtest.h"

#include "LogWrapper.h"
#include "common.h"

namespace com {
namespace renren {
namespace tripod {


TEST(ConfigData, Str) {
  MCE_DEFAULT_INIT("WARN");

  ConfigDataPtr data(new ConfigData());
  data->set_name("data1");
  data->set_value("value1");
  data->set_version("version1");

  ConfigDataPtr data2(new ConfigData());
  data2->set_name("data2");
  data2->set_value("value2");
  data2->set_version("version2");

  data->AddChild(data2->name(), data2);
  MCE_INFO(data->Str());
}

}}} // end com::renren::tripod


