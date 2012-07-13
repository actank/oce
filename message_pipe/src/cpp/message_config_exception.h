#ifndef _MESSAGECONFIG_EXCEPTION_H_
#define _MESSAGECONFIG_EXCEPTION_H_

#include <exception>
#include <string>

namespace com {
namespace renren {
namespace messageconfig {

class Exception : public std::exception {
 public:
  Exception();
  Exception(const std::string& msg);
  virtual ~Exception() throw() {}
  virtual const char* what() const throw();
 protected:
  std::string message_;
};

}}} // end com::renren::messageconfig

#endif

