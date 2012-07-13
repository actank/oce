#ifndef FEED_DB_ADAPTER_H_
#define FEED_DB_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedDB.h"

namespace xce{
namespace feed{



using namespace MyUtil;

static inline BlockInfo64Ptr ToBlockInfo64(const BlockInfoPtr& blk){
  BlockInfo64Ptr blk64 = new BlockInfo64;
  blk64->serverId = blk->serverId;
  blk64->key = blk->userId;
  blk64->totalCount = blk->totalCount;
  blk64->curCount = blk->curCount;
  blk64->fileId = blk->fileId;
  blk64->blockIndex = blk->blockIndex;
  blk64->preFileId = blk->preFileId;
  blk64->preBlockIndex = blk->preBlockIndex;
  blk64->weight = blk->weight;
  return blk64;
}

static inline BlockInfo64Seq ToBlockInfo64Seq(const BlockInfoSeq& blks){
  BlockInfo64Seq seq;
  for(size_t i=0; i<blks.size(); i++){
    seq.push_back(ToBlockInfo64(blks.at(i)));
  }
  return seq;
}

static inline BlockInfoPtr ToBlockInfo(const BlockInfo64Ptr& blk64){
  BlockInfoPtr blk = new BlockInfo;
  blk->serverId = blk64->serverId;
  blk->userId = blk64->key&0xFFFFFFFF;
  blk->totalCount = blk64->totalCount;
  blk->curCount = blk64->curCount;
  blk->fileId = blk64->fileId;
  blk->blockIndex = blk64->blockIndex;
  blk->preFileId = blk64->preFileId;
  blk->preBlockIndex = blk64->preBlockIndex;
  blk->weight = blk64->weight;
  return blk;
}

static inline BlockInfoSeq ToBlockInfoSeq(const BlockInfo64Seq& blks64){
  BlockInfoSeq seq;
  for(size_t i=0; i<blks64.size(); i++){
    seq.push_back(ToBlockInfo(blks64.at(i)));
  }
  return seq;
}

static inline UserFeedCount64Ptr ToUserFeedCount64Ptr(const UserFeedCountPtr& uc){
  UserFeedCount64Ptr res = new UserFeedCount64;
  res->key = uc->userId;
  res->count = uc->count;
  return res;
}

static inline UserFeedCount64Seq ToUserFeedCount64Seq(const UserFeedCountSeq& ucs){
  UserFeedCount64Seq seq;
  for(size_t i=0; i<ucs.size(); i++){
    seq.push_back(ToUserFeedCount64Ptr(ucs.at(i)));
  }
  return seq;
}

class FeedDBDataAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedDBChannel, FeedDBDataAdapter> {
public:
	FeedDBDataAdapter(){
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void            put( const FeedItem& feed, const BlockInfo64Ptr& blkInfo);
	void            putBatch(const FeedItem& feed, const BlockInfo64Seq& blkInfoSeq);
	void            putBatchBinary(const FeedItem& feed, const BlockInfo64Seq& blkInfoSeq);
	//void            putBatch(const FeedItem& feed, const Int2IntMap& userWeight);
	FeedItemSeq     get(const BlockInfo64Ptr& blkInfo, int begin, int count);

	//FileInfo		queryFile(int fileId);
	bool		moveBlock(const BlockInfo64Ptr& blkInfoFrom, const BlockInfo64Ptr& blkInfoTo);
	void		removeFeed(const BlockInfo64Ptr& blkInfo, Ice::Long feedId);

	int				getCluster(){
		return (int)cluster();
	}

	BlockInfo64Ptr decodeData(const string& data);
	string	encodeData(const BlockInfo64Ptr& blkInfo);
	string	encodeDataSeq(const BlockInfo64Seq& seq);
	BlockInfo64Seq decodeDataSeq(const string& data);
protected:
	virtual string name() {
		return "FeedDBData";
	}
	virtual string endpoints() {
		return "@FeedDBData";
	}
	virtual size_t cluster() {
		return 5;
	}

	FeedDBDataPrx getManager(int id, int timeout=260);
	FeedDBDataPrx getManagerOneway(int id);

	vector<FeedDBDataPrx> _managersOneway;
	vector<FeedDBDataPrx> _managers;
};


class FeedDBMasterAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedDBChannel, FeedDBMasterAdapter> {
public:
	FeedDBMasterAdapter(){
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	BlockInfo64Ptr	getBlock(Ice::Long key, bool write=false);
	BlockInfo64Ptr	newBlock(Ice::Long key);
	void		removeBlock(Ice::Long key);
	void			putBatch(const FeedItem& feed, const Long2IntMap& userWeight);
	BlockInfo64Seq	getBlockBatch(const LongSeq& ids, bool write=false);
	BlockInfo64Seq	newBlockBatch(const LongSeq& ids);
	void		decTotalCount(const UserFeedCount64Seq& feedCounts);
protected:
	virtual string name() {
		return "FeedDBMaster";
	}
	virtual string endpoints() {
		return "@FeedDBMaster";
	}
	virtual size_t cluster() {
		return 0;
	}

	FeedDBMasterPrx getManager(int id, int timeout=260);
	FeedDBMasterPrx getManagerOneway(int id);

	vector<FeedDBMasterPrx> _managersOneway;
	vector<FeedDBMasterPrx> _managers;
};
class FeedDBAdapter : public Singleton<FeedDBAdapter>{
public:
	void			put(const FeedItem& feed, const Long2IntMap& userWeight);
	FeedItemSeq     get(Ice::Long key, int begin, int count);
	void		remove(Ice::Long key, Ice::Long feedId);
	void		removeAll(Ice::Long key); 
};

};
};


#endif /*RESTMEDIATORADAPTER_H_*/
