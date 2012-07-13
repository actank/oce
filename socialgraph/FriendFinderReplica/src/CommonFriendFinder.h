#ifndef _COMMONFRIENDFINDER_H_
#define _COMMONFRIENDFINDER_H_
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ServiceI.h"
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>
#include <ext/hash_map>
#include <FriendFinder.h>
#include "QueryRunner.h"

namespace com {
namespace xiaonei {
namespace service {

const int HASHSIZE=9839;

inline bool CommonFriendLess(const CommonFriend& lhs, const CommonFriend& rhs) {
	return lhs.userId<rhs.userId;
}

inline bool CommonFriendWeight(const CommonFriend& lhs, const CommonFriend& rhs) {
	return lhs.weight>rhs.weight;
}

struct index_hash {
	size_t operator()(const int& i) const {
		return i%HASHSIZE;
	}

};

class CommonFriendCalculator {
public:
	CommonFriendCalculator(int userId) :
		_userId(userId), _weightCount(HASHSIZE) {
		return;
	}
	std::vector<CommonFriend> getComm();
	typedef __gnu_cxx::hash_map<int,int> weightType;
	typedef weightType::iterator weightIter;
private:
	void buildWeight(const MyUtil::Int2IntSeqMap& users, int weight);
	void fetchData();

	int _userId;
	MyUtil::IntSeq _friendList;
	MyUtil::IntSeq _applyList;
	MyUtil::IntSeq _blockList;
	MyUtil::IntSeq _blockedList;
	weightType _weightCount;
	MyUtil::Int2IntSeqMap _shareCount;
	std::vector<CommonFriend> _ret;
};

}
;
}
;
}
;
#endif
