#ifndef  _FRIEND_FETCHER_H_
#define  _FRIEND_FETCHER_H_

#include "../common/Common.h"
#include "../common/UserModel.h"
#include "QueryRunner.h"

class FriendReadHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		FriendReadHandler(map<USERID,UserModel>& models) : _models(models)	{}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		map<USERID,UserModel>& _models;
};

class FriendFetcher
{
	public:
		FriendFetcher();
		virtual ~FriendFetcher() {}
		int UpdateBatch(map<USERID,UserModel>& models, int low, int high);
	private:
		string dbInstance;
};

#endif

