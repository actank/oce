#include "ModelFetcher.h"

// Constructor & Destructor {{{
ModelFetcher::ModelFetcher()
{
	const char* idxfile = "/data/xce/friendrank/data/friend_rank/user_models.idx";
	if (access(idxfile, R_OK)==0)
	{
		MCE_INFO("Index exists, load from "<<idxfile);
		LoadFromIndexFile(idxfile);
	}
	else
	{
		MCE_INFO("Index doesn't exist, load from data file ...");
		LoadFromDataFile();
		MCE_INFO("Save to index file "<<idxfile);
		SaveIndexFile(idxfile);
	}

	// 打开文件，以备后续读取
	MCE_INFO("Cache data file ...");
	char fname[128];
	for (int i=0; i<WORKERNUM; ++i)
	{
		sprintf(fname, "/data/xce/friendrank/data/friend_rank/user_models.dat_%d", i);
		long filesize = get_filesize(fname);
		mModels[i] = new char[filesize];
		MCE_INFO("\tsize="<<filesize<<" : "<<fname);
		FILE* fp = fopen(fname, "rb");
		assert(fp!=NULL);
		fread(mModels[i], filesize, 1, fp);
		fclose(fp);
	}
}

ModelFetcher::~ModelFetcher()
{
	MCE_INFO("\trelease object ModelFetcher ... ");
	for (int i=0; i<WORKERNUM; ++i)
	{
		delete[] mModels[i];
		mModels[i] = NULL;
	}

	mModelPos.clear();
	MCE_INFO("\trelease object ModelFetcher ... OK");
}// }}}

// LoadFromDataFile {{{
void ModelFetcher::LoadFromDataFile()
{
	char* buffer = new char[5<<20];
	FILE* fpModel[WORKERNUM];

	char fname[128];
	for (int i=0; i<WORKERNUM; ++i)
	{
		sprintf(fname, "/data/xce/friendrank/data/friend_rank/user_models.dat_%d", i);
		MCE_INFO("\tfrom "<<fname);
		fpModel[i] = fopen(fname, "rb");
		assert(fpModel[i]!=NULL);

		UserModelHead head;
		long start;
		while (true)
		{
			start = ftell(fpModel[i]);
			if (fread(&head, sizeof(UserModelHead), 1, fpModel[i])!=1)
				break;
			if (fread(buffer, head.len, 1, fpModel[i])!=1)
				break;

			hash_map<USERID,pair<char,long> >::const_iterator iter = mModelPos.find(head.uid);
			if (iter != mModelPos.end())
			{
				MCE_ERROR("mModelPos has multiple key[" << head.uid << "] at " << fname);
			}
			mModelPos[head.uid] = make_pair<char,long>(i,start);

			//if (head.uid==239737004)
			//{
			//	UserModel model(head.uid);
			//	model.FromBuffer(buffer, head.len);
			//	model.Print();
			//	getchar();
			//}
		}
		//setvbuf(fpModel[i], 0, _IONBF, 0);
		fclose(fpModel[i]);
	}

	delete[] buffer;
	buffer = NULL;
}// }}}
// LoadFromIndexFile {{{
void ModelFetcher::LoadFromIndexFile(const char* fname)
{
	FILE* fp = fopen(fname, "rb");
	assert(fp!=NULL);
	int uid = 0;
	pair<char,long> pos;

	while (true)
	{
		if (fread(&uid, sizeof(int), 1, fp)!=1)
			break;
		if (fread(&pos, sizeof(pair<char,long>), 1, fp)!=1)
			break;
		mModelPos[uid] = pos;
	}
	fclose(fp);
}// }}}
// SaveIndexFile {{{
void ModelFetcher::SaveIndexFile(const char* fname)
{
	FILE* fp = fopen(fname, "wb");
	assert(fp!=NULL);
	int uid = 0;
	pair<char,long> pos;

	for (hash_map<USERID,pair<char,long> >::const_iterator iter = mModelPos.begin();
			iter != mModelPos.end(); ++iter)
	{
		uid = iter->first;
		pos = iter->second;
		fwrite(&uid, sizeof(int), 1, fp);
		fwrite(&pos, sizeof(pair<char,long>), 1, fp);
	}
	fclose(fp);
}// }}}

// FetchModel {{{
// 此处不是增量更新
int ModelFetcher::FetchModel(const USERID uid, char**& buff/*, size_t maxlen*/)
{
	//for (int i=0; i<2002; ++i)
	//	*(buff+i) = NULL;

	//int friendnum = 0;  // 实际好友数(有些好友信息不存在)
	//char* pp = buff+sizeof(int);  //  第一个int存储实际好友数

	//if (GetModelBuff(uid, pp+sizeof(int), *(int*)pp)<0)
	//	return -1;
	if (GetModelBuff(uid, buff[0])<0)
		return -1;
	
	//------------------------------------
	// 方案1：完全解析出'我'的模型，遍历好友列表，读取好友的模型Buff
	//UserModel meModel(uid);
	//meModel.FromBuffer(pp+sizeof(int), *(int*)pp);
	//pp += sizeof(int) + *(int*)pp;  // 跳过buff长度，以及我的buff占用长度!!!

	////fprintf(stderr, "GetModelBuff: uid=%d\t", uid);
	//for (set<int>::const_iterator iter = meModel.friends.begin();
	//		iter != meModel.friends.end(); ++iter)
	//{
	//	USERID fid = *iter;
	//	if (GetModelBuff(fid, pp+sizeof(int), *(int*)pp)<0)
	//		continue;

	//	//UserModel frModel(0);
	//	//frModel.FromBuffer(pp+sizeof(int), *(int*)pp);
	//	//StaticRank tempStaticRank;
	//	//meModel.ComputeRelevance(frModel, tempStaticRank);

	//	pp += sizeof(int) + *(int*)pp;
	//	++friendnum;
	//}
	
	//------------------------------------
	// 速度更快，方案2：部分解析出'我'的模型，遍历好友列表，读取好友的模型Buff
	//char* p = pp+sizeof(int);  // 第一个int存储buff长度，这里不需要
	//pp += sizeof(int) + *(int*)pp;  // 跳过buff长度，以及我的buff占用长度!!!
	//// 跳过basic info
	//p+=sizeof(int);  // 跳过uid
	//for (int i=0; i<5; ++i)  // 跳过basic info
	//{
	//	while (*p!='\0')		++p;
	//	++p;  // eat '\0'
	//}
	//
	//// friend list
	//int num = *(int*)p;  p+=sizeof(int);
	//USERID fid = 0;
	////cout << "\t" << num << endl;
	//for (int i=0; i<num; ++i)
	//{
	//	fid = *(int*)p;  p+=sizeof(int);
	//	if (GetModelBuff(fid, pp+sizeof(int), *(int*)pp)<0)
	//		continue;
	//	pp += sizeof(int) + *(int*)pp;
	//	++friendnum;
	//}

	char* p = buff[0] + sizeof(UserModelHead);
	p += 3*sizeof(int);  // 跳过uid, gender, birth_year

	// friend list
	int num = *(int*)p;  p+=sizeof(int);
	int realnum = 0;
	USERID fid = 0;
	for (int i=0; i<num && realnum<2000; ++i)
	{
		fid = *(int*)p;  p+=sizeof(int);
		//if (GetModelBuff(fid, pp+sizeof(int), *(int*)pp)<0)
		if (GetModelBuff(fid, buff[realnum+1])>=0)  // 第一个被'我'占了
			++realnum;
	}
	buff[realnum+1] = NULL;

	//------------------------------------
	//*(int*)buff = friendnum;  // friendnum!=meModel.friends.size();
	//if (pp-buff >maxlen)
	//{
	//  fprintf(stderr, "%d's len=%ld, maxlen=%ld\n", uid, pp-buff, maxlen);
	//}
	return 0;
}// }}}

// GetModelBuff {{{
int ModelFetcher::GetModelBuff(const USERID uid, char*& buff/*, char* buff, int& len*/)
{
	hash_map<USERID,pair<char,long> >::const_iterator iter = mModelPos.find(uid);
	if (iter == mModelPos.end())
	{
		//fprintf(stderr, "GetModelBuff: 文件中不存在%d\n", uid);
		//这种情况是可能的，例如
		//+-----------+-----------+------+---------------------+---------+
		//| host      | guest     | disc | time                | groupid |
		//+-----------+-----------+------+---------------------+---------+
		//| 220955294 | 244852932 |    0 | 2008-05-09 11:42:29 |       0 |
		//+-----------+-----------+------+---------------------+---------+
		return -2;
	}

	//char* p = mModels[uid%8] + iter->second;
	//UserModelHead* head = (UserModelHead*)p;
	//memcpy(buff, p+sizeof(UserModelHead), head->len);
	//len = head->len;
	buff = mModels[(int)(iter->second.first)] + iter->second.second;
	return 0;
}// }}}

// PrintUserInfo {{{
int ModelFetcher::PrintUserInfo(const USERID uid, size_t num, const char* endstr)
{
	//map<string,string> userinfo =
	//	UserCacheAdapter::instance().getUserCache(uid);
	//printf("<%s,%d> %ld friends%s", userinfo["name"].c_str(), uid, num, endstr);
	printf("<%d> %ld friends%s", uid, num, endstr);
	fflush(stdout);
	return 0;
}// }}}

