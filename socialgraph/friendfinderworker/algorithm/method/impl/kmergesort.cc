#include "socialgraph/friendfinderworker/algorithm/method/impl/kmergesort.h"

namespace xce {
namespace socialgraph {

void KMergeSort::CalComm(CommonFriendSeq& comm) {
	// 初始化block列表
	InitBlockList();

	// k路归并计算二度好友
	MergeAll(ffmap_, 10, afmap_, 7);

	// 获取共同好友
	GetCommFriends(CREATE_COMMON_FRIENDS_SIZE, comm);
}

void KMergeSort::Display(const CommonFriendSeq& comm) const {
	int count = 0;
	for (CommonFriendSeq::const_iterator iter = comm.begin();
			iter != comm.end(); ++iter) {
		std::cout << iter->userId << " " << iter->weight << " " << iter->sharesCount << std::endl;
		xce::PRINT_ELEMENTS(iter->shares);
		std::cout << std::endl;
		if (++count == 100) {
			break;
		}
	}
}

void KMergeSort::MergeAll(const std::map<int, std::vector<int> >& id_map_1, 
            							int weight_1,       
            							const std::map<int, std::vector<int> >& id_map_2, 
            							int weight_2) {
  bucket_.resize(BUCKET_SIZE);

	//初始化堆
  priority_queue< HeapItem, vector<HeapItem>, IdAscComp > fliter_heap;
	AddFliterHeap(fliter_heap, id_map_1, weight_1);
	AddFliterHeap(fliter_heap, id_map_2, weight_2);

	//堆排序
  vector<HeapItem> result_list;          
  while (!fliter_heap.empty()) {
    HeapItem item = fliter_heap.top();

    if (result_list.empty()) {
      result_list.push_back(item);
    } else {
      if (*result_list.back().begin_iter == *item.begin_iter) {
        result_list.push_back(item);
      } else {
        FlushSortVector(result_list);
        result_list.push_back(item);
      }
    }
    fliter_heap.pop();

    if (++item.begin_iter != item.end_iter) {
      fliter_heap.push(item);
    }
  }

  if (!result_list.empty()) {
    FlushSortVector(result_list);
  }
}


void KMergeSort::InitBlockList() {
	// 不推荐自己
	block_sort_list_.push_back(userid_);

	// 不推荐黑名单用户
	block_sort_list_.insert(block_sort_list_.end(), blocklist_.begin(), blocklist_.end());

	// 不推荐已经删除过的推荐
	block_sort_list_.insert(block_sort_list_.end(), commonblock_.begin(), commonblock_.end());
 
	// 不推荐用户好友
	block_sort_list_.insert(block_sort_list_.end(), friendlist_.begin(), friendlist_.end());

	// 不推荐用户发过好友申请的用户
	block_sort_list_.insert(block_sort_list_.end(), applylist_.begin(), applylist_.end());

	sort(block_sort_list_.begin(), block_sort_list_.end());	
	find_iter_ = block_sort_list_.begin(); 
}

bool KMergeSort::IsBlock(int userid) {
  if (block_sort_list_.empty()) {
    return false;
  }

  while (find_iter_ != block_sort_list_.end()) {
    if (*find_iter_ < userid) {
      ++find_iter_;
    } else if (*find_iter_ == userid) {
      return true;
    } else {
      break;
    }
  }

  return false;
}

void KMergeSort::InsertBucket(const CommonFriend& comm) {
  int index = comm.weight / STEP; 

  if (index < 0) {
    return;
  }
  
  if (index > BUCKET_SIZE) {
    bucket_[BUCKET_SIZE - 1].push_back(comm);
  } else {
    bucket_[index].push_back(comm);
  }
}

void KMergeSort::FlushSortVector(std::vector<HeapItem>& sort_vec) {
  if (sort_vec.empty() || IsBlock(*(sort_vec.front().begin_iter))) {
    sort_vec.clear();
    return;
  }

  CommonFriend comm;
  for (vector<HeapItem>::iterator iter = sort_vec.begin();
      iter != sort_vec.end(); ++iter) {
    comm.userId = *iter->begin_iter;
    comm.weight += iter->weight;
    comm.shares.push_back(iter->host_id);
  }
  comm.sharesCount = (int)comm.shares.size();
  InsertBucket(comm);
  sort_vec.clear();
}

void KMergeSort::GetCommFriends(int limit, std::vector<CommonFriend>& comm_list) {
  if (bucket_.size() != BUCKET_SIZE) {
    return;
  }

  for (int i = BUCKET_SIZE - 1; i >= 0; --i) {
    if (!bucket_[i].empty()) {
      for (vector<CommonFriend>::iterator iter = bucket_[i].begin();
          iter != bucket_[i].end(); ++iter) {
        if (limit <= comm_list.size()) {
          return;
        }
        comm_list.push_back(*iter);
      } 
    }
  }
}

void KMergeSort::AddFliterHeap(priority_queue< HeapItem, vector<HeapItem>, IdAscComp >& fliter_heap,
															const std::map<int, std::vector<int> >& id_map,
															int weight) {
  for (map<int, vector<int> >::const_iterator iter = id_map.begin();   
      iter != id_map.end(); ++iter) {
    if (iter->second.empty()) {
      continue;
    }
    HeapItem item;       
    item.host_id = iter->first;
    item.weight = weight;
    item.begin_iter = iter->second.begin();
    item.end_iter = iter->second.end();
    fliter_heap.push(item);
  }
}

}
}
