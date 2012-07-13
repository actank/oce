#ifndef RECOMMEND_INTEREST_MODEL_H_
#define RECOMMEND_INTEREST_MODEL_H_

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <assert.h>

class RecommendInterestModelHead {
public:
	int uid;
	int len;
};

//--------------------------------------------------------------------------

class RecommendInterestModel {
public:
	RecommendInterestModel(int userid): userid_(userid) {}

	void SetRecommendList(std::vector<int>& recommend_list) {
		recommend_list_.swap(recommend_list);
	}

	void SetInterestList(std::set<std::string>& interest_list) {
		interest_list_.swap(interest_list);
	}

	void SetBlockUgcList(std::set<long>& block_ugc_list) {
		block_ugc_list_.swap(block_ugc_list);
	}

	int GetUserId() const {
		return userid_;
	}

	void PrintInfo() const {
		std::cout << userid_ << " : " << std::endl;
		std::ostringstream oss;
		for (std::vector<int>::const_iterator iter = recommend_list_.begin();
				iter != recommend_list_.end(); ++iter) {
			oss << *iter << " ";
		}
		std::cout << oss.str() << std::endl;

		std::ostringstream ire;
		for (std::set<std::string>::const_iterator iter = interest_list_.begin();
				iter != interest_list_.end(); ++iter) {
			ire << *iter << " ";
		}
		std::cout << ire.str() << std::endl;

		std::ostringstream bl;
		for (std::set<long>::const_iterator iter = block_ugc_list_.begin();
				iter != block_ugc_list_.end(); ++iter) {
			bl << *iter << " ";
		}
		std::cout << bl.str() << std::endl;
		std::cout << std::endl;
	}

	int ToBuffer (char* buff); 
	int FromBuffer(char* buff, int buf_len);

private:
	int userid_;
	std::vector<int> recommend_list_;
	std::set<std::string> interest_list_;
	std::set<long> block_ugc_list_;
};

#endif
