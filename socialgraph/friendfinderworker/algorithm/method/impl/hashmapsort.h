#ifndef HASHMAP_SORT_H_
#define HASHMAP_SORT_H_

#include <iostream>
#include <map>
#include <vector>

#include "socialgraph/friendfinderworker/algorithm/method/commonfriendcalculator.h"

namespace xce {
namespace socialgraph {

const int HASH_COMMON_FRIENDS_SIZE = 1000; //共同好友最多的1000个二度好友

typedef __gnu_cxx::hash_map<int, int> WeightType;
typedef WeightType::iterator WeightIter;

//--------------------------------------------------------------------

class HashMapSort : public CommonFriendCalculator {
public:
	HashMapSort() {
		maxweight_ = 0;
	}
	virtual ~HashMapSort() {}

	void CalComm(CommonFriendSeq& comm);
	void Display(const CommonFriendSeq& comm) const;

	//好友为0并且发申请数为0不计算
	bool Hook() {
		if (friendlist_.empty() && applylist_.empty()) {
			return false;	
		}
		return true;
	}

private:
	void CreateCommonMap();
	void InitBlockItem();
	void InitSortVector(); 
	void BuildWeight(const std::map<int, std::vector<int> >& users, int weight); 
	void SetBlockItem(const std::vector<int>& blocklist);
	void GetTarget(CommonFriendSeq& comm);

	WeightType weightcount_;
	__gnu_cxx::hash_map<int, std::vector<int> > sharecount_;
	std::vector< std::vector<int> > sortvector_;
	int maxweight_;
	CommonFriendSeq ret_;
};

}
}

#endif
