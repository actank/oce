#ifndef __COMMONFRIEND_UTIL_H__
#define __COMMONFRIEND_UTIL_H__

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

namespace xce {
namespace offerfriends {

using namespace com::xiaonei::service;

const int COMMHASHSIZE=9839;

inline bool CommonFriendLess(const CommonFriend& lhs, const CommonFriend& rhs) {
	return lhs.userId<rhs.userId;
}

inline bool CommonFriendWeight(const CommonFriend& lhs, const CommonFriend& rhs) {
	return lhs.weight>rhs.weight;
}

struct comm_index_hash {
	size_t operator()(const int& i) const {
		return i%COMMHASHSIZE;
	}

};

class CommonFriendCalculator {
public:
	CommonFriendCalculator(int userId,
			const MyUtil::IntSeq& friendList,
			const MyUtil::IntSeq& applyList,
			const MyUtil::IntSeq& blockList,
			const MyUtil::IntSeq& commBlockList,
			const MyUtil::Int2IntSeqMap& ff,
			const MyUtil::Int2IntSeqMap& af ):
		_weightCount(COMMHASHSIZE),
		_userId(userId),
		_friendList(friendList),
		_applyList(applyList),
		_blockList(blockList),
		_commBlockList(commBlockList),
		_ff(ff),
		_af(af) {
	}
	com::xiaonei::service::CommonFriendSeq getComm(int limit);
	typedef __gnu_cxx::hash_map<int,int> weightType;
	typedef weightType::iterator weightIter;
private:
	void buildWeight(const MyUtil::Int2IntSeqMap& users, int weight);
	void fetchData();

	weightType _weightCount;
	int _userId;
	const MyUtil::IntSeq& _friendList;
	const MyUtil::IntSeq& _applyList;
	const MyUtil::IntSeq& _blockList;
	const MyUtil::IntSeq& _commBlockList;
	const MyUtil::Int2IntSeqMap& _ff;
	const MyUtil::Int2IntSeqMap& _af;

	MyUtil::Int2IntSeqMap _shareCount;
	std::vector<CommonFriend> _ret;
};

};
};
#endif
