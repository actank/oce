#ifndef  _BASIC_FETCHER_H_
#define  _BASIC_FETCHER_H_

#include "../common/Common.h"
#include "../common/UserModel.h"
#include "QueryRunner.h"

class BasicReadHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		BasicReadHandler(map<USERID,UserModel>& models) : _models(models)	{}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		map<USERID,UserModel>& _models;
};

class BasicFetcher
{
	public:
		BasicFetcher();
		virtual ~BasicFetcher() {}
		int UpdateBatch(map<USERID,UserModel>& models, int low, int high);

	private:
		string dbInstance;
};

#endif

