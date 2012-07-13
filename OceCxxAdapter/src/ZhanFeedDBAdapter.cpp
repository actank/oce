#include <IceLogger.h>
#include "ZhanFeedDBAdapter.h"


using namespace xce::feed;


/////////////////////////////////////////////////////////////////
FeedDBDataPrx ZhanFeedDBDataAdapter::getManager(int id, int timeout) {
	return locate<FeedDBDataPrx>(_managers, "M", id, TWO_WAY, timeout);
}

FeedDBDataPrx ZhanFeedDBDataAdapter::getManagerOneway(int id) {
	return locate<FeedDBDataPrx>(_managersOneway, "M", id, ONE_WAY);
}

void ZhanFeedDBDataAdapter::put(const FeedItem& feed, const BlockInfoPtr& blkInfo){
	getManagerOneway(blkInfo->serverId)->put(feed, blkInfo);
}

void ZhanFeedDBDataAdapter::modify(const FeedItem& feed, const BlockInfoPtr& blkInfo){
	getManagerOneway(blkInfo->serverId)->modify(feed, blkInfo);
}

void ZhanFeedDBDataAdapter::putBatch(const FeedItem& feed, const BlockInfoSeq& blkInfoSeq){
	if(blkInfoSeq.empty()){
		return;
	}
	//MCE_INFO("@@@@@@@");
	//getManagerOneway(0)->putBatch(feed, blkInfoSeq);
	//getManager(blkInfoSeq.at(0)->serverId, 500)->putBatch(feed, blkInfoSeq);
	getManagerOneway(blkInfoSeq.at(0)->serverId)->putBatch(feed, blkInfoSeq);
	//getManager(1)->putBatch(feed, blkInfoSeq);
}

string ZhanFeedDBDataAdapter::encodeData(const BlockInfoPtr& blkInfo){
	string data;
	char tmp[1024] = {0};
	char* p = tmp;
	*(int*)p = blkInfo->serverId;
	*(int*)(p+=sizeof(int)) = blkInfo->userId;		
	*(int*)(p+=sizeof(int)) = blkInfo->totalCount;		
	*(int*)(p+=sizeof(int)) = blkInfo->curCount;		
	*(int*)(p+=sizeof(int)) = blkInfo->fileId;		
	*(int*)(p+=sizeof(int)) = blkInfo->blockIndex;		
	*(int*)(p+=sizeof(int)) = blkInfo->preFileId;
	*(int*)(p+=sizeof(int)) = blkInfo->preBlockIndex;
	*(int*)(p+=sizeof(int)) = blkInfo->weight;
	data.insert(data.end(), tmp, p+sizeof(int)); 
	return data;
}

BlockInfoPtr ZhanFeedDBDataAdapter::decodeData(const string& data){
	const char* p = data.c_str();
	BlockInfoPtr blkInfo = new BlockInfo;
	blkInfo->serverId 	= *(int*)p;
	blkInfo->userId 	= *(int*)(p+=sizeof(int));
	blkInfo->totalCount 	= *(int*)(p+=sizeof(int));
	blkInfo->curCount 	= *(int*)(p+=sizeof(int));
	blkInfo->fileId 	= *(int*)(p+=sizeof(int));
	blkInfo->blockIndex 	= *(int*)(p+=sizeof(int));
	blkInfo->preFileId 	= *(int*)(p+=sizeof(int));
	blkInfo->preBlockIndex 	= *(int*)(p+=sizeof(int));
	blkInfo->weight  	= *(int*)(p+=sizeof(int));
	return blkInfo;
}

string ZhanFeedDBDataAdapter::encodeDataSeq(const BlockInfoSeq& seq){
	string data;
	for(int i=0; i<seq.size(); i++){
		string tmp = encodeData(seq.at(i));
		data.insert(data.end(), tmp.begin(), tmp.end()); 
	}
	return data;
}

BlockInfoSeq ZhanFeedDBDataAdapter::decodeDataSeq(const string& data){
	int pos = 0;
        int UNIT = 36;
        BlockInfoSeq seq;
        for(;;){
                string tmp = data.substr(pos, UNIT);
                if(tmp.size() != UNIT){
                        break;
		}
                BlockInfoPtr blk = decodeData(tmp);
                seq.push_back(blk);
                pos+=UNIT;
        }
	return seq;
}

void ZhanFeedDBDataAdapter::putBatchBinary(const FeedItem& feed, const BlockInfoSeq& blkInfoSeq){
	if(blkInfoSeq.empty()){
		return;
	}
	string data = encodeDataSeq(blkInfoSeq);	
	getManager(blkInfoSeq.at(0)->serverId, 500)->putBatchBinary(feed, data);
}

FeedItemSeq ZhanFeedDBDataAdapter::get(const BlockInfoPtr& blkInfo, int begin, int count){
	return getManager(blkInfo->serverId, 1000)->get(blkInfo, begin, count);
}

void ZhanFeedDBDataAdapter::removeFeed(const BlockInfoPtr& blkInfo, Ice::Long feedId){
	getManagerOneway(blkInfo->serverId)->removeFeed(blkInfo, feedId);
}

bool ZhanFeedDBDataAdapter::moveBlock(const BlockInfoPtr& blkInfoFrom, const BlockInfoPtr& blkInfoTo){
	return getManagerOneway(blkInfoFrom->serverId)->moveBlock(blkInfoFrom, blkInfoTo);
}

//////////////////////////////////////////////////////////////////////////////
FeedDBMasterPrx ZhanFeedDBMasterAdapter::getManager(int id, int timeout) {
        return locate<FeedDBMasterPrx>(_managers, "M", id, TWO_WAY, timeout);
}

FeedDBMasterPrx ZhanFeedDBMasterAdapter::getManagerOneway(int id) {
        return locate<FeedDBMasterPrx>(_managersOneway, "M", id, ONE_WAY);
}

BlockInfoPtr ZhanFeedDBMasterAdapter::getBlock(int userid, bool write){
	return getManager(userid)->getBlock(userid, write);
}

void ZhanFeedDBMasterAdapter::putBatch(const FeedItem& feed, const Int2IntMap& userWeight){
	if(userWeight.empty()){
		return;
	}
	//getManagerOneway(0)->putBatch(feed, userWeight);
	getManagerOneway(0)->putBatch(feed, userWeight);
}

BlockInfoPtr ZhanFeedDBMasterAdapter::newBlock(int userid){
	return getManager(0)->newBlock(userid); 
}

void ZhanFeedDBMasterAdapter::removeBlock(int userid){
	return getManager(0)->removeBlock(userid); 
}

BlockInfoSeq ZhanFeedDBMasterAdapter::getBlockBatch(const IntSeq& ids, bool write){
	if(ids.empty()){
		return BlockInfoSeq();
	}
	return getManager(ids.at(0), 1000)->getBlockBatch(ids, write); 
}

BlockInfoSeq ZhanFeedDBMasterAdapter::newBlockBatch(const IntSeq& ids){
	if(ids.empty()){
		return BlockInfoSeq();
	}
	return getManager(ids.at(0), 1000)->newBlockBatch(ids); 
}

void ZhanFeedDBMasterAdapter::decTotalCount(const UserFeedCountSeq& feedCounts){
	if(feedCounts.empty()){
		return;
	}
	return getManager(0)->decTotalCount(feedCounts); 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void ZhanFeedDBAdapter::put(const FeedItem& feed, const Int2IntMap& userWeight){
	ZhanFeedDBMasterAdapter::instance().putBatch(feed, userWeight);
}

void ZhanFeedDBAdapter::modify(int userId, const FeedItem& feed){
  BlockInfoPtr blk;
	try{
		blk = ZhanFeedDBMasterAdapter::instance().getBlock(userId, false);
	}catch(Ice::Exception& e){	
		MCE_WARN("ZhanFeedDBAdapter::modify --> call master err, "  << userId << ", " << e);
	}
	
  if(blk){
		try{
			return ZhanFeedDBDataAdapter::instance().modify(feed, blk);
		}catch(Ice::Exception& e){
			MCE_WARN("ZhanFeedDBAdapter::modify --> call data err, " << userId << ", " << e);
		}
	}
}

FeedItemSeq ZhanFeedDBAdapter::get(int userId, int begin, int count){
	BlockInfoPtr blk;
	try{
		blk = ZhanFeedDBMasterAdapter::instance().getBlock(userId, false);
	}catch(Ice::Exception& e){	
		MCE_WARN("ZhanFeedDBAdapter::get --> call master err, "  << userId << ", " << e);
	}
	if(blk){
		try{
			return ZhanFeedDBDataAdapter::instance().get(blk, begin, count);
		}catch(Ice::Exception& e){
			MCE_WARN("ZhanFeedDBAdapter::get --> call data err, " << userId << ", " << e);
		}
	}
	return FeedItemSeq();
}

void ZhanFeedDBAdapter::remove(int userId, Ice::Long feedId){
	BlockInfoPtr blk;
	try{
		blk = ZhanFeedDBMasterAdapter::instance().getBlock(userId, false);
	}catch(Ice::Exception& e){
		MCE_WARN("ZhanFeedDBAdapter::remove --> call master err, "  << userId << ", " << e);
	}
	if(blk){
		try{
			ZhanFeedDBDataAdapter::instance().removeFeed(blk, feedId);
		}catch(Ice::Exception& e){
			MCE_WARN("ZhanFeedDBAdapter::remove --> call data err, " << userId << ", " << e);
		}
	}
}

void ZhanFeedDBAdapter::removeAll(int userId){
	ZhanFeedDBMasterAdapter::instance().removeBlock(userId);
}

