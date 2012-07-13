#include "gtest/gtest.h"

#include <iostream>

#include "LogWrapper.h"
#include "singleton.h"

namespace com {
namespace renren {
namespace tripod {

class Foo : public Singleton<Foo> {
 public:
  void foo() {
    std::cout << "Foo::foo() method" << std::endl;
    MCE_INFO("Foo::foo() method");
  }
  virtual ~Foo() {
    std::cout << "Foo::~Foo()" << std::endl;
  }
 private:
  Foo() {
    std::cout << "Foo::Foo()" << std::endl;
  }
  friend class Singleton<Foo>;
};


TEST(singleton, test) {
  {
  MCE_DEFAULT_INIT("WARN");
  }
  Foo::GetInstance().foo();
}


}}}


