#include <assert.h>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "socialgraph/socialgraphutil/clogging.h"
#include "OceCxxAdapter/src/FriendFinderWorkerAdapter.h"

using namespace xce::socialgraph;

void GetLoginIds(const std::string& file, std::vector<int>& userids);

int main(int argc, char* argv[]) {
	InitLogger("CommonFriends");
	if (argc != 2) {
		CLOG(ERROR) << "Error input parameter!!";
		return 0;
	}
	CLOG(INFO) << argv[1];
	std::string load_file = std::string(argv[1]);
	std::vector<int> userids;
	GetLoginIds(load_file, userids);		//获取最近一天登录的用户id列表
	CLOG(INFO) << "userids size :" << userids.size();

	int count = 0;			//计时器
	for (std::vector<int>::iterator iter = userids.begin();
			iter != userids.end(); ++iter) {
		if (count++ % 200 == 0) {				// 每发送200个数据sleep 1秒
			sleep(1);
			CLOG(INFO) << "CommonFriends sleep count:" << count;
		}

		Info information;
		information.operation = UPDATE;
		information.hostId = *iter;

		try {
			FriendFinderWorkerAdapter::instance().Report(*iter, information);	  			//通知服务进行重算
			CLOG(INFO) << "CommonFriends notify id:" << *iter;
		} catch (Ice::Exception& e) {
			CLOG(WARN) << "CommonFriends FriendFinderWorkerAdapter Ice::exception:" << e.what();
		} catch (std::exception& e) {
			CLOG(WARN) << "CommonFriends FriendFinderWorkerAdapter std::exception:" << e.what();
		} catch (...) {
			CLOG(WARN) << "CommonFriends FriendFinderWorkerAdapter unknown exception.";
		}
	}
}

void GetLoginIds(const std::string& file, std::vector<int>& userids) {
	CLOG(INFO) << "file:" << file << "  userids size:" << userids.size();
	std::fstream file_stream;
	file_stream.open(file.c_str(), std::ios::in);
	assert(file_stream);		//判断文件是否存在，不存在就不做

	std::ifstream infile(file.c_str(), std::ifstream::in);
	std::string line = "";
	while (getline(infile, line)) {
		int id = 0;
		std::istringstream stream(line);
		stream >> id;

		if (0 != id) {
			userids.push_back(id);
		}
	}
}
