#include "gtest/gtest.h"

#include "boost/random.hpp"
#include "boost/foreach.hpp"

#include "base/logging.h"

namespace xce {
namespace socialgraph {

TEST(random_number, generate) {
  static boost::mt19937 gen;
  boost::uniform_int<> dist(0, 0);
  boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(gen, dist); 
  for (int i = 0; i < 30; ++i) {
    int v = die();
    EXPECT_TRUE((v >= 0 && v <= 1000));
    LOG(INFO) << "v:" << v;
  }

}

}}


