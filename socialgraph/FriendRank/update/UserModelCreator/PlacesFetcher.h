#ifndef  _PLACE_FETCHER_H_
#define  _PLACE_FETCHER_H_

#include "../common/Common.h"
#include "../common/UserModel.h"
#include "QueryRunner.h"

class PlacesReadHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		PlacesReadHandler(map<USERID,UserModel>& models) : _models(models)	{}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		map<USERID,UserModel>& _models;
};

class PlacesFetcher
{
	public:
		PlacesFetcher();
		virtual ~PlacesFetcher() {}
		int UpdateBatch(map<USERID,UserModel>& models, int low, int high);
	private:
		string dbInstance;
};

#endif

