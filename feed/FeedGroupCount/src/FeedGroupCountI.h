#ifndef _FEED_GROUP_COUNT_H_
#define _FEED_GROUP_COUNT_H_

#include "Singleton.h"
#include "RFeed.h"
#include "Evictor.h"
#include "Notify/util/fcgi_request.h"

using namespace std;
using namespace MyUtil;
using namespace xce::notify;

namespace xce {
namespace feed {

class FeedGroupCountI : public FeedGroupCount, public MyUtil::Singleton<FeedGroupCountI> {
public:
  FeedGroupCountI();
  MyUtil::Int2IntMap GetUnread(Ice::Int uid, const MyUtil::IntSeq & groups, const Ice::Current& = Ice::Current());
private:
	typedef MyUtil::TimedEvictor<Ice::Int, MyUtil::Int2IntMap> CountCache;
	CountCache cache_;
};

}
}

#endif // _FEED_GROUP_COUNT_H_

