#ifndef _MODEL_FETCHER_H_
#define _MODEL_FETCHER_H_

#include "../common/Common.h"
#include "../common/StaticRank.h"
#include "../common/UserModel.h"

#define WORKERNUM 100

// not thread-safe
class ModelFetcher
{
	public:
		ModelFetcher();
		virtual ~ModelFetcher();
		// get models of uid and his friends
		int FetchModel(const USERID uid, char**& buff/*, size_t maxlen*/);

	private:
		void LoadFromIndexFile(const char* fname);
		void SaveIndexFile(const char* fname);
		void LoadFromDataFile();
		int PrintUserInfo(const USERID uid, size_t num, const char* endstr);

		int Put(USERID uid, map<USERID,StaticRank>& pvScore);
		int Get(USERID uid, map<USERID,StaticRank>& pvScore);
		int GetModelBuff(const USERID uid, char*& buff);

	private:
		hash_map<USERID,pair<char,long> > mModelPos;
		char* mModels[WORKERNUM];
};

#endif

