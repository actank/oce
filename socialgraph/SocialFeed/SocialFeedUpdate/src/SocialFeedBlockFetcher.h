#ifndef  _SOCIALFEED_BLOCK_FETCHER_H_
#define  _SOCIALFEED_BLOCK_FETCHER_H_

#include "Common.h"
#include "QueryRunner.h"

class SocialFeedBlockHandler : public com::xiaonei::xce::ResultHandler
{
	public:
		SocialFeedBlockHandler(set<long>& feedfps) : _feedfps(feedfps)	{}
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		set<long>& _feedfps;
};

class SocialFeedBlockFetcher
{
	public:
		SocialFeedBlockFetcher();
		virtual ~SocialFeedBlockFetcher() {}
		int Read(int uid, set<long>& feedfps);
	private:
		string dbInstance;
};

#endif

