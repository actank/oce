#ifndef _USER_MODEL_H_
#define _USER_MODEL_H_

class PassportFetcher;
class BasicFetcher;
class FriendFetcher;
class FriendInviter;
class GroupFetcher;
class HotFriendFetcher;
class PlacesFetcher;
class GiftFetcher;
class FeedConfigFetcher;
#include "StaticRank.h"

class UserModelHead
{
	public:
		int uid;
		int len;
};

// not thread-safe
class UserModel
{
	public:
		UserModel(int uid);
		UserModel(char* buff, int len) {
			FromBuffer(buff, len);
		}
		~UserModel();
		int Init(int uid);
		void Print() const;
		void PrintOneLine() const;
		void PrintFriendOf(const UserModel& me) const;
		void PrintTrainExample(const int rank, const UserModel& me) const;
		int ToBuffer(char* buff) const;
		int FromBuffer(char* buff, int len_buff);
		int FromMySql(FriendFetcher* mFetcher, FriendInviter* mInviter,
				PlacesFetcher* mPlace, BasicFetcher* mBasic, 
				GroupFetcher* mGroup, HotFriendFetcher* mHotFriend,
				GiftFetcher* mGiftFetcher, FeedConfigFetcher* mFeedConfig);
		int ComputeRelevance(const UserModel& other, StaticRank& rank) const;
		int ComputeRelevanceByBuff(char* buff, int len_buff, StaticRank& rank,
				char* friendbuff, long IDLENGHT) const;

	private:
		short CommonElemCnt(const set<int>& vec1, const set<int>& vec2) const;
		float RankByPlace(const map<string,int>& p1, const map<string,int>& p2) const;

	public:
		int uid;
		//string user_basic[5];  // [0]id, [1]name, [2]status, [3]gender, [4]birth_year
		int gender;  // 0女 1男 -1未知
		int birth_year;
		set<int> friends;
		map<string,int> places;

		set<int> groups;  // 家人亲戚分组
		set<int> guests;
		set<int> hotFriends;
		set<int> giftReceivers;
		set<int> feedlove;
		set<int> feedhate;
};

#endif

