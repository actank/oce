#include "gid.h"
#include <gtest/gtest.h>

using namespace gid;

TEST(GID, Blog) {
  int64_t blogid = 0xBA987654321;
  ASSERT_EQ(kNull, G::ExtractType(blogid));
  int64_t gblogid = 0x32000BA987654321;
  ASSERT_EQ(gblogid, G::Id(blogid, kBlog));
  ASSERT_EQ(kBlog, G::ExtractType(gblogid));
  ASSERT_EQ(blogid, G::ExtractX(gblogid));
  ASSERT_TRUE(G::IsTypeOf(gblogid, kBlog));
}

TEST(GID, Uid) {
  int64_t uid = 200435454;
  ASSERT_EQ(uid, G::Id(uid, kPerson));
  ASSERT_EQ(kPerson, G::ExtractType(uid));
  ASSERT_EQ(uid, G::ExtractX(uid));
  ASSERT_TRUE(G::IsTypeOf(uid, kPerson));
}

//极限值测试...
TEST(GID, NoLimit) {
  int64_t uid = 0;
  ASSERT_EQ(0, G::Id(-1, kPage));
  ASSERT_NE(0, G::Id(0xfffffffffff,kShare));
  ASSERT_EQ(0, G::Id(0x10000000000000, kShare));
  ASSERT_EQ(kNull, G::ExtractType(0));
  ASSERT_EQ(kNull, G::ExtractType(2147483648L)); 
}
