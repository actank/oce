#ifndef  _PASSPORT_FETCHER_H_
#define  _PASSPORT_FETCHER_H_

#include "../common/Common.h"
#include "../common/UserModel.h"
#include "QueryRunner.h"

class PassportReadHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		PassportReadHandler(FILE* fp, int& maxuid, int& recnum) : 
			_fp(fp), _maxuid(maxuid), _recnum(recnum) {}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		FILE* _fp;
		int& _maxuid;
		int& _recnum;
};

class PassportFetcher
{
	public:
		PassportFetcher();
		virtual ~PassportFetcher() {}
		pair<int,int> Save2File(int step, int beginid, FILE* fp);

	private:
		string dbInstance;
};

#endif

