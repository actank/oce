/*
 * FeedContentReplySync.cpp
 *
 *  Created on: 2010-11-17
 *      Author: wu
 */

#include "FeedContentReplySync.h"

namespace xce{
namespace feed{


void ReplyDbUpdateTimer::handle() {
	FeedReplyBuffer::instance().updateDB();
}

//////////////////////FeedReplyBuffer//////////////////////////
void FeedReplyBuffer::changeReply(Ice::Long feed,
		const FeedReplyPtr& reply) {
	MCE_INFO("FeedReplyBuffer::changeReply: id : "<<feed);
	if(feed<0)
	{
		return;
	}
	IceUtil::Mutex::Lock lock(_mutex);
	_replyBuffer[feed] = reply;

}


void FeedReplyBuffer::updateDB() {
	MCE_INFO("FeedReplyBuffer::updateDB()");

	map<Ice::Long, FeedReplyPtr> buffer;
	_getReplyBuffer(buffer);

	if (buffer.empty()) {
		MCE_INFO("FeedReplyBuffer::updateDB() buffer is empty!!");
		return;
	}
	typedef map<Ice::Long, FeedReplyPtr> BufferMap;
	typedef BufferMap::iterator BufferMapIter;
	map<int, vector<BufferMapIter> > tmp;
	for (BufferMapIter iter = buffer.begin(); iter != buffer.end(); ++iter) {
		tmp[iter->first % 100].push_back(iter);
	}
	for (map<int, vector<BufferMapIter> >::iterator iter = tmp.begin(); iter
			!= tmp.end(); ++iter) {
		MCE_INFO("FeedReplyBuffer::updateDB() Into Loop");
		Statement sql;

		sql << " insert into " << ContentDB::getTableName(iter->first)
				<< " (feed,older_reply_id,older_reply,newer_reply_id,newer_reply, comment_count) values ";
		for (vector<BufferMapIter>::iterator h = iter->second.begin(); h
				!= iter->second.end(); ++h) {
			if (h != iter->second.begin()) {
				sql << " , ";
			}
			sql << " (" << (*h)->first << "," << (*h)->second->oldReplyId
					<< "," << mysqlpp::quote << (*h)->second->oldReply << ","
					<< (*h)->second->newReplyId << "," << mysqlpp::quote
					<< (*h)->second->newReply << "," << (*h)->second->count
					<< ") ";
		}

		sql<< " on duplicate key update older_reply_id=values(older_reply_id),older_reply=values(older_reply),newer_reply_id=values(newer_reply_id),newer_reply=values(newer_reply),comment_count=values(comment_count)";
		MCE_INFO("FeedReplyBuffer::updateDB() schedule sql");

		QueryRunner(ContentDB::getInstance(), CDbWServer, ContentDB::getExpr(
				iter->first)).schedule(sql);
	}

}


void FeedContentReplySync::replaceFeedReply(const MyUtil::LongSeq& feedIds,
		const FeedReplyPtr& reply) {

	for (size_t h = 0; h < feedIds.size(); ++h) {
		Ice::Long feed = feedIds[h];
		FeedReplyBuffer::instance().changeReply(feed, reply);
	}

}

}
}
