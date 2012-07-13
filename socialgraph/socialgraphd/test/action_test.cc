#include "gtest/gtest.h"
#include "base/logging.h"
#include "socialgraph/socialgraphutil/zkclient.h"
#include "socialgraph/socialgraphutil/mcclient.h"
#include "socialgraph/socialgraphd/action4.h"
#include <string>
#include <vector>


TEST(GetAction, DoPost) {
  using namespace xce::socialgraph;
  TestAction action;
  while (true) {
  action.Process(0 , 0);
  }
}
