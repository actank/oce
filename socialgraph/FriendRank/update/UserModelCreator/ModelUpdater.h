#ifndef _MODEL_UPDATER_H_A
#define _MODEL_UPDATER_H_A

#include "../common/Common.h"
#include "../common/UserModel.h"
#include <ServiceI.h>   // for MCE_INFO

#include "BasicFetcher.h"
#include "FriendFetcher.h"
#include "InviteFetcher.h"
#include "PlacesFetcher.h"
#include "GroupFetcher.h"
#include "HotFriendFetcher.h"
#include "GiftFetcher.h"
#include "FeedConfigFetcher.h"

// not thread-safe
class ModelUpdater
{
	public:
		ModelUpdater();
		~ModelUpdater();
		int ReadGroupFamilyId(long WORKER);
		int UpdateModel(long WORKER, long WORKERNUM, long STEP);

	private:
		int GetStepInterval(long WORKER, long WORKERNUM, long STEP, int stepno, map<USERID,UserModel>& models);
		int GetStepOverall(long WORKER, long WORKERNUM, long STEP, int stepno, map<USERID,UserModel>& models);
		int ReadUserInfo(map<USERID,UserModel>& models, int* usetime);
		int SaveModels(const map<USERID,UserModel>& models, FILE* fp);
		int Put(const UserModel& model);
		int Get(UserModel& model);

	private:
		char* buffer;
		const size_t BUFFLEN;
		set<int> familyIds;

		FriendFetcher* mFriend;
		InviteFetcher* mInviter;
		PlacesFetcher* mPlace;
		BasicFetcher* mBasic;
		GroupFetcher* mGroup;
		HotFriendFetcher* mHotFriend;
		GiftFetcher* mGift;
		FeedConfigFetcher* mFeedConfig;
};

#endif

