#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <boost/lexical_cast.hpp>
#include "Cache.h"
#include "IceLogger.h"
#include "PerformanceStat.h"
#include "IOExecutor.h"

#include "SSDFeedDBAdapter.h"
using namespace MyUtil;

///////////////////////////////////////////////////////
void CacheManager::preLoad(const string& path){
	struct stat     statbuf;
	dirent   *dirp;
	DIR	*dp;
	set<int> files;
	MCE_INFO("CacheManager::preLoad --> preload " << path);
	if ((dp = opendir(path.c_str())) == NULL){
		MCE_WARN("CacheManager::preLoad --> preload err, " << strerror(errno) << " " << path);
		return;
	}
	//int count = 0;
	while ((dirp = readdir(dp)) != NULL) {
		if((strcmp(dirp->d_name, ".") == 0) || (strcmp(dirp->d_name, "..") == 0)){
			continue;
		}
		string name = dirp->d_name;
		ostringstream os;
		os << path << "/" << name;
		int res = stat(os.str().c_str(), &statbuf);
		if(res!=0){
			MCE_WARN("CacheManager::preLoad --> stat err, " << strerror(errno));
			continue;
		}
		if(S_ISREG(statbuf.st_mode)){
			if(name.find("old") != string::npos){
				continue;
			}
			MCE_INFO("##@@ " << dirp->d_name);
			int fileId = 0;
			try{
				int pos = name.find(".");
				fileId = boost::lexical_cast<int>(name.substr(0, pos));
			}catch(...){
				MCE_WARN("CacheManager::preLoad --> lexical_cast err, " << strerror(errno));
			}
			if(fileId != 0){
				files.insert(fileId);
			}
		}
	}

	char* buf;
	if(posix_memalign((void**)&buf, 512, BLOCKSIZE*(size_t)BLOCKCOUNT)!=0){
		MCE_WARN("CacheManager::preLoad --> posix_memalign err");
		return;
	}
	size_t i = 0;
	for(set<int>::iterator it=files.begin(); it!=files.end(); ++it){
		DataFilePtr file = locateDataFile(*it);
		LogFilePtr log = locateLogFile(*it);
		if(!file || !log){
			continue;
		}
		size_t tmp = files.size()-i;
		if(tmp <= 4 && (MAX_CACHE-cacheSize() > tmp*BLOCKCOUNT)){
		//后面3个文件，如果内存够的话全加载
			size_t count = file->loadAllData(buf);
			/*if(count != (size_t)BLOCKCOUNT){
                                continue;
                        }*/
			file->LocateAll(buf, count);
			log->Restore(0);
		}else if(log->LogCount2Restore() > 10000){
		//如果回复的日志较大，全加载文件
			file->loadAllData(buf);
			/*if(count != (size_t)BLOCKCOUNT){
				continue;
			}*/
			log->Restore(buf);
		}else{
		//直接随机读恢复
			log->Restore(0);
		}
		i++;
	}
	free(buf);
	restoring_ = false;	
	//for test
	/*vector<void* > tmpSeq;
	for(int i=0; i<4000000; i++){
		void* p = MemPool::instance().GetObject();
		tmpSeq.push_back(p);
		//MemPool::instance().ReleaseObject(p);
	}
	for(int i=0; i<20; i++){
		DataFilePtr file = locateDataFile(i);
                LogFilePtr log = locateLogFile(i);
	}*/
}
void CacheManager::put(const FeedItemBlockInfoPtr& data, int flag, bool overwrite){
	BlockInfoPtr blk = data->blk;
	if(blk->userId <= 0){
		MCE_WARN("CacheManager::put--> invalid userid f:" << blk->fileId << " b:" << blk->blockIndex << " userid:" << blk->userId << " cc:" << blk->curCount
                << " preF:" << blk->preFileId << " preB:" << blk->preBlockIndex); 
	}
	if(blk->userId==261947900 || blk->userId==227805680){
		MCE_INFO("@put@ userid:"<<blk->userId << " node:" << blk->serverId <<" file:"<<blk->fileId<<" index:"<<blk->blockIndex
			<<" tc:"<<blk->totalCount<<" cc:"<<blk->curCount << " preF:" << blk->preFileId << " preB:" << blk->preBlockIndex
			<<" -- " << " feed:" << data->item.feed << " -- flag:" << flag);
	}
	PerformanceStat::instance().stat("put", 0);
	DataFilePtr file = locateDataFile(blk->fileId);
	if(!file){
		MCE_DEBUG("CacheManager::put --> can not locate file " << blk->fileId);
		return;
	}

	{
		IceUtil::Mutex::Lock lock(mutexs_[makeIndex(blk->fileId, blk->blockIndex)%MUTEX_COUNT]);
		BlockCachePtr cache = NULL;
		if(blk->curCount != 1){
			cache = file->Locate(blk->blockIndex);
		}else{
			//如果是第一次写，不需要从文件加载
			cache = file->Locate(blk->blockIndex, false);
		}
		if(!cache){
			return;
		}
		if(cache->GetHead()->user_id!=0 && (cache->GetHead()->user_id!=(UINT32)blk->userId)){
			MCE_WARN("CacheManager::put --> userid err, " << cache->GetHead()->user_id << " " << blk->userId << " f:" << blk->fileId << " b:" << blk->blockIndex << " flag:" << flag);
			return;
		}
		//特殊的数据，用来修改next和pre指向
		if(blk->curCount == -1){
			MCE_INFO("CacheManager::put --> change pre pointor user:" << cache->GetHead()->user_id << " f:" << cache->file_id() << " b:" << cache->index()
					<< " preF:" << blk->preFileId << " preB:" << blk->preBlockIndex);
			cache->GetHead()->pre_file_id = blk->preFileId;
			cache->GetHead()->pre_block_index = blk->preBlockIndex;
			//cache->dirty = true;
			file->set_dirty(cache->index());
			/*if(!cache->Valid()){
				MCE_WARN("CacheManager::put --> ch pre " << cache->Head2Str());
			}*/
			return;
		}else if(blk->curCount == -2){
			MCE_INFO("CacheManager::put --> change next pointor user:" << cache->GetHead()->user_id << " f:" << cache->file_id() << " b:" << cache->index()
					<< " nextF:" << blk->preFileId << " nextB:" << blk->preBlockIndex);
			cache->GetHead()->next_file_id = blk->preFileId;
			cache->GetHead()->next_block_index = blk->preBlockIndex;
			//cache->dirty = true;
			file->set_dirty(cache->index());
			/*if(!cache->Valid()){
				MCE_WARN("CacheManager::put --> ch next " << cache->Head2Str());
			}*/
			return;
		}


		if((UINT32)blk->curCount <= RECORD_PER_BLOCK){
			//MCE_INFO("@@@@@@");
			if(cache->PutData(blk, &(data->item), overwrite)){
				file->set_dirty(cache->index());
			}
		}

		if(blk->curCount == 1){
		//只有每一块的第一条记录会带有上一块的位置信息
			cache->GetHead()->pre_file_id = blk->preFileId;
			cache->GetHead()->pre_block_index = blk->preBlockIndex;
		}
	}
}

void CacheManager::syncPut(const FeedItemBlockInfoPtr& data, bool log, int flag){
	BlockInfoPtr blk = data->blk;
	if(log && locateDataFile(blk->fileId)){
		locateLogFile(blk->fileId)->Log(data);
	}
	
	put(data, flag);

	if(blk->curCount == 1){
		//更新上一块的next指针
		changeNextPtr(blk->userId, blk->preFileId, blk->preBlockIndex, blk->fileId, blk->blockIndex, true);
	}
}

void CacheManager::asyncPut(const FeedItemBlockInfoPtr& data, bool log, int flag, bool overwrite){
	//TimeStat t;
	BlockInfoPtr blk = data->blk;
	if(log && locateDataFile(blk->fileId)){
		//PerformanceStat::instance().incAsyncPut(t.getTime());
		locateLogFile(blk->fileId)->Log(data);
	}

	TaskManager::instance().execute(new PutTask(data, flag, overwrite));
	if(blk->curCount == 1){
		//�~[��~V���~J��~@�~]~W�~Z~Dnext�~L~G�~R~H
		changeNextPtr(blk->userId, blk->preFileId, blk->preBlockIndex, blk->fileId, blk->blockIndex, false);
	}
}

void CacheManager::asyncPut(const FeedItemBlockInfoSeq& data, bool log, int flag, bool overwrite){
	TimeStat t;
	int file_id = data.front()->blk->fileId;
	if(log && locateDataFile(file_id)){
		LogFilePtr log = locateLogFile(file_id);
		PerformanceStat::instance().stat("LockCost", t.getTime());
		t.reset();
		log->Log(data);
		PerformanceStat::instance().stat("LogCost", t.getTime());
	}

	t.reset();
	TaskManager::instance().execute(new PutTask(const_cast<FeedItemBlockInfoSeq&>(data), flag, overwrite));
	for(int i=0; i<data.size(); i++){
		BlockInfoPtr blk = data.at(i)->blk;
		//const_cast<FeedItem*>(data)->weight = blk->weight;	
		//TaskManager::instance().execute(new PutTask(data.at(i), flag));
		if(data.at(i)->blk->curCount == 1){
			//�~[��~V���~J��~@�~]~W�~Z~Dnext�~L~G�~R~H
			changeNextPtr(blk->userId, blk->preFileId, blk->preBlockIndex, blk->fileId, blk->blockIndex, false);
		}
	}
	PerformanceStat::instance().stat("PushTaskCost", t.getTime());
}

void CacheManager::changeNextPtr(int userId, int fileId, int blockIndex, int nextFileId, int nextBlockIndex, bool sync){
	if(fileId==0 || blockIndex==-1){
		return;
	}
	BlockInfoPtr tmp = new BlockInfo;
	tmp->userId = userId;
	tmp->totalCount = -2;
	tmp->curCount = -2;
	tmp->preFileId = nextFileId;
	tmp->preBlockIndex = nextBlockIndex;
	tmp->fileId = fileId;
	tmp->blockIndex = blockIndex;
	
	FeedItemBlockInfoPtr data = new FeedItemBlockInfo;
	data->blk = tmp;
	//FeedItem item;
	//memset(&item, 0, sizeof(FeedItem));
	if(sync){
		syncPut(data, true, 3);
	}else{
		asyncPut(data, true, 3);
	}
}

void CacheManager::changePrePtr(int userId, int fileId, int blockIndex, int preFileId, int preBlockIndex, bool sync){
	if(fileId==0 || blockIndex==-1){
		return;
	}
	BlockInfoPtr tmp = new BlockInfo;
	tmp->userId = userId;
	tmp->totalCount = -1;
	tmp->curCount = -1;
	tmp->preFileId = preFileId;
	tmp->preBlockIndex = preBlockIndex;
	tmp->fileId = fileId;
	tmp->blockIndex = blockIndex;
	
	FeedItemBlockInfoPtr data = new FeedItemBlockInfo;
	data->blk = tmp;
	//FeedItem item;
	//memset(&item, 0, sizeof(FeedItem));
	if(sync){
		syncPut(data, true, 4);
	}else{
		asyncPut(data, true, 4);
	}

}
int CacheManager::get(const BlockInfoPtr& blk, FeedItem* data, int count, bool log){

	if(log){
		MCE_INFO("CacheManager::get --> userid:"<<blk->userId<<" file:"<<blk->fileId<<" index:"<<blk->blockIndex
			<<" tc:"<<blk->totalCount<<" cc:"<<blk->curCount);
	}
	PerformanceStat::instance().stat("get", 0);

	//int blockCount = (blk->totalCount+RECORD_PER_BLOCK-1)/RECORD_PER_BLOCK;
	//MCE_INFO("#get# block count " << blockCount);
	//BlockCachePtr cs = new BlockCache[blockCount-1];

	int file_id = blk->fileId;
	int block_index = blk->blockIndex;
	//int pos = 0;
	int c = 0;
	for(int i=0; c<count; i++){
		IceUtil::Mutex::Lock lock(mutexs_[makeIndex(file_id, block_index)%MUTEX_COUNT]);
		if(file_id==0 || block_index==-1){
			break;
		}
		DataFilePtr file = locateDataFile(file_id);
		if(!file){
			MCE_DEBUG("#get# can not find file " << blk->userId << " " << file_id << " " << block_index);
			break;
		}
		BlockCachePtr cache = file->Locate(block_index);
		//BlockCachePtr cache = locateBlockCache(fileId, blockIndex);
		if(!cache){
			MCE_INFO("#get# can not find block "  << blk->userId << " " << block_index << " " << file_id);
			break;
		}
		//blk里的userId不为0时，不检查用户合法性，测试用
		if(blk->userId!=0 && (cache->GetHead()->user_id!=(UINT32)blk->userId)){
			//MCE_WARN("CacheManager::get --> userid err, " << cache->GetHead()->user_id << " " << blk->userId << " f:" << blk->fileId << " b:" << blk->blockIndex);
			break;
		}

		if(blk->userId==0 || log){
			//BlockHead* head = cache->GetHead();
			MCE_INFO("CacheManager::get --> " << cache->Head2Str());
			/*MCE_INFO("CacheManager::get --> userid:" << blk->userId << " f:" << file_id << " b:" << block_index << " cc:" 
				<< head->cur_count << " preF:" << head->pre_file_id << " preB:" << head->pre_block_index
				<< " nextF:" << head->next_file_id << " nextB:" << head->next_block_index);*/
		}
		int len = cache->GetData(data, count-c);
		data += len;
		c += len;
		file_id = cache->GetHead()->pre_file_id;
		block_index = cache->GetHead()->pre_block_index;
	}
	return c;
}

void CacheManager::removeAll(int userId, int fileId, int blockIndex, bool log){
	IceUtil::Mutex::Lock lock(mutexs_[makeIndex(fileId, blockIndex)%MUTEX_COUNT]);
	DataFilePtr file = locateDataFile(fileId);
	if(!file){
		return;
	}
	BlockCachePtr cache = file->Locate(blockIndex);
	if(!cache){
		return;
	}

	if(log){
		BlockInfoPtr tmp = new BlockInfo;
		tmp->userId = userId;
		tmp->totalCount = -3;
		tmp->curCount = -3;
		tmp->preFileId = 0;
		tmp->preBlockIndex = -1;
		tmp->fileId = fileId;
		tmp->blockIndex = blockIndex;
		FeedItemBlockInfoPtr data = new FeedItemBlockInfo;
		data->blk = tmp;
		//FeedItem item;
		//memset(&item, 0, sizeof(FeedItem));
	
		locateLogFile(fileId)->Log(data);
	}
	cache->RemoveAll();
}

void CacheManager::remove(const BlockInfoPtr& blk, long long feed, bool depth, bool log){
	PerformanceStat::instance().stat("remove", 0);
	if(!support_remove_cfg_){
		return;
	}
	int file_id = blk->fileId;
	int block_index = blk->blockIndex;
	bool deleteBlock = false;
	bool deleted = false;
	for(;;){
		if(file_id == 0 || block_index == -1){
			return;
		}
		IceUtil::Mutex::Lock lock(mutexs_[makeIndex(file_id, block_index)%MUTEX_COUNT]);
		DataFilePtr file = locateDataFile(file_id);
		if(!file){
			MCE_DEBUG("CacheManager::remove --> can not find file " << file_id << " " << block_index);
			return;
		}
		BlockCachePtr cache = file->Locate(block_index);
		//BlockCachePtr cache = locateBlockCache(fileId, blockIndex);
		if(!cache){
			MCE_WARN("remove --> can not find block " << file_id << " " << block_index);
			return;
		}
		if(blk->userId!=0 && cache->GetHead()->user_id != (UINT32)blk->userId){
			MCE_WARN("CacheManager::remove --> userid err, " << cache->GetHead()->user_id << " " << blk->userId 
			<< " f:" << blk->fileId << " b:" << blk->blockIndex << " feed:" << feed);
		}
		BlockHead* head = cache->GetHead();

		//用于测试，可以打印日志
		if(blk->userId==0){
			MCE_INFO("CacheManager::remove --> " << cache->Head2Str() << " -- feed:" << feed);
		}
		if(cache->Remove(feed)){
			deleted = true;
			//if(blk->userId==0){
			//	MCE_INFO("CacheManager::remove --> deleted:" << cache->Head2Str() << " -- feed:" << feed);
			//}
			if(cache->Empty()){
				if(head->next_file_id!=0 && head->next_block_index!=-1){
					deleteBlock = true;
				}
			}
			file->set_dirty(cache->index());
			break;
		}
		//cache->evict = true;
		file_id = head->pre_file_id;
		block_index = head->pre_block_index;
		if(!depth){
			break;
		}
	}
	MCE_INFO("CacheManager::remove --> user:" << blk->userId << " -- feed:" << feed << " deleted:" << deleted);
	if(deleted && log){
		DataFilePtr file = locateDataFile(file_id);
		if(!file){
			return;
		}
		BlockInfoPtr tmp = new BlockInfo;
		tmp->userId = blk->userId;
		tmp->fileId = file_id;
		tmp->blockIndex = block_index;
		//FeedItem item;
		//memset(&item, 0, sizeof(FeedItem));
		//item.feed = -feed;
		FeedItemBlockInfoPtr data = new FeedItemBlockInfo;
		data->blk = tmp;
		data->item.feed = -feed;
		locateLogFile(file_id)->Log(data);
	}
	if(deleteBlock){
		DataFilePtr file = locateDataFile(file_id);
		if(!file){
			return;
		}
		int userid = 0;
		int preFileId = 0;
		int preBlockIndex = 0;
		int nextFileId = 0;
		int nextBlockIndex = 0;
		{
			IceUtil::Mutex::Lock lock(mutexs_[makeIndex(file_id, block_index)%MUTEX_COUNT]);
			BlockCachePtr cache = file->Locate(block_index);
			if(!cache){
				return;
			}
			BlockHead* head = cache->GetHead();
			userid = head->user_id;
			preFileId = head->pre_file_id;
			preBlockIndex = head->pre_block_index;
			nextFileId = head->next_file_id;
			nextBlockIndex = head->next_block_index;
			MCE_INFO("CacheManager::remove --> delete block:" << cache->Head2Str());
		}
		/*
		*恢复的时候不用记log
		*1.恢复的时候，如果一块已经被删除，则只用修改当前删除的块的pre和next值，而不能去修改下一块的pre值，否则会导致加载
		*异常，例如当加载第1块的时候，发现其中一块被删除，而去修改第5快的pre值，那么5被加载，导致2，3，4加载异常
		*2.这里虽然没修改上一块的next和下一块的pre，但是修改的标志是在相应的日志文件里有的，所以也能正常删除
		*/
		if(log){
			changePrePtr(userid, nextFileId, nextBlockIndex, preFileId, preBlockIndex, true);
			changeNextPtr(userid, preFileId, preBlockIndex, nextFileId, nextBlockIndex, true);
		}
		changePrePtr(userid, file_id, block_index, 0, -1, true);
		changeNextPtr(userid, file_id, block_index, 0, -1, true);
	}
}

/*void CacheManager::syncRemove(const BlockInfoPtr& blk, long long feedId, bool log){
	if(log){
		FeedItem item;
		item.feed = -feedId;
		locateLogFile(blk->fileId)->Log(&item, blk);
	}
	remove(blk, feedId);
}*/

void CacheManager::asyncRemove(const BlockInfoPtr& blk, long long feedId, bool log){
	//FeedItem item;
	//memset(&item, 0, sizeof(FeedItem));
	//item.feed = -feedId;
	/*if(log){
		locateLogFile(blk->fileId)->Log(&item, blk);
	}*/
	FeedItemBlockInfoPtr data = new FeedItemBlockInfo;
	data->blk = blk;
	data->item.feed = -feedId;
	TaskManager::instance().execute(new PutTask(data, 0));
}

void CacheManager::deleteDataFile(int fileId){
	DataFilePtr file;
	{
		IceUtil::RWRecMutex::WLock lock(mutex_);
		map<int, DataFilePtr>::iterator it = files_.find(fileId);
		if(it != files_.end()){
			file = it->second;
			files_.erase(it);
		}
	}
	file->Delete();
}

void CacheManager::deleteLogFile(int fileId){
	LogFilePtr log;
	{
		IceUtil::RWRecMutex::WLock lock(mutex_);
        	map<int, LogFilePtr>::iterator it = logs_.find(fileId);
        	if(it != logs_.end()){
                	log = it->second;
                	logs_.erase(it);
        	}
	}
	log->Delete();
}

DataFilePtr CacheManager::locateDataFile(int fileId/*, int blockIndex, bool load*/){
	//MCE_DEBUG("#locate# " << fileId << " " << blockIndex << " key:" << makeKey(fileId, blockIndex));
	//DataFile* fileCache;
	//TimeStat t;
	bool newfile = false;
	{
		IceUtil::RWRecMutex::RLock lock(mutex_);
		//PerformanceStat::instance().incAsyncPut(t.getTime());
		newfile = (files_.find(fileId) == files_.end());
		if(!newfile){
			return files_[fileId];
		}
	}

	IceUtil::RWRecMutex::WLock lock(mutex_);
	newfile = (files_.find(fileId) == files_.end());
	if(!newfile){
		return files_[fileId];
	}
	//中间空的文件已被删除，如果有残留的数据还落在这个文件上，丢弃这部分数据，否则又导致删除的文件被重新加载
	//比如删除文件1的时候，后面的日志文件里可能会有记录，需要修改next指针的数据落在1上
	if(!files_.empty()){
		//int low = files_.begin()->first;
		int high = (--files_.end())->first;
		if(/*fileId>low &&*/ fileId<high){
			return 0;
		}
	}
	DataFilePtr fileCache = new DataFile(fileId, BLOCKSIZE);
	if(!fileCache->Init()){
		//fileCache->Release();
		return 0;
	}
	files_[fileId] = fileCache;
	return files_[fileId];
}

DataFilePtr CacheManager::findDataFile(int fileId/*, int blockIndex, bool load*/){
	//MCE_DEBUG("#locate# " << fileId << " " << blockIndex << " key:" << makeKey(fileId, blockIndex));
	IceUtil::RWRecMutex::RLock lock(mutex_);
	if(files_.find(fileId) == files_.end()){
		return 0;
	}
	return files_[fileId];
}

/*
DataFilePtr CacheManager::locateOldFile(){
	DataFilePtr fileCache;
	IceUtil::Mutex::Lock lock(mutex_);
	if(files_.empty()){
		return 0;
	}
	map<int, DataFilePtr>::iterator it = files_.begin();
	fileCache = it->second;
	int fileId = 0;
	if(fileCache->file_id() < 0){
		fileId = fileCache->file_id();
		if(!fileCache->full()){
			return fileCache;
		}
	}
	fileId--;
	fileCache = new DataFile(fileId, BLOCKSIZE);
	fileCache->Init(true);
	files_[fileId] = fileCache;
	return files_[fileId];
}*/

LogFilePtr CacheManager::locateLogFile(int fileId){
	LogFilePtr logger = 0;
	//bool load = false;
	bool newfile = false;
	{
		IceUtil::RWRecMutex::RLock lock(mutex_);
		newfile = (logs_.find(fileId) == logs_.end());
		if(!newfile){
			return logs_[fileId];
		}
	}

	
	IceUtil::RWRecMutex::WLock lock(mutex_);
	newfile = (logs_.find(fileId) == logs_.end());
	if(!newfile){
		 return logs_[fileId];
	}
	
	logger = new LogFile(fileId);
	if(!logger->Init()){
		return 0;
	}
	logs_[fileId] = logger;
	return logger;
}

void CacheManager::evict(){
	evicting_ = true;
}

void CacheManager::handle(){
	MCE_INFO("CacheManager::handle --> timer");
	//cacheSize() > MAX_CACHE
	static bool haveCleanBlk = true;
	if(!timer_begin_ && cacheSize() <= MAX_CACHE*0.5){
		return;
	}
	timer_begin_ = true;
	if(cacheSize() >= MAX_CACHE && haveCleanBlk){
		MCE_INFO("CacheManager::handle --> timer evict");
		if(!evictNoDirtyBlock((size_t)(MAX_CACHE*0.05))){
			haveCleanBlk = false;
		}
		return;
	}

	bool finished = flushFile(false);
	//if(cacheSize() > MAX_CACHE){
	//	timer_action_ = EVICT;
	//}
	if(finished){
		haveCleanBlk = true;
	}
	if(finished && timer_par_.delete_file_id_ != 0){
		MCE_INFO("CacheManager::handle --> timer delete beg " << timer_par_.delete_file_id_);
		//set<int> file_ids;
		try{
			int fileId = timer_par_.delete_file_id_;
			timer_par_.delete_file_id_ = 0;
			if(!fileCanDelete(fileId)){
				return;
			}
			if(fileId > 0){
				deleteFileKeepMin(fileId);
			}
			/*else{
				file_ids = deleteFileForce(fileId);
			}*/
			//file_ids = deleteFile(timer_par_.delete_file_id_);
		}catch(Ice::Exception& e){
			MCE_WARN("CacheManager::handle --> del file err, " << e);
		}catch(std::exception& e){
			MCE_WARN("CacheManager::handle --> del file err, " << e.what());
		}catch(...){
			MCE_WARN("CacheManager::handle --> del file err");
		}


		/*MCE_INFO("CacheManager::handle --> timer delete end " << timer_par_.delete_file_id_ << " affacted file:" << *file_ids.begin());
		for(set<int>::iterator it=file_ids.begin(); it!=file_ids.end(); ++it){
			timer_par_.flushing_file_ids_.push_back(*it);
		}
		MCE_INFO("CacheManager::handle --> timer delete end " << timer_par_.delete_file_id_ << " affacted file:" << timer_par_.flushing_file_ids_.front());
		*/
	}
}

bool CacheManager::flushFile(bool evict){
	vector<BlockCachePtr> seq;
	vector<BlockCachePtr> noDirtys;
	//timeval beg, end;
	//bool evicting = evict;
	static int fileId = 0;//file_id_flushed_;
	static int blockIndex = 0;
	static int sequenceFlushFileId = 0;
	static bool sequence = false;
	static int idleTime = 60;
	//gettimeofday(&beg, NULL);
	for(;;){
		DataFilePtr file;
		int newFileId = 0;
		{
			IceUtil::RWRecMutex::RLock lock(mutex_);
			TimeStat t;
			map<int, DataFilePtr>::iterator it = files_.find(fileId);
			if(it == files_.end()){
				//MCE_INFO("CacheManager::flushFile --> find flushing file " << fileId);
				blockIndex = 0;

				if(!timer_par_.flushing_file_ids_.empty()){
					fileId = timer_par_.flushing_file_ids_.front();
					timer_par_.flushing_file_ids_.pop_front();
					MCE_INFO("CacheManager::flushFile --> flushing specified file " << fileId);
				}

				if(fileId==0 && !restoring_){
					fileId = getMaxSizeLogFile();
					newFileId = fileId;
					if(fileId != 0){
						MCE_INFO("CacheManager::flushFile --> flush max file " << fileId);
					}
				}

				if((fileId == 0) && (idleTime>=10)){
					idleTime = 0;
					map<int, DataFilePtr>::iterator tmpIt = files_.find(sequenceFlushFileId);
					if(tmpIt!=files_.end()){
						//sequenceFlushFileId = it->first;
						fileId = (tmpIt)->first;
					}else if(files_.size() > 1){
						fileId = files_.begin()->first;
					}
					if(fileId != 0){
						sequence = true;
						sequenceFlushFileId = fileId;
						newFileId = fileId;
						MCE_INFO("CacheManager::flushFile --> sequence flushing file " << fileId);
					}
				}
			}
		}
		if(fileId == 0){
			//MCE_INFO("CacheManager::handle @@@--> flush f:" << fileId << " b:" << blockIndex);
			idleTime++;
			return true;
		}
		MCE_INFO("#### @@ timer handle flush " << fileId << " " << blockIndex);
		file = locateDataFile(fileId);
		if(!file){
			MCE_INFO("CacheManager::flushFile --> can not locate file " << fileId);
			return true;
		}
		if(fileId!=0 && blockIndex==0){
			locateLogFile(fileId)->CreateNewFile();
		}
		if(fileId!=0 && blockIndex >= BLOCKCOUNT){
			if(fileId != 0){
				locateLogFile(fileId)->DeleteOldFile();
				locateDataFile(fileId)->FlushHead();
			}
			fileId = 0;
			if(sequence){
				int tmp = sequenceFlushFileId;
				map<int, DataFilePtr>::iterator it = files_.find(sequenceFlushFileId);
				++it;
				if(it!=files_.end()){
					sequenceFlushFileId = it->first;
				}else if(!files_.empty()){
					sequenceFlushFileId = files_.begin()->first;
				}
				MCE_INFO("CacheManager::flushFile --> next sequence flushing file " << tmp << " " << sequenceFlushFileId << " b:" << blockIndex);
				sequence = false;
			}
			MCE_INFO("CacheManager::flushFile --> flushing file change" << fileId << " b:" << blockIndex);
			//MCE_INFO("#### @@ timer handle flush " << fileId << " " << blockIndex);
			//blockIndex = 0;
			return true;
		}

		//MCE_INFO("CacheManager::handle --> flush f:" << fileId << " b:" << blockIndex);
		bool flushed = file->Flush(blockIndex, flush_count_cfg_, &ioExe_);
		blockIndex += flush_count_cfg_;
		if(flushed){
			break;
		}
	}
	return false;
}

bool CacheManager::evictNoDirtyBlock(size_t count){
	size_t evictCount = count;
	/*if(cacheSize() > MAX_CACHE){
		//逐出5%
		evictCount = (int)(0.05*MAX_CACHE);
	}*/

	//int tmp = evictCount;
	timeval beg, end;
	//int checkCount = 0;
	int fileId = 0;
	DataFilePtr file = 0;
	{
		IceUtil::RWRecMutex::RLock lock(mutex_);
		if(files_.empty()){
			return true;
		}
		fileId = files_.begin()->first;
		file = files_.begin()->second;
	}
	//bool finish = false;
	gettimeofday(&beg, NULL);
	for(;;){
		int tmp = file->Evict(evictCount);
		evictCount -= tmp;
		gettimeofday(&end, NULL);
		float elipse = ((end.tv_sec-beg.tv_sec)*1000000 + end.tv_usec-beg.tv_usec)/1000.0;
		MCE_INFO("CacheManager::evict --> evict from " << fileId << " evict:" << tmp << " ec:" << evictCount << " cache:" << cacheSize() << " t:" << elipse);
		gettimeofday(&beg, NULL);
		if(evictCount <= 0){
			break;
		}
		//if(finish){
		//	break;
		//}
		IceUtil::RWRecMutex::RLock lock(mutex_);
		map<int, DataFilePtr>::iterator it = files_.find(fileId);
		it++;
		if(it!=files_.end()){
			fileId = it->first;
			file = it->second;
		}else{
			break;
			return false;
		}
	}
	return true; 
	//gettimeofday(&end, NULL);
	//float elipse = ((end.tv_sec-beg.tv_sec)*1000000 + end.tv_usec-beg.tv_usec)/1000.0;
	//MCE_INFO("CacheManager::evict --> timer evict from " << " f:" << fileId << " " << elipse << " evict:" << tmp << " " << evictCount << " cache:" << cacheSize());
}

FileInfoPtr CacheManager::queryFile(int fileId){
	FileInfoPtr fileInfo = new FileInfo;
	fileInfo->inCacheBlockCount = 0;
	fileInfo->dirtyBlockCount = 0;
	fileInfo->fileId = fileId;
	DataFilePtr file = findDataFile(fileId);
	if(file){
		fileInfo->inCacheBlockCount = file->block_in_cache();
		fileInfo->dirtyBlockCount = file->block_dirty();
	}
	return fileInfo;
}

bool CacheManager::moveBlock(const BlockInfoPtr& blkInfoFrom, const BlockInfoPtr& blkInfoTo){
	return true;
}

void CacheManager::updateConfig(const map<string, string>& cfgs){
	map<string, string>::const_iterator it;
	if((it = cfgs.find("cache_size_cfg")) != cfgs.end()){
		int tmp = 0;
		try{
			tmp = boost::lexical_cast<int>(it->second);
		}catch(...){
		}
		if(tmp>0 && tmp%1000000==0 && tmp<50000000){
			cache_size_cfg_ = tmp;
		}
	}
	if((it = cfgs.find("flush_count_cfg")) != cfgs.end()){
		int tmp = 0;
		try{
			tmp = boost::lexical_cast<int>(it->second);
		}catch(...){
		}
		if(tmp>0 && tmp%1000==0 && tmp<=50000){
			flush_count_cfg_ = tmp;
		}
	}
	if((it = cfgs.find("max_log_size_cfg")) != cfgs.end()){
		int tmp = 0;
		try{
			tmp = boost::lexical_cast<int>(it->second);
		}catch(...){
		}
		if(tmp>0 && tmp%(1000000)==0){
			max_log_size_cfg_ = tmp;
		}
	}
	if((it = cfgs.find("support_remove_cfg")) != cfgs.end()){
		try{
			support_remove_cfg_ = boost::lexical_cast<int>(it->second);
		}catch(...){
		}
	}

	MCE_INFO("CacheManager::updateConfig --> cachesize:" << cache_size_cfg_ << " flushcount:" 
		<< flush_count_cfg_ << " logsize:" << max_log_size_cfg_ << " remove:" << support_remove_cfg_);
}

unsigned int CacheManager::cacheSize(){
	IceUtil::RWRecMutex::RLock lock(mutex_);
	map<int, DataFilePtr>::iterator it = files_.begin();
	int size = 0;
	for(; it!=files_.end(); ++it){
		size += it->second->block_in_cache();
	}
	/*if(size > _cache_size_cfg){
		_evicting = true;
	}else{
		_evicting = false;
	}*/
	return size;
}

int CacheManager::getMaxSizeLogFile(){
	//IceUtil::Mutex::Lock lock(_mutex);
	if(logs_.size() < 2){
		return 0;
	}
	map<int, LogFilePtr>::iterator it = logs_.begin();
	Ice::Long max = max_log_size_cfg_;
	int fileId = 0;
	for(; it!=logs_.end(); ++it){
		Ice::Long size = it->second->Size();
		if(size > max){
			max = size;
			fileId = it->first;
		}
	}
	return fileId;
}

int CacheManager::getMaxSizeCacheFile(){
	map<int, DataFilePtr>::iterator it = files_.begin();
	int max = 0;
	int fileId = 0;
	for(; it!=files_.end(); ++it){
		int size = it->second->block_in_cache();
		if(size > max){
			max = size;
			fileId = it->first;
		}
	}
	return fileId;
}

bool CacheManager::fileCanDelete(int fileId){
	IceUtil::RWRecMutex::RLock lock(mutex_);
	map<int, DataFilePtr>::iterator it = files_.begin();
	map<int, DataFilePtr>::iterator pIt = files_.end();
	map<int, DataFilePtr>::iterator nIt = files_.end();
	for(; it!=files_.end(); ++it){
		if(it->first > 0){
			pIt = it;
			break;
		}
		if(it->first < 0){
			nIt = it;
		}
	}
	
	if(fileId > 0){
		if(pIt!=files_.end() && pIt->first==fileId && pIt!=(--files_.end())){
			return true;
		}
	}else if(fileId < 0){
		if(nIt!=files_.end() && nIt->first==fileId && nIt!=files_.begin()){
			return true;
		}
	}
	return false;
}

void CacheManager::decTotalCount(const UserFeedCountSeq& userFeedCounts){
	for(int trycount=0; trycount<10; trycount++){
		try{
			MCE_INFO("CacheManager::decTotalCount --> dec total count " << userFeedCounts.size());
			SSDFeedDBMasterAdapter::instance().decTotalCount(userFeedCounts);
			break;
		}catch(Ice::Exception& e){
			MCE_WARN("CacheManager::decTotalCount --> dec total count err " << trycount << " size:" << userFeedCounts.size() << " " << e);
			sleep(5);
		}
	}
}

BlockInfoSeq CacheManager::getBlockInfoBatch(const IntSeq& ids){
	BlockInfoSeq blkInfos;
	for(int trycount=0; trycount<10; trycount++){
		try{
			blkInfos = SSDFeedDBMasterAdapter::instance().getBlockBatch(ids);
			break;
		}catch(Ice::Exception& e){
			MCE_WARN("CacheManager::getBlockInfoBatch --> " << trycount << " ids:" << ids.size() << " " << e);
			sleep(5);
		}
	}
	MCE_INFO("CacheManager::getBlockInfoBatch --> getBlocks " << ids.size() << " " << blkInfos.size());
	return blkInfos;
}

void CacheManager::checkBlocks(const BlockInfoSeq& blkInfos, const vector<BlockCache*>& caches,
			vector<BlockCache*>& blksToDelete, vector<int>& userIdsToNew, vector<BlockCache*>& blksToNew, vector<BlockCache*>& blksToMove)
{
	for(size_t i=0; i<blkInfos.size(); i++){
		BlockInfoPtr blkInfo = blkInfos.at(i);
		BlockCachePtr c = caches.at(i);
		BlockHead* h = c->GetHead();
		if(!blkInfo){
			MCE_WARN("CacheManager::checkBlocks --> blk not exist " << h->user_id);
			continue;
		}
		if((unsigned int)blkInfo->userId != h->user_id){
			MCE_WARN("CacheManager::checkBlocks --> userid err " << blkInfo->userId << " " << h->user_id);
			continue;
		}
		if((blkInfo->totalCount > 1000 && h->pre_file_id==0 && h->pre_block_index==-1)){
		//可以直接删除的，可能还存在更老的块
			blksToDelete.push_back(c);
		}else if(c->GetHead()->cur_count == 0){
		//用户删除，用户删除的时候指针关系已经被修改，直接丢弃
			MCE_INFO("CacheManager::checkBlocks--> delete block:" << c->Head2Str());
		}else if(blkInfo->fileId == c->file_id() && blkInfo->blockIndex == c->index()){
		//分配新块
			userIdsToNew.push_back(blkInfo->userId);
			blksToNew.push_back(c);
		}else{
		//移动到老的文件
			blksToMove.push_back(c);
		}
	}
}

size_t CacheManager::deleteFilePrepare(int fileId){
	DataFilePtr file = locateDataFile(fileId);
	if(!file){
		MCE_WARN("CacheManger::deleteFilePrepare --> no file " << fileId);
		return 0;
	}
	
	MCE_INFO("CacheManager::deleteFilePrepare --> begin evict " << fileId);
	if((MAX_CACHE - cacheSize()) < BLOCKCOUNT){
		evictNoDirtyBlock(BLOCKCOUNT);
	}
	
	char* buf;
	if(posix_memalign((void**)&buf, 512, BLOCKSIZE*(size_t)BLOCKCOUNT)!=0){
		MCE_WARN("CacheManager::deleteFilePrepare --> posix_memalign err");
		return 0;
	}
	size_t count = file->loadAllData(buf);
	file->LocateAll(buf, count);
	free(buf);
	return count;
}

void CacheManager::deleteBlocks(const vector<BlockCache*>& blksToDelete){
	UserFeedCountSeq userFeedCounts;
	for(size_t i=0; i<blksToDelete.size(); i++){
		BlockHead* head = blksToDelete.at(i)->GetHead();
		UserFeedCountPtr feedCount = new UserFeedCount;
		feedCount->userId = head->user_id; 
		feedCount->count = RECORD_PER_BLOCK; 
		userFeedCounts.push_back(feedCount);
		if(head->next_file_id==0 || head->next_block_index < 0){
			continue;
		}
		//修改下一块的pre指针
		changePrePtr(head->user_id, head->next_file_id, head->next_block_index, 0, -1, true);
	}
	decTotalCount(userFeedCounts);	
	//MCE_INFO("CacheManager::deleteBlocks --> delete " << blksToDelete.size());

}

void CacheManager::newBlocks(const vector<BlockCache*>& blksToNew, const vector<int>& userIdsToNew){
	if(userIdsToNew.empty()){
		return;
	}
	if(blksToNew.size() != userIdsToNew.size()){
		MCE_INFO("CacheManager::newBlocks --> something wrong " << blksToNew.size() << " " << userIdsToNew.size());
		return;
	}
	BlockInfoSeq newBlkInfos;
	for(int trycount=0; trycount<10; trycount++){
		try{
			newBlkInfos = SSDFeedDBMasterAdapter::instance().newBlockBatch(userIdsToNew);
			break;
		}catch(Ice::Exception& e){
			MCE_WARN("CacheManager::newBlocks --> new block err " << trycount << " size:" << userIdsToNew.size() << " " << e);
			sleep(5);
		}
	}
	MCE_INFO("CacheManager::newBlocks --> new " << newBlkInfos.size() << " " << userIdsToNew.size());
	for(size_t i=0; i<newBlkInfos.size(); i++){
		BlockInfoPtr newBlkInfo = newBlkInfos.at(i);
		BlockCachePtr c = blksToNew.at(i);
		if(c->GetHead()->user_id != (unsigned int)newBlkInfo->userId){
			MCE_WARN("CacheManager::newBlocks --> userid err when new blk " << newBlkInfo->userId << " " << c->GetHead()->user_id);
			continue;
		}
		
		BlockInfoPtr tmp = new BlockInfo;
		tmp->userId = c->GetHead()->user_id;
		tmp->totalCount = 0;
		tmp->curCount = 0;
		tmp->preFileId = c->GetHead()->pre_file_id;
		tmp->preBlockIndex = c->GetHead()->pre_block_index;
		tmp->fileId = newBlkInfo->fileId;
		tmp->blockIndex = newBlkInfo->blockIndex;

		vector<FeedItem* > items = c->GetFeedItems();
		FeedItemBlockInfoPtr data = new FeedItemBlockInfo;
		for(size_t j=0; j<items.size(); j++){
			tmp->totalCount++;
			tmp->curCount++;
			data->blk = tmp;
			data->item = *(items.at(j));
			syncPut(data, true, 5);
		}
		changeNextPtr(tmp->userId, tmp->preFileId, tmp->preBlockIndex, newBlkInfo->fileId, newBlkInfo->blockIndex, true);
		MCE_INFO("CacheManager::newBlocks --> userid:" << tmp->userId << " preF:" << c->file_id() << " preB:" << c->index()
				<< " newF:" << tmp->fileId << " newB:" << tmp->blockIndex);
	}
	//MCE_INFO("CacheManager::newBlocks --> new " << newBlkInfos.size() << " " << blksToNew.size());
}

/*void CacheManager::moveBlocks(int fileId, const vector<BlockCache*>& blksToMove, vector<BlockCache*>& need2Rectify){
	for(size_t i=0; i<blksToMove.size(); i++){
		BlockHead* head = blksToMove.at(i)->GetHead();//c->GetHead();
		if(head->cur_count != 0){
			DataFilePtr old_file = locateDataFile(-fileId);
			BlockCachePtr c = old_file->GetFreeBlock();

			BlockInfoPtr tmp = new BlockInfo;
			tmp->userId = blksToMove.at(i)->GetHead()->user_id;
			tmp->totalCount = 0;
			tmp->curCount = 0;
			tmp->preFileId = blksToMove.at(i)->GetHead()->pre_file_id;
			tmp->preBlockIndex = blksToMove.at(i)->GetHead()->pre_block_index;
			tmp->fileId = c->file_id();
			tmp->blockIndex = c->index();
			vector<FeedItem* > items = blksToMove.at(i)->GetFeedItems();
			//移动到新的块
			for(size_t j=0; j<items.size(); j++){
				tmp->totalCount++;
				tmp->curCount++;
				syncPut(tmp, items.at(j), true, 6);
			}
			//修改新块的next指针
			changeNextPtr(blksToMove.at(i)->GetHead()->user_id,
			c->file_id(), c->index(),
			blksToMove.at(i)->GetHead()->next_file_id, blksToMove.at(i)->GetHead()->next_block_index, true);
			
			if(head->next_file_id == 0 || head->next_block_index==-1){
				MCE_WARN("CacheManager::moveBlocks --> need rectify blk, " << blksToMove.at(i)->Head2Str());
				//当移动一块正好满的块时，master里已经分配新块，但是data还不知道，这样可能导致被移动的块丢失,必须在删除文件完成后进行纠正
				need2Rectify.push_back(c);
				//need2RectifyIds.push_back(c->GetHead()->user_id);
				continue;
			}
			//修改next块的pre指针
			changePrePtr(head->user_id, head->next_file_id, head->next_block_index, c->file_id(), c->index(), true);

		}else{
			MCE_WARN("CacheManager::moveBlocks --> err blk, " << blksToMove.at(i)->Head2Str());
		}
	}
	//MCE_INFO("CacheManager::moveBlocks--> move " << blksToMove.size());
}
*/
set<int> CacheManager::deleteFileForce(int fileId){
	set<int> files;
	/*if(fileId>0 || !fileCanDelete(fileId)){
		MCE_WARN("CacheManager::deleteFileForce --> the file can not be deleted " << fileId);
		return files;
	}*/
	size_t count = deleteFilePrepare(fileId);
	MCE_INFO("CacheManager::deleteFileForce --> delete " << fileId << " c:" << count);
	DataFilePtr file = locateDataFile(fileId);
	if(!file){
		return files;
	}
	UserFeedCountSeq userFeedCounts;
	for(size_t index=0; index<count; index++){
		BlockCache* cache = file->Get(index);
		if(!cache){
			continue;
		}
		BlockHead* head = cache->GetHead();
		UserFeedCountPtr feedCount = new UserFeedCount;
		feedCount->userId = head->user_id;
		feedCount->count = RECORD_PER_BLOCK;
		userFeedCounts.push_back(feedCount);
		changePrePtr(head->user_id, head->next_file_id, head->next_block_index, 0, -1, true);
		if(index>0 && index%10000==0){
			MCE_INFO("CacheManager::deleteFileForce --> delete " << fileId << " c:" << count << " deleted:" << index);
			decTotalCount(userFeedCounts);	
			userFeedCounts.clear();
		}
	}
	MCE_INFO("CacheManager::deleteFileForce --> delete " << fileId << " c:" << count << " deleted:" << count);
	return files;
}

/*
set<int> CacheManager::deleteFile(int fileId){
	//MCE_INFO("CacheManager::deleteFile --> " << fileId);
	timer_par_.delete_file_id_ = 0;
	int delete_blk_count = 0;
	int move_blk_count = 0;
	int new_blk_count = 0;
	
	set<int> files;
	
	//if(!fileCanDelete(fileId)){
	//	MCE_WARN("CacheManager::deleteFile --> the file can not be deleted " << fileId);
	//	return files;
	//}
	//map<int, BlockCache*> users;
	vector<BlockCache*> caches;
	vector<BlockCache*> need2Rectify;
	vector<int> need2RectifyIds;
	MyUtil::IntSeq ids;
	MyUtil::IntSeq blocks;
	map<int, int> user_count;
	
	size_t count = deleteFilePrepare(fileId);

	DataFilePtr file = locateDataFile(fileId);
	if(!file){
		MCE_WARN("CacheManger::deleteFile --> no file " << fileId);
		return files;
	}
	size_t index = 0;
	for(; index<count; index++){
		//每10000块批量查一次master
		if((caches.size()==10000) || (index == count-1)){
			MCE_INFO("CacheManager::deleteFile --> " << fileId << " " << index);
			BlockInfoSeq blkInfos = getBlockInfoBatch(ids);
			vector<BlockCache*> blksToMove;
			vector<int>	userIdsToNew;
			vector<BlockCache*> blksToDelete;
			vector<BlockCache*> blksToNew;
			UserFeedCountSeq userFeedCounts;
			checkBlocks(blkInfos, caches, blksToDelete, userIdsToNew, blksToNew, blksToMove);
			delete_blk_count += blksToDelete.size();
			move_blk_count += blksToMove.size();
			new_blk_count += blksToNew.size();
	
			//delete block
			deleteBlocks(blksToDelete);
			MCE_INFO("CacheManager::deleteFile --> delete " << index << " " << blksToDelete.size());	
			//new  block
			newBlocks(blksToNew, userIdsToNew);
			MCE_INFO("CacheManager::deleteFile --> new " << index << " " << blksToNew.size());	
	
			//move to new block
			moveBlocks(fileId, blksToMove, need2Rectify);
			MCE_INFO("CacheManager::deleteFile --> move " << index << " " << blksToMove.size());	
			
			//MCE_INFO("CacheManager::deleteFile --> " << index << " f:" << fileId << " del:" << blksToDelete.size() << " new:" 
			//		<< blksToNew.size() << " move:" << blksToMove.size() << " caches:" << file->block_in_cache());
			//及时释放内存	
			for(size_t i=0; i<caches.size(); i++){
				BlockCache* c = caches.at(i);
				IceUtil::Mutex::Lock lock(mutexs_[makeIndex(c->file_id(), c->index())%MUTEX_COUNT]);			
				file->Del(c->index());
				c->Release();
			}
			caches.clear();
			ids.clear();
		}
		BlockCache* blk = file->Get(index);
		if(!blk){
			continue;
		}
		if(!blk->HasData()){
			MCE_WARN("CacheManger::deleteFile --> has no data " << blk->Head2Str());
			continue;
		}
		//users[blk->GetHead()->user_id] = blk;
		caches.push_back(blk);
		ids.push_back(blk->GetHead()->user_id);
	}

	//修正临界块
	rectifyBlocks(fileId, need2Rectify);

	MCE_INFO("CacheManager::deleteFile --> total " << index << " f:" << fileId << " del:" << delete_blk_count << " new:" << new_blk_count << " move:" << move_blk_count
			<< " caches:" << file->block_in_cache());
	deleteDataFile(fileId);
	deleteLogFile(fileId);
	files.insert(-fileId);
	return files;
}
*/

void CacheManager::rectifyBlocks(int fileId, const vector<BlockCache*>& need2Rectify){
	MCE_INFO("CacheManager::retifyBlocks --> need to rectify " << need2Rectify.size());
	BlockInfoSeq blkInfos;
	vector<int> tmpSeq;
	for(size_t i=0; i<need2Rectify.size(); i++){
		BlockCache* cache = need2Rectify.at(i);
		BlockHead* head = cache->GetHead();
		tmpSeq.push_back(head->user_id);
		if((tmpSeq.size()==10000) || (i==need2Rectify.size()-1)){
			BlockInfoSeq tmpBlkInfos = getBlockInfoBatch(tmpSeq);
			for(size_t j=0; j<tmpBlkInfos.size(); j++){
				blkInfos.push_back(tmpBlkInfos.at(j));
			}
		}
		tmpSeq.clear();
	}
	MCE_INFO("CacheManager::retifyBlocks --> rectify getBlockBatch blks:" << need2Rectify.size() << " info:" << blkInfos.size());
	if(blkInfos.size() != need2Rectify.size()){
		return;
	}
	for(size_t i=0; i<need2Rectify.size(); i++){
		BlockInfoPtr blkInfo = blkInfos.at(i);
		BlockCache* c = need2Rectify.at(i);
		if(c->GetHead()->user_id != (unsigned int)blkInfo->userId){
			MCE_WARN("CacheManager::retifyBlocks --> rectify id err " << blkInfo->userId << " " << c->GetHead()->user_id);
			continue;
		}
		if(blkInfo->preFileId!=fileId){
			MCE_WARN("CacheManager::retifyBlocks --> rectify something err " << blkInfo->userId << " preF:" << blkInfo->preFileId);
                        continue;
		}
		changePrePtr(blkInfo->userId, blkInfo->fileId, blkInfo->blockIndex, c->file_id(), c->index(), true);
	}
	MCE_INFO("CacheManager::rectifyBlocks --> rectify " << need2Rectify.size() << " " << blkInfos.size());

}
void CacheManager::AsyncDeleteFile(int file_id){
	//timer_action_ = DELETE_FILE;
	timer_par_.delete_file_id_ = file_id;
}

ServiceInfoPtr CacheManager::QueryServiceInfo(){
	ServiceInfoPtr si = new ServiceInfo;
	vector<int> ids;
	{
		IceUtil::RWRecMutex::RLock lock(mutex_);
		map<int, DataFilePtr>::iterator it = files_.begin();
		for(; it!=files_.end(); ++it){
			ids.push_back(it->first);
		}
	}
	for(size_t i=0; i<ids.size(); i++){
		FileInfoPtr f = queryFile(ids.at(i));
		si->fileSeq.push_back(f);
	}
	si->caches = cacheSize();
	return si;
}

void CacheManager::deleteFileKeepMin(int fileId){
	int delete_blk_count = 0;
	int move_blk_count = 0;
	int new_blk_count = 0;
	
	vector<BlockCache*> caches;
	MyUtil::IntSeq ids;
	MyUtil::IntSeq blocks;
	map<int, int> user_count;
	
	size_t count = deleteFilePrepare(fileId);

	DataFilePtr file = locateDataFile(fileId);
	if(!file){
		MCE_WARN("CacheManger::deleteFile --> no file " << fileId);
		return;
	}
	size_t index = 0;
	//=count 需要最后确认一下
	for(; index<=count; index++){
		//每10000块批量查一次master
		if((caches.size()==10000) || (index==count && !caches.empty())){
			MCE_INFO("CacheManager::deleteFile --> " << fileId << " " << index);
			BlockInfoSeq blkInfos = getBlockInfoBatch(ids);
			vector<BlockCache*> blksToMove;
			vector<BlockCache*> blksToMove1;
			vector<int>	userIdsToMove;	

			vector<BlockCache*> blksToNew;
			vector<int>	userIdsToNew;

			vector<BlockCache*> blksToDelete;
			UserFeedCountSeq userFeedCounts;
			
			for(size_t i=0; i<blkInfos.size(); i++){
				BlockInfoPtr blkInfo = blkInfos.at(i);
				BlockCachePtr c = caches.at(i);
				BlockHead* h = c->GetHead();
				if(!blkInfo){
					MCE_WARN("CacheManager::deleteFileKeepMin --> blk not exist " << h->user_id);
					continue;
				}
				if((unsigned int)blkInfo->userId != h->user_id){
					MCE_WARN("CacheManager::deleteFileKeepMin --> userid err " << blkInfo->userId << " " << h->user_id);
					continue;
				}

				if((blkInfo->fileId == c->file_id() && blkInfo->blockIndex == c->index())){
				//分配新块，并移动
					userIdsToNew.push_back(blkInfo->userId);
					blksToNew.push_back(c);	
				}else if(h->next_file_id == blkInfo->fileId && h->next_block_index == blkInfo->blockIndex){
				//保证每个用户至少30条新鲜事，其实已经超过30了
					DataFilePtr tmpFile = locateDataFile(blkInfo->fileId);
					if(!file){
						blksToDelete.push_back(c);
						continue;
					}
					BlockCache* tmpCache = tmpFile->Locate(blkInfo->blockIndex);
					if(!tmpCache){
						blksToDelete.push_back(c);
						continue;	
					}
					blksToMove.push_back(c);
					blksToMove1.push_back(tmpCache);
					userIdsToMove.push_back(blkInfo->userId);

				}else{
				//可以直接删除的
					blksToDelete.push_back(c);
				}		
			}
					
	
			delete_blk_count += blksToDelete.size();
			move_blk_count += blksToMove.size();
			new_blk_count += blksToNew.size();
	
			//delete block
			/*
			*1.不修改totalcount值，因为当用户setallread的时候，删除老的块时是不该减的，但无法判断
			*2.不修改下一块的pre值，因为随机读操作，导致删除很慢。已经删除的文件id是不能被locate到的，所以不修改pre值为空是没问题的
			*/
			//deleteBlocks(blksToDelete);

			MCE_INFO("CacheManager::deleteFile --> delete " << index << " " << blksToDelete.size());	
			//new  block
			newBlocks(blksToNew, userIdsToNew);
			MCE_INFO("CacheManager::deleteFile --> new " << index << " " << blksToNew.size());	
	
			//move to new block
			newBlocks(blksToMove1, userIdsToMove);
			for(size_t i=0; i<blksToMove.size(); i++){
				BlockCache* c1 = blksToMove.at(i);
				BlockCache* c2 = blksToMove1.at(i);
				if(c1 && c2 && (c1->GetHead()->user_id == c2->GetHead()->user_id)){
					
					removeAll(c2->GetHead()->user_id, c2->file_id(), c2->index(), true);
					
					BlockInfoPtr tmp = new BlockInfo;
					tmp->userId = c1->GetHead()->user_id;
					tmp->totalCount = 0;
					tmp->curCount = 0;
					tmp->preFileId = c1->GetHead()->pre_file_id;
					tmp->preBlockIndex = c1->GetHead()->pre_block_index;
					tmp->fileId = c2->file_id();
					tmp->blockIndex = c2->index();

					vector<FeedItem* > items = c1->GetFeedItems();
					FeedItemBlockInfoPtr data = new FeedItemBlockInfo;
					MCE_INFO("CacheManager::deleteFile --> move " << tmp->userId <<" size:" << items.size() 
					<< " from f:" << c1->file_id() << " b:" <<c1->index()
					<< " to f:" << c2->file_id() << " b:" << c2->index());
					for(size_t j=0; j<items.size(); j++){
						tmp->totalCount++;
						tmp->curCount++;
						data->blk = tmp;
						data->item = *(items.at(j));
						syncPut(data, true, 5);
					}
					changeNextPtr(c1->GetHead()->user_id, c2->file_id(), c2->index(), 
						c1->GetHead()->next_file_id, c1->GetHead()->next_block_index, true);
					changePrePtr(c1->GetHead()->user_id, c1->GetHead()->next_file_id, c1->GetHead()->next_block_index, 
						c2->file_id(), c2->index(), true);
				}else{
					MCE_WARN("CacheManager::deleteFile --> move err " << c1->GetHead()->user_id << " " << c2->GetHead()->user_id
					<< " from f:" << c1->file_id() << " b:" <<c1->index()
					<< " to f:" << c2->file_id() << " b:" << c2->index());
				}
			}
			MCE_INFO("CacheManager::deleteFile --> move " << index << " " << blksToMove.size());	
			
			//及时释放内存	
			for(size_t i=0; i<caches.size(); i++){
				BlockCache* c = caches.at(i);
				IceUtil::Mutex::Lock lock(mutexs_[makeIndex(c->file_id(), c->index())%MUTEX_COUNT]);			
				file->Del(c->index());
				c->Release();
			}
			caches.clear();
			ids.clear();
		}
		if(index >= count){
			break;
		}
		BlockCache* blk = file->Get(index);
		if(!blk){
			continue;
		}
		if(!blk->HasData()){
			MCE_WARN("CacheManger::deleteFile --> has no data " << blk->Head2Str());
			continue;
		}
		//users[blk->GetHead()->user_id] = blk;
		caches.push_back(blk);
		ids.push_back(blk->GetHead()->user_id);
	}

	MCE_INFO("CacheManager::deleteFile --> total " << index << " f:" << fileId << " del:" << delete_blk_count << " new:" << new_blk_count << " move:" << move_blk_count
			<< " caches:" << file->block_in_cache());
	deleteDataFile(fileId);
	deleteLogFile(fileId);
}
