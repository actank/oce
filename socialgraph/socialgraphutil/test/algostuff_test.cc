#include "gtest/gtest.h"

#include "base/logging.h"

#include "socialgraph/socialgraphutil/algostuff.h"


TEST(algostuff, Intersection) {
  std::vector<int> v1, v2;
  xce::INSERT_ELEMENT(v1, 1, 9);
  xce::INSERT_ELEMENT(v2, 5,20);

  xce::PRINT_ELEMENTS(v1, "v1:");
  xce::PRINT_ELEMENTS(v2, "v2:");
  
  std::vector<int> r;
  int common = xce::Intersection(v1, v2, r);
  xce::PRINT_ELEMENTS(r, "r:");
  LOG(INFO) << "common:" << common;
 
}
