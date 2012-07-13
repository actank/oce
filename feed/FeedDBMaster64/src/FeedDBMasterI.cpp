#include <IceLogger.h>
#include "ServiceI.h"
#include "FeedDBMasterI.h"
#include "DataNodeManager.h"
#include "FeedDB64Adapter.h"

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
  return ToBlockInfo(getBlock64(userid, write));
}

BlockInfoPtr FeedDBMasterI::newBlock(int userid, const Ice::Current&){
  return ToBlockInfo(newBlock64(userid));
}

void FeedDBMasterI::putBatch(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, const Ice::Current&){
  MyUtil::Long2IntMap weights;
  for(MyUtil::Int2IntMap::const_iterator it=usersWeight.begin(); it!=usersWeight.end(); ++it){
    weights[it->first] = it->second;
  }
  putBatch64(feed, weights);	
}

void FeedDBMasterI::addNode(int serverid, const Ice::Current&){
  addNode64(serverid);
}

DataNodeMap FeedDBMasterI::queryNodeInfo(const Ice::Current&){
	return queryNodeInfo64(); 
}

BlockInfoPtr FeedDBMasterI::moveBlock(const BlockInfoPtr& blkInfoOld, const Ice::Current&){
	return ToBlockInfo(moveBlock64(ToBlockInfo64(blkInfoOld)));
}

BlockInfoSeq FeedDBMasterI::getBlockBatch(const MyUtil::IntSeq& userIds, bool write, const Ice::Current&){
  MyUtil::LongSeq keys;
  for(size_t i=0; i<userIds.size(); i++){
    keys.push_back(userIds.at(i));
  }
  return ToBlockInfoSeq(getBlockBatch64(keys, write));
}

BlockInfoSeq FeedDBMasterI::newBlockBatch(const MyUtil::IntSeq& userIds, const Ice::Current&){
	MyUtil::LongSeq keys;
  for(size_t i=0; i<userIds.size(); i++){
    keys.push_back(userIds.at(i));
  }
  return ToBlockInfoSeq(newBlockBatch64(keys));
}

void FeedDBMasterI::removeBlock(int userid, const Ice::Current&){
  removeBlock64(userid);
}

void FeedDBMasterI::decTotalCount(const UserFeedCountSeq& feedCounts, const Ice::Current&){
  decTotalCount64(ToUserFeedCount64Seq(feedCounts));
}






BlockInfo64Ptr FeedDBMasterI::getBlock64(Ice::Long key, bool write, const Ice::Current&){
	/*BlockInfoPtr blk = 0;
	BlockData blkData;
	if(DataNodeManager::instance().get(userid, blkData, inc)){
		blk = new BlockInfo;
		return blk;
	}
	//blk.index = -1;
	return blk;*/
	return DataNodeManager::instance().get(key, write);
}

BlockInfo64Ptr FeedDBMasterI::newBlock64(Ice::Long key, const Ice::Current&){
	//DataNodeManager::instance().remove(userid);
	return DataNodeManager::instance().newBlock(key);
}

void FeedDBMasterI::putBatch64(const FeedItem& feed, const MyUtil::Long2IntMap& usersWeight, const Ice::Current&){
	//DataNodeManager::instance().putBatch(feed, usersWeight);
	PerformanceStat::instance().stat("FeedAll", 0);
  ostringstream os;
  os << "Feed" << (feed.type&0xFFFFFF);
	PerformanceStat::instance().stat(os.str(), 0);
  MyUtil::TaskManager::instance().execute(new PutTask(feed, const_cast<MyUtil::Long2IntMap&>(usersWeight)));
}

void FeedDBMasterI::addNode64(int serverid, const Ice::Current&){
	DataNodeManager::instance().addDataNode(serverid);
}

DataNodeMap FeedDBMasterI::queryNodeInfo64(const Ice::Current&){
	return DataNodeManager::instance().getNodeInfo();
}

BlockInfo64Ptr FeedDBMasterI::moveBlock64(const BlockInfo64Ptr& blkInfoOld, const Ice::Current&){
	return DataNodeManager::instance().moveBlock(blkInfoOld);
}

BlockInfo64Seq FeedDBMasterI::getBlockBatch64(const MyUtil::LongSeq& keys, bool write, const Ice::Current&){
	BlockInfo64Seq blks;
	for(size_t i=0; i<keys.size(); i++){
		BlockInfo64Ptr blkInfo = getBlock64(keys.at(i), write);
		//if(blkInfo){
		blks.push_back(blkInfo);
		//}
	}
	return blks;
}

BlockInfo64Seq FeedDBMasterI::newBlockBatch64(const MyUtil::LongSeq& keys, const Ice::Current&){
	BlockInfo64Seq blks;
	for(size_t i=0; i<keys.size(); i++){
		BlockInfo64Ptr blkInfo = newBlock64(keys.at(i));
		//if(blkInfo){
		blks.push_back(blkInfo);
		//}
	}
	return blks;
}

void FeedDBMasterI::removeBlock64(Ice::Long key, const Ice::Current&){
	DataNodeManager::instance().remove(key);
}

void FeedDBMasterI::decTotalCount64(const UserFeedCount64Seq& feedCounts, const Ice::Current&){
	DataNodeManager::instance().decTotalCount(feedCounts);
}








