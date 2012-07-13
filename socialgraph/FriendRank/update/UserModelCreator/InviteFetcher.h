#ifndef  _INVITE_FETCHER_H_
#define  _INVITE_FETCHER_H_

#include "../common/Common.h"
#include "../common/UserModel.h"
#include "QueryRunner.h"

typedef int USERID;

class InviteReadHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		InviteReadHandler(map<USERID,UserModel>& models) : _models(models)	{}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		map<USERID,UserModel>& _models;
};

class InviteFetcher
{
	public:
		InviteFetcher();
		virtual ~InviteFetcher() {}
		int UpdateBatch(map<USERID,UserModel>& models, int low, int high);
	private:
		string dbInstance;
};

#endif

