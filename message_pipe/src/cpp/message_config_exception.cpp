#include "message_config_exception.h"

namespace com {
namespace renren {
namespace messageconfig {

Exception::Exception() : message_("") {

}
Exception::Exception(const std::string& msg) : message_(msg) {

}
const char* Exception::what() const throw() {
  if (message_.empty()) {
    return "empty exception message";
  }
  return message_.c_str();
}

}}} //end com::renren::messageconfig

