#ifndef  _FEEDCONFIG_FETCHER_H_
#define  _FEEDCONFIG_FETCHER_H_

#include "../common/Common.h"
#include "../common/UserModel.h"
#include "QueryRunner.h"

class WhiteListReadHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		WhiteListReadHandler(map<USERID,UserModel>& models) : _models(models)	{}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		map<USERID,UserModel>& _models;
};
class BlackListReadHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		BlackListReadHandler(map<USERID,UserModel>& models) : _models(models)	{}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		map<USERID,UserModel>& _models;
};

class FeedConfigFetcher
{
	public:
		FeedConfigFetcher();
		virtual ~FeedConfigFetcher() {}
		int UpdateBatch(map<USERID,UserModel>& models, int low, int high);
		int ReadFeedConfig(map<USERID,UserModel>& models, int low, int high,
				const char* tablename, bool love);
	private:
		string dbInstance;
};

#endif

