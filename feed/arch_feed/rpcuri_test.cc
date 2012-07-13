#include "gtest/gtest.h"
#include "feed/arch_feed/rpcuri.h"

using namespace xce;

TEST(Rpcuri, CreateChannelTest) {}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc,argv); 
  int r = RUN_ALL_TESTS();
  return r;
}
