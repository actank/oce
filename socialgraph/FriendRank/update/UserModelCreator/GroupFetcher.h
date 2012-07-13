#ifndef  _GROUP_FETCHER_H_
#define  _GROUP_FETCHER_H_

#include "../common/Common.h"
#include "../common/UserModel.h"
#include "QueryRunner.h"

class GroupReadHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		GroupReadHandler(map<USERID,UserModel>& models, set<int>& familyIds) : 
			_models(models), _familyIds(familyIds)	{}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		map<USERID,UserModel>& _models;
		set<int>& _familyIds;
};
class GroupNameHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		GroupNameHandler(set<int>& familyIds) : 
			_familyIds(familyIds)	{}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		set<int>& _familyIds;
};

class GroupFetcher
{
	public:
		GroupFetcher();
		virtual ~GroupFetcher() {}
		int UpdateBatch(map<USERID,UserModel>& models, int low, int high);
	private:
		int ReadGroupFamilyId(int begin, int end, set<int>& familyIds);
		string dbInstance;
};

#endif

