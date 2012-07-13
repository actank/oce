#ifndef  _SOCIALFEED_TASKS_H_
#define  _SOCIALFEED_TASKS_H_

#include "SocialFeed.h"
#include "FriendFetcher.h"
#include "SocialFeedBlockFetcher.h"
#include "TaskManager.h"
//#include "OfferFriendsReplicaAdapter.h"

extern FriendFetcher* friendFetcher;
extern SocialFeedBlockFetcher* socialFeedBlockFetcher;
const int TASK_LEVEL_READ_FRIENDS = 1;
const int TASK_LEVEL_CREATE_FEEDS = 2;

class ReadFriendTask : public MyUtil::Task
{
	public:
		ReadFriendTask(int uid, vector<USERID>& friends,
				::Ice::Int level=TASK_LEVEL_READ_FRIENDS):
			MyUtil::Task(level), _friends(friends) {
				_uid = uid;
			}
		virtual void handle()
		{
			//IceUtil::RWRecMutex::WLock lock(_mutex);
			//fprintf(stderr, "uid=%d, size=%ld\n", _uid, _friends.size());
			friendFetcher->Read(_uid, _friends);
		}
	private:
		int _uid;
		vector<USERID>& _friends;
		//IceUtil::RWRecMutex _mutex;
};

class CreateSocailFeedTask : public MyUtil::Task
{
	public:
		CreateSocailFeedTask(int uid, const vector<USERID>& changeFriends,
				const map<USERID,set<USERID> >& secondLayer,
				::Ice::Int level=TASK_LEVEL_CREATE_FEEDS):
			MyUtil::Task(level), _changeFriends(changeFriends), _secondLayer(secondLayer) {
				_uid = uid;
			}
		virtual void handle()
		{
			vector<SocialFeed> feeds;
			CreateSocailFeed(feeds, _uid, _changeFriends, _secondLayer);
			SetSocialFeedCache(_uid, feeds);
			//fprintf(stderr, "uid = %d :\n", _uid);
			//for (vector<SocialFeed>::const_iterator iter = feeds.begin();
			//		iter != feeds.end(); ++iter)
			//{
			//	iter->Print();
			//}
		}
		int SetSocialFeedCache(int uid, const vector<SocialFeed>& feeds);
		int CreateSocailFeed(vector<SocialFeed>& feeds,
				USERID uid, const vector<USERID>& changeFriends,
				const map<USERID,set<USERID> >& secondLayer);
		int GetOfferFriendSort(int userId, int limit, map<int,float>& offerfriend_sort);
	private:
		int _uid;
		const vector<USERID>& _changeFriends;
		const map<USERID,set<USERID> >& _secondLayer;
};

#endif

