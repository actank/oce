#ifndef DATANODE_MANAGER_H_
#define DATANODE_MANAGER_H_

#include <vector>
#include <tchdb.h>
#include <Singleton.h>
#include <FeedDB.h>
#include <IceLogger.h>
#include "TaskManager.h"
#include "PerformanceStat.h"

namespace xce{
namespace feed{

typedef int NodeId;
typedef int BlockDBKey;

struct BlockData{
	int serverId;
	int curCount;
	int totalCount;
	int fileId;
	int blockIndex;
};
using namespace std;
//////////////////////////
class HashDB{
public:
	static const int BUCKET_NUM = 10000000;
	static const long long MAPPED_MEM = 4*1024*1024*(long long)1024;


	bool	init(const char* filename);
	bool	put(BlockDBKey key, const BlockData& blk);
	bool	get(BlockDBKey key, BlockData& blk);
	void	remove(BlockDBKey key);
	map<int, DataNode>	loadDataNode();
	bool			flushDataNode(const map<int, DataNode>& nodes);
private:
	TCHDB* _db;
};

class DataNodeManager : public MyUtil::Singleton<DataNodeManager>{
public:
	static const int MUTEX_COUNT = 10000000;
	static const int ALLOC_COUNT = 10000;
	static const int MAX_DATANODE = 100;
	static const int RECORD_COUNT_PER_BLOCK = BLOCKSIZE/sizeof(FeedItem) - 1;

	bool			init(const char* filename);
	void			addDataNode(int serverid);
	//void 			putBatch(const FeedItem& feed, const MyUtil::IntSeq& userIds);
	void 			putBatch(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight);
	BlockInfoPtr		get(BlockDBKey key, bool write=false);
	BlockInfoPtr		newBlock(BlockDBKey key);
	void			remove(BlockDBKey key);
	BlockInfoPtr 		moveBlock(const BlockInfoPtr& blkInfoOld);
	void			decTotalCount(const UserFeedCountSeq& feedCounts);
	map<int, DataNode> getNodeInfo(){
		IceUtil::Mutex::Lock lock(_nodeMutex);
		return _dataNodes;
	}
protected:
	bool			allocFreeBlock(BlockDBKey key, BlockData& blk);
	bool			allocFreeBlockInNode(BlockData& blk, int nodeId);
	void			allocFreeBlockInNodeNoLock(BlockData& blk, int nodeId);

	int 			findDataNode();
	BlockInfoPtr 	BlockDataToBlockInfo(const BlockData& blk);
	BlockData	BlockInfoToBlockData(const BlockInfoPtr& blk);
private:
	HashDB				_hashDB;

	map<int, DataNode> 	_dataNodes;
	int					_curNode;
	int					_curCount;

	IceUtil::Mutex 		_nodeMutex;
	IceUtil::Mutex		_blockMutexs[MUTEX_COUNT];
};

class RetryTimer : public MyUtil::Timer, public MyUtil::Singleton<RetryTimer>{
public:
	RetryTimer():Timer(10*1000){}
	void push(const FeedItem& feed, BlockInfoSeq& seq){
		IceUtil::Mutex::Lock lock(mutex_);
		if(feeds_.size() > 500000){
			return;
		}
		feeds_.push_back(make_pair(feed, BlockInfoSeq()));
		feeds_.back().second.swap(seq);
	}
	void handle();
private:
	list<pair<FeedItem, BlockInfoSeq> > feeds_;
	IceUtil::Mutex mutex_;
};

class PutTask : public MyUtil::Task{
public:
	PutTask(const FeedItem& feed, MyUtil::Int2IntMap& usersWeight){
		_feed = feed;
		_usersWeight.swap(usersWeight);
	}
	void handle();
public:
	FeedItem 		_feed;
	MyUtil::Int2IntMap 	_usersWeight;
};


};
}

#endif
