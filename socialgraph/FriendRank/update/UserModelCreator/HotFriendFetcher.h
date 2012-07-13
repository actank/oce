#ifndef  _HOTFRIEND_FETCHER_H_
#define  _HOTFRIEND_FETCHER_H_

#include "../common/Common.h"
#include "../common/UserModel.h"
#include "QueryRunner.h"

class HotFriendReadHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		HotFriendReadHandler(map<USERID,UserModel>& models) : _models(models)	{}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		map<USERID,UserModel>& _models;
};

class HotFriendFetcher
{
	public:
		HotFriendFetcher();
		virtual ~HotFriendFetcher() {}
		int UpdateBatch(map<USERID,UserModel>& models, int low, int high);
	private:
		string dbInstance;
};

#endif

