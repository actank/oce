#include "MyTasks.h"
using namespace MyUtil;

FriendFetcher* friendFetcher = NULL;
SocialFeedBlockFetcher* socialFeedBlockFetcher = NULL;

void PrintTime();
char* time2str(const time_t ltime, char* str);
void SigRoutine(int dunno);
time_t Begin();
time_t Endup();

int CreateSecondLayer(map<USERID,set<USERID> >& secondLayer, const char* fname);
int CreateFirstLayer(map<USERID,vector<USERID> >& firstLayer,
		const map<USERID,set<USERID> >& secondLayer,
		const map<USERID,vector<USERID> >& userFriends);
int ReadFriendLists(const map<USERID,set<USERID> >& secondLayer,
		map<USERID,vector<USERID> >& userFriends);
int CreateSocialFeedBatch(const map<USERID,vector<USERID> >& firstLayer,
		const map<USERID,set<USERID> >& secondLayer);

int main(int argc, char* argv[])
{
	const char* fname = "/data/xce/Project/socialfeed/data/addfriend.recently";
	PrintTime();
	if (access(fname, R_OK) != 0) {
		fprintf(stderr, "Error: file %s not exist!\n", fname);
		return -1;
	}
	time_t start = Begin();
	time_t phrase_start=0, phrase_end=0;
	
	phrase_start = time(NULL);
	map<USERID,set<USERID> > secondLayer;
	fprintf(stderr, "\ncreate secondLayer ...\n");
	CreateSecondLayer(secondLayer, fname);
	phrase_end = time(NULL);
	fprintf(stderr, "sizeof secondLayer = %ld, using %.1fm\n",
			secondLayer.size(), (phrase_end-phrase_start)/60.0);

	phrase_start = time(NULL);
	map<USERID,vector<USERID> > userFriends;
	fprintf(stderr, "\nread friend lists ...\n");
	ReadFriendLists(secondLayer, userFriends);
	phrase_end = time(NULL);
	fprintf(stderr, "second layer's userFriends size=%ld, using %.1fm\n", 
			userFriends.size(), (phrase_end-phrase_start)/60.0);

	phrase_start = time(NULL);
	map<USERID,vector<USERID> > firstLayer;
	fprintf(stderr, "\ncreate firstLayer ...\n");
	CreateFirstLayer(firstLayer, secondLayer, userFriends);
	phrase_end = time(NULL);
	fprintf(stderr, "sizeof firstLayer = %ld, using %.1fm\n",
			firstLayer.size(), (phrase_end-phrase_start)/60.0);

	phrase_start = time(NULL);
	fprintf(stderr, "\ncreate social feed (add friends) ...\n");
	CreateSocialFeedBatch(firstLayer, secondLayer);
	phrase_end = time(NULL);
	fprintf(stderr, "%ld social feed completed, using %.1fm\n",
			firstLayer.size(), (phrase_end-phrase_start)/60.0);

	remove(fname);
	time_t end = Endup();
	printf("time used %ld second\n", end-start);
	return 0;
}

// SigRoutine 
void SigRoutine(int sig)
{
	switch (sig)
	{
		case 1:
			printf("Get a signal -- SIGHUP \n");
			break;
		case 2:
			printf("Get a signal -- SIGINT \n");
			break;
		case 3:
			printf("Get a signal -- SIGQUIT \n");
			break;
	}

	Endup();
	exit(0);
}

void PrintTime() 
{
	time_t now = time(NULL);
	char date_str[20];
	time2str(now, date_str);
	fprintf(stderr, "+------------------------------+\n");
	fprintf(stderr, "| %s          |\n", date_str);
	fprintf(stderr, "+------------------------------+\n");
}

char* time2str(const time_t ltime, char* str) 
{
	assert(str!=NULL);

	struct tm *Tm = localtime(&ltime);
	sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d",
			Tm->tm_year+1900,
			Tm->tm_mon+1,
			Tm->tm_mday,
			Tm->tm_hour,
			Tm->tm_min,
			Tm->tm_sec);
	return str;
}


time_t Begin() 
{
	signal(SIGHUP, SigRoutine); // 下面设置三个信号的处理方法
	signal(SIGQUIT, SigRoutine);
	signal(SIGINT, SigRoutine);
	//fprintf(stderr, "\033[?25l");  // 隐藏stderr光标
	friendFetcher = new FriendFetcher;
	socialFeedBlockFetcher = new SocialFeedBlockFetcher;

	return time(NULL);
}

time_t Endup() 
{
	fprintf(stderr, "\ncleaning jobs ... OK\n");
	delete friendFetcher;
	delete socialFeedBlockFetcher;
	//fprintf(stderr, "\033[?25h");  // 恢复stderr光标
	return time(NULL);
}

int CreateSocialFeedBatch(const map<USERID,vector<USERID> >& firstLayer, 
		const map<USERID,set<USERID> >& secondLayer)
{
	TaskManager::instance().config(TASK_LEVEL_CREATE_FEEDS, ThreadPoolConfig(0, 100));
	int task_size = 0;
	long wait = 0;
	long count = 0;

	for (map<USERID,vector<USERID> >::const_iterator firstIter = firstLayer.begin();
			firstIter != firstLayer.end(); ++firstIter)
	{
	//	if (firstIter->first % 10 != 2)
	//		continue;
		//if (firstIter->first != 239737004 &&
		//		firstIter->first != 238111132 &&
		//		firstIter->first != 27640369 &&
		//		firstIter->first != 285340707 &&
		//		firstIter->first != 237767538 &&
		//		firstIter->first != 19875 &&
		//		firstIter->first != 1002
		//	 )
		//	continue;

		if (++count % 5000 == 0)
			fprintf(stderr, "creating (%d), %ld / %ld\n",
					firstIter->first, count, firstLayer.size());

		TaskManager::instance().execute(new CreateSocailFeedTask(
					firstIter->first, firstIter->second, secondLayer));

		while ((task_size=TaskManager::instance().size(TASK_LEVEL_CREATE_FEEDS))>5000)
		{
			wait+=2;
			sleep(2);
			if (wait % 60 == 0)
				fprintf(stderr, "TASK_LEVEL_CREATE_FEEDS: sleep %ld second. left %d tasks.\n",
						wait, task_size);
		}
	}
	while (TaskManager::instance().size(TASK_LEVEL_CREATE_FEEDS)>0)
	{
		sleep(2);
	}
	return 0;
} 

int ReadFriendLists(const map<USERID,set<USERID> >& secondLayer, 
		map<USERID,vector<USERID> >& userFriends)
{
	TaskManager::instance().config(TASK_LEVEL_READ_FRIENDS, ThreadPoolConfig(0, 100));
	int task_size = 0;
	long wait = 0;
	long count = 0;

	for (map<USERID,set<USERID> >::const_iterator iter = secondLayer.begin();
			iter != secondLayer.end(); ++iter)
	{
		if (++count % 5000 == 0)
			fprintf(stderr, "%ld / %ld\r", count, secondLayer.size());

		pair<map<USERID,vector<USERID> >::iterator, bool> result =
			userFriends.insert(make_pair<USERID,vector<USERID> >(iter->first, vector<USERID>()));
		TaskManager::instance().execute(
				new ReadFriendTask(iter->first, result.first->second));

		while ((task_size=TaskManager::instance().size(TASK_LEVEL_READ_FRIENDS))>10000)
		{
			wait+=2;
			sleep(2);
			//fprintf(stderr, "sleep %ld second. left %d tasks.\r", wait, task_size);
		}
	}
	fprintf(stderr, "\nread friendlist completed.\n");

	while (TaskManager::instance().size(TASK_LEVEL_READ_FRIENDS)>0)
	{
		sleep(2);
	}
	return 0;
} 

int CreateSecondLayer(map<USERID,set<USERID> >& secondLayer, const char* fname) 
{
	FILE* fp = fopen(fname, "rt");  // addfriend.recently test
	assert(fp!=NULL);
	int uid = 0;
	int fid = 0;
	long line = 0;
	map<USERID,set<USERID> >::iterator iter;

	while (!feof(fp))
	{
		if (++line % 100000 == 0)
			fprintf(stderr, "%ld\r", line);
		fscanf(fp, "%d\t%d\n", &uid, &fid);
		//fprintf(stderr, "<%d><%d>\n", uid, fid);
		//-----------------------------

		iter = secondLayer.find(uid);
		if (iter == secondLayer.end())
		{
			pair<map<USERID,set<USERID> >::iterator, bool> result =
				secondLayer.insert(make_pair<USERID,set<USERID> >(uid, set<USERID>()));
			iter =result.first;
		}
		iter->second.insert(fid);
		//-----------------------------
		iter = secondLayer.find(fid);
		if (iter == secondLayer.end())
		{
			pair<map<USERID,set<USERID> >::iterator, bool> result =
				secondLayer.insert(make_pair<USERID,set<USERID> >(fid, set<USERID>()));
			iter =result.first;
		}
		iter->second.insert(uid);
	}
	fprintf(stderr, "\n");
	fclose(fp);
	return 0;
} 

int CreateFirstLayer(map<USERID,vector<USERID> >& firstLayer, 
		const map<USERID,set<USERID> >& secondLayer,
		const map<USERID,vector<USERID> >& userFriends)
{
	map<USERID,vector<USERID> >::iterator firstIter;
	map<USERID,vector<USERID> >::const_iterator friendIter;
	long line = 0;

	for (map<USERID,set<USERID> >::const_iterator iter = secondLayer.begin();
			iter != secondLayer.end(); ++iter)
	{
		if (++line % 5000 == 0)
			fprintf(stderr, "%ld / %ld\r", line, secondLayer.size());

		friendIter = userFriends.find(iter->first);
		assert(friendIter != userFriends.end());
		const vector<USERID>& friendlist = friendIter->second;
		for (vector<USERID>::const_iterator fidIter = friendlist.begin();
				fidIter != friendlist.end(); ++fidIter)
		{
			//fprintf(stderr, "%d,", *fidIter);
			firstIter = firstLayer.find(*fidIter);
			if (firstIter == firstLayer.end())
			{
				pair<map<USERID,vector<USERID> >::iterator, bool> result =
					firstLayer.insert(make_pair<USERID,vector<USERID> >(*fidIter, vector<USERID>()));
				firstIter =result.first;
			}
			firstIter->second.push_back(iter->first);
		}
		//getchar();
	}
	return 0;
} 

