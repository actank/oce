#include <IceLogger.h>
#include "ServiceI.h"
#include "FeedDBMasterI.h"
#include "DataNodeManager.h"

using namespace xce::feed;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	string filename = props->getPropertyWithDefault("HashDBName","/data/xce/XFeed/data/hashdb");
	bool suc = DataNodeManager::instance().init(filename.c_str());
	if(!suc){
		MCE_WARN("DataNodeManager initialize error");
		return;
	}
	//TaskManager::instance().scheduleRepeated(&StatTask::instance());
	TaskManager::instance().scheduleRepeated(&RetryTimer::instance());
	service.getAdapter()->add(&FeedDBMasterI::instance(), service.createIdentity("M", ""));
	TaskManager::instance().scheduleRepeated(&PerformanceStat::instance());	
}

BlockInfoPtr FeedDBMasterI::getBlock(int userid, bool write, const Ice::Current&){
	/*BlockInfoPtr blk = 0;
	BlockData blkData;
	if(DataNodeManager::instance().get(userid, blkData, inc)){
		blk = new BlockInfo;
		return blk;
	}
	//blk.index = -1;
	return blk;*/
	return DataNodeManager::instance().get(userid, write);
}

BlockInfoPtr FeedDBMasterI::newBlock(int userid, const Ice::Current&){
	//DataNodeManager::instance().remove(userid);
	return DataNodeManager::instance().newBlock(userid);
}

void FeedDBMasterI::putBatch(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, const Ice::Current&){
	//DataNodeManager::instance().putBatch(feed, usersWeight);
	PerformanceStat::instance().stat("FeedAll", 0);
  ostringstream os;
  os << "Feed" << (feed.type&0xFFFF);
	PerformanceStat::instance().stat(os.str(), 0);
  MyUtil::TaskManager::instance().execute(new PutTask(feed, const_cast<MyUtil::Int2IntMap&>(usersWeight)));
}

void FeedDBMasterI::addNode(int serverid, const Ice::Current&){
	DataNodeManager::instance().addDataNode(serverid);
}

DataNodeMap FeedDBMasterI::queryNodeInfo(const Ice::Current&){
	return DataNodeManager::instance().getNodeInfo();
}

BlockInfoPtr FeedDBMasterI::moveBlock(const BlockInfoPtr& blkInfoOld, const Ice::Current&){
	return DataNodeManager::instance().moveBlock(blkInfoOld);
}

BlockInfoSeq FeedDBMasterI::getBlockBatch(const MyUtil::IntSeq& userIds, bool write, const Ice::Current&){
	BlockInfoSeq blks;
	for(size_t i=0; i<userIds.size(); i++){
		BlockInfoPtr blkInfo = getBlock(userIds.at(i), write);
		//if(blkInfo){
		blks.push_back(blkInfo);
		//}
	}
	return blks;
}

BlockInfoSeq FeedDBMasterI::newBlockBatch(const MyUtil::IntSeq& userIds, const Ice::Current&){
	BlockInfoSeq blks;
	for(size_t i=0; i<userIds.size(); i++){
		BlockInfoPtr blkInfo = newBlock(userIds.at(i));
		//if(blkInfo){
		blks.push_back(blkInfo);
		//}
	}
	return blks;
}

void FeedDBMasterI::removeBlock(int userid, const Ice::Current&){
	DataNodeManager::instance().remove(userid);
}

void FeedDBMasterI::decTotalCount(const UserFeedCountSeq& feedCounts, const Ice::Current&){
	DataNodeManager::instance().decTotalCount(feedCounts);
}
