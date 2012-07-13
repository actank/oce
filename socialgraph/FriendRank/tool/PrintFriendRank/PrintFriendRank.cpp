#include <string>
#include <FriendRankCacheAdapter.h>
#include "QueryRunner.h"
#include <BuddyByIdReplicaAdapter.h>
#include <UserBasicAdapter.h>
using namespace xce::adapter::userbasic;
using namespace xce::buddy::adapter;
using namespace xce::socialgraph;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace std;

typedef int USERID;

string GetUserName(int uid);
void GetFriend(int uid);
void GetRank(int uid);
void GetRevRank(int uid);
void GetRankFromDB(int uid);
void GetRankFromDBTest();

struct RankCompByScore
{
	bool operator()(const RankData& p1, const RankData& p2) {
		return p1.weight > p2.weight;
	}
};

int main(int argc, char* argv[])
{
	//ConnectionPoolManager::instance().initialize();
	//for (int i=0; i<3; ++i)
	//{
	//	GetRankFromDBTest();
	//}
	//return 0;
	if (argc!=2)
	{
		fprintf(stderr, "To run, like this: %s <userid>\n", argv[0]);
		return -1;
	}
	int uid = atoi(argv[1]);
	printf(">>>>>>>>>>>>>> %s_%d <<<<<<<<<<<<<<<\n", GetUserName(uid).c_str(), uid);

	GetRankFromDB(uid);
	GetRank(uid);
	GetRevRank(uid);
	GetFriend(uid);
	return 0;
}

// GetRank {{{
void GetRank(int uid)
{
	RankSeq vecRank = FriendRankCacheAdapter::instance().GetRank(uid);

	sort(vecRank.begin(), vecRank.end(), RankCompByScore());
	printf("\033[31m\nGetRank(%d), 我关注的人数 [%ld个]\n\033[0m", uid, vecRank.size());
	for (RankSeq::const_iterator iter = vecRank.begin();
			iter != vecRank.end(); ++iter)
	{
		printf("<%s_%d,%d> ", GetUserName(iter->userid).c_str(), iter->userid, iter->weight);
	}
	printf("\n");
}// }}}

// GetRevRank {{{
void GetRevRank(int uid)
{
	map<int,int> mapRank = FriendRankCacheAdapter::instance().GetRevRank(uid);
	RankSeq vecRank;
	RankData elem;
	for (map<int,int>::const_iterator iter = mapRank.begin();
			iter != mapRank.end(); ++iter)
	{
		elem.userid = iter->first;
		elem.weight = iter->second;
		vecRank.push_back(elem);
	}

	sort(vecRank.begin(), vecRank.end(), RankCompByScore());
	printf("\033[31m\nGetRevRank(%d), 关注我的人数 [%ld个]\n\033[0m", uid, vecRank.size());
	for (RankSeq::const_iterator iter = vecRank.begin();
			iter != vecRank.end(); ++iter)
	{
		printf("<%s_%d,%d> ", GetUserName(iter->userid).c_str(), iter->userid, iter->weight);
	}
	printf("\n");
}// }}}

// GetFriend {{{
void GetFriend(int uid)
{
	vector<int> userFriend = 
		BuddyByIdCacheAdapter::instance().getFriendListAsc(uid,2000);

	printf("\033[31m\n%d的好友数 [%ld个]\n\033[0m", uid, userFriend.size());
	for (vector<int>::const_iterator iter = userFriend.begin();
			iter != userFriend.end(); ++iter)
	{
		printf("%s_%d ", GetUserName(*iter).c_str(), *iter);
		//printf("%d ", *iter);
	}
	cout << endl;
}// }}}

// GetUserName {{{
string GetUserName(int uid)
{
	string name = "";
	try {
		UserBasicDataPtr basicPtr = UserBasicAdapter::instance().getUserBasic(uid);
		name = basicPtr->name;
	}catch( Ice::Exception e){
		cout << "\nIce-> " << e.what() << ": GetUserName(" << uid << ")" << endl;
	}catch( std::exception e){
		cout << "\nstd-> " << e.what() << ": GetUserName(" << uid << ")" << endl;
	}catch( ... ){
		cout << "\nunknown exception" << ": GetUserName(" << uid << ")" << endl;
	}

	return name;
}// }}}

// class FriendRankHandler {{{
class FriendRankHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		FriendRankHandler(vector<RankData>& rank):
			_rank(rank) {}
		virtual bool handle(mysqlpp::Row& row) const
		{
			//USERID uid = (int)row["user_id"];
			const int num = row["rank"].length() / sizeof(int);
			const char* p = row["rank"].c_str();
			RankData elem;
			for (int i=0; i<num; i+=2)
			{
				elem.userid = *(int*)p;  p += sizeof(int);
				elem.weight = *(int*)p;  p += sizeof(int);
				//MCE_INFO(userid<<weight);
				_rank.push_back(elem);
			}
			return true;
		}
	private:
		vector<RankData>& _rank;
};// }}}
// GetRankFromDB {{{
void GetRankFromDB(int uid)
{
	ConnectionPoolManager::instance().initialize();
	// 拼凑sql
	Statement sql;
	sql
		<< "SELECT user_id,rank FROM friend_rank"
		<< " WHERE user_id=" << uid;

	// 查数据库
	vector<RankData> rank;
	try {
		QueryRunner("friend_rank", CDbRServer).query(sql, FriendRankHandler(rank));
	} catch (Ice::Exception& e) {
		cout << time(NULL) << " [main]DB Connection Exception : " << e.what() << "\n";
	}

	// 打印结果
	printf("\033[31m\nGetRankFromDB(%d), 我关注的人数%ld :\n\033[0m", uid, rank.size());
	for (vector<RankData>::const_iterator iter = rank.begin();
			iter != rank.end(); ++iter)
	{
		printf("<%s_%d,%d> ", GetUserName(iter->userid).c_str(), iter->userid, iter->weight);
	}
	printf("\n");
}// }}}


// class FriendRankBatchHandler {{{
class FriendRankBatchHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		FriendRankBatchHandler(map<int,vector<RankData> >& ranks):
			_ranks(ranks) {}
		virtual bool handle(mysqlpp::Row& row) const
		{
			USERID uid = row["user_id"];
			const int num = row["rank"].length() / sizeof(int);
			const char* p = row["rank"].c_str();

			vector<RankData> rank;
			RankData elem;
			for (int i=0; i<num; i+=2)
			{
				elem.userid = *(int*)p;  p += sizeof(int);
				elem.weight = *(int*)p;  p += sizeof(int);
				rank.push_back(elem);
			}

			_ranks.insert(make_pair<int,vector<RankData> >(uid,rank));
			return false;
		}
	private:
		map<int,vector<RankData> >& _ranks;
};// }}}
// GetRankFromDBBatch {{{
void GetRankFromDBTest()
{
	// 拼凑sql
	Statement sql;
	sql
		<< "SELECT user_id,rank FROM friend_rank LIMIT 10";

	// 查数据库
	map<int,vector<RankData> > ranks;
	try {
		QueryRunner("friend_rank", CDbRServer).query(sql, FriendRankBatchHandler(ranks));
	} catch (Ice::Exception& e) {
		cout << time(NULL) << " [main]DB Connection Exception : " << e.what() << "\n";
	}

	// 打印结果
	for (map<int,vector<RankData> >::iterator allIter = ranks.begin();
			allIter != ranks.end(); ++allIter)
	{
		printf("\033[31m\nGetRankFromDB(%d), 我关注的人数%ld :\n\033[0m", 
				allIter->first, allIter->second.size());
		for (vector<RankData>::const_iterator iter = allIter->second.begin();
				iter != allIter->second.end(); ++iter)
		{
			printf("<%s_%d,%d> ", GetUserName(iter->userid).c_str(), iter->userid, iter->weight);
		}
		printf("\n");
	}
}// }}}

