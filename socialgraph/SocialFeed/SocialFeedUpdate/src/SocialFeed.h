#ifndef _SOCIAL_FEED_H_
#define _SOCIAL_FEED_H_

#include "Common.h"
#include "MD5Util.h"
#include <SocialFeedCacheAdapter.h>

struct FeedInnerComp {
	bool operator()(const pair<USERID,float>& p1, const pair<USERID,float>& p2)	{
		return p1.second > p2.second;
	}
};

class SocialFeed
{
	public:
		SocialFeed(const vector<USERID>& friends, const vector<USERID>& strangers,
				const map<int,float>& offerfriend_sort);
		char GetType() const;
		long Finger() const;
		int Transfer(xce::socialgraph::Feed& feed) const;
		int Print() const;
	public:
		vector<pair<USERID,float> > _friends;
		vector<pair<USERID,float> > _strangers;
};

struct SocialFeedComp {
	bool operator()(const SocialFeed& p1, const SocialFeed& p2) {
		char typep1 = p1.GetType();
		char typep2 = p2.GetType();
		if (typep1 != typep2)
			return typep1 > typep2;

		if (p1._friends.size() != p2._friends.size())
			return p1._friends.size() > p2._friends.size();
		
		float max_stranger_p1 = p1._strangers[0].second;
		float max_stranger_p2 = p2._strangers[0].second;
		return max_stranger_p1 > max_stranger_p2;
	}
};

#endif

