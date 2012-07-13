#include "gtest/gtest.h"

#include "base/logging.h"

#include "socialgraph/socialgraphd/video/worker/tree.h"

using namespace xce::socialgraph;

class TreeTest : public testing::Test {
 public:
  TreeTest() {
    tree = new Tree("../../tmp/video_share_result", "../../tmp/tag_index");
  }
 public:
  Tree* tree; 
};

TEST_F(TreeTest, SearchNode) {
  VideoShare* vs = tree->SearchVideoShare("3255bc668066cf1ef348bdf343d5f527");
  ASSERT_TRUE(vs != NULL);
  LOG(INFO) << vs->VideoShareInfo();
  Node* node = tree->SearchNode(2);
  ASSERT_TRUE(node != NULL);
  LOG(INFO) << node->NodeInfo();
}



TEST_F(TreeTest, NodeChildrenSize) {
  tree->Display();
}



