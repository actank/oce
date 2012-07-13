/**
 * @brief 通过 actor,source,stype获得最老的回复和最新的回复
 * 通过stype和actor确定表的名称，表是按照actor来散的，
 * 在表中使用actor和source作为key获取最老的回复和最新的回复。
 * 回复的先后顺序通过回复的id进行反应。
 */

#ifndef _FEEDREPLYI_H_
#define _FEEDREPLYI_H_

#include "RFeed.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "QueryRunner.h"
//#include "FeedReply.h"
#include <ext/hash_map>
#include <sstream>


namespace xce {
namespace feed {

using namespace std;
using namespace com::xiaonei::xce;

//const std::string REPLY_GETER = "RG";
const std::string REPLY_MANAGER = "RM";
const std::string DB_TABLE = "reply_table_desc";
const std::string DB_INSTANCE = "feed_test";

typedef struct ReplyTextPattern {
	string db;
	string expression;
	string table;
	int tableCount;
	string actorColumn;
	string sourceColumn;
	string replyIdColumn;
	string replyColumn;
} ReplyTextPattern;

class TextPatternResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	TextPatternResultHandlerI(hash_map<int, ReplyTextPattern>& props):_props(props){
	}
	~TextPatternResultHandlerI() {
	}
	virtual bool handle(mysqlpp::Row& row) const;
private:

	hash_map<int, ReplyTextPattern>& _props;
};

class ReplyResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	ReplyResultHandlerI(FeedReplySeq& reply, string& replyIdColumn,
			string& replyColumn) :
		_reply(reply), _replyIdColumn(replyIdColumn), _replyColumn(replyColumn) {
	}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	FeedReplySeq& _reply;
	string _replyIdColumn;
	string _replyColumn;
};

class FeedReplyManagerI: public FeedReplyManager,
						 public MyUtil::Singleton<FeedReplyManagerI> {
public:
	FeedReplyManagerI() {
		_load();
	}
	virtual FeedReplyPtr getOldFeedReply(Ice::Int actor, Ice::Long source,
			Ice::Int stype, const Ice::Current& = Ice::Current());
	virtual FeedReplyPtr getNewFeedReply(Ice::Int actor, Ice::Long source,
			Ice::Int stype, const Ice::Current& = Ice::Current());
	virtual FeedReplySeq getFeedReplyPair(Ice::Int actor, Ice::Long source,
			Ice::Int stype, const Ice::Current& = Ice::Current());
	virtual void reload(const Ice::Current& = Ice::Current());
	void swapMap(hash_map<int, ReplyTextPattern>& newMap);
private:
	void _load();
	string _getTableName(string tableName, Ice::Int actor, int tableCount);
	string _getExpression( string expression, Ice::Int actor, int tableCount );
	FeedReplyPtr _getFeedReply(Ice::Int actor, Ice::Long source,
			Ice::Int stype, int newold);

//	IceUtil::Mutex _mutex;
	hash_map<int, ReplyTextPattern> _replyTableTextPattern;
};

}
;
}
;

#endif /* _FEEDREPLY_H_ */
