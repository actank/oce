#ifndef CACHE_H_
#define CACHE_H_

#include <stdlib.h>
#include <map>
#include "FeedDB.h"
#include "TaskManager.h"
#include "IceLogger.h"
//#include "BlockCache.h"
//#include "FileCache.h"
#include "File.h"
#include "IOExecutor.h"

using namespace std;
using namespace xce::feed;
using namespace MyUtil;


enum ETimerAction{NONE, FLUSH_FILE, EVICT, DELETE_FILE};

struct TimerPar{
	list<int> 	flushing_file_ids_;
	int		flushing_block_index_;
	int		delete_file_id_;
};

class CacheManager : public MyUtil::Timer, public MyUtil::Singleton<CacheManager>{
public:
	CacheManager() : Timer(1000){
		evicting_ = false;
		evict_dirty_ = false;
		cache_size_cfg_ = MAX_CACHE;
		flush_count_cfg_ = FLUSH_PER_TIME;
		max_log_size_cfg_ = MAX_LOG_SIZE;
		support_remove_cfg_ = true;
		timer_par_.flushing_block_index_ = 0;
		timer_par_.delete_file_id_ = 0;
		timer_action_ = FLUSH_FILE;
		timer_begin_ = false;
		restoring_ = true;
	}
	static const unsigned int MAX_CACHE = 10000000;
	static const unsigned int FLUSH_PER_TIME = 10000;
	static const unsigned int MUTEX_COUNT = MAX_CACHE;
	static const unsigned int EVICT_PER_TIME = 5000;
	static const unsigned int MAX_COUNT_PER_USER = 5;
	static const unsigned int RECORD_PER_BLOCK = BLOCKSIZE/sizeof(FeedItem)-1;//保留一个record作为BlockHead
	static const unsigned int MAX_LOG_SIZE = 5000000;//200*1024*1024;//当log文件达到200M时， 往磁盘上刷新其对应的数据

	DataFilePtr		locateDataFile(int fileId/*, int blockIndex, bool load=true*/);
	DataFilePtr		findDataFile(int fileId/*, int blockIndex, bool load=true*/);
	void			deleteDataFile(int fileId);
	void			deleteLogFile(int fileId);
	LogFilePtr		locateLogFile(int fileId);
	//DataFilePtr 		locateOldFile();
	void			put(const FeedItemBlockInfo64Ptr& data, int flag, bool overwrite=false);
	void			syncPut(const FeedItemBlockInfo64Ptr& data, bool log, int flag);
	void			asyncPut(const FeedItemBlockInfo64Ptr& data, bool log, int flag, bool overwrite=false);
  void      asyncPut(const FeedItemBlockInfo64Seq& data, bool log, int flag, bool overwrite=false);
  int       get(const BlockInfo64Ptr& blk, FeedItem* data, int count, bool log);
	void			remove(const BlockInfo64Ptr& blk, long long feedId, bool depth, bool log);
	void			removeAll(Ice::Long key, int fileId, int blockIndex, bool log);
	//void			syncRemove(const BlockInfoPtr& blk, long long feedId, bool log);
	void			asyncRemove(const BlockInfo64Ptr& blk, long long feedId, bool log);

	//int				locateFile(int fileId);
	void			handle();
	FileInfoPtr		queryFile(int fileId);
	ServiceInfoPtr		QueryServiceInfo();
	bool			moveBlock(const BlockInfo64Ptr& blkInfoFrom, const BlockInfo64Ptr& blkInfoTo);
	void			preLoad(const string& path);
	void			updateConfig(const map<string, string>& cfgs);
	//set<int>		deleteFile(int fileId);
	bool			fileCanDelete(int fileId);
	void 			decTotalCount(const UserFeedCount64Seq& userFeedCounts);
	size_t			deleteFilePrepare(int fileId);
	set<int>		deleteFileForce(int fileId);
	void			deleteFileKeepMin(int fileId);
	void			AsyncDeleteFile(int file_id);
	int			getMaxSizeLogFile();
	int			getMaxSizeCacheFile();
	bool			evictNoDirtyBlock(size_t count);
	bool			flushFile(bool evict);
	unsigned int 		cacheSize();
	void			checkBlocks(const BlockInfo64Seq& blkInfos, const vector<BlockCache*>& caches,
                         		vector<BlockCache*>& blksToDelete, vector<Ice::Long>& userIdsToNew, vector<BlockCache*>& blksToNew, vector<BlockCache*>& blksToMove);
	BlockInfo64Seq		getBlockInfoBatch(const LongSeq& ids);
	//void			moveBlocks(int fileId, const vector<BlockCache*>& blksToMove, vector<BlockCache*>& need2Rectify);
	void			newBlocks(const vector<BlockCache*>& blksToNew, const vector<Ice::Long>& userIdsToNew);
	void			deleteBlocks(const vector<BlockCache*>& blksToDelete);
	void 			rectifyBlocks(int fielId, const vector<BlockCache*>& need2Rectify);
	void lock(int fileid, int blockindex){
		mutexs_[makeIndex(fileid, blockindex)%MUTEX_COUNT].lock();
	}
	void unlock(int fileid, int blockindex){
		mutexs_[makeIndex(fileid, blockindex)%MUTEX_COUNT].unlock();
	}
	void setDBFilePath(const string& data_path, const string& log_path){
		DataFile::file_path_ = data_path;
		LogFile::file_path_ = log_path;
		DataFile::block_count_ = BLOCKCOUNT;
	}
protected:
	void changeNextPtr(Ice::Long key, int fileId, int blockIndex, int nextFileId, int nextBlockIndex, bool sync);
	void changePrePtr(Ice::Long key, int fileId, int blockIndex, int preFileId, int preBlockIndex, bool sync);
	UINT64 makeIndex(int fileId, int blockIndex){
		static int tmp = MUTEX_COUNT/BLOCKCOUNT;
		return (fileId%tmp)*BLOCKCOUNT + blockIndex;
		//return (((UINT64)fileId)<<32) | blockIndex;
	}
	void evict();
private:
	map<int, DataFilePtr>	files_;
	map<int, LogFilePtr>	logs_;
	bool				evicting_;
	bool				evict_dirty_;
	unsigned int		cache_size_cfg_;
	unsigned int		flush_count_cfg_;
	unsigned int		max_log_size_cfg_;
	bool			support_remove_cfg_;

	IOExecutor			ioExe_;
	IceUtil::RWRecMutex	mutex_;
	//IceUtil::Mutex		data_mutex_;
	IceUtil::Mutex		mutexs_[MUTEX_COUNT];
	ETimerAction		timer_action_;
	TimerPar			timer_par_;
	bool			timer_begin_;
	bool			restoring_;
};

class PutTask : public MyUtil::Task{
public:
  PutTask(FeedItemBlockInfo64Seq& data, int flag, bool overwrite=false) : _flag(flag), _overwrite(overwrite){
    _data.swap(data);
  };
  PutTask(const FeedItemBlockInfo64Ptr& data, int flag, bool overwrite=false) : _flag(flag), _overwrite(overwrite){
    _data.push_back(data);
  }
  void handle(){
		for(int i=0; i<(int)_data.size(); i++){
			if(_data.at(i)->item.feed < 0){
				CacheManager::instance().remove(_data.at(i)->blk, -_data.at(i)->item.feed, true, true);
			}else{
				CacheManager::instance().put(_data.at(i), _flag, _overwrite);
			}
		}
	}
private:
	FeedItemBlockInfo64Seq _data;
  int _flag;
  bool _overwrite;
};

#endif
