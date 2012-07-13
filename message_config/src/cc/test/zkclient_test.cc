#include "gtest/gtest.h"


#include <boost/thread/thread.hpp>
#include <boost/foreach.hpp>

#include "zkclient.h"

#include "LogWrapper.h"

namespace com {
namespace renren {
namespace messageconfig {


class MyListener : public NamespaceListener {
 public:
  MyListener(const std::string& ns) : NamespaceListener(ns) {

  }
  bool Update(const std::list<std::string>& children_names) {
    static int loop = 1;
    BOOST_FOREACH(const std::string& name, children_names) {
      MCE_DEBUG("\tname:" << name);
    }
    if (++loop > 5) {
      loop = 1;
      return true;
    }
    return true;
  }
};


TEST(zkclient, test) {
  MCE_DEFAULT_INIT("DEBUG");
  ZkClientPtr zc = ZkClient::GetInstance("10.2.78.253:2181");
  MCE_DEBUG("zc.use_count():" << zc.use_count());
  std::list<std::string> nodes;
  std::string ns("test2");
  zc->GetNodes(ns, &nodes);
  zc->AddNamespaceListener(NamespaceListenerPtr(new MyListener("test2")));
  boost::this_thread::sleep(boost::posix_time::milliseconds(10000));
  zc.reset();
}

}}} // end com::renren::tripod


