#ifndef FEED_CONTENT_REPLY_SYNC_H
#define FEED_CONTENT_REPLY_SYNC_H


#include "Singleton.h"

#include "QueryRunner.h"
#include "RFeed.h"

#include <Ice/Ice.h>
#include<string>

using namespace com::xiaonei::xce;

namespace xce {
namespace feed {

using namespace std;

class ContentDB {
public:
	ContentDB() {
	}
	static const string getTableName(Ice::Long id) {
		ostringstream tableName;
		tableName << "feed_content_" << id % 100;
		//tableName << "feed_content";
		return tableName.str();
	}
	static const string getInstance() {
		return "feed_db";
		//return "feed_exp";
	}
	static const string getExpr(Ice::Long id) {
		ostringstream expr;
		expr << "feed_content_" << id % 100;
		//expr << "feed_content";
		return expr.str();
	}

};


class FeedContentReplySync : public MyUtil::Singleton<FeedContentReplySync> {
public:

	void replaceFeedReply(const MyUtil::LongSeq& feedIds,const FeedReplyPtr& reply);
};

class FeedReplyBuffer: public MyUtil::Singleton<FeedReplyBuffer> {
public:
	void changeReply(Ice::Long feedid, const FeedReplyPtr& reply);
	void updateDB();
private:
	map<Ice::Long, FeedReplyPtr> _replyBuffer;
	IceUtil::Mutex _mutex;
	void _getReplyBuffer(map<Ice::Long, FeedReplyPtr>& tmp) {
		IceUtil::Mutex::Lock lock(_mutex);
		_replyBuffer.swap(tmp);
	}
};

class ReplyDbUpdateTimer: public MyUtil::Timer {
public:
	ReplyDbUpdateTimer() :
		MyUtil::Timer(2000) {
	}
	virtual void handle();
};


}
}

#endif
