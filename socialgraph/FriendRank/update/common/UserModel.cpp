#include "UserModel.h"
#include "../UserModelCreator/BasicFetcher.h"
#include "../UserModelCreator/FriendFetcher.h"
#include "../UserModelCreator/InviteFetcher.h"
#include "../UserModelCreator/GroupFetcher.h"
#include "../UserModelCreator/HotFriendFetcher.h"
#include "../UserModelCreator/PlacesFetcher.h"
#include "../UserModelCreator/GiftFetcher.h"
#include "../UserModelCreator/FeedConfigFetcher.h"
#include <sys/time.h>

// Constructor & Destructor {{{
UserModel::UserModel(int uid)
{
	Init(uid);
}

UserModel::~UserModel()
{
}// }}}
// Init {{{
int UserModel::Init(int uid)
{
	this->uid = uid;
	gender = -1;
	birth_year = 0;
	friends.clear();
	groups.clear();
	guests.clear();
	hotFriends.clear();
	places.clear();
	giftReceivers.clear();
	feedlove.clear();
	feedhate.clear();
	return 0;
}// }}}

// Print {{{
void UserModel::Print() const
{
	printf("\n===================================================\n");
	printf("[basic info]\n");
	printf("%d, %d, %d\n", uid, gender, birth_year);
	printf("\n------------------------\n");
	printf("[friend list]\n");
	for (set<int>::const_iterator iter = friends.begin();
			iter != friends.end(); ++iter)
	{
		printf("%d  ", *iter);
	}
	printf("\n");
	printf("\n------------------------\n");
	printf("[user group : family]\n");
	for (set<int>::const_iterator iter = groups.begin();
			iter != groups.end(); ++iter)
	{
		printf("%d  ", *iter);
	}
	printf("\n");
	printf("\n------------------------\n");
	printf("[invite guest]\n");
	for (set<int>::const_iterator iter = guests.begin();
			iter != guests.end(); ++iter)
	{
		printf("%d  ", *iter);
	}
	printf("\n");
	printf("\n------------------------\n");
	printf("[hot friend list]\n");
	for (set<int>::const_iterator iter = hotFriends.begin();
			iter != hotFriends.end(); ++iter)
	{
		printf("%d  ", *iter);
	}
	printf("\n");
	printf("\n------------------------\n");
	printf("[company,school list]\n");
	for (map<string,int>::const_iterator iter = places.begin();
			iter != places.end(); ++iter)
	{
		printf("<%s,%d> ", iter->first.c_str(), iter->second);
	}
	printf("\n");
	printf("\n------------------------\n");
	printf("[gift receivers]\n");
	for (set<int>::const_iterator iter = giftReceivers.begin();
			iter != giftReceivers.end(); ++iter)
	{
		printf("%d  ", *iter);
	}
	printf("\n");
	printf("\n------------------------\n");
	printf("[feed config love users]\n");
	for (set<int>::const_iterator iter = feedlove.begin();
			iter != feedlove.end(); ++iter)
	{
		printf("%d  ", *iter);
	}
	printf("\n");
	printf("\n------------------------\n");
	printf("[feed config hate users]\n");
	for (set<int>::const_iterator iter = feedhate.begin();
			iter != feedhate.end(); ++iter)
	{
		printf("%d  ", *iter);
	}
	printf("\n");
	printf("\n------------------------\n");
}// }}}
// PrintOneLine {{{
void UserModel::PrintOneLine() const
{
	printf("%d\t%d\t%d\t", uid, gender, birth_year);
	printf("%ld\t", friends.size());
	for (map<string,int>::const_iterator iter = places.begin();
			iter != places.end(); ++iter)
	{
		printf("<%s,%d>\t", iter->first.c_str(), iter->second);
	}
	printf("\n");
}// }}}
// PrintFriendOf {{{
void UserModel::PrintFriendOf(const UserModel& me) const
{
	// user_basic
	printf("%d\t%d\t%d\t", uid, gender, birth_year);
	// friends
	printf("%ld\t%d\t", this->friends.size(), CommonElemCnt(me.friends, this->friends));
	// user group . family
	if (me.groups.find(uid)!=me.groups.end())
	{
		printf("y");
	}
	printf("\t");
	// invite guest
	if (me.guests.find(uid) != me.guests.end())
	{
		printf("y");
	}
	printf("\t");
	//hot friend list
	if (me.hotFriends.find(uid) != me.hotFriends.end())
	{
		printf("y");
	}
	printf("\t");
	// company,school list
	//for (map<string,int>::const_iterator iter = places.begin();
	//		iter != places.end(); ++iter)
	//{
	//	printf("<%s,%d>", iter->first.c_str(), iter->second);
	//}
	int year0 = 0;
	int year3 = 0;
	int yearm = 0;
	for (map<string,int>::const_iterator meIter = me.places.begin();
			meIter != me.places.end(); ++meIter)
	{
		map<string,int>::const_iterator frIter = this->places.find(meIter->first);
		if (frIter != this->places.end())
		{
			int span = abs(frIter->second - meIter->second);  // uid is entry year here
			if (span<=0)
				++year0;
			else if (span<=3)
				++year3;
			else
				++yearm;
		}
	}
	printf("%d\t%d\t%d", year0, year3, yearm);
	printf("\t");
	// gift receivers
	if (me.giftReceivers.find(uid) != me.giftReceivers.end())
	{
		printf("y");
	}
	printf("\t");
	// feed config love users
	if (me.feedlove.find(uid) != me.feedlove.end())
	{
		printf("w");
	}
	if (me.feedhate.find(uid) != me.feedhate.end())
	{
		printf("b");
	}
	printf("\n");
}// }}}
// PrintTrainExample {{{
void UserModel::PrintTrainExample(const int rank, const UserModel& me) const
{
	printf("%d qid:%d ", rank, me.uid);  // rank and query
	printf("1:%d ", gender != me.gender ? 1 : 0);  // 是异性吗？1是，0否，下同
	int year_diff = birth_year - me.birth_year;
	if (year_diff>20)
		year_diff = 2;
	if (year_diff>10)
		year_diff = 10;
	printf("2:%d ", year_diff);  // 年龄差

	short a = me.friends.size() + 1;
	short b = friends.size() + 1;
	short c = CommonElemCnt(me.friends, friends) + 2;
	printf("3:%f ", 10*sqrt((float)c/(a+b-c)));  // 共同好友

	int score_group = (me.groups.find(uid)!=me.groups.end()) ? 20 : 0;
	printf("4:%d ", score_group);  // 用户分组
	printf("5:%d ", me.guests.find(uid)!=me.guests.end() ? 1 : 0);  // 是邀请注册？
	printf("6:%d ", me.hotFriends.find(uid)!=me.hotFriends.end() ? 1 : 0);  // 是特别好友？
	printf("7:%f ", RankByPlace(me.places, places));  // 公司学校
	printf("8:%d ", me.giftReceivers.find(uid)!=me.giftReceivers.end() ? 1 : 0);  // 送过礼物？
	
	int feed_score = 0;
	if (me.feedlove.find(uid) != me.feedlove.end())
		feed_score = 1;
	else if (me.feedhate.find(uid) != me.feedhate.end())
		feed_score = -1;
	printf("9:%d ", feed_score);  // 新鲜事黑白名单：1白，-1黑，0没设置
	printf("# %d\n", uid);  // 注释
}// }}}

// FromMySql {{{
int UserModel::FromMySql(FriendFetcher* mFetcher, FriendInviter* mInviter,
		PlacesFetcher* mPlace, BasicFetcher* mBasic, 
		GroupFetcher* mGroup, HotFriendFetcher* mHotFriend,
		GiftFetcher* mGift, FeedConfigFetcher* mFeedConfig)
{
	Init(uid);
	// friend list
	//mFetcher->Fetch(uid, this->friends);
	// Invite list
	//mInviter->Fetch(uid, this->guests);
	// Places : Corpration, University, Highschool
	//mPlace->Fetch(uid, this->places);
	// BasicInfo: Gender, Birth_year
	//mBasic->Fetch(uid, this->user_basic);
	// hot friend
	//mHotFriend->Fetch(uid, this->hotFriends);
	// groups
	//mGroup->Fetch(uid, this->groups);
	// gift
	//mGift->Fetch(uid, this->giftReceivers);
	// feed config
	//mFeedConfig->Fetch(uid, this->feedlove, this->feedhate);
	return 0;
}// }}}
// FromBuffer {{{
int UserModel::FromBuffer(char* buff, int len_buff)
{
	this->Init(0);
	int len = 0;
	USERID uid = 0;
	char* p = buff;

	// basic info
	this->uid = *(int*)p;  p+=sizeof(int);
	this->gender = *(int*)p;  p+=sizeof(int);
	this->birth_year = *(int*)p;  p+=sizeof(int);

	// friend list
	len = *(int*)p;  p+=sizeof(int);
	for (int i=0; i<len; ++i)
	{
		uid = *(int*)p;  p+=sizeof(int);
		friends.insert(uid);
	}

	// company,school list
	len = *(int*)p;  p+=sizeof(int);
	for (int i=0; i<len; ++i)
	{
		char* name = p;
		while (*p!=0)  ++p;
		++p;
		uid = *(int*)p;  p+=sizeof(int);
		places.insert(make_pair<string,int>(name,uid));
	}

	// user group
	len = *(int*)p;  p+=sizeof(int);
	for (int i=0; i<len; ++i)
	{
		uid = *(int*)p;  p+=sizeof(int);
		groups.insert(uid);
	}

	// invite guest
	len = *(int*)p;  p+=sizeof(int);
	for (int i=0; i<len; ++i)
	{
		uid = *(int*)p;  p+=sizeof(int);
		guests.insert(uid);
	}

	// special friend list
	len = *(int*)p;  p+=sizeof(int);
	for (int i=0; i<len; ++i)
	{
		uid = *(int*)p;  p+=sizeof(int);
		hotFriends.insert(uid);
	}
	
	// gift receivers
	len = *(int*)p;  p+=sizeof(int);
	for (int i=0; i<len; ++i)
	{
		uid = *(int*)p;  p+=sizeof(int);
		giftReceivers.insert(uid);
	}
	
	// feed config love & hate users
	len = *(int*)p;  p+=sizeof(int);
	for (int i=0; i<len; ++i)
	{
		uid = *(int*)p;  p+=sizeof(int);
		feedlove.insert(uid);
	}
	len = *(int*)p;  p+=sizeof(int);
	for (int i=0; i<len; ++i)
	{
		uid = *(int*)p;  p+=sizeof(int);
		feedhate.insert(uid);
	}

	assert(len_buff==p-buff);
	return 0;
}// }}}
// ToBuffer {{{
int UserModel::ToBuffer(char* buff) const
{
	int len = 0;
	char* p = buff;

	// basic info
	*(int*)p = this->uid;  p+=sizeof(int);
	*(int*)p = this->gender;  p+=sizeof(int);
	*(int*)p = this->birth_year;  p+=sizeof(int);

	// friend list
	len = friends.size();
	*(int*)p = len;  p+=sizeof(int);
	for (set<int>::const_iterator iter = friends.begin();
			iter != friends.end(); ++iter)
	{
		*(int*)p = *iter;  p+=sizeof(int);
	}
	
	// company,school list
	len = places.size();
	*(int*)p = len;  p+=sizeof(int);
	for (map<string,int>::const_iterator iter = places.begin();
			iter != places.end(); ++iter)
	{
		strcpy(p, iter->first.c_str());
		while (*p!=0)  ++p;
		++p;

		*(int*)p = iter->second;  p+=sizeof(int);
	}

	// user group
	len = groups.size();
	*(int*)p = len;  p+=sizeof(int);
	for (set<int>::const_iterator iter = groups.begin();
			iter != groups.end(); ++iter)
	{
		*(int*)p = *iter;  p+=sizeof(int);
	}

	// invite guest
	len = guests.size();
	*(int*)p = len;  p+=sizeof(int);
	for (set<int>::const_iterator iter = guests.begin();
			iter != guests.end(); ++iter)
	{
		*(int*)p = *iter;  p+=sizeof(int);
	}

	// special friend list
	len = hotFriends.size();
	*(int*)p = len;  p+=sizeof(int);
	for (set<int>::const_iterator iter = hotFriends.begin();
			iter != hotFriends.end(); ++iter)
	{
		*(int*)p = *iter;  p+=sizeof(int);
	}

	// gift receivers
	len = giftReceivers.size();
	*(int*)p = len;  p+=sizeof(int);
	for (set<int>::const_iterator iter = giftReceivers.begin();
			iter != giftReceivers.end(); ++iter)
	{
		*(int*)p = *iter;  p+=sizeof(int);
	}
	
	// feed config love & hate users
	len = feedlove.size();
	*(int*)p = len;  p+=sizeof(int);
	for (set<int>::const_iterator iter = feedlove.begin();
			iter != feedlove.end(); ++iter)
	{
		*(int*)p = *iter;  p+=sizeof(int);
	}
	len = feedhate.size();
	*(int*)p = len;  p+=sizeof(int);
	for (set<int>::const_iterator iter = feedhate.begin();
			iter != feedhate.end(); ++iter)
	{
		*(int*)p = *iter;  p+=sizeof(int);
	}

	return p-buff;
}// }}}

// ComputeRelevance {{{
int UserModel::ComputeRelevance(const UserModel& other, StaticRank& rank) const
{
	rank.Init(0);
	map<string,string>::const_iterator meIter;
	map<string,string>::const_iterator frIter;

	rank.fid = other.uid;
	// 基本信息：性别、年龄
	int meyear = this->birth_year;
	int fryear = other.birth_year;
	if (meyear<2009 && meyear>1949 && fryear<2009 && fryear>1949)
	{
		rank.diffBirth = (char)abs(meyear-fryear);
	}
	//printf("%d-%d=%d, %d\n", meyear, fryear, meyear-fryear, 
	// (char)(curIter->second.diffBirth));

	rank.diffGender =	(this->gender != other.gender) ? true : false;

	// 好友关系
	rank.friends[0] = this->friends.size();
	rank.friends[1] = other.friends.size();
	rank.friends[2] = CommonElemCnt(this->friends, other.friends);
	// 位置相关
	rank.commonInfors = RankByPlace(this->places, other.places);
	// 邀请记录
	rank.isInvited = 
		(this->guests.find(other.uid)!=this->guests.end()) ? true : false;
	// 特别好友
	rank.isSpecial = 
		(this->hotFriends.find(other.uid)!=this->hotFriends.end()) ? true : false;
	// 礼物接收者
	rank.isGiftReceiver = 
		(this->giftReceivers.find(other.uid)!=this->giftReceivers.end()) ? true : false;
	// 新鲜事设置名单
	if (this->feedlove.find(other.uid)!=this->feedlove.end())
		rank.feedconfig += 1;
	if (this->feedhate.find(other.uid)!=this->feedhate.end())
		rank.feedconfig -= 1;
	// 用户分组
	rank.scoreGroup = (groups.find(other.uid)!=groups.end()) ? 20 : 0;
	return 0;
}// }}}
// RankByPlace {{{
float UserModel::RankByPlace(const map<string,int>& p1, const map<string,int>& p2) const
{
	const map<string,int>* little = &p1;
	const map<string,int>* more = &p2;
	if (p1.size()>p2.size())
	{
		little = &p2;
		more = &p1;
	}

	float score = 0;
	for (map<string,int>::const_iterator littleIter = little->begin();
			littleIter != little->end(); ++littleIter)
	{
		map<string,int>::const_iterator moreIter = more->find(littleIter->first);
		if (moreIter != more->end())
		{
			int span = abs(littleIter->second - moreIter->second);
			if (span > 10)
				span = 10;
			score += 5/(1+span);
		}
	}
	return score;
}// }}}
// CommonFriendCnt {{{
short UserModel::CommonElemCnt(const set<int>& fa, const set<int>& fb) const
{
	const set<int>* lessone = &fa;
	const set<int>* moreone = &fb;
	if (fa.size()>fb.size())
	{
		lessone = &fb;
		moreone = &fa;
	}
	set<int>::const_iterator lessIter;
	set<int>::const_iterator moreIter;
	short common = 0;

	//for (lessIter=lessone->begin(); lessIter!=lessone->end(); ++lessIter)
	//	printf("%d,", *lessIter);
	//printf("\n");
	//for (moreIter=moreone->begin(); moreIter!=moreone->end(); ++moreIter)
	//	printf("%d,", *moreIter);
	//printf("\n");
	//getchar();

	if (lessone->size() * 5 < moreone->size())
	{
		// O(nlogm) when n<<m
		for (lessIter = lessone->begin();
				lessIter != lessone->end(); ++lessIter)
		{
			if (moreone->find(*lessIter)!=moreone->end())
				++common;
		}
	}
	else
	{
		lessIter = lessone->begin();
		moreIter = moreone->begin();
		// O(n+m) when n~~m
		while (lessIter!=lessone->end() && moreIter!=moreone->end())
		{
			if (*lessIter<*moreIter)
				++lessIter;
			else if (*moreIter<*lessIter)
				++moreIter;
			else {
				++lessIter;  ++moreIter;
				++common;
			}
		}
	}
	return common;
}// }}}

// ComputeRelevanceByBuff {{{
int UserModel::ComputeRelevanceByBuff(char* buff, int len_buff, StaticRank& rank,
		char* friendbuff, long IDLENGHT) const
{
	rank.Init(0);
	int len = 0;
	int temp = 0;
	char* p = buff;
	map<string,string>::const_iterator meIter;

	//------------------------------------
	//struct timeval start[6];
	//gettimeofday(&start[0], NULL);
	// 基本信息：性别、年龄
	rank.fid = *(int*)p;  p+=sizeof(int);
	int other_gender = *(int*)p;  p+=sizeof(int);
	int other_birth_year = *(int*)p;  p+=sizeof(int);
	rank.diffGender = (this->gender != other_gender) ? true : false;

	int meyear = this->birth_year;
	int fryear = other_birth_year;
	if (meyear<2009 && meyear>1949 && fryear<2009 && fryear>1949)
		rank.diffBirth = (char)abs(meyear-fryear);
	//printf("%d-%d=%d, %d\n", meyear, fryear, meyear-fryear, 
	// (char)(curIter->second.diffBirth));

	//------------------------------------
	// 好友关系
	//gettimeofday(&start[1], NULL);
	len = *(int*)p;  p+=sizeof(int);
	//int accurate = 0;
	for (int i=0; i<len; ++i)
	{
		temp = *(int*)p;  p+=sizeof(int);
		rank.friends[2] += friendbuff[temp%IDLENGHT];  // 0非好友，1好友
		//if (this->friends.find(temp)!=this->friends.end())
		//	++accurate;
	}
	rank.friends[0] = this->friends.size();
	rank.friends[1] = len;
	//fprintf(stderr, "共同好友数：真实值%d，[%%ld]近似值%d\n", 
	//		accurate, IDLENGHT, rank.friends[2]);
	//getchar();

	//------------------------------------
	// 位置相关 company,school list
	//gettimeofday(&start[2], NULL);
	len = *(int*)p;  p+=sizeof(int);
	for (int i=0; i<len; ++i)
	{
		char* name = p;
		while (*p!=0)  ++p;
		++p;
		temp = *(int*)p;  p+=sizeof(int);
		map<string,int>::const_iterator thisIter = this->places.find(name);
		if (thisIter != this->places.end())
		{
			int span = abs(thisIter->second - temp);  // temp is entry year here
			if (span > 10)
				span = 10;
			rank.commonInfors += 3/(1+span);
		}
	}

	// skip all following buff
	// ...

	//------------------------------------
	// 用户分组
	//gettimeofday(&start[3], NULL);
	rank.scoreGroup = (this->groups.find(rank.fid)!=this->groups.end()) ? 20 : 0;

	//------------------------------------
	//gettimeofday(&start[4], NULL);
	// 邀请记录
	rank.isInvited = 
		(this->guests.find(rank.fid)!=this->guests.end()) ? true : false;
	
	// 特别好友
	rank.isSpecial = 
		(this->hotFriends.find(rank.fid)!=this->hotFriends.end()) ? true : false;

	// 礼物接收者
	rank.isGiftReceiver = 
		(this->giftReceivers.find(rank.fid)!=this->giftReceivers.end()) ? true : false;
	
	// 新鲜事设置名单
	// TODO: [Error] rank.feedconfig += 1; rank.feedconfig -= 1
	if (this->feedlove.find(rank.fid)!=this->feedlove.end())
		rank.feedconfig = (char)1;
	else if (this->feedhate.find(rank.fid)!=this->feedhate.end())
		rank.feedconfig = (char)-1;
	//gettimeofday(&start[5], NULL);

	//------------------------------------
	//for (int i=0; i<5; ++i)
	//{
	//	fprintf(stdout, "[%d:%ld]", i, 
	//			(start[i+1].tv_sec-start[i].tv_sec)*1000000+start[i+1].tv_usec-start[i].tv_usec);
	//}
	//fprintf(stderr, "\n");	getchar();
	return 0;
}// }}}

