#include "FeedReplyI.h"

using namespace std;
using namespace xce::feed;
using namespace com::xiaonei::xce;
using namespace MyUtil;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&FeedReplyManagerI::instance(),
			service.createIdentity(REPLY_MANAGER, ""));
}

FeedReplyPtr FeedReplyManagerI::getOldFeedReply(Ice::Int actor,
		Ice::Long source, Ice::Int stype, const Ice::Current&) {
	MCE_DEBUG("FeedReplyManagerI::getOldFeedReply");
	return _getFeedReply(actor, source, stype, 0);
}

FeedReplyPtr FeedReplyManagerI::getNewFeedReply(Ice::Int actor,
		Ice::Long source, Ice::Int stype, const Ice::Current&) {
	return _getFeedReply(actor, source, stype, 1);
}

string FeedReplyManagerI::_getTableName(string table, Ice::Int actor,
		int tableCount) {
	ostringstream tableName;
	tableName << table << actor % tableCount;
	return tableName.str();
}


string FeedReplyManagerI::_getExpression(
	string expression, Ice::Int actor, int tableCount ){
	ostringstream exp;
	exp << expression << actor % tableCount;
	return exp.str();
}

FeedReplySeq FeedReplyManagerI::getFeedReplyPair(Ice::Int actor,
		Ice::Long source, Ice::Int stype, const Ice::Current&) {
	FeedReplySeq reply;
	hash_map<int, ReplyTextPattern>::iterator iter;
	iter = _replyTableTextPattern.find(stype);
	if (iter == _replyTableTextPattern.end()) {
		MCE_WARN("FeedReplyManagerI::_getFeedReply ---> can not find this stype:"<<stype);
		return reply;
	}
	ReplyTextPattern& pattern = iter->second;
	string tableName = _getTableName(pattern.table, actor, pattern.tableCount);
	Statement sql;
	sql<<"(SELECT "<< pattern.replyIdColumn << "," << pattern.replyColumn
	   <<" FROM "<< tableName
	   <<" WHERE "<< pattern.actorColumn << "=" << actor
	   <<" AND "<< pattern.sourceColumn << "=" << source
	   <<" order by "<< pattern.replyIdColumn <<" limit 1)"
	   <<" UNION "
	   <<"(SELECT "<< pattern.replyIdColumn << "," << pattern.replyColumn
	   <<" FROM "<< tableName
	   <<" WHERE "<< pattern.actorColumn << "=" << actor
	   <<" AND "<< pattern.sourceColumn << "=" << source
	   <<" order by "<< pattern.replyIdColumn <<" desc limit 1);";
	ReplyResultHandlerI handler(reply, pattern.replyIdColumn,
			pattern.replyColumn);
	QueryRunner(pattern.db, CDbRServer,
				_getExpression(pattern.expression,actor,pattern.tableCount)).query(sql, handler);
	return reply;
}

void FeedReplyManagerI::reload(const Ice::Current&)
{
	_load();
}

void FeedReplyManagerI::_load() {
	hash_map<int, ReplyTextPattern> replyTextPatternMap;
	TextPatternResultHandlerI handler(replyTextPatternMap);
	Statement sql;
	sql << "SELECT * FROM " << DB_TABLE << ";";
	QueryRunner(DB_INSTANCE, CDbRServer,DB_TABLE).query(sql, handler);
	swapMap(replyTextPatternMap);
}

FeedReplyPtr FeedReplyManagerI::_getFeedReply(Ice::Int actor, Ice::Long source,
		Ice::Int stype, int oldnew) {
	MCE_DEBUG("FeedReplyManagerI::_getFeedReply");
	hash_map<int, ReplyTextPattern>::iterator iter;
	iter = _replyTableTextPattern.find(stype);
	if (iter == _replyTableTextPattern.end()) {
		MCE_WARN("FeedReplyManagerI::_getFeedReply ---> can not find this stype:"<<stype);
		return NULL;
	}
	ReplyTextPattern& pattern = iter->second;
	Statement sql;
	string tableName = _getTableName(pattern.table, actor, pattern.tableCount);
	sql<<"SELECT "<<pattern.replyIdColumn<<","<<pattern.replyColumn
	   <<" FROM "<< tableName
	   <<" WHERE "<<pattern.replyIdColumn <<" = "
	   <<" ( ";
	if (oldnew == 0) {//old
		sql << "SELECT MIN("<<pattern.replyIdColumn<<")";
	} else {
		sql << "SELECT MAX("<<pattern.replyIdColumn<<")";
	}
	sql<<" FROM "<< tableName
	   << " WHERE "<< pattern.actorColumn << "=" << actor << " AND "
	   << pattern.sourceColumn << "=" << source
	   <<" ) ;";
	//sql<<"SELECT id,body FROM album_comment_82 WHERE owner=61782 AND album=254733276 order by id limit 1) UNION (SELECT id,body FROM album_comment_82 WHERE owner=61782 AND album=254733276 order by id desc limit 2";
	FeedReplySeq reply;
	ReplyResultHandlerI handler(reply, pattern.replyIdColumn,
			pattern.replyColumn);
	QueryRunner(pattern.db, CDbRServer,
				_getExpression(pattern.expression,actor,pattern.tableCount)).query(sql, handler);
	if (reply.size() == 1) {
		return reply.at(0);
	} else {
		return NULL;
	}
}

void FeedReplyManagerI::swapMap(hash_map<int, ReplyTextPattern>& newMap) {
//	IceUtil::Mutex::Lock lock(_mutex);
	_replyTableTextPattern.swap(newMap);
}

bool TextPatternResultHandlerI::handle(mysqlpp::Row& row) const {

	ReplyTextPattern pattern;
	pattern.db = row["db"].data();
	pattern.table = row["table"].data();
	pattern.tableCount = row["table_count"];
	pattern.actorColumn = row["actor_column"].data();
	pattern.sourceColumn = row["source_column"].data();
	pattern.replyIdColumn = row["replyid_column"].data();
	pattern.replyColumn = row["reply_column"].data();
	pattern.expression = pattern.db+"_";
	int stype = row["small_type"];
	_props[stype] = pattern;
	return true;
}


bool ReplyResultHandlerI::handle(mysqlpp::Row& row) const {
	FeedReplyPtr preply = new FeedReply();
	preply->id = row[_replyIdColumn.c_str()];
	preply->reply = row[_replyColumn.c_str()].data();
	_reply.push_back(preply);
	MCE_DEBUG("get reply from db: replyid="<<preply->id<<" reply="<<preply->reply);
	return true;
}
