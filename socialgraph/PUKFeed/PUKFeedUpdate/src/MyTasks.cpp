#include "MyTasks.h"

using namespace xce::socialgraph;
using namespace MyUtil;
using namespace tasks;
using namespace std;
using namespace dbhelper;

void CreateFeedAndSetCacheTask::handle() {
	//cout << "_ownerId : " << _ownerId << "; _recommendList size : " << _recommendList.size() << endl;
	PUKFeedSeq result;

	vector<int> ownerVec;
	ownerVec.push_back(_ownerId);

	map<int, vector<int> > friendList;

	g_readfriend_ptr->GetFriendListSynchronism(ownerVec, friendList);

	for (vector<int>::iterator iter = _recommendList.begin();
			iter != _recommendList.end(); ++iter) {
		PUKFeedItem item = GetMostValuePUKFeed(*iter, friendList[_ownerId]);

		if (0 == item.GetFriendId()) {
			continue;
		}

		PUKFeed p_feed;
		p_feed.friendId = *iter;
		p_feed.commonId = item.GetFriendId();
		result.push_back(p_feed);
	}

	//cout << "_ownerId : " << _ownerId << ", result size : " << result.size() << endl;
	//for (PUKFeedSeq::iterator iter = result.begin();
	//		iter != result.end(); ++iter) {
	//	cout << iter->friendId << " : " << iter->commonId << endl;
	//}
	cout << "output CreateFeedAndSetCacheTask " << _ownerId << " " << result.size() << endl;

	if (result.empty()) {
		return;
	} else {
		try {
			PUKFeedCacheAdapter::instance().Set(_ownerId, result);
		} catch (Ice::Exception& e) {
				cout << "CreateFeedAndSetCacheTask PUKFeedCacheAdapterice exception " << e.what() << endl;
		} catch (std::exception& e) {
				cout << "CreateFeedAndSetCacheTask PUKFeedCacheAdapterice std exception " << e.what() << endl;
		} catch (...) {
				cout << "CreateFeedAndSetCacheTask PUKFeedCacheAdapterice unknow exception" << endl;
		}
	}
}

PUKFeedItem CreateFeedAndSetCacheTask::GetMostValuePUKFeed(int friendId, const std::vector<int>& ownerFriends) {
	vector<PUKFeedItem> PUKFeedList;	
	//PUKFeedList.clear();

	map<int, set<int> >::const_iterator fIt = _buddyApplyMap.find(friendId);
	if (fIt == _buddyApplyMap.end()) {
		return PUKFeedItem(0, vector<int>(), vector<int>());
	}

	const set<int>& addPeople = fIt->second;

	//cout << "blockRecommendSet list" << endl;
	set<int> blockRecommendSet;
	PUKFeedDBHelper::GetRemovePUKeed(_ownerId, friendId, blockRecommendSet);
	//for (set<int>::iterator iter = blockRecommendSet.begin();
	//		iter != blockRecommendSet.end(); ++iter) {
	//	cout << *iter << " ";
	//}
	//cout << endl;

	vector<int> idList;

	for (set<int>::const_iterator iter = addPeople.begin();
			iter != addPeople.end(); ++iter) {
		vector<int>::const_iterator finderIter = lower_bound(ownerFriends.begin(), ownerFriends.end(), *iter);
		if ((finderIter != ownerFriends.end() && *finderIter == *iter) || 
				*iter == _ownerId || 
				blockRecommendSet.find(*iter) != blockRecommendSet.end()) {     //过滤好友列表,自己以及block列表
			continue;
		}
		idList.push_back(*iter);
	}

	map<int, vector<int> > userFriendMap;

	g_readfriend_ptr->GetFriendListSynchronism(idList, userFriendMap);

	for (vector<int>::iterator iter = idList.begin();
			iter != idList.end(); ++iter) {
		PUKFeedItem item(*iter, ownerFriends, userFriendMap[*iter]);
		PUKFeedList.push_back(item);
	}

	sort(PUKFeedList.begin(), PUKFeedList.end(), PUKFeedComp());
	
	if (PUKFeedList.empty()) {
		return PUKFeedItem(0, vector<int>(), vector<int>());
	} else {
		return PUKFeedList.at(0);
	}
}
