#include "gtest/gtest.h"

#include "LogWrapper.h"
#include "common.h"


namespace com {
namespace renren {
namespace tripod {

class ObI : public Observer {
 public:
  ObI(int index):index_(index)  {}
  void Update(const std::string& key, const std::string& value) {
    MCE_INFO("ObI::Update() index:" << index_ << "\tkey:" << key << "\tvalue:" << value);
  }
  virtual ~ObI() {
    MCE_INFO("ObI::~ObI() index:" << index_);
  }
 private:
  int index_;
};

TEST(ob_sub, test) {

  MCE_DEFAULT_INIT("WARN");

  ObserverPtr ob1(new ObI(1));
  ObserverPtr ob2(new ObI(2));
  ObserverPtr ob3(new ObI(3));

  Subject subject;
  subject.Attach(ob1);
  subject.Attach(ob2);
  subject.Attach(ob3);

  subject.NotifyObservers("fuck", "world");

  ObserverPtr ob4 = ob1;

  subject.Detach(ob4);

  subject.NotifyObservers("fuck", "world");
}

}}}  // end com::renren::tripod

