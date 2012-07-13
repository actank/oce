#include "socialgraph/friendfinderworker/algorithm/method/impl/hashmapsort.h"

namespace xce {
namespace socialgraph {

void HashMapSort::CalComm(CommonFriendSeq& comm) {
	std::cout << "HashMapSort CalComm" << std::endl;
	//创建推荐列表
	CreateCommonMap();

	//过滤block项
	InitBlockItem();

	//桶管理
	InitSortVector();

	//获取comm
	GetTarget(comm);
}

void HashMapSort::Display(const CommonFriendSeq& comm) const {
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

void HashMapSort::CreateCommonMap() {
	BuildWeight(ffmap_, 100);
	BuildWeight(afmap_, 70);
}

void HashMapSort::InitBlockItem() {
	// 不推荐自己
	weightcount_[userid_] = numeric_limits<int>::min();
	
	// 不推荐用户黑名单的用户
	SetBlockItem(blocklist_);

	// 不推荐删除过的推荐
	SetBlockItem(commonblock_);

	// 不推荐用户的好友
	SetBlockItem(friendlist_);

	// 不推荐发过申请的用户
	SetBlockItem(applylist_);
}

void HashMapSort::BuildWeight(const std::map<int, std::vector<int> >& users, int weight) {
  try {
    for (std::map<int, std::vector<int> >::const_iterator iter = users.begin();
        iter != users.end(); ++iter) {
      for (std::vector<int>::const_iterator ui = iter->second.begin();
          ui != iter->second.end(); ++ui) {
        weightcount_[*ui] += weight;
      
        if (weightcount_[*ui] > maxweight_) {
          maxweight_ = weightcount_[*ui];
        }

        __gnu_cxx::hash_map<int, std::vector<int> >::iterator sIt = sharecount_.find(*ui);
        if (sIt == sharecount_.end()) {
          std::vector<int> firstShares;
          firstShares.push_back(iter->first);
          sharecount_.insert(make_pair(*ui, firstShares));
        } else {
          sIt->second.push_back(iter->first);
        }
      }
    }
  } catch(Ice::Exception& e) {
		std::cout << "buildUser IceException " << userid_ << std::endl;
  } catch(std::exception& e) {
		std::cout << "buildUser stdException " << userid_ << std::endl;
  } catch(...) {
		std::cout << "buildUser exception " << userid_ << std::endl;
  }
}

void HashMapSort::SetBlockItem(const std::vector<int>& blocklist) {
	for (std::vector<int>::const_iterator iter = blocklist.begin();
			iter != blocklist.end(); ++iter) {
		weightcount_[*iter] = numeric_limits<int>::min();
	}
}

void HashMapSort::InitSortVector() {
  try {
		sortvector_.resize(maxweight_ + 1);
		for (WeightIter iter = weightcount_.begin();
				iter != weightcount_.end(); ++iter) {
			if (iter->second >= 0) {
				sortvector_[iter->second].push_back(iter->first);
			}
		}
  } catch(Ice::Exception& e) {
		std::cout << "InitSortVector IceException " << e << std::endl;
  } catch(std::exception& e) {
		std::cout << "InitSortVector StdException " << e.what() << std::endl;
  } catch(...) { 
		std::cout << "InitSortVector Unknow Exception " << std::endl;
  }
}

void HashMapSort::GetTarget(CommonFriendSeq& comm) {
	try {
		CommonFriend cf;
		for (int i = maxweight_; i >= 0; --i) {
		  const vector<int>& subvec = sortvector_[i];
		  for (vector<int>::const_iterator iter = subvec.begin();
		      iter != subvec.end(); ++iter) {
		    __gnu_cxx::hash_map<int, MyUtil::IntSeq>::iterator shareit = sharecount_.find(*iter);
		
		    if (HASH_COMMON_FRIENDS_SIZE <= (int)comm.size()) {
		      return;
		    }
		
		    if (shareit != sharecount_.end()) {
		      cf.userId = *iter;
		      cf.weight = i;
		      cf.shares = shareit->second;
		      cf.sharesCount = (int)cf.shares.size();
		      comm.push_back(cf);
		    }
		  }
		}
	} catch (Ice::Exception& e) {
		std::cout << "getComm IceException" << e << std::endl;
	} catch (std::exception& e) {
		std::cout << "getComm StdException" << e.what() << std::endl;
	} catch (...) {
		std::cout << "getComm unknown exception" << std::endl;
	}
}

}
}
