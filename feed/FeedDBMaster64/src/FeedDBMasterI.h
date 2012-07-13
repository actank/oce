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


  BlockInfo64Ptr 	getBlock64(Ice::Long key, bool write, const Ice::Current& = Ice::Current());
	BlockInfo64Ptr	newBlock64(Ice::Long key, const Ice::Current& = Ice::Current());
	void		        removeBlock64(Ice::Long key, const Ice::Current& = Ice::Current());
	void		        putBatch64(const FeedItem& feed, const MyUtil::Long2IntMap& usersWeight, const Ice::Current& = Ice::Current());
	BlockInfo64Seq	getBlockBatch64(const MyUtil::LongSeq& keys, bool write, const Ice::Current& = Ice::Current());
	BlockInfo64Seq	newBlockBatch64(const MyUtil::LongSeq& keys, const Ice::Current& = Ice::Current());
	void		        addNode64(int serverid, const Ice::Current& = Ice::Current());
	BlockInfo64Ptr	moveBlock64(const BlockInfo64Ptr& blkInfoOld, const Ice::Current& = Ice::Current());
	DataNodeMap 	  queryNodeInfo64(const Ice::Current& = Ice::Current());
	void		        decTotalCount64(const UserFeedCount64Seq& feedCounts, const Ice::Current& = Ice::Current());
};

};
}
#endif
