#include "ModelUpdater.h"

// Constructor & Destructor {{{
ModelUpdater::ModelUpdater() : BUFFLEN(5<<20)
{
	buffer = new char[BUFFLEN];

	mFriend = new FriendFetcher;
	mInviter = new InviteFetcher;
	mPlace = new PlacesFetcher;
	mBasic = new BasicFetcher;
	mHotFriend = new HotFriendFetcher;
	mGroup = new GroupFetcher;
	mGift = new GiftFetcher;
	mFeedConfig = new FeedConfigFetcher;

	srand(time(NULL));
}

ModelUpdater::~ModelUpdater()
{
	delete[] buffer;

	delete mFriend;
	delete mInviter;
	delete mPlace;
	delete mBasic;
	delete mHotFriend;
	delete mGroup;
	delete mGift;
	delete mFeedConfig;
}// }}}

// GetStepInterval {{{
int ModelUpdater::GetStepInterval(long WORKER, long WORKERNUM, long STEP,
		int stepno, map<USERID,UserModel>& models)
{
	long begin = ((stepno * WORKERNUM + WORKER) * STEP) * sizeof(int);
	char* fname = "/data/xce/friendrank/data/passport.list";
	long filesize = get_filesize(fname);
	if (begin>filesize)
		return -1;

	int* idlist = new int[STEP];
	FILE* fp = fopen(fname, "rb");
	assert(fp!=NULL);
	fseek(fp, begin, SEEK_SET);
	int idnum = fread(idlist, sizeof(int), STEP, fp);
	fclose(fp);

	for (int i=0; i<idnum; ++i)
	{
		models.insert(make_pair<USERID,UserModel>(idlist[i], UserModel(idlist[i])));
	}
	delete[] idlist;
	return 0;
}// }}}
// GetStepOverall {{{
int ModelUpdater::GetStepOverall(long WORKER, long WORKERNUM, long STEP,
		int stepno, map<USERID,UserModel>& models)
{
	char* fname = "/data/xce/friendrank/data/passport.list";
	
	long filesize = get_filesize(fname);
	assert(filesize % sizeof(int) == 0);
	long usersize = filesize / sizeof(int);
	long stepsize = (long)ceil((double)usersize / WORKERNUM);  // by user
	
	long worker_begin = (WORKER * stepsize) * sizeof(int);
	long worker_end = worker_begin + stepsize * sizeof(int);  // [worker_begin,worker_end)
	if (worker_end>filesize)
		worker_end = filesize;
	assert((worker_end-worker_begin) % sizeof(int) == 0);

	long begin = worker_begin + (stepno * STEP) * sizeof(int);
	if (begin>=worker_end)
		return -1;
	long end = begin + STEP * sizeof(int);
	if (end>=worker_end)
		end = worker_end;
	assert((end-begin) % sizeof(int) == 0);

	int* idlist = new int[STEP];
	FILE* fp = fopen(fname, "rb");
	assert(fp!=NULL);
	fseek(fp, begin, SEEK_SET);
	int idnum = fread(idlist, sizeof(int), /*STEP*/(end-begin)/sizeof(int), fp);
	fclose(fp);

	for (int i=0; i<idnum; ++i)
	{
		models.insert(make_pair<USERID,UserModel>(idlist[i], UserModel(idlist[i])));
	}
	delete[] idlist;
	return 0;
}// }}}

// UpdateModel 此处不是增量更新
int ModelUpdater::UpdateModel(long WORKER, long WORKERNUM, long STEP)
{
	char fname[128];
	sprintf(fname, "/data/xce/friendrank/data/user_models.dat_%ld", WORKER);
	FILE* fp = fopen(fname, "wb");
	if (fp==NULL)
	{
		MCE_ERROR("ModelUpdater::UpdateModel(): "<<strerror(errno)<<": " << fname);
		return -1;
	}
	
	long totalUser = 0;
	long increUser = 0;
	int stepno = 0;
	map<USERID,UserModel> models;

	int usetime[8];
	memset(usetime, 0, 8*sizeof(int));
	time_t time_start = 0, time_end = 0;

	while (true)
	{
		//GetStepInterval(WORKER, WORKERNUM, STEP, stepno++, models);
		GetStepOverall(WORKER, WORKERNUM, STEP, stepno++, models);
		
		if ((increUser=models.size())>0)
		{
			time_start = time(NULL);
			ReadUserInfo(models, usetime);
			SaveModels(models, fp);
			models.clear();
			time_end = time(NULL);

			totalUser += increUser;
			MCE_INFO("0:"<<usetime[0]<<" 1:"<<usetime[1]<<" 2:"<<usetime[2]<<" 3:"<<usetime[3]<<" 4:"<<usetime[4]<<" 5:"<<usetime[5]<<" 6:"<<usetime[6]<<" 7:"<<usetime[7]<<"  | ["<<WORKER<<"] "<<time_end-time_start<<" | "<<increUser<<" / "<<totalUser);
		}

		if (increUser<STEP)
		{
			MCE_INFO("Worker["<<WORKER<<"] read "<<totalUser<<" users totally. byebye.");
			break;
		}
	}
	fclose(fp);
	return 0;
}//

int ModelUpdater::ReadUserInfo(map<USERID,UserModel>& models, int* usetime)
{
	long begin = models.begin()->first;
	long end = (--models.end())->first;
	int r = rand() % 24;  // 4!=4*3*2*1
	switch (r)
	{
		// case 0~23 {{{
		case 0:
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			break;
		case 1:
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			break;
		case 2:
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			break;
		case 3:
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			break;
		case 4:
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			break;
		case 5:
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			break;
		case 6:
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			break;
		case 7:
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			break;
		case 8:
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			break;
		case 9:
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			break;
		case 10:
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			break;
		case 11:
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			break;
		case 12:
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			break;
		case 13:
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			break;
		case 14:
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			break;
		case 15:
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			break;
		case 16:
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			break;
		case 17:
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			break;
		case 18:
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			break;
		case 19:
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			break;
		case 20:
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			break;
		case 21:
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			break;
		case 22:
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			break;
		case 23:
			usetime[3] = mGift->UpdateBatch(models, begin, end);
			usetime[2] = mPlace->UpdateBatch(models, begin, end);
			usetime[1] = mGroup->UpdateBatch(models, begin, end);
			usetime[0] = mFriend->UpdateBatch(models, begin, end);
			break;
		default:
			fprintf(stderr, "Error random number\n");
			break;
			// }}}
	}

	//usetime[0] = mFriend->UpdateBatch(models, begin, end);
	//usetime[1] = mGroup->UpdateBatch(models, begin, end);
	//usetime[2] = mPlace->UpdateBatch(models, begin, end);
	//usetime[3] = mGift->UpdateBatch(models, begin, end);
	usetime[4] = mBasic->UpdateBatch(models, begin, end);
	usetime[5] = mInviter->UpdateBatch(models, begin, end);
	usetime[6] = mHotFriend->UpdateBatch(models, begin, end);
	usetime[7] = mFeedConfig->UpdateBatch(models, begin, end);
	return 0;
}

// SaveModels {{{
int ModelUpdater::SaveModels(const map<USERID,UserModel>& models, FILE* fp)
{
	UserModelHead head;
	for (map<USERID,UserModel>::const_iterator iter = models.begin();
			iter != models.end(); ++iter)
	{
		const UserModel& model = iter->second;
		head.uid = model.uid;
		head.len = model.ToBuffer(buffer);
		fwrite(&head, sizeof(UserModelHead), 1, fp);
		fwrite(buffer, head.len, 1, fp);

		//model.Print();  getchar();
		//UserModel temp(head.uid);
		//temp.FromBuffer(buffer, head.len);
		//temp.Print();  getchar();
	}
	return 0;
}// }}}

