#include <IceLogger.h>
#include "DataNodeManager.h"
#include "FeedDBTAdapter.h"

using namespace xce::feed;

//const char* HashDB::DB_NAME = "hashdb";
bool HashDB::init(const char* filename){
	_db = tchdbnew();
	if(!_db){
		MCE_WARN("BlockDB::init --> new db error");
		return false;
	}
	bool suc = tchdbtune(_db, BUCKET_NUM, -1, -1, HDBTLARGE);
	if(!suc){
		MCE_WARN("BlockDB::init --> set db option error");
		return false;
	}
	suc = tchdbsetxmsiz(_db, MAPPED_MEM);
	if(!suc){
		MCE_WARN("BlockDB::init --> set extra mapped memory error");
		return false;
	}
	suc = tchdbsetmutex(_db);
	if(!suc){
		MCE_WARN("BlockDB::init --> set mutex error");
		return false;
	}
	suc = tchdbopen(_db, filename, HDBOWRITER|HDBOREADER|HDBOCREAT);
	if(!suc){
		MCE_WARN("BlockDB::init --> open db error, " << filename);
		return false;
	}
	return true;
}

bool HashDB::put(BlockDBKey key, const BlockData& blk){
	TimeStat t;
	bool suc = tchdbput(_db, &key, sizeof(BlockDBKey), &blk, sizeof(BlockData));
	PerformanceStat::instance().stat("HashDBPut", t.getTime());
	return suc;
}

void HashDB::remove(BlockDBKey key){
	tchdbout(_db, &key, sizeof(BlockDBKey));
}

bool HashDB::get(BlockDBKey key, BlockData& blk){
	TimeStat t;
	int size = 0;
	void* res = tchdbget(_db, &key, sizeof(BlockDBKey), &size);
	if(!res){
		return false;
	}
	blk = *((BlockData*)res);
	free(res);
	PerformanceStat::instance().stat("HashDBGet", t.getTime());
	return true;
}

map<int, DataNode> HashDB::loadDataNode(){
	map<int, DataNode> nodes;
	int size = 0;
	BlockDBKey key = 0;
	void* res = tchdbget(_db, &key, sizeof(BlockDBKey), &size);
	if(!res){
		return nodes;
	}
	for(int i=0; i<int(size/sizeof(DataNode)); i++){
		DataNode node = *(((DataNode*)res)+i);
		nodes[node.serverId] = node;
	}
	free(res);
	return nodes;
}

bool HashDB::flushDataNode(const map<int, DataNode>& nodes){
	if(nodes.empty()){
		return false;
	}
	TimeStat t;
	static DataNode data[DataNodeManager::MAX_DATANODE];
	map<int, DataNode>::const_iterator it = nodes.begin();
	for(int i=0; it!=nodes.end(); i++, ++it){
		*(data+i) = it->second;
	}
	int key = 0;
	bool suc = tchdbput(_db, &key, sizeof(int), data, nodes.size()*sizeof(DataNode));
	PerformanceStat::instance().stat("HashDBPut", t.getTime());
	return suc;
}


/////////////////////////////////////////////////////////////////
bool DataNodeManager::init(const char* filename){
	bool suc = _hashDB.init(filename);
	if(!suc){
		return false;
	}

	_dataNodes = _hashDB.loadDataNode();
	if(!_dataNodes.empty()){
		MCE_WARN("DataNodeManager::init --> load from file");
		return true;
	}
	//int recordsize = sizeof(FeedItem);
	//int recordsize = sizeof(FeedItem);
	int	nNodes = FeedDBDataTAdapter::instance().getCluster();
	if(nNodes == 0){
		nNodes = 1;
	}
	//nNodes = 1;
	for(int i=0; i<nNodes; i++){
		DataNode node;
		node.serverId = i;
		node.fileId = 1;
		node.usedBlockCount = 0;
		/*try{
			MCE_WARN("DataNodeManager::init --> new file " << i);
			SSDFeedDBDataAdapter::instance().newFile(node.serverId, node.fileId);
		}catch(Ice::Exception& e){
			MCE_WARN("DataNodeManager::init --> can not find server " << node.serverId);
			continue;
		}*/
		_dataNodes[node.serverId] = node;
	}
	suc = _hashDB.flushDataNode(_dataNodes);
	if(!suc){
		return false;
	}
	return true;
}

void DataNodeManager::addDataNode(int serverid){
	IceUtil::Mutex::Lock lock(_nodeMutex);
	DataNode node;
	node.serverId = serverid;
	//node.totalblockcount = BLOCKCOUNT;
	node.fileId = 1;
	node.usedBlockCount = 0;
	if(_dataNodes.find(serverid) == _dataNodes.end()){
		_dataNodes[serverid] = node;
	}
}

int DataNodeManager::findDataNode(){
	if(_dataNodes.empty()){
		return -1;
	}
	int c = _dataNodes.begin()->second.usedBlockCount;
	int f = _dataNodes.begin()->second.fileId;
	
	int index = 0;
	map<int, DataNode>::iterator it = _dataNodes.begin();
	++it;
	for(; it!=_dataNodes.end(); ++it){
		if(it->second.fileId<f || (it->second.fileId==f && it->second.usedBlockCount<c)){
			c = it->second.usedBlockCount;
			f = it->second.fileId;
			index = it->first;
		}
	}
	//if(_dataNodes.at(index).usedBlockCount >= BLOCKCOUNT){
	//	return -1;
	//}
	return index;
}

void DataNodeManager::allocFreeBlockInNodeNoLock(BlockData& blk, int nodeId){
	DataNode& node = _dataNodes.at(nodeId);
	blk.serverId = node.serverId;
	blk.blockIndex = node.usedBlockCount;
	blk.fileId = node.fileId;
	blk.curCount = 1;
	blk.totalCount = 1;
	if(node.usedBlockCount < BLOCKCOUNT){
		node.usedBlockCount++;
		_hashDB.flushDataNode(_dataNodes);
		//MCE_INFO("DataNodeManager::allocFreeBlockInNodeNoLock --> " << node.fileId << " " << node.usedBlockCount);
		return;
	}

	node.fileId++;
	node.usedBlockCount = 1;
	_hashDB.flushDataNode(_dataNodes);

	blk.blockIndex = 0;
	blk.fileId = node.fileId;
}

bool DataNodeManager::allocFreeBlockInNode(BlockData& blk, int nodeId){
	IceUtil::Mutex::Lock lock(_nodeMutex);
	allocFreeBlockInNodeNoLock(blk, nodeId);
	return true;
}

bool DataNodeManager::allocFreeBlock(BlockDBKey key, BlockData& blk){
	IceUtil::Mutex::Lock lock(_nodeMutex);
	if(_dataNodes.empty()){
		return false;
	}
	if(_curNode < 0 || _curCount >= ALLOC_COUNT){
		_curNode = findDataNode();
		_curCount = 0;
	}
	if(_curNode < 0){
		return false;
	}
	_curCount++;
	allocFreeBlockInNodeNoLock(blk, _curNode);
	return true;
}

BlockInfoPtr DataNodeManager::BlockDataToBlockInfo(const BlockData& blk){
	BlockInfoPtr blkInfo = new BlockInfo;

	blkInfo->serverId = blk.serverId;
	blkInfo->totalCount = blk.totalCount;
	blkInfo->curCount = blk.curCount;
	blkInfo->fileId = blk.fileId;
	blkInfo->blockIndex = blk.blockIndex;
	blkInfo->preFileId = 0;
	blkInfo->preBlockIndex = -1;

	return blkInfo;
}

BlockData DataNodeManager::BlockInfoToBlockData(const BlockInfoPtr& blk){
	BlockData data;
	data.serverId = blk->serverId;
	data.totalCount = blk->totalCount;
	data.curCount = blk->curCount;
	data.fileId = blk->fileId;
	data.blockIndex = blk->blockIndex;
	
	return data;
}

BlockInfoPtr DataNodeManager::get(BlockDBKey key, bool write){
	TimeStat t;
	IceUtil::Mutex::Lock lock(_blockMutexs[key%MUTEX_COUNT]);
	if(key == 0){
		return 0;
	}
	BlockData blkOld;
	BlockInfoPtr blkInfo;
	bool suc = _hashDB.get(key, blkOld);
	if(!suc){
		if(!write){
			PerformanceStat::instance().stat("get", t.getTime());
			return 0;
		}else{
			BlockData newBlk;
			if(allocFreeBlock(key, newBlk)){
				blkInfo = BlockDataToBlockInfo(newBlk);
				blkInfo->userId = key;
				_hashDB.put(key, newBlk);
				MCE_INFO("DataNodeManager::get --> new blk node:" << blkInfo->serverId << " u:" << blkInfo->userId 
				<< " tc:" << blkInfo->totalCount << " cc:" << blkInfo->curCount
				<< " f:" << blkInfo->fileId << " b:" << blkInfo->blockIndex
				<< " preF:" << blkInfo->preFileId << " preB:" << blkInfo->preBlockIndex);
				PerformanceStat::instance().stat("get", t.getTime());
				return blkInfo;
			}
		}
	}else if(!write){
		blkInfo = BlockDataToBlockInfo(blkOld);
		blkInfo->userId = key;
		PerformanceStat::instance().stat("get", t.getTime());
		return blkInfo;
	}else if(blkOld.curCount < RECORD_COUNT_PER_BLOCK){
		blkOld.curCount++;
		blkOld.totalCount++;
		blkInfo = BlockDataToBlockInfo(blkOld);
		blkInfo->userId = key;
		_hashDB.put(key, blkOld);
		PerformanceStat::instance().stat("get", t.getTime());
		return blkInfo;
	}else{
		BlockData newBlk;
		if(allocFreeBlockInNode(newBlk, blkOld.serverId)){
			newBlk.totalCount = blkOld.totalCount + 1;
			blkInfo = BlockDataToBlockInfo(newBlk);
			blkInfo->userId = key;
			blkInfo->preFileId = blkOld.fileId;
			blkInfo->preBlockIndex = blkOld.blockIndex;
			_hashDB.put(key, newBlk);
			MCE_INFO("DataNodeManager::get --> new blk node:" << blkInfo->serverId << " u:" << blkInfo->userId 
			<< " tc:" << blkInfo->totalCount << " cc:" << blkInfo->curCount
			<< " f:" << blkInfo->fileId << " b:" << blkInfo->blockIndex
			<< " preF:" << blkInfo->preFileId << " preB:" << blkInfo->preBlockIndex);
			PerformanceStat::instance().stat("get", t.getTime());
			return blkInfo;
		}
		MCE_WARN("allocFreeBlock err");
	}
	return 0;
}

void DataNodeManager::remove(BlockDBKey key){
	IceUtil::Mutex::Lock lock(_blockMutexs[key%MUTEX_COUNT]);
	MCE_INFO("DataNodeManager::remove --> " << key);
	_hashDB.remove(key);
}

BlockInfoPtr DataNodeManager::newBlock(BlockDBKey key){
	IceUtil::Mutex::Lock lock(_blockMutexs[key%MUTEX_COUNT]);
	if(key == 0){
		return 0;
	}
	BlockData blkOld;
	BlockInfoPtr blkInfo;
	bool suc = _hashDB.get(key, blkOld);
	if(!suc){
		MCE_WARN("DataNodeManager::newBlock --> not exist " << key);
	}else{
		BlockData newBlk;
		if(allocFreeBlockInNode(newBlk, blkOld.serverId)){
			newBlk.totalCount = blkOld.totalCount;
			newBlk.curCount = blkOld.curCount;
			blkInfo = BlockDataToBlockInfo(newBlk);
			blkInfo->userId = key;
			blkInfo->preFileId = blkOld.fileId;
			blkInfo->preBlockIndex = blkOld.blockIndex;
			_hashDB.put(key, newBlk);
			MCE_INFO("DataNodeManager::newBlock --> new blk " << blkInfo->serverId << " u:" << blkInfo->userId << " f:" << blkInfo->fileId << " b:" << blkInfo->blockIndex
				<< " preF:" << blkInfo->preFileId << " preB:" << blkInfo->preBlockIndex);
			return blkInfo;
		}
		MCE_WARN("allocFreeBlock err");
	}
	return 0;
}

BlockInfoPtr DataNodeManager::moveBlock(const BlockInfoPtr& blkInfoOld){
	BlockData newBlk;
	BlockInfoPtr newBlkInfo = new BlockInfo;
	*newBlkInfo = *blkInfoOld;

	//BlockData blk;
	//bool suc = _hashDB.get(blkInfoOld->userId);
	//if()	
	allocFreeBlockInNode(newBlk, blkInfoOld->serverId);
	newBlkInfo->fileId = newBlk.fileId;
	newBlkInfo->blockIndex = newBlk.blockIndex;

	try{
		FeedDBDataTAdapter::instance().moveBlock(blkInfoOld, newBlkInfo);
	}catch(Ice::Exception& e){
		MCE_WARN("DataNodeManager::moveBlock --> invoke data node to move err, " << e);
		return 0;
	}
	_hashDB.put(newBlkInfo->userId, newBlk);
	return newBlkInfo;
}

void DataNodeManager::putBatch(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight){
	static const unsigned int MAX_MSG_SIZE = 5000;
	map<int, vector<pair<bool, BlockInfoSeq> > > blkInfoMap;
	Int2IntMap::const_iterator it = usersWeight.begin();
	//pair<bool, BlockInfoSeq> p = make_pair<bool, BlockInfoSeq>(false, BlockInfoSeq());
	for(; it!=usersWeight.end(); ++it){
		//if((it->first%10 > 5)){
		//	continue;
		//}
		if(it->first == 0){
			continue;
		}
		BlockInfoPtr blk = get(it->first, true);
		if(!blk){
			MCE_WARN("DataNodeManager::putBatch --> can not get a block " << it->first);
			continue;
		}
		if(blk->userId==261947900 || blk->userId==227805680){
			MCE_INFO("#put# userid:"<<it->first<<" file:"<<blk->fileId<<" index:"<<blk->blockIndex
                        <<" tc:"<<blk->totalCount<<" cc:"<<blk->curCount << " preF:" << blk->preFileId << " preB:" << blk->preBlockIndex
                        <<" -- " << " feed:" << feed.feed);
		}
		blk->weight = it->second;
		//MCE_INFO("@@get block " << userIds.at(i) << " " << blk->serverId);
		//Blk2FeedPtr tmp = new Blk2Feed;
		//tmp->blkInfo = blk;
		//tmp->feed = it->second;
		if(blkInfoMap[blk->serverId].empty() || blkInfoMap[blk->serverId].rbegin()->second.size() >= MAX_MSG_SIZE){
			blkInfoMap[blk->serverId].push_back(pair<bool, BlockInfoSeq>(false, BlockInfoSeq()));
		}
		if(blk->curCount == 1){
			blkInfoMap[blk->serverId].rbegin()->first = true;
		}
		blkInfoMap[blk->serverId].rbegin()->second.push_back(blk);
		//blkInfoMap[blk->serverId].push_back(blk);
	}
	map<int, vector<pair<bool, BlockInfoSeq> > >::iterator tmpIt = blkInfoMap.begin();
	for(; tmpIt!=blkInfoMap.end(); ++tmpIt){
		for(size_t i=0; i<tmpIt->second.size(); i++){
			//最多重试10次
			//for(int j=0; j<10; j++){
				TimeStat t;
				try{
					MCE_INFO("DataNodeManager::putBatch --> invoke FeedDBData" << tmpIt->first << " size:" << tmpIt->second.at(i).second.size());
					FeedDBDataTAdapter::instance().putBatchBinary(feed, tmpIt->second.at(i).second);
				}catch(Ice::Exception& e){
					MCE_WARN("DataNodeManager::putBatch --> invoke FeedDBData" << tmpIt->first << " err, " << e);
					RetryTimer::instance().push(feed, tmpIt->second.at(i).second);
					//string tmp = e.what();
					//if(tmp.find("TimeoutException") != string::npos){
					//	break;
					//}
					//if(!tmpIt->second.at(i).first){
					//	break;
					//}
				}
				PerformanceStat::instance().stat("CallData", t.getTime());
			//}
		}
	}
	//MCE_INFO("batch put --> " << usersWeight.size() << " " << blkInfoMap.size());

}

void DataNodeManager::decTotalCount(const UserFeedCountSeq& feedCounts){
	for(size_t i=0; i<feedCounts.size(); i++){
		int key = feedCounts.at(i)->userId;
		if(key==0){
			continue;
		}
		IceUtil::Mutex::Lock lock(_blockMutexs[key%MUTEX_COUNT]);
		BlockData blkOld;
		bool suc = _hashDB.get(key, blkOld);
		if(!suc){
			MCE_WARN("DataNodeManager::decTotalCount --> not exist " << key);
			continue;
		}
		MCE_INFO("DataNodeManager::decTotalCount --> " << key << " " << blkOld.totalCount << " - " << feedCounts.at(i)->count);
		blkOld.totalCount -= feedCounts.at(i)->count;
		_hashDB.put(key, blkOld);
	}
}

void PutTask::handle(){
	DataNodeManager::instance().putBatch(_feed, _usersWeight);
}

void RetryTimer::handle(){
	int c = 0;
	{
		IceUtil::Mutex::Lock lock(mutex_);
		c = feeds_.size();
	}
	MCE_INFO("RetryTimer::handle --> retry list " << c);
	if(c==0){
		return;
	}
	while(true){
		pair<FeedItem, BlockInfoSeq> p;
		{
			IceUtil::Mutex::Lock lock(mutex_);
			if(feeds_.empty()){
				break;
			}
			p.first = feeds_.front().first;
			p.second.swap(feeds_.front().second);
			feeds_.pop_front();
		}
		try{
			FeedDBDataTAdapter::instance().putBatch(p.first, p.second);
		}catch(Ice::Exception& e){
			MCE_WARN("RetryTimer::handle --> invoke FeedDBData err, " << e);
			IceUtil::Mutex::Lock lock(mutex_);
			push(p.first, p.second);
			break;
		}
	}
	MCE_INFO("RetryTimer::handle --> retry list end " << c);
}
