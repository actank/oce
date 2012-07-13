#include "recommend_interest_model.h"

using namespace std;

int RecommendInterestModel::ToBuffer(char* buff) {
	int len = 0;
	char* p = buff;

	//userid
	*(int*)p = this->userid_; p += sizeof(int);

	//recommend list
	len = (int)recommend_list_.size();
	*(int*)p = len; p += sizeof(int);

	for (vector<int>::iterator iter = recommend_list_.begin();
			iter != recommend_list_.end(); ++iter) {
		*(int*)p = *iter; p += sizeof(int);
	}

	//block list
	len = (int)block_ugc_list_.size();
	*(int*)p = len; p += sizeof(int);

	for (set<long>::iterator iter = block_ugc_list_.begin();
			iter != block_ugc_list_.end(); ++iter) {
		*(long*)p = *iter; p += sizeof(long);
	}

	//interest list
	len = (int)interest_list_.size();
	*(int*)p = len; p += sizeof(int);

	for (set<string>::iterator iter = interest_list_.begin();
			iter != interest_list_.end(); ++iter) {
		strcpy(p, iter->c_str());
		while (*p != 0) ++p;
		++p;
	}

	return p - buff;
}

int RecommendInterestModel::FromBuffer(char* buff, int buf_len) {
	char* p = buff;
	int len = 0;

	//userid
	this->userid_ = *(int*)p; p += sizeof(int);

	//recommend list
	len = *(int*)p; p += sizeof(int);
	for (int i = 0; i < len; ++i) {
		int fid = *(int*)p; p += sizeof(int);
		recommend_list_.push_back(fid);
	}
	vector<int>(recommend_list_).swap(recommend_list_);	

	//block list
	len = *(int*)p; p += sizeof(int);
	for (int i = 0; i < len; ++i) {
		long ugc_id = *(long*)p; p += sizeof(long);
		block_ugc_list_.insert(ugc_id);
	}

	//interest list
	len = *(int*)p; p += sizeof(int);

	for (int i = 0; i < len; ++i) {
		char* interest = p;
		while (*p != 0) ++p;
		++p;
		interest_list_.insert(string(interest));
	}
	assert(buf_len == p - buff);
	return 0;
}
