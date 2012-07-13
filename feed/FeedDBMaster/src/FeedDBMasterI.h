#ifndef FEED_DB_MASTER_H_
#define FEED_DB_MASTER_H_

#include <string>
#include <FeedDB.h>
#include <Singleton.h>

namespace xce{
namespace feed{

class FeedDBMasterI : public FeedDBMaster,
						public MyUtil::Singleton<FeedDBMasterI>{
public:
	BlockInfoPtr 	getBlock(int userid, bool write, const Ice::Current& = Ice::Current());
	BlockInfoPtr	newBlock(int userid, const Ice::Current& = Ice::Current());
	void		removeBlock(int userid, const Ice::Current& = Ice::Current());
	//void		putBatch(const FeedItem& feed, const MyUtil::IntSeq& userIds, const Ice::Current& = Ice::Current());
	void		putBatch(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, const Ice::Current& = Ice::Current());
	//void		putBatch(const Int2FeedItemMap& items, const Ice::Current& = Ice::Current());
	BlockInfoSeq	getBlockBatch(const MyUtil::IntSeq& userIds, bool write, const Ice::Current& = Ice::Current());
	BlockInfoSeq	newBlockBatch(const MyUtil::IntSeq& userIds, const Ice::Current& = Ice::Current());
	void		addNode(int serverid, const Ice::Current& = Ice::Current());
	BlockInfoPtr	moveBlock(const BlockInfoPtr& blkInfoOld, const Ice::Current& = Ice::Current());
	DataNodeMap 	queryNodeInfo(const Ice::Current& = Ice::Current());
	void		decTotalCount(const UserFeedCountSeq& feedCounts, const Ice::Current& = Ice::Current());
};

};
}
#endif
