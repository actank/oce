#include "socialgraph/socialgraphutil/zkclient.h"
#include "socialgraph/socialgraphutil/mcclient.h"
#include "socialgraph/socialgraphproto/pukfriendrecommend.pb.h"
#include <string>
#include <vector>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <cstring>
#include <iostream>

void test(char* filename) {
	using namespace xce::socialgraph;
	const char* kFileName = filename;
	std::ifstream fin(kFileName);
	if (!fin) {
		std::cerr << "cannot open file" << std::endl;
		exit(1);
	}
	int MAX_LENGTH = 100;
	char line[MAX_LENGTH];
	int count = 0;
	while (fin.getline(line, MAX_LENGTH)) {
		int id = -1;
		try {
			id = boost::lexical_cast<int>(std::string(line, std::strlen(line)));
		} catch (...) {
			std::cout << "error line:" << line << std::endl;
			continue;
		}
		std::ostringstream oss;
		oss << "FFW_" << id;
		const std::string mckey = oss.str();
		std::vector<char> data;
		if(xce::MemcachedClient::GetInstance().Get(mckey, data)) {
			boost::shared_ptr<PukFriendRecommend> pfr(new PukFriendRecommend());
			pfr->ParseFromArray(data.data(), data.size());
			std::cout << " mckey:" << mckey 
								<< " pfr->commonfriendseq_size():"  << pfr->commonfriendseq_size() 
								<< " raw_data.size():" << data.size()
							  << std::endl;
			++count;
		}
		else {
			std::cerr << " mckey:" << mckey << " fetch data failed!" << std::endl;
		}
	}
	std::cout << "count:" << count << std::endl;

	fin.close();

}

int main(int argc, char* argv[]) {


	if (argc != 2) {
		std::cout << "usage: ./data_test filename" << std::endl;
		exit(0);
	}


	//test(argv[1]);

	return 0;

}
