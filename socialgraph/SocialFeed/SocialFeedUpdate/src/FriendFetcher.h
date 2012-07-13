#ifndef  _FRIEND_FETCHER_H_
#define  _FRIEND_FETCHER_H_

#include "Common.h"
#include "QueryRunner.h"

class FriendReadHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		FriendReadHandler(vector<USERID>& friends) : _friends(friends)	{}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		vector<USERID>& _friends;
};

class FriendFetcher
{
	public:
		FriendFetcher();
		virtual ~FriendFetcher() {}
		int Read(int uid, vector<USERID>& friends);
	private:
		string dbInstance;
};

#endif

