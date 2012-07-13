#include "ModelUpdater.h"

int CreateUserModel(long WORKER, long WORKERNUM, long STEP)
{
	MCE_INFO("Start worker ["<<WORKER<<"] WORKERNUM="<<WORKERNUM<<" STEP="<<STEP);
	time_t start = time(NULL);
	//---------------------------
	const char* fname = "/data/xce/friendrank/data/passport.list";
	assert(access(fname, R_OK)==0);

	ModelUpdater* ranker = new ModelUpdater;
	ranker->UpdateModel(WORKER, WORKERNUM, STEP);
	delete ranker;
	//---------------------------
	time_t end = time(NULL);
	MCE_INFO("Worker["<<WORKER<<"] Time used "<<end-start<<" second.");
	return 0;
}

