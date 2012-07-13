#include "gtest/gtest.h"


#include "LogWrapper.h"
#include "message_config_exception.h"

namespace com {
namespace renren {
namespace messageconfig {

TEST(exception, throw_test) {
  MCE_DEFAULT_INIT("DEBUG");
  try {
    throw Exception("test exception....");
  } catch (std::exception& e) {
    MCE_DEBUG(e.what());
  }

  try {
    throw Exception();
  } catch (std::exception& e) {
    MCE_DEBUG(e.what());
  }
} 

}}} 


