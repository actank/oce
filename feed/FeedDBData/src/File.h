#ifndef FILE_H_
#define FILE_H_

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <FeedDB.h>
#include <IceUtil/IceUtil.h>
#include "IceLogger.h"
#include "BlockCache.h"
#include "IOExecutor.h"

using namespace xce::feed;
using namespace std;
typedef long long INT64;
class File : public IceUtil::Shared{
public:
	static void		DeleteFile(const char* file_path);
	static void		RenameFile(const char* src, const char* dst);
	static void 	CombineFile(const char* src, const char* dst);
	static Ice::Long 	FileSize(const char* file_path);
	static bool 	FileExist(const char* file_path);

	bool	Open(const char* file_path, int flag);
	bool	Write(char* data, size_t count = 1);
	//size_t	Read(char* data, size_t count = 1);
	//size_t	ReadAll(char* data);
	bool	Pwrite(off_t pos, char* data, size_t count = 1);
	size_t	Pread(off_t pos, char* data, size_t count = 1);
	bool	Reserve(size_t count);
	void	Close();
	INT64	Size();
	INT64	ActualSize();
	void	Seek2End();

	File(/*int file_id, */int block_size){
		//file_id_ = file_id;
		block_size_ = block_size;
	}
	int	fd(){
		return fd_;
	}
protected:
	int	fd_;
	//int	file_id_;
	int	block_size_;
};


/*
namespace std {
template<> struct less<BlockInfoPtr> :
	public binary_function<BlockInfoPtr, BlockInfoPtr, bool> {
	bool operator()(const BlockInfoPtr& a, const BlockInfoPtr& b) const {
		//if(a->fileId != b->fileId){
		//	return a->fileId < b->fileId;
		//}else
		if(a->blockIndex != b->blockIndex){
			return a->blockIndex < b->blockIndex;
		}else{
			return a->totalCount < b->totalCount;
		}
	}
};
};*/

class LogFile : public File{
public:
	static std::string file_path_;
	LogFile(int fileId);
	bool	Init();
	bool	Restore(char* buf);
	void	Log(const FeedItemBlockInfoPtr& data);
	void	Log(const FeedItemBlockInfoSeq& data);
	void	CreateNewFile();
	void	DeleteOldFile();
	void	Delete();
	//void 	LoadDataFromFile(const char* fileName, map<unsigned int, map<BlockInfoPtr, FeedItem> >& feeds);
	//void	LoadData();
	size_t	LogCount2Restore(){
		return log_count_;
	}
private:
	char*		log_buf_;
	size_t		log_count_;
	int		file_id_;
	bool	changing_file_;
	IceUtil::Monitor<IceUtil::Mutex> _monitor;
};
typedef IceUtil::Handle<LogFile> LogFilePtr;

struct DataFileHead{
	size_t 	used_block_count;
	int	timestamp;
};

class DataFile : public File{
public:
	//const static char MASK[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	static const char MASK[8];
	static string file_path_;
	static unsigned int block_count_;
	//static unsigned int	blcck_size_;
	~DataFile();
	bool 			Init(bool truncate=true);
	void 			Del(unsigned int index);
	BlockCachePtr 	Get(unsigned int index);
	void			Release();
	BlockCachePtr 	Locate(unsigned int index, bool load = true);
	//void Locate(vector<unsigned int>, bool load = true);
	int				IncBlockCount(int count);
	int				BlockCount();
	bool			full();
	void			UpdateUsedBlockCount(size_t index);
	size_t		loadAllData(char* data);
	void		LocateAll(char* data, size_t count);
	void		LocateBatch(char* data, const vector<unsigned int>& indexs);
	BlockCachePtr	GetFreeBlock();
	bool		Flush(size_t index, size_t count, IOExecutor* ioExe);
	bool		FlushHead();
	size_t		Evict(size_t count);
	void		Delete();

	int		block_dirty();
	static void Config(unsigned int block_count/*, unsigned int block_size*/){
		block_count_ = block_count;
		//block_size_ = block_size;
	}
	DataFile(int file_id, int block_size) : File(block_size){
		file_id_ = file_id;
		block_in_cache_ = 0;
	}
	int block_in_cache(){
		int count = 0;
		for(size_t i=0; i<block_count_; i++){
			if(blocks_[i]){
				count++;
			}
		}
		return count;
		//return block_in_cache_;
	}
	int file_id(){
		return file_id_;
	}
	//DataFileHead* FileHead(){
	//	return (DataFileHead*)(blocks_[0]->buf);
	//}
	void	set_dirty(size_t index){
		IceUtil::Mutex::Lock lock(mutex_);
		//static char MASK[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
		size_t t1 = index >> 3; // /8
		size_t t2 = index - (t1 << 3);
		dirty_bitmap_[t1] |= MASK[t2];
		//MCE_INFO("@@@@@ MASK set_dirty f:" << file_id_ << " b:" << index << " p1:" << t1 << " p2:" << t2 << " v:" << (dirty_bitmap_[t1] & MASK[t2]));
	}
	void	set_clean(size_t index){
		IceUtil::Mutex::Lock lock(mutex_);
		size_t t1 = index >> 3; // /8
                size_t t2 = index - (t1 << 3);
                dirty_bitmap_[t1] &= (~MASK[t2]);
		//MCE_INFO("@@@@@ MASK set_clean f:" << file_id_ << " b:" << index << " p1:" << t1 << " p2:" << t2 << " v:" << (dirty_bitmap_[t1] & MASK[t2]));
	}

	void	set_clean_all(){
		memset(dirty_bitmap_, 0, block_count_>>3);
	}

	bool	dirty(size_t index){
		size_t t1 = index >> 3; // /8
                size_t t2 = index - (t1 << 3);
		//MCE_INFO("@@@@@ MASK dirty f:" << file_id_ << " b:" << index << " p1:" << t1 << " p2:" << t2 << " v:" << (dirty_bitmap_[t1] & MASK[t2]));
		//MCE_INFO("@@@@@ MASK dirty " << index << " " << t1 << " " << t2);
		bool tmp = dirty_bitmap_[t1] & MASK[t2];
		//if(!tmp){
		//	MCE_INFO("@@@@@ MASK no dirty f:" << file_id_ << " b:" << index << " p1:" << t1 << " p2:" << t2 << " v:" << (dirty_bitmap_[t1] & MASK[t2]));
		//}
		return tmp;
	}
protected:
	bool	CheckFileHead();
	//bool read(int index, char* buf);
private:
	int				block_in_cache_;
	//int				_fd;
	int				file_id_;
	BlockCachePtr* 			blocks_;
	char*				dirty_bitmap_;
	DataFileHead*			file_head_;
	IceUtil::Mutex			mutex_;
};
typedef IceUtil::Handle<DataFile> DataFilePtr;

#endif
