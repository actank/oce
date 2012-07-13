#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include "base/logging.h"
#include "socialgraph/blogrecommend/algorithm/methodfactory.h"

using namespace xce::socialgraph;
using namespace xce;

//TEST(BLOGTEST, t0) {
//	LOG(INFO)	<< "BLOGTEST t0";
//	std::string tmp = "hello";
//	EXPECT_STREQ(tmp.c_str(), "hello");
//}

TEST(BLOGTEST, t1) {
	int userid = 238111132;
	Executer executer;
	executer.SetInstance(MethodFactory::GetInstance().Create(Participle));
	executer.Process(userid);
}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
