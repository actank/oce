#ifndef _PUK_FEED_ITEM_H_
#define _PUK_FEED_ITEM_H_

#include <vector>

class PUKFeedItem {
public:
	PUKFeedItem(int friendId, const std::vector<int>& ownerFriendsList, const std::vector<int>& commonFriendsList) {
		//cout << friendId << " : " << ownerFriendsList.size() << " : " << commonFriendsList.size() << endl;
		_friendId = friendId;
		_sharesCount = CountShareFriends(ownerFriendsList, commonFriendsList);
		//cout << _friendId << " : " << _sharesCount << endl;
	}

	int GetFriendId() const {
		return _friendId;
	}

	int GetSharesCount() const {
		return _sharesCount;
	}

private:
	int CountShareFriends(const std::vector<int>& ownerFriendsList, const std::vector<int>& commonFriendsList) {
		//for (vector<int>::const_iterator it = ownerFriendsList.begin();
		//		it != ownerFriendsList.end(); ++it) {
		//	cout << *it << " ";
		//}
		//cout << endl;
		//cout << endl;
		//for (vector<int>::const_iterator it = commonFriendsList.begin();
		//		it != commonFriendsList.end(); ++it) {
		//	cout << *it << " ";
		//}
		//cout << endl;
		//cout << endl;

		int shares_count = 0;
		
		if (ownerFriendsList.empty() && commonFriendsList.empty()) {
			return shares_count;
		}		

		//cout << "calculate" << endl;

		std::vector<int>::const_iterator owner_friends_iter = ownerFriendsList.begin();
		std::vector<int>::const_iterator common_friends_iter = commonFriendsList.begin();

		while (owner_friends_iter != ownerFriendsList.end() && common_friends_iter != commonFriendsList.end()) {
			if (*owner_friends_iter == *common_friends_iter) {
				++shares_count;
				++owner_friends_iter;
				++common_friends_iter;
			} else if (*owner_friends_iter > *common_friends_iter) {
				++common_friends_iter;
			} else {
				++owner_friends_iter;
			}
		}

		return shares_count;
	}

	int _friendId;
	int _sharesCount;
};

#endif
