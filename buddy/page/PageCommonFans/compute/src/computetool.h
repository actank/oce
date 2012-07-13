#ifndef _COMPUTE_TOOL_H_
#define _COMPUTE_TOOL_H_

#include "FriendRankCacheAdapter.h"
#include "BuddyByIdReplicaAdapter.h"
#include "PageCommonFansCacheAdapter.h"
#include "FansPageCacheAdapter.h"

#include "base/logging.h"
#include "base/ptime.h"

void printElement(int ele) {
	std::cout << ele << " ";
}


void printMap(std::map<int, std::vector<int> >& map) {
	std::map<int , std::vector<int> >::const_iterator iter = map.begin();
  while (iter != map.end()) {
		std::cout << iter->first << " : ";
		std::for_each(iter->second.begin(), iter->second.end(), printElement);
		std::cout << std::endl;
		++iter;
	}
}

template<typename T>
inline void PRINT_ELEMENTS(const T& roll, const char* optcstr = "") {
	typename T::const_iterator pos;
	std::cout << optcstr;
	for (pos = roll.begin(); pos != roll.end(); ++pos) {
		std::cout << *pos << " ";
	}
	std::cout << std::endl;
}

void ReadFileGetId(const char* fileName, std::vector<int>& ids) {
	const int SZ = 100;
	char buf[SZ];

	std::ifstream in(fileName, std::ifstream::in);
  
  if (!in.is_open()) {
		std::cerr << "fileName:" << fileName << " error!" << std::endl;
    return;
	}
	while (in.getline(buf, SZ)) {
		int id = std::atoi(buf);
		if (id != 0) {
			ids.push_back(id);
		}
	}
	in.close();
}

//将map1修改成倒排后 生成map2
void ReverseSort(std::map<int, std::vector<int> >& map1, std::map<int, std::vector<int> >& map2) {
	std::map<int, std::vector<int> >::const_iterator iter1 = map1.begin();
	for (; iter1 != map1.end(); ++iter1) {
		int uid = iter1->first;
		for (std::vector<int>::const_iterator sub_iter1 = iter1->second.begin(); sub_iter1
				!= iter1->second.end(); ++sub_iter1) {
			int pid = *sub_iter1;
			std::map<int, std::vector<int> >::iterator findIter =
					map2.find(pid);
			if (findIter != map2.end()) { //已经有
				findIter->second.push_back(uid);
			} else { //没有 需要增加pair
				std::vector<int> v;
				v.push_back(uid);
				map2.insert(std::make_pair(pid, v));
			}
		}
	}
}


/*
//............
int CompareRank(xce::socialgraph::RankData& rd1, xce::socialgraph::RankData& rd2) {
	return rd1.weight > rd2.weight;
}

void SortByFriendRank(int uid, std::vector<int>& seq) {
  // RankSeq is a sequence of xce::socialgraph::RankData which contains int userid and int weight!
	xce::socialgraph::RankSeq rankSeq;
  try {
		rankSeq = xce::socialgraph::FriendRankCacheAdapter::instance().GetRank(uid);
    std::sort(rankSeq.begin(), rankSeq.end(), CompareRank);
		std::vector<int> inter;
    for (size_t i = 0; i < rankSeq.size(); ++i) {
			inter.push_back(rankSeq[i].id);
		}
		
    for (size_t i = seq.size() -1; i >= 0; --i) {
		}
  } catch(...) {
		std::cerr << "GetRank uid:" << uid << "   exception ..." << std::endl;
	}

}
*/
#endif
