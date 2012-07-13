#include "gtest/gtest.h"
#include <fstream>

#include "base/logging.h"
#include "base/stringdigest.h"

TEST(string, MD5) {
  std::ifstream in("../base/test/md5_url.txt");
  if (!in) {
    LOG(LERROR) << "cannot load file";
  }
  std::string line;
  std::getline(in, line);
  int c = 0;
  while (!line.empty()) {
    ++c;
    std::string md5 = line.substr(0, line.find("\t", 0));
    std::string url = line.substr(line.find("\t", 0) + 1, line.size());
    ASSERT_TRUE(md5 == xce::MD5(url));
    line.clear();
    std::getline(in, line);
  }
  LOG(INFO) << "c:" << c;
}
