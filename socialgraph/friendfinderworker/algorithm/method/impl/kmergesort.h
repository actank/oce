#ifndef K_MERGE_SORT_H_
#define K_MERGE_SORT_H_

#include <iostream>
#include <ext/hash_set>
#include <queue>
#include <map>
#include <vector>

#include "socialgraph/friendfinderworker/algorithm/method/commonfriendcalculator.h"

namespace xce {
namespace socialgraph {

static const int BUCKET_SIZE = 1000;
static const int STEP = 20;
static const int CREATE_COMMON_FRIENDS_SIZE = 1000;

//--------------------------------------------------------------------

struct HeapItem {
  int host_id;
  int weight;
  std::vector<int>::const_iterator begin_iter;
  std::vector<int>::const_iterator end_iter;
};

//--------------------------------------------------------------------

struct IdAscComp {
  bool operator() (const HeapItem& lhs, const HeapItem& rhs) const {
    return *lhs.begin_iter > *rhs.begin_iter;
  }
};

//--------------------------------------------------------------------

class KMergeSort : public CommonFriendCalculator {
public:
	KMergeSort() {}
	virtual ~KMergeSort() {}

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
	void InitBlockList();
	void MergeAll(const std::map<int, std::vector<int> >& id_map_1, 
              int weight_1,       
              const std::map<int, std::vector<int> >& id_map_2, 
              int weight_2); 
	void GetCommFriends(int limit, std::vector<CommonFriend>& comm_list); 
	bool IsBlock(int userid); 
	void InsertBucket(const CommonFriend& comm); 
	void FlushSortVector(std::vector<HeapItem>& sort_vec); 
	void AddFliterHeap(priority_queue< HeapItem, vector<HeapItem>, IdAscComp >& fliter_heap,
										const std::map<int, std::vector<int> >& id_map,
										int weight);

	std::vector<int> block_sort_list_;
	std::vector<int>::iterator find_iter_;
	std::vector< CommonFriendSeq > bucket_;
};

}
}

#endif
