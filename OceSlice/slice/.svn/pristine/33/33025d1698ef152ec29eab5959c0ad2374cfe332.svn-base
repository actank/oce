#ifndef __FEEDDB_ICE__
#define __FEEDDB_ICE__

#include <RFeed.ice>
#include <Util.ice>

module xce{
module feed{

	//const int BLOCK_COUNT_EACH_FILE = ;
	//const int BLOCK_COUNT_EACH_USER = 128;
	const int BLOCKCOUNT = 2000000;
	const int BLOCKSIZE = 2048;
	
	//sequence<FeedItem> FeedItemSeq;
	
	class BlockInfo{
		int		serverId;
		int		userId;
		int		totalCount;
		int		curCount;
		int		fileId;
		int		blockIndex;
		int		preFileId;
		int		preBlockIndex;
		int		weight;
	};
	sequence<BlockInfo> BlockInfoSeq;
	dictionary<int, BlockInfo> BlockInfoMap;
	
  class FeedItemBlockInfo{
		FeedItem item;
		BlockInfo blk;
	};
	sequence<FeedItemBlockInfo> FeedItemBlockInfoSeq;

	class UserFeedCount{
		int userId;
		int count;
	};
	sequence<UserFeedCount> UserFeedCountSeq;
	

  class BlockInfo64{
		int		serverId;
		long	key;
		int		totalCount;
		int		curCount;
		int		fileId;
		int		blockIndex;
		int		preFileId;
		int		preBlockIndex;
		int		weight;
	};
	sequence<BlockInfo64> BlockInfo64Seq;
	dictionary<int, BlockInfo64> BlockInfo64Map;
	
  class FeedItemBlockInfo64{
		FeedItem item;
		BlockInfo64 blk;
	};
	sequence<FeedItemBlockInfo64> FeedItemBlockInfo64Seq;

	class UserFeedCount64{
		long key;
		int count;
	};
	sequence<UserFeedCount64> UserFeedCount64Seq;

  /*class Test{
		int a1;
		int a2;
		int a3;
		int a4;
		int a5;
		int a6;
		int a7;
		int a8;
	};	
	class Blk2Feed{
		BlockInfo	blkInfo;
		//FeedItem	feed;
		Test		feed;
	};*/
	//dictionary<BlockInfo, int>	Blk2WeightMap;

	
	struct DataNode{
		int	serverId;
		int fileId;
		int usedBlockCount;
	};
	sequence<DataNode> DataNodeSeq;
	dictionary<int, DataNode> DataNodeMap;
	
	class FileInfo{
		int fileId;
		int dirtyBlockCount;
		int inCacheBlockCount;
	};
	sequence<FileInfo> FileInfoSeq;
	class ServiceInfo{
		int caches;
		FileInfoSeq fileSeq;
	};

	class FeedDBData{
		void 			      put(FeedItem feed, BlockInfo blkInfo);
    void            modify(FeedItem feed, BlockInfo blkInfo);
		void			      putBatch(FeedItem feed, BlockInfoSeq blkInfoSeq);
		void			      putBatchBinary(FeedItem feed, string data);
		FeedItemSeq 		get(BlockInfo blkInfo, int begin, int count);
		void			      removeFeed(BlockInfo blkInfo, long feedId);
		FileInfo		    queryFile(int fileId);
		ServiceInfo		  QueryServiceInfo();
		void			      deleteFile(int fileId);
		bool			      moveBlock(BlockInfo blkInfoFrom, BlockInfo blkInfoTo);
		void			      updateConfig(MyUtil::Str2StrMap cfg);
	
    void 			      put64(FeedItem feed, BlockInfo64 blkInfo);
    void            modify64(FeedItem feed, BlockInfo64 blkInfo);
		void			      putBatch64(FeedItem feed, BlockInfo64Seq blkInfo64Seq);
		void			      putBatchBinary64(FeedItem feed, string data);
		FeedItemSeq 		get64(BlockInfo64 blkInfo, int begin, int count);
		void			      removeFeed64(BlockInfo64 blkInfo, long feedId);
		FileInfo		    queryFile64(int fileId);
		ServiceInfo		  QueryServiceInfo64();
		void			      deleteFile64(int fileId);
		bool			      moveBlock64(BlockInfo64 blkInfoFrom, BlockInfo64 blkInfoTo);
		void			      updateConfig64(MyUtil::Str2StrMap cfg);
  };
	
	class FeedDBMaster{
		BlockInfo 	      getBlock(int userid, bool write);
		BlockInfo	        newBlock(int userid);
		void		          removeBlock(int userid);
		BlockInfoSeq	    getBlockBatch(MyUtil::IntSeq ids, bool write);
		BlockInfoSeq	    newBlockBatch(MyUtil::IntSeq ids);
		void		          putBatch(FeedItem feed, MyUtil::Int2IntMap userWeight);
		void		          decTotalCount(UserFeedCountSeq feedCounts);	
		BlockInfo	        moveBlock(BlockInfo blkInfoOld);
		void		          addNode(int serverid);
		DataNodeMap	      queryNodeInfo();
	

    BlockInfo64       getBlock64(long key, bool write);
		BlockInfo64       newBlock64(long key);
		void		          removeBlock64(long key);
		BlockInfo64Seq	  getBlockBatch64(MyUtil::LongSeq ids, bool write);
		BlockInfo64Seq	  newBlockBatch64(MyUtil::LongSeq ids);
		void		          putBatch64(FeedItem feed, MyUtil::Long2IntMap userWeight);
		void		          decTotalCount64(UserFeedCount64Seq feedCounts);	
		BlockInfo64       moveBlock64(BlockInfo64 blkInfoOld);
		void		          addNode64(int serverid);
		DataNodeMap	      queryNodeInfo64();
  };
	
	class FeedDBDispatcher{
		void putBatch(FeedItem feed, MyUtil::IntSeq ids);
	};
};
};
#endif


