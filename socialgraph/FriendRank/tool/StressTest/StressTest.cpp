#include <FriendRankCacheAdapter.h>
#include "QueryRunner.h"
#include <BuddyByIdReplicaAdapter.h>
using namespace xce::buddy::adapter;
using namespace xce::socialgraph;
using namespace com::xiaonei::xce;
using namespace MyUtil;

typedef int USERID;

void GetRank(int uid);
void GetRevRank(int uid);
void GetRankFromDB(int uid);
void GetFriend(int uid);

int main(int argc, char* argv[])
{
	if (argc!=2)
	{
		fprintf(stderr, "To run, like this: %s <userid>\n", argv[0]);
		return -1;
	}
	int uid = atoi(argv[1]);
	printf(">>>>>>>>>>>>>> %d <<<<<<<<<<<<<<<\n", uid);

	//GetRank(uid);
	GetRevRank(uid);
	//GetRankFromDB(uid);
	//GetFriend(uid);

	return 0;
}

void GetRank(int uid)
{
	vector<RankData> rank = FriendRankCacheAdapter::instance().GetRank(uid); 
	//map<int,int> rank = FriendRankCacheAdapter::instance().GetRank(uid);
	printf("\033[31m\nGetRank(%d), 我关注的人数 [%ld个]\n\033[0m", uid, rank.size());
	for (vector<RankData>::iterator iter = rank.begin();
			iter != rank.end(); ++iter) {
		printf("<%d,%d> ", iter->userid, iter->weight);
	}
	printf("\n");
}
void GetRevRank(int uid)
{
	map<int,int> rank = FriendRankCacheAdapter::instance().GetRevRank(uid);
	printf("\033[31m\nGetRevRank(%d), 关注我的人数 [%ld个]\n\033[0m", uid, rank.size());
	for (map<int,int>::const_iterator iter = rank.begin();
			iter != rank.end(); ++iter)
	{
		printf("<%d,%d> ", iter->first, iter->second);
	}
	printf("\n");
}

void GetFriend(int uid)
{
	vector<int> userFriend = 
		BuddyByIdCacheAdapter::instance().getFriendListAsc(uid,2000);

	printf("\033[31m\n%d的好友数 [%ld个]\n\033[0m", uid, userFriend.size());
	for (vector<int>::const_iterator iter = userFriend.begin();
			iter != userFriend.end(); ++iter)
	{
		cout << *iter << " ";
	}
	cout << endl;
}

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
};

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
		cout << "<" << iter->userid << "," << iter->weight << "> ";
	}
	printf("\n");
}

