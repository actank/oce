#include <IceLogger.h>
#include "SSDFeedDBAdapter.h"


using namespace xce::feed;


/////////////////////////////////////////////////////////////////
FeedDBDataPrx SSDFeedDBDataAdapter::getManager(int id, int timeout) {
	return locate<FeedDBDataPrx>(_managers, "M", id, TWO_WAY, timeout);
}

FeedDBDataPrx SSDFeedDBDataAdapter::getManagerOneway(int id) {
	return locate<FeedDBDataPrx>(_managersOneway, "M", id, ONE_WAY);
}

void SSDFeedDBDataAdapter::put(const FeedItem& feed, const BlockInfoPtr& blkInfo){
	getManagerOneway(blkInfo->serverId)->put(feed, blkInfo);
}

void SSDFeedDBDataAdapter::modify(const FeedItem& feed, const BlockInfoPtr& blkInfo){
	getManagerOneway(blkInfo->serverId)->modify(feed, blkInfo);
}

void SSDFeedDBDataAdapter::putBatch(const FeedItem& feed, const BlockInfoSeq& blkInfoSeq){
	if(blkInfoSeq.empty()){
		return;
	}
	//MCE_INFO("@@@@@@@");
	//getManagerOneway(0)->putBatch(feed, blkInfoSeq);
	//getManager(blkInfoSeq.at(0)->serverId, 500)->putBatch(feed, blkInfoSeq);
	getManagerOneway(blkInfoSeq.at(0)->serverId)->putBatch(feed, blkInfoSeq);
	//getManager(1)->putBatch(feed, blkInfoSeq);
}

string SSDFeedDBDataAdapter::encodeData(const BlockInfoPtr& blkInfo){
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

BlockInfoPtr SSDFeedDBDataAdapter::decodeData(const string& data){
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

string SSDFeedDBDataAdapter::encodeDataSeq(const BlockInfoSeq& seq){
	string data;
	for(int i=0; i<seq.size(); i++){
		string tmp = encodeData(seq.at(i));
		data.insert(data.end(), tmp.begin(), tmp.end()); 
	}
	return data;
}

BlockInfoSeq SSDFeedDBDataAdapter::decodeDataSeq(const string& data){
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

void SSDFeedDBDataAdapter::putBatchBinary(const FeedItem& feed, const BlockInfoSeq& blkInfoSeq){
	if(blkInfoSeq.empty()){
		return;
	}
	string data = encodeDataSeq(blkInfoSeq);	
	getManager(blkInfoSeq.at(0)->serverId, 500)->putBatchBinary(feed, data);
}

FeedItemSeq SSDFeedDBDataAdapter::get(const BlockInfoPtr& blkInfo, int begin, int count){
	return getManager(blkInfo->serverId, 1000)->get(blkInfo, begin, count);
}

void SSDFeedDBDataAdapter::removeFeed(const BlockInfoPtr& blkInfo, Ice::Long feedId){
	getManagerOneway(blkInfo->serverId)->removeFeed(blkInfo, feedId);
}

bool SSDFeedDBDataAdapter::moveBlock(const BlockInfoPtr& blkInfoFrom, const BlockInfoPtr& blkInfoTo){
	return getManagerOneway(blkInfoFrom->serverId)->moveBlock(blkInfoFrom, blkInfoTo);
}

//////////////////////////////////////////////////////////////////////////////
FeedDBMasterPrx SSDFeedDBMasterAdapter::getManager(int id, int timeout) {
        return locate<FeedDBMasterPrx>(_managers, "M", id, TWO_WAY, timeout);
}

FeedDBMasterPrx SSDFeedDBMasterAdapter::getManagerOneway(int id) {
        return locate<FeedDBMasterPrx>(_managersOneway, "M", id, ONE_WAY);
}

BlockInfoPtr SSDFeedDBMasterAdapter::getBlock(int userid, bool write){
	return getManager(userid)->getBlock(userid, write);
}

void SSDFeedDBMasterAdapter::putBatch(const FeedItem& feed, const Int2IntMap& userWeight){
	if(userWeight.empty()){
		return;
	}
	//getManagerOneway(0)->putBatch(feed, userWeight);
	getManagerOneway(0)->putBatch(feed, userWeight);
}

BlockInfoPtr SSDFeedDBMasterAdapter::newBlock(int userid){
	return getManager(0)->newBlock(userid); 
}

void SSDFeedDBMasterAdapter::removeBlock(int userid){
	return getManager(0)->removeBlock(userid); 
}

BlockInfoSeq SSDFeedDBMasterAdapter::getBlockBatch(const IntSeq& ids, bool write){
	if(ids.empty()){
		return BlockInfoSeq();
	}
	return getManager(ids.at(0), 1000)->getBlockBatch(ids, write); 
}

BlockInfoSeq SSDFeedDBMasterAdapter::newBlockBatch(const IntSeq& ids){
	if(ids.empty()){
		return BlockInfoSeq();
	}
	return getManager(ids.at(0), 1000)->newBlockBatch(ids); 
}

void SSDFeedDBMasterAdapter::decTotalCount(const UserFeedCountSeq& feedCounts){
	if(feedCounts.empty()){
		return;
	}
	return getManager(0)->decTotalCount(feedCounts); 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SSDFeedDBAdapter::put(const FeedItem& feed, const Int2IntMap& userWeight){
	SSDFeedDBMasterAdapter::instance().putBatch(feed, userWeight);
}

void SSDFeedDBAdapter::modify(int userId, const FeedItem& feed){
  BlockInfoPtr blk;
	try{
		blk = SSDFeedDBMasterAdapter::instance().getBlock(userId, false);
	}catch(Ice::Exception& e){	
		MCE_WARN("SSDFeedDBAdapter::modify --> call master err, "  << userId << ", " << e);
	}
	
  if(blk){
		try{
			return SSDFeedDBDataAdapter::instance().modify(feed, blk);
		}catch(Ice::Exception& e){
			MCE_WARN("SSDFeedDBAdapter::modify --> call data err, " << userId << ", " << e);
		}
	}
}

FeedItemSeq SSDFeedDBAdapter::get(int userId, int begin, int count){
	BlockInfoPtr blk;
	try{
		blk = SSDFeedDBMasterAdapter::instance().getBlock(userId, false);
	}catch(Ice::Exception& e){	
		MCE_WARN("SSDFeedDBAdapter::get --> call master err, "  << userId << ", " << e);
	}
	if(blk){
		try{
			return SSDFeedDBDataAdapter::instance().get(blk, begin, count);
		}catch(Ice::Exception& e){
			MCE_WARN("SSDFeedDBAdapter::get --> call data err, " << userId << ", " << e);
		}
	}
	return FeedItemSeq();
}

void SSDFeedDBAdapter::remove(int userId, Ice::Long feedId){
	BlockInfoPtr blk;
	try{
		blk = SSDFeedDBMasterAdapter::instance().getBlock(userId, false);
	}catch(Ice::Exception& e){
		MCE_WARN("SSDFeedDBAdapter::remove --> call master err, "  << userId << ", " << e);
	}
	if(blk){
		try{
			SSDFeedDBDataAdapter::instance().removeFeed(blk, feedId);
		}catch(Ice::Exception& e){
			MCE_WARN("SSDFeedDBAdapter::remove --> call data err, " << userId << ", " << e);
		}
	}
}

void SSDFeedDBAdapter::removeAll(int userId){
	SSDFeedDBMasterAdapter::instance().removeBlock(userId);
}

