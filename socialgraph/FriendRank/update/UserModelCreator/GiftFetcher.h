#ifndef  _GIFT_FETCHER_H_
#define  _GIFT_FETCHER_H_

#include "../common/Common.h"
#include "../common/UserModel.h"
#include "QueryRunner.h"

class GiftReadHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		GiftReadHandler(map<USERID,UserModel>& models) : _models(models)	{}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		map<USERID,UserModel>& _models;
};

class GiftFetcher
{
	public:
		GiftFetcher();
		virtual ~GiftFetcher() {}
		int UpdateBatch(map<USERID,UserModel>& models, int low, int high);
	private:
		string dbInstance;
};

#endif

