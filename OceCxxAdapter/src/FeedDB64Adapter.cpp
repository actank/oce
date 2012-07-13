#include <IceLogger.h>
#include "FeedDB64Adapter.h"


using namespace xce::feed;


/////////////////////////////////////////////////////////////////
FeedDBDataPrx FeedDBDataAdapter::getManager(int id, int timeout) {
	return locate<FeedDBDataPrx>(_managers, "M", id, TWO_WAY, timeout);
}

FeedDBDataPrx FeedDBDataAdapter::getManagerOneway(int id) {
	return locate<FeedDBDataPrx>(_managersOneway, "M", id, ONE_WAY);
}

void FeedDBDataAdapter::put(const FeedItem& feed, const BlockInfo64Ptr& blkInfo){
	getManagerOneway(blkInfo->serverId)->put64(feed, blkInfo);
}

void FeedDBDataAdapter::putBatch(const FeedItem& feed, const BlockInfo64Seq& blkInfoSeq){
	if(blkInfoSeq.empty()){
		return;
	}
	//getManagerOneway(blkInfoSeq.at(0)->serverId)->putBatch(feed, blkInfoSeq);
	getManager(blkInfoSeq.at(0)->serverId, 500)->putBatch64(feed, blkInfoSeq);
	//getManager(1)->putBatch(feed, blkInfoSeq);
}

string FeedDBDataAdapter::encodeData(const BlockInfo64Ptr& blkInfo){
	string data;
	char tmp[1024] = {0};
	char* p = tmp;
	*(int*)p = blkInfo->serverId;
	*(Ice::Long*)(p+=sizeof(int)) = blkInfo->key;		
	*(int*)(p+=sizeof(Ice::Long)) = blkInfo->totalCount;		
	*(int*)(p+=sizeof(int)) = blkInfo->curCount;		
	*(int*)(p+=sizeof(int)) = blkInfo->fileId;		
	*(int*)(p+=sizeof(int)) = blkInfo->blockIndex;		
	*(int*)(p+=sizeof(int)) = blkInfo->preFileId;
	*(int*)(p+=sizeof(int)) = blkInfo->preBlockIndex;
	*(int*)(p+=sizeof(int)) = blkInfo->weight;
	data.insert(data.end(), tmp, p+sizeof(int)); 
	return data;
}

BlockInfo64Ptr FeedDBDataAdapter::decodeData(const string& data){
	const char* p = data.c_str();
	BlockInfo64Ptr blkInfo = new BlockInfo64;
	blkInfo->serverId 	= *(int*)p;
	blkInfo->key 	= *(Ice::Long*)(p+=sizeof(int));
	blkInfo->totalCount 	= *(int*)(p+=sizeof(Ice::Long));
	blkInfo->curCount 	= *(int*)(p+=sizeof(int));
	blkInfo->fileId 	= *(int*)(p+=sizeof(int));
	blkInfo->blockIndex 	= *(int*)(p+=sizeof(int));
	blkInfo->preFileId 	= *(int*)(p+=sizeof(int));
	blkInfo->preBlockIndex 	= *(int*)(p+=sizeof(int));
	blkInfo->weight  	= *(int*)(p+=sizeof(int));
	return blkInfo;
}

string FeedDBDataAdapter::encodeDataSeq(const BlockInfo64Seq& seq){
	string data;
	for(int i=0; i<seq.size(); i++){
		string tmp = encodeData(seq.at(i));
		data.insert(data.end(), tmp.begin(), tmp.end()); 
	}
	return data;
}

BlockInfo64Seq FeedDBDataAdapter::decodeDataSeq(const string& data){
	int pos = 0;
        int UNIT = 36;
        BlockInfo64Seq seq;
        for(;;){
                string tmp = data.substr(pos, UNIT);
                if(tmp.size() != UNIT){
                        break;
                }
                BlockInfo64Ptr blk = decodeData(tmp);
                seq.push_back(blk);
                pos+=UNIT;
        }
	return seq;
}

void FeedDBDataAdapter::putBatchBinary(const FeedItem& feed, const BlockInfo64Seq& blkInfoSeq){
	if(blkInfoSeq.empty()){
		return;
	}
	string data = encodeDataSeq(blkInfoSeq);	
	getManager(blkInfoSeq.at(0)->serverId, 500)->putBatchBinary64(feed, data);
	//getManager(1)->putBatch(feed, blkInfoSeq);
}

FeedItemSeq FeedDBDataAdapter::get(const BlockInfo64Ptr& blkInfo, int begin, int count){
	return getManager(blkInfo->serverId, 1000)->get64(blkInfo, begin, count);
}

void FeedDBDataAdapter::removeFeed(const BlockInfo64Ptr& blkInfo, Ice::Long feedId){
	getManagerOneway(blkInfo->serverId)->removeFeed64(blkInfo, feedId);
}

bool FeedDBDataAdapter::moveBlock(const BlockInfo64Ptr& blkInfoFrom, const BlockInfo64Ptr& blkInfoTo){
	return getManagerOneway(blkInfoFrom->serverId)->moveBlock64(blkInfoFrom, blkInfoTo);
}

//////////////////////////////////////////////////////////////////////////////
FeedDBMasterPrx FeedDBMasterAdapter::getManager(int id, int timeout) {
        return locate<FeedDBMasterPrx>(_managers, "M", id, TWO_WAY, timeout);
}

FeedDBMasterPrx FeedDBMasterAdapter::getManagerOneway(int id) {
        return locate<FeedDBMasterPrx>(_managersOneway, "M", id, ONE_WAY);
}

BlockInfo64Ptr FeedDBMasterAdapter::getBlock(Ice::Long key, bool write){
	return getManager(key)->getBlock64(key, write);
}

void FeedDBMasterAdapter::putBatch(const FeedItem& feed, const Long2IntMap& userWeight){
	if(userWeight.empty()){
		return;
	}
	//getManagerOneway(0)->putBatch(feed, userWeight);
	getManagerOneway(0)->putBatch64(feed, userWeight);
}

BlockInfo64Ptr  FeedDBMasterAdapter::newBlock(Ice::Long key){
	return getManager(0)->newBlock64(key); 
}

void FeedDBMasterAdapter::removeBlock(Ice::Long key){
	return getManager(0)->removeBlock64(key); 
}

BlockInfo64Seq  FeedDBMasterAdapter::getBlockBatch(const LongSeq& ids, bool write){
	if(ids.empty()){
		return BlockInfo64Seq();
	}
	return getManager(ids.at(0), 1000)->getBlockBatch64(ids, write); 
}

BlockInfo64Seq  FeedDBMasterAdapter::newBlockBatch(const LongSeq& ids){
	if(ids.empty()){
		return BlockInfo64Seq();
	}
	return getManager(ids.at(0), 1000)->newBlockBatch64(ids); 
}

void FeedDBMasterAdapter::decTotalCount(const UserFeedCount64Seq& feedCounts){
	if(feedCounts.empty()){
		return;
	}
	return getManager(0)->decTotalCount64(feedCounts); 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void FeedDBAdapter::put(const FeedItem& feed, const Long2IntMap& userWeight){
	FeedDBMasterAdapter::instance().putBatch(feed, userWeight);
}

FeedItemSeq FeedDBAdapter::get(Ice::Long key, int begin, int count){
	BlockInfo64Ptr blk;
	try{
		blk = FeedDBMasterAdapter::instance().getBlock(key, false);
	}catch(Ice::Exception& e){
		MCE_WARN("FeedDBAdapter::get --> call master err, "  << key << ", " << e);
	}
	if(blk){
		try{
			return FeedDBDataAdapter::instance().get(blk, begin, count);
		}catch(Ice::Exception& e){
			MCE_WARN("FeedDBAdapter::get --> call data err, " << key << ", " << e)
		}
	}
	return FeedItemSeq();
}

void FeedDBAdapter::remove(Ice::Long key, Ice::Long feedId){
	BlockInfo64Ptr blk;
	try{
		blk = FeedDBMasterAdapter::instance().getBlock(key, false);
	}catch(Ice::Exception& e){
		MCE_WARN("FeedDBAdapter::remove --> call master err, "  << key << " " << feedId << ", " << e);
	}
	if(blk){
		try{
			FeedDBDataAdapter::instance().removeFeed(blk, feedId);
		}catch(Ice::Exception& e){
			MCE_WARN("FeedDBAdapter::remove --> call data err, " << key << " " << feedId << ", " << e)
		}
	}
}

void  FeedDBAdapter::removeAll(Ice::Long key){
	FeedDBMasterAdapter::instance().removeBlock(key);
}

