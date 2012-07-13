#include "gtest/gtest.h"
#include <boost/shared_ptr.hpp>
#include "base/logging.h"
#include "socialgraph/socialgraphutil/zkclient.h"
#include "socialgraph/socialgraphutil/mcclient.h"
#include "socialgraph/socialgraphproto/pukfriendrecommend.pb.h"
#include "socialgraph/socialgraphd/base_action.h"
#include <string>
#include <vector>

/*
TEST(mcclient, GET) {
	const std::string key("liyong_1014");
	std::vector<char> value;
	bool bl = xce::MemcachedClient::GetInstance().Get(key, value);
	if (bl) {
		LOG(INFO) << "成功get " << " value:" << value.data();
	}
	else {
		LOG(INFO) << "失败get";
	}

}
*/

/*
TEST(mcclient, SET) {
	const std::string key("liyong_1014");
	std::vector<char> value;
	value.push_back('M');
	value.push_back('M');
	value.push_back('M');
	value.push_back('M');
	value.push_back('M');
	
	bool bl = xce::MemcachedClient::GetInstance().Set(key, value);
	if (bl) {
		LOG(INFO) << "成功set";
	}
	else {
		LOG(INFO) << "失败set";
	}
}
*/

/*
TEST(mcclient, DELETE) {
	const std::string key("liyong_1014");
	bool bl = xce::MemcachedClient::GetInstance().Delete(key);
	if (bl) {
		LOG(INFO) << "成功delete";
	}
	else {
		LOG(INFO) << "失败delete";
	}
}
*/

TEST(mcclient, MULTIGET) {
	int id = 27640369;
	std::vector<std::string> keys;
	keys.push_back("TEST_27640369");
	keys.push_back("TEST2_27640369");
	keys.push_back("TEST3_27640369");
	keys.push_back("TEST4_27640369");
	std::map<std::string, std::vector<char> > map;
	bool bl = xce::MemcachedClient::GetInstance().MultiGet(id, keys, map);
	if (bl) {
		std::map<std::string, std::vector<char> >::iterator it = map.begin();
		while (it != map.end()) {
			LOG(INFO) << it->first << "|" << it->second.size();
			++it;
		}

	}
	else {
		LOG(INFO) << "multiget failed!";
	}
}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc,argv); 
  int r = RUN_ALL_TESTS();
  return r;
}
