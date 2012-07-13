#include "gtest/gtest.h"
#include <boost/shared_ptr.hpp>
#include "base/logging.h"
#include "socialgraph/socialgraphutil/zkclient.h"
#include "socialgraph/socialgraphutil/mcclient.h"
#include "socialgraph/socialgraphd/base_action.h"
#include <string>
#include <vector>

class Ob : public xce::Observer {
public:
	Ob() {
		xce::ZooConfig::GetInstance().Attach(this);
	}
	
	virtual ~Ob() {
		xce::ZooConfig::GetInstance().Detach(this);
	}
	virtual void Update(const std::string& key,const std::string& value) {
		LOG(INFO) << "Update key:" << key << " value:" << value;
	}
};

TEST(zkclient, set) {
	xce::ZooConfig& zc = xce::ZooConfig::GetInstance();	
	std::string v;
	if (zc.Get("/test", &v)) {
		LOG(INFO) << "get success  /test=" << v;
	}
	Ob ob;
	std::string new_value("good_test");
	if (zc.Set("/test", &new_value)) {
		LOG(INFO) << "set success /test";
	}
	else {
		LOG(INFO) << "set failed /test";
	}
}
/*
TEST(zkclient, get) {
	xce::ZooConfig& zc = xce::ZooConfig::GetInstance();	
	cwf::socialgraph::BaseAction ba;
//	Ob ob;
	for (int i = 0; i < 5; ++i) {
		std::string v3;
		if (zc.Get("/test", &v3)) {
			LOG(INFO) << "test:" << v3;
	//		sleep(8);
		}
	}
//	zc.Destroy();
}
*/
/*
TEST(mcclient, get) {
	using namespace xce::socialgraph;
	std::vector<char> data;
	bool flag = xce::MemcachedClient::GetInstance().Get(std::string("FFW_27640369"), data);
	if (flag) {
		LOG(INFO) << "data.size()" << data.size();
	}
	std::vector<char> data2;
	flag = xce::MemcachedClient::GetInstance().Get(std::string("FFW_242679045"), data2);
	if (flag) {
		LOG(INFO) << "data2.size()" << data2.size();
		boost::shared_ptr<PukFriendRecommend> pfr(new PukFriendRecommend());
		pfr->ParseFromArray(data2.data(), data2.size());
		LOG(INFO) << "pfr->commonfriendseq_size():" << pfr->commonfriendseq_size();
		for (int i = 0; i < pfr->commonfriendseq_size(); ++i) {
			const PukFriendRecommend_CommonFriend& pfrc = pfr->commonfriendseq(i);
			int id = pfrc.userid();
			LOG(INFO) << "id:" << id;
			for (int j = 0; j  < pfrc.shares_size(); ++j) {
				int share_id = pfrc.shares(j);
				LOG(INFO) << "**** share_id:" << share_id;
			}
		} 
	}
	
}
*/
/*
TEST(mcclient, SET) {
	std::vector<char> data;
	data.push_back('L');
	data.push_back('I');
	data.push_back('Y');
	data.push_back('O');
	data.push_back('N');
	data.push_back('Y');
	const std::string key("liyong_1014");
	bool bl = xce::MemcachedClient::GetInstance().Set(key, data);
	data.clear();
	if (bl) {
		LOG(INFO) << "成功set";
	}
	else {
		LOG(INFO) << "失败set";
	}
}
*/

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc,argv); 
  int r = RUN_ALL_TESTS();
  return r;
}
