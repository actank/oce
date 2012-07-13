/*
 * =====================================================================================
 *
 *       Filename:  FeedData.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月22日 16时57分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "UserFeeds.h"

namespace xce {
namespace feed {

struct FeedMetaData {
	FeedMetaData() : userid(0), feedid(0), type(0), time(0), weight(0), mergeid(0) {}
	FeedMetaData(const FeedMeta& fm) : userid(fm.userid), feedid(fm.feedid), type(fm.type), time(fm.time)
   							,weight(fm.weight), mergeid(fm.mergeid) {}

	FeedMeta createFrom(int id) const {
		FeedMeta fm;
		fm.userid = userid;
		fm.feedid = feedid;
		fm.type = type;
		fm.time = time;
		fm.weight = weight;
		fm.mergeid = mergeid;
		return fm;
	}

	bool operator<(const FeedMetaData& other) const {
		int t = time - other.time;
		if(t == 0)
			return feedid < other.feedid;
		return t < 0;
	}

	bool operator==(const FeedMetaData& other) const {
		return 0 == memcmp(this, &other, sizeof(FeedMetaData));
	}

	::Ice::Int userid;
	::Ice::Long feedid;
	::Ice::Int type;
	::Ice::Int time;
	::Ice::Int weight;
	::Ice::Long mergeid;
};

struct FeedMetaCompare {
	inline bool operator()(const FeedMeta& a, const FeedMeta& b) {
		int d = a.time - b.time;
		if(d == 0)
			return a.feedid > b.feedid;
		return d > 0;
	}
};

}
}
