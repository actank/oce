#include "gtest/gtest.h"
#include "arch/rpcuri.h"

using namespace xce;

TEST(Rpcuri, CreateChannelTest) {}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc,argv); 
  int r = RUN_ALL_TESTS();
  return r;
}
