#ifndef XCE_ARCH_LOGGER_H__
#define XCE_ARCH_LOGGER_H__

#include <Ice/Ice.h>
#include "base/logging.h"

namespace xce {

class IceLogger : public Ice::Logger {
public:
  void print(const std::string& message) {
    // LOG(LS_INFO) << "[ice] " << message;
    LogMultiline(LS_ERROR, "[ice]", false, message.c_str(), true, false, &lms_);
  }

  void trace(const std::string& category, const std::string& message) {
    // LOG(LS_VERBOSE) << "[ice " << category << "] " << message;
    LogMultiline(LS_VERBOSE, "[ice]", false, message.c_str(), true, false, &lms_);
  }

  // TODO: use log mutli-line
  void warning(const std::string& message) {
    // LOG(LS_WARNING) << "[ice] " << message;    
    LogMultiline(LS_WARNING, "[ice]", false, message.c_str(), true, false, &lms_);
  }

  void error(const std::string& message) {
    // LOG(LS_ERROR) << "[ice] " << message;
    LogMultiline(LS_ERROR, "[ice]", false, message.c_str(), true, false, &lms_);
  }
  LogMultilineState lms_;
};

}
#endif // XCE_ARCH_LOGGER_H__
