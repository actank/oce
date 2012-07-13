#include <IceLogger.h>
#include "TaskManager.h"
#include "FeedDBDataI.h"
#include "ServiceI.h"
#include "PerformanceStat.h"
#include "Cache.h"
#include "IOExecutor.h"
#include "MemPool.h"
#include "ZhanFeedDBAdapter.h"

using namespace xce::feed;
using namespace xce::feed::db;


static const int MAX_RECORD_COUNT = 10000;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	//BLOCK_COUNT BLOCK_SIZE/
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	string data_path = props->getPropertyWithDefault("FeedDBDataPath","/data/xce/XceFeedDB/data/feeddb/data");
	string log_path = props->getPropertyWithDefault("FeedDBLogPath","/data/xce/XceFeedDB/data/feeddb/log");
	int index = props->getPropertyAsIntWithDefault("Index", -1);
	if(index < 0){
		MCE_WARN("### index err");
		return;
	}
	FeedDBDataI::instance().setIndex(index);
	//MemPool::instance().Init(CacheManager::MAX_CACHE/10, CacheManager::MAX_CACHE/100);
	MemPool::instance().Init(BLOCKCOUNT, BLOCKCOUNT);
	CacheManager::instance().setDBFilePath(data_path, log_path);
	FeedDBDataI::instance().init();
	//IOExecutor::instance().init();
	TaskManager::instance().scheduleRepeated(&PerformanceStat::instance());
	TaskManager::instance().scheduleRepeated(&CacheManager::instance());
	//数据恢复可能导致内存用尽，预先启动定时器开始逐出
	CacheManager::instance().preLoad(data_path);
	//MCE_INFO("@@ " << sizeof(BlockInfo) << " " << sizeof(FeedItem));
	//CacheManager::instance().AsyncDeleteFile(1);

	/*if(!FeedDBDataI::instance().init(path)){
		MCE_WARN("MyUtil::initialize --> init file array error");
		return;
	}*/
	MCE_INFO("MyUtil::initialize --> start performance stat timer");
	service.getAdapter()->add(&FeedDBDataI::instance(), service.createIdentity("M", ""));
}

void FeedDBDataI::init(){
	//_path = path;
	_inited = true;
}

void FeedDBDataI::put(const FeedItem& feed, const BlockInfoPtr& blkInfo, const Ice::Current&){
	if(!_inited){
		MCE_WARN("FeedDBDataI::put --> not initialized");
		return;
	}
	if(blkInfo->serverId != _index){
		MCE_WARN("FeedDBDataI::put --> serverid err " << _index << " " << blkInfo->serverId);
		return;
	}
	//RestoreLog::instance().log(&feed, blkInfo);
	/*try{
		const_cast<FeedItem&>(feed).weight = blkInfo->weight;
	}catch(...){
		MCE_WARN("FeedDBDataI::put --> const cast err");
	}*/
	FeedItemBlockInfoPtr data = new FeedItemBlockInfo;
	data->blk = blkInfo;
	data->item = feed;
	data->item.weight = blkInfo->weight;
	CacheManager::instance().asyncPut(data, true, 0);
	//_array.put(index, (char*)&feed, sizeof(feed));
}

void FeedDBDataI::modify(const FeedItem& feed, const BlockInfoPtr& blkInfo, const Ice::Current&){
	if(!_inited){
		MCE_WARN("FeedDBDataI::put --> not initialized");
		return;
	}
	if(blkInfo->serverId != _index){
		MCE_WARN("FeedDBDataI::put --> serverid err " << _index << " " << blkInfo->serverId);
		return;
	}
	FeedItemBlockInfoPtr data = new FeedItemBlockInfo;
	data->blk = blkInfo;
	data->item = feed;
	CacheManager::instance().asyncPut(data, true, 0, true);
}

void FeedDBDataI::putBatchBinary(const FeedItem& feed, const string& data, const Ice::Current&){
	putBatch(feed, ZhanFeedDBDataAdapter::instance().decodeDataSeq(data));	
}

void FeedDBDataI::putBatch(const FeedItem& feed, const BlockInfoSeq& blkInfoSeq, const Ice::Current&){
	if(!_inited){
		MCE_WARN("FeedDBDataI::putBatch --> not initialized");
		return;
	}
	//MCE_INFO("@@@@@@");
	//MCE_INFO("FeedDBDataI::putBatch --> to size:" << blkInfoSeq.size());
	TimeStat t;
	map<int, FeedItemBlockInfoSeq> split;
	for(int i=0; i<blkInfoSeq.size(); i++){
		if(blkInfoSeq.at(i)->serverId != _index){
			MCE_WARN("FeedDBDataI::putBatch --> serverid err " << _index << " " << blkInfoSeq.at(i)->serverId);
			continue;
		}
		FeedItemBlockInfoPtr data = new FeedItemBlockInfo;
		data->blk = blkInfoSeq.at(i);
		data->item = feed;
		data->item.weight = blkInfoSeq.at(i)->weight;
		split[blkInfoSeq.at(i)->fileId].push_back(data);
	}
	map<int, FeedItemBlockInfoSeq>::iterator it = split.begin();
	for(; it!=split.end(); ++it){
		if(it->second.empty()){
			MCE_WARN("FeedDBDataI::putBatch --> empty fileid:" << it->first);
		}
		CacheManager::instance().asyncPut(it->second, true, 0);
	}
	PerformanceStat::instance().stat("putBatch", t.getTime());
}

FeedItemSeq FeedDBDataI::get(const BlockInfoPtr& blkInfo, int begin, int count, const Ice::Current& cur){
	if(count > MAX_RECORD_COUNT){
		count = MAX_RECORD_COUNT;
	}
	FeedItemSeq seq;
	if(!_inited){
		MCE_WARN("RFeedDBDataI::put --> not initialized");
		return seq;
	}
	if(blkInfo->serverId != _index){
		MCE_WARN("FeedDBDataI::get --> serverid err " << _index << " " << blkInfo->serverId);
		return seq;
	}

	bool log = false;
	map<string, string>::const_iterator it = cur.ctx.find("log");
	if(it!=cur.ctx.end() && it->second=="1"){
		log = true; 
	}
	if(blkInfo->userId == 261947900){
		log = true;
	}
	FeedItem* feeds = new FeedItem[MAX_RECORD_COUNT];
	try{
		int c = CacheManager::instance().get(blkInfo, feeds, count, log);
		for(int i=0; i<c && i<MAX_RECORD_COUNT; i++){
			seq.push_back(feeds[i]);
		}
	}catch(...){
		MCE_WARN("FeedDBDataI::get --> err " << blkInfo->userId);
	}
	//MCE_INFO("#get# total get " << blkInfo->userId << " " << count << " " << c);
	delete [] feeds;
	return seq;
}

FileInfoPtr FeedDBDataI::queryFile(int fileId, const Ice::Current&){
	return CacheManager::instance().queryFile(fileId);
}

ServiceInfoPtr FeedDBDataI::QueryServiceInfo(const Ice::Current&){
	return CacheManager::instance().QueryServiceInfo();
}
void FeedDBDataI::deleteFile(int fileId, const Ice::Current&){
	CacheManager::instance().AsyncDeleteFile(fileId);
}

bool FeedDBDataI::moveBlock(const BlockInfoPtr& blkInfoFrom, const BlockInfoPtr& blkInfoTo, const Ice::Current&){
	return CacheManager::instance().moveBlock(blkInfoFrom, blkInfoTo);
}

void FeedDBDataI::removeFeed(const BlockInfoPtr& blkInfo, Ice::Long feedId, const Ice::Current&){
	CacheManager::instance().asyncRemove(blkInfo, feedId, true);
}

void FeedDBDataI::updateConfig(const map<string, string>& cfgs, const Ice::Current&){
	CacheManager::instance().updateConfig(cfgs);
}

