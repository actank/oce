#include <errno.h>
#include <cstring>
#include "File.h"
#include "MemPool.h"
#include "Cache.h"
#include "PerformanceStat.h"
#include "Cache.h"

using namespace MyUtil;
using namespace std;


void File::DeleteFile(const char* file_path){
	unlink(file_path);
}

void File::RenameFile(const char* src, const char* dst){
	int err = rename(src, dst);
	if(err !=0 ){
		MCE_WARN("File::RenameFile --> rename err, " << strerror(errno));
	}
}

Ice::Long File::FileSize(const char* file_path){
	struct stat buf;
	if(stat(file_path, &buf) !=0 ){
		return 0;
	}
	return buf.st_size;
}

bool File::FileExist(const char* file_path){
	int fd = open(file_path, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if(fd > 0){
		::close(fd);
		return true;
	}
	return false;
}

void File::CombineFile(const char* src, const char* dst){
	int srcFd = open(src, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if(srcFd < 0){
		MCE_WARN("File::CombineFile --> open error, " << src << " " << strerror(errno));
		return;
	}
	int dstFd = open(dst, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if(dstFd < 0){
		MCE_WARN("File::CombineFile --> open error, " << dst << " " << strerror(errno));
		return;
	}
	if(lseek(dstFd, 0, SEEK_END) < 0){
		MCE_WARN("File::CombineFile --> seek error, " << dst << " " << strerror(errno));
	}
	size_t len = FileSize(src);
	char* data = new char[len];
	int res = ::read(srcFd, data, len);
	if(res < 0){
		MCE_WARN("File::CombineFile --> read error, " << src << " " << strerror(errno));
		return;
	}
	if(::write(dstFd, data, res) < 0){
		MCE_WARN("File::CombineFile --> write error, " << dst << " " << strerror(errno));
	}
	::close(srcFd);
	::close(dstFd);
	delete[] data;
}

bool File::Open(const char* file_path, int flag){
	fd_ = ::open(file_path, flag, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if(fd_ < 0){
		MCE_WARN("File::open --> open error, " << file_path << " " << strerror(errno));
		return false;
	}
	return true;
}

bool File::Write(char* data, size_t count){
	const off_t WRITE_LIMIT = (1024*1024*1024)/block_size_;
	size_t c = (count + WRITE_LIMIT - 1)/WRITE_LIMIT;

	off_t off = 0;
	for(size_t i=0; i<c; i++){
		off_t len = (i == c-1 ? count - (c-1)*WRITE_LIMIT : WRITE_LIMIT);
                len *= block_size_;
		off_t res = ::write(fd_, data+off, len);
		if(res <= 0){
			MCE_WARN("File::Write --> write error " << res << " " << strerror(errno));
			return false;
		}
		off += res;
	}
	return true;
}
/*
size_t File::Read(char* data, size_t count){
	static const off_t READ_LIMIT = (1024*1024*1024)/block_size_;
	size_t c = (count + READ_LIMIT - 1)/READ_LIMIT;

	off_t off = 0;
	for(size_t i=0; i<c; i++){
		off_t len = (i == c-1 ? count - (c-1)*READ_LIMIT : READ_LIMIT);
		len *= block_size_;
		MCE_DEBUG("File::Pread --> bufoff:" << off);
		off_t res = ::read(fd_, data+off, len);
		if(res <= 0){
			MCE_WARN("File::read --> read error, or reach end, " << res << " " << strerror(errno));
			break;
		}
		off += res;
	}
	return off/block_size_;
}

size_t File::ReadAll(char* data){
	return pRead(data, Size());
}
*/
bool File::Pwrite(off_t pos, char* data, size_t count){
	const off_t READ_LIMIT = (1024*1024*1024)/block_size_;
	size_t c = (count + READ_LIMIT - 1)/READ_LIMIT;

	off_t p = pos*block_size_;
	off_t off = 0;
	for(size_t i=0; i<c; i++){
		off_t len = (i == c-1 ? count - (c-1)*READ_LIMIT : READ_LIMIT);
		len *= block_size_;
		off_t res = ::pwrite(fd_, data+off, len, p);
		if(res <= 0){
			MCE_WARN("File::Pwrite --> write error, " << res << " " << strerror(errno));
			break;
		}
		off += res;
		p += res;
	}
	if(off != (off_t)count*block_size_){
		MCE_WARN("File::Pwrite --> write error, " << count << " " << off << " " << off/block_size_);
	}
	return off == (off_t)count*block_size_;
}

size_t File::Pread(off_t pos, char* data, size_t count){
	const off_t READ_LIMIT = (1024*1024*1024)/block_size_;
	size_t c = (count + READ_LIMIT - 1)/READ_LIMIT;
	//MCE_INFO("File::Pread --> c:" << c << " blksize:" << block_size_ << " limit:" << READ_LIMIT << " count:" << count);
	off_t p = pos*block_size_;
	off_t off = 0;
	for(size_t i=0; i<c; i++){
		off_t len = (i == c-1 ? count - (c-1)*READ_LIMIT : READ_LIMIT);
		len *= block_size_;
		off_t res = ::pread(fd_, data+off, len, p);
		//MCE_DEBUG("File::Pread --> bufoff:" << off << " fileoff:" << p << "read:" << res << " c:" << count);
		if(res <= 0){
			MCE_WARN("File::Pread --> read error, or reach end, " << res << " " << strerror(errno));
			break;
		}
		off += res;
		p += res;
	}
	return off/block_size_;
}

bool File::Reserve(size_t count){
	int err = ftruncate(fd_, block_size_*(off_t)count);
	if(err < 0){
		MCE_WARN("File::Reserve --> ftruncate error, " << " " << strerror(errno));
		return false;
	}
	return true;
}

void File::Seek2End(){
	if(lseek(fd_, 0, SEEK_END) < 0){
		MCE_WARN("File::Seek2End --> seek error, " << strerror(errno));
	}
}

void File::Close(){
	close(fd_);
}

INT64 File::Size(){
	struct stat buf;
	if(fstat(fd_, &buf) !=0 ){
		return 0;
	}
	return buf.st_size/(INT64)block_size_;
}

INT64 File::ActualSize(){
	struct stat buf;
        if(fstat(fd_, &buf) !=0 ){
                return 0;
        }
        return buf.st_blocks*512/(INT64)block_size_;
}



/////////////////////////////////////////////////////////////////////////

string LogFile::file_path_ = "/data/xce/XceFeedDB/data/feeddb/log";

LogFile::LogFile(int fileId) : File(sizeof(FeedItem)+sizeof(BlockInfo64)){
	changing_file_ = false;
	file_id_ = fileId;
}

bool LogFile::Init(){
	ostringstream os;
	os << file_path_ << "/" << file_id_ << ".log";
	MCE_INFO("RestoreLog::Init --> init " << file_id_ << " " << os.str());
	
	//合并文件
	if(FileExist((os.str()+".old").c_str())){
		MCE_INFO("RestoreLog::loadData --> join file");
		CombineFile(os.str().c_str(), (os.str()+".old").c_str());
		DeleteFile(os.str().c_str());
		RenameFile((os.str()+".old").c_str(), os.str().c_str());
		//Close();
		//Open(os.str().c_str(), O_RDWR);
	}
	if(!Open(os.str().c_str(), O_RDWR|O_CREAT)){
		return false;
	}
	log_buf_ = 0;
        log_count_ = 0;

	//加载数据
	log_count_ = Size();
	if(log_count_>0){
		log_buf_ = new char[log_count_*block_size_];
		size_t res = Pread(0, log_buf_, log_count_);//ReadAll(data);
		if(res != log_count_){
			return false;
		}
		MCE_INFO("LogFile::Init --> load all data fid:" << file_id_ << " c:" << log_count_ << " res:" << res);
	}
	Seek2End();
	return true;
}

void LogFile::Delete(){
	MCE_INFO("LogFile::Delete --> " << file_id_);
	Close();
	ostringstream os;
        os << file_path_ << "/" << file_id_ << ".log";
	DeleteFile(os.str().c_str());
	DeleteFile((os.str()+".old").c_str());
}

bool LogFile::Restore(char* buf){
	MCE_INFO("RestoreLog::Restore --> begin restore fid:" << file_id_ << " logs:" << log_count_);
	//LoadData();
	if(log_count_ <= 0){
		return false;
		//DataFilePtr file = CacheManager::instance().locateFileCache(file_id_);
		//file->loadAllData();
	}

	if(buf){
		vector<unsigned int> seq;
		for(size_t i=0; i<log_count_; i++){
			BlockInfo64* blk = (BlockInfo64*)(log_buf_ + i*block_size_);
			seq.push_back(blk->blockIndex);
		}
		DataFilePtr file = CacheManager::instance().locateDataFile(file_id_);
		if(file){
			file->LocateBatch(buf, seq);
		}
		MCE_INFO("LogFile::Restore --> restore from buf " << file_id_ << " logs:" << log_count_);
	}else{
		MCE_INFO("LogFile::Restore --> restore directly " << file_id_ << " logs:" << log_count_);
	}
	for(size_t i=0; i<log_count_; i++){
		BlockInfo64Ptr blk = new BlockInfo64;
		*blk = *(BlockInfo64*)(log_buf_ + i*block_size_);
		FeedItem* item = (FeedItem*)(log_buf_ + i*block_size_ + sizeof(BlockInfo64));
		if(item->feed < 0){
			CacheManager::instance().remove(blk, -item->feed, false, false);
		}else{
			FeedItemBlockInfo64Ptr data = new FeedItemBlockInfo64;
			data->blk = blk;
			data->item = *(item);
			CacheManager::instance().syncPut(data, false, 1);
		}
		if(blk->curCount == -3){
			CacheManager::instance().removeAll(blk->key, blk->fileId, blk->blockIndex, false);
		}
		if(i>0 && i%10000==0){
			int size = CacheManager::instance().locateDataFile(file_id_)->block_in_cache();
			MCE_INFO("RestoreLog::Restore --> restored " << file_id_ << " " << i << " cachesize:" << size);
		}
	}
	delete[] log_buf_;
	return true;
}

void LogFile::Log(const FeedItemBlockInfo64Ptr& data){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
	if(changing_file_){
		_monitor.wait();
	}
	//TimeStat t;
	char buf[128] = {0};
	memcpy(buf, data->blk.get(), sizeof(BlockInfo64));
	memcpy(buf + sizeof(BlockInfo64), &(data->item), sizeof(FeedItem));
	Write(buf);
	//PerformanceStat::instance().incAsyncPut(t.getTime());
}

void LogFile::Log(const FeedItemBlockInfo64Seq& data){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
	if(changing_file_){
		_monitor.wait();
	}
	static const int MAX_LOGBUF_SIZE = 64*1024;
	char buf[MAX_LOGBUF_SIZE] = {0};
	int c = MAX_LOGBUF_SIZE/block_size_;
	int off = 0;
	int count = 0;
	for(int i=0; i<(int)data.size(); i++){
		memcpy(buf+off, data.at(i)->blk.get(), sizeof(BlockInfo64));
		memcpy(buf+off+sizeof(BlockInfo64), &(data.at(i)->item), sizeof(FeedItem));
		count++;
		off = (count*block_size_);
		if(count >= c || i==(int)(data.size()-1)){
			//MCE_INFO("LogFile::Log --> fileid:" << file_id_ << " size:" << data.size() << " c:" << c << " count:" << count << " off:" << off);
			Write(buf, count);
			count = 0;
			off = 0;
		}
	}
}

void LogFile::CreateNewFile(){
	//MCE_INFO("RestoreLog::createNewFile --> @@@@@@@@@@@");
	//deleteOldFile();
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
	changing_file_ = true;

	Close();
	ostringstream os;
	os << file_path_ << "/" << file_id_ << ".log";
	MCE_INFO("RestoreLog::createFile --> rename " << os.str() << " -> " << (os.str()+".old"));
	RenameFile(os.str().c_str(), (os.str()+".old").c_str());
	MCE_INFO("RestoreLog::deleteOldFile --> open " << os.str());
	Open(os.str().c_str(), O_RDWR|O_CREAT);
	changing_file_ = false;
	_monitor.notifyAll();
}

void LogFile::DeleteOldFile(){
	ostringstream os;
	os << file_path_ << "/" << file_id_ << ".log.old";
	DeleteFile(os.str().c_str());
}


/////////////////////////////////////////////////////////////////////////////

string DataFile::file_path_ = "/data/xce/XceFeedDB/data/feeddb/data";
unsigned int DataFile::block_count_ = 0;
const char DataFile::MASK[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
//unsigned int DataFile::blcck_size_ = 0;

bool DataFile::Init(bool truncate){
	blocks_ = new BlockCachePtr[block_count_];
	memset(blocks_, 0, sizeof(BlockCachePtr)*block_count_);
	ostringstream os;
	os << file_path_ << "/" << file_id_;
	MCE_INFO("FileCache::init --> open " << os.str());
	if(!Open(os.str().c_str(), O_RDWR|O_DIRECT|O_CREAT)){
		return false;
	}
	//保留一块给文件头用
	if(truncate && !Reserve(block_count_+1)){
		return false;
	}
	//char* data = new char[block_size_];
	file_head_ = (DataFileHead*)MemPool::instance().GetObject();
	//BlockCachePtr pc = new BlockCache(file_id_, 0);
	//if(!pc->init()){
	//	return false;
	//}
	MCE_INFO("@@@ buf:" << (Ice::Long)file_head_);
	Pread(0, (char*)file_head_);
	MCE_INFO("DataFile::Init --> f:" << file_id_ << " used:" << file_head_->used_block_count);
	//blocks_[0] = pc;
	if(file_head_->timestamp == 0){
		file_head_->timestamp = time(NULL);
	}
	size_t len = block_count_>>3;
	MCE_INFO("@@@@@@@@@@ " << block_count_ << " " << (block_count_>>3));
	dirty_bitmap_ = new char[len];
	memset(dirty_bitmap_, 0, len);
	//test
	//memset(blocks_[0], 0, block_size_);
	//
	//Pread();
	return true;
}

void DataFile::Delete(){
	MCE_INFO("DataFile::Delete --> " << file_id_);
	Close();
	ostringstream os;
        os << file_path_ << "/" << file_id_;
	DeleteFile(os.str().c_str());
}

bool DataFile::CheckFileHead(){
	/*size_t before_check = file_head_->used_block_count;
	size_t index = file_head_->used_block_count;
	Pread(index, (char*)blocks_[index]);
	BlockCachePtr cache = blocks_[index];
	if(cache->GetHead()->user_id == 0){
		return true;
	}

	size_t total_count = ActualSize();
	size_t i = total_count - 1;
	for(;;i--){
		Pread(i, (char*)blocks_[i]);
		if(blocks_[i]->GetHead()->user_id != 0){
			break;
		}
		file_head_->used_block_count_ = i;
	}
	//其中一块为文件头
	file_head_->used_block_count_ -= 1;
	MCE_INFO("DataFile::CheckFileHead --> " << before_check << " " << file_head_->used_block_count_);*/
	return true;
}

void DataFile::Del(unsigned int index){
	if((index >= 0) && (index < block_count_)){
		blocks_[index] = 0;
		block_in_cache_--;
	}
}

BlockCachePtr DataFile::Get(unsigned int index){
	if((index >= 0) && (index < block_count_)){
		return blocks_[index];
	}
	return 0;
}

void DataFile::UpdateUsedBlockCount(size_t index){
	if(index > file_head_->used_block_count){
		file_head_->used_block_count = index;
	}
}

BlockCachePtr DataFile::Locate(unsigned int index, bool load){
	if((index >= 0) && (index < block_count_)){
		PerformanceStat::instance().stat("LocateCount", 0);		
		if(blocks_[index]){
			PerformanceStat::instance().stat("LocateHit", 0);	
			return blocks_[index];
		}
		BlockCachePtr pc = new BlockCache(file_id_, index);
		if((!pc) || (!pc->Init())){
			MCE_WARN("FileCache::locate --> new block cache err");
			if(pc){
				pc->Release();
			}
			return 0;
		}
		//if(load){
		//	PerformanceStat::instance().incRead();
		//}
		timeval beg, end;
		if(load){
			gettimeofday(&beg, NULL);
		}
		//第一块为文件头
		if(!load || Pread(index+1, pc->buf())==1){
			if(load){
				gettimeofday(&end, NULL);
				float elipse = ((end.tv_sec-beg.tv_sec)*1000000 + end.tv_usec-beg.tv_usec)/1000.0;
				PerformanceStat::instance().stat("read", elipse);		
			}
			blocks_[index] = pc;
			block_in_cache_++;
			//index�0~1999999
			UpdateUsedBlockCount(index+1);
			BlockHead* head = blocks_[index]->GetHead();
			/*if(load && !blocks_[index]->Valid()){
				MCE_WARN("DataFile::Locate--> read " << blocks_[index]->Head2Str());
			}*/
			//读到的数据可能为空，重新初始化
			if(head->user_id ==0 && head->pre_block_index==0 && head->next_block_index==0){
				//MCE_DEBUG("DataFile::Locate --> re init " << file_id_ << " " << index);
				//MCE_WARN("DataFile::locate--> reinit f:" << file_id_ << " b:" << index);
				blocks_[index]->Init();
			}
			//MCE_INFO("DataFile::locate--> new " << head->user_id << " f:" << pc->file_id() << " b:" << pc->index()
			//<< " cc:" << head->cur_count
            //    	<< " pf:" << head->pre_file_id << " pb:" <<head->pre_block_index << " nf:" << head->next_file_id << " nb:" << head->next_block_index);

			return pc;
		}else{
			MCE_WARN("FileCache::locate --> read file err, " << file_id_ << " " << index);
			pc->Release();
			return 0;
		}
	}
	return 0;
}
/*
void DataFile::Locate(vector<unsigned int> indexs, bool load){
	vector<BlockCachePtr> caches;
	for(size_t i=0; i<indexs.size(); i++){
		BlockCachePtr c = Locate(indexs.at(i), false);
		caches.push_back(c);
	}
	static IOExecutor io;
	io.execute(fd(), caches, false);
	//return 0;
}*/

BlockCachePtr DataFile::GetFreeBlock(){
	size_t index = file_head_->used_block_count;
	BlockCachePtr pc = new BlockCache(file_id_, index);
	if((!pc) || (!pc->Init())){
		MCE_WARN("DataFile::GetFreeBlock --> new block cache err");
		if(pc){
			pc->Release();
		}
		return 0;
	}
	if(blocks_[index]){
		MCE_WARN("DataFile::GetFreeBlock --> the block had been allocated " << index);
	}
	blocks_[index] = pc;
	block_in_cache_++;
	UpdateUsedBlockCount(index+1);
	return blocks_[index];
}

void DataFile::Release(){
/*	if(block_in_cache_ == 0){
		return;
	}
	for(int i=0; i<BLOCKCOUNT; i++){
		if(blocks_[i]){
			delete blocks_[i];
		}
	}
	delete []blocks_;
	delete this;
*/
}

DataFile::~DataFile(){
	//Close();
	MCE_INFO("DataFile::~DataFile --> begin " << file_id_);
	for(unsigned int i=0; i<block_count_; i++){
		if(blocks_[i]){
                        blocks_[i]->Release();
			Del(i);
		}
	}
	MemPool::instance().ReleaseObject(file_head_);
	MCE_INFO("DataFile::~DataFile --> release block " << file_id_);
	delete [] blocks_;
	delete [] dirty_bitmap_;	
	MCE_INFO("DataFile::~DataFile --> end " << file_id_);
}

int DataFile::IncBlockCount(int count){
	/*struct stat buf;
        if(fstat(_fd, &buf) != 0){
                return 0;
        }
	int curCount = buf.st_size/BLOCKSIZE;
	if(curCount == BLOCKCOUNT){
		return 0;
	}
	int tmp = count;
	curCount += count;
	if((curCount + count) > BLOCKCOUNT){
		tmp = BLOCKCOUNT - curCount;
		curCount = BLOCKCOUNT;
	}
	int err = ftruncate(_fd, curCount*(off_t)BLOCKSIZE);
	if(err < 0){
		MCE_WARN("FileCache::incFileSize --> ftruncate error, " << strerror(errno));
		return 0;
	}
	return tmp;*/
	return 0;
}

bool DataFile::full(){
	//其中一块为文件头
	return file_head_->used_block_count == block_count_;
}

int DataFile::BlockCount(){
	return file_head_->used_block_count;
	/*struct stat buf;
        if(fstat(_fd, &buf) !=0 ){
                return 0;
        }
        return buf.st_size/BLOCKSIZE;*/
	return 0;
}

bool DataFile::Flush(size_t index, size_t count, IOExecutor* ioExe){
	vector<BlockCachePtr> dirtys;
	for(size_t i=index; i<(count + index) && i<block_count_; i++){
		if(dirty(i)){
			BlockCachePtr cache = Get(i);
			if(!cache){
				MCE_WARN("DataFile::Flush --> dirty, but no block im memory, f:" << file_id_ << " b:" << i);
				continue;
			}
			//cache->dirty = false;
			//BlockHead* head = cache->GetHead();
			//if(!cache->Valid()){
			//	MCE_WARN("DataFile::Flush-->  " << cache->Head2Str());
			//}
			set_clean(i);
			dirtys.push_back(cache);
		}
	}
	if(dirtys.empty()){
		MCE_INFO("DataFile::Flush --> timer " << " f:" << file_id_ << " b:" << index << " no dirty");
		return false;
	}
	timeval beg, end;
	gettimeofday(&beg, NULL);
	try{
		ioExe->execute(fd(), dirtys, true);
	}catch(std::exception& e){
		MCE_WARN("DataFile::Flush --> io execute err, " << e.what());
	}catch(...){
		MCE_WARN("DataFile::Flush --> io execute err");
	}
	gettimeofday(&end, NULL);
	float elipse = ((end.tv_sec-beg.tv_sec)*1000000 + end.tv_usec-beg.tv_usec)/1000.0;
	MCE_INFO("DataFile::Flush --> timer " << " f:" << file_id_ << " b:" << index << " " << elipse << "ms, check " << count << ", write " << dirtys.size());
	/*if(index + count >= block_count_){
		MCE_INFO("DataFile::Flush --> flush file head " << file_id_ << " " << file_head_->used_block_count_);
		Pwrite(0, (char*)file_head_);
	}*/
	return true;
}

bool DataFile::FlushHead(){
	MCE_INFO("DataFile::FlushHead --> flush file head " << file_id_ << " " << file_head_->used_block_count);
	return Pwrite(0, (char*)file_head_);
}

size_t DataFile::Evict(size_t count){
	size_t c = 0;
	for(size_t i=0; i<block_count_; i++){
		if(!dirty(i)){
			BlockCachePtr cache = Get(i);
			if(!cache){
				continue;
			}
			CacheManager::instance().lock(file_id_, i);
			if(!dirty(i)){
				Del(i);
				cache->Release();
				c++;
			}
			CacheManager::instance().unlock(file_id_, i);
		}
		if(i>0 && (i%100000 == 0 || i==block_count_-1)){
			MCE_INFO("DataFile::Evict --> f:" << file_id_ << " total:" << count << " check:" << i << " count:" << c);
		}
		if(c>=count){
			break;
		}
	}
	return c;
}

int DataFile::block_dirty(){
	//static char[256]
	int count = 0;
	for(size_t i=0; i<block_count_; i++){
		if(dirty(i)){
			count++;
		}
	}
	return count;
}

void DataFile::LocateBatch(char* data, const vector<unsigned int>& indexs){
	MCE_INFO("DataFile::LocateBatch --> LocateBatch " << file_id_ << " " << indexs.size());
	for(size_t i=0; i<indexs.size(); i++){
		unsigned int index = indexs.at(i);
		if(blocks_[index]){
			continue;
		}
		BlockHead* head = (BlockHead*)((FeedItem*)(data + index*(Ice::Long)block_size_) + CacheManager::RECORD_PER_BLOCK);
		if(head->user_id == 0){
			continue;	
		}
		BlockCachePtr cache = Locate(index, false);
		memcpy(cache->buf(), data + index*(Ice::Long)block_size_, block_size_);
		/*if(!cache->Valid()){
			MCE_WARN("DataFile::LocateBatch--> " << cache->Head2Str());
		}*/
	}
}

void DataFile::LocateAll(char* data, size_t count){
	MCE_INFO("DataFile::LocateAll --> LocateAll " << file_id_ << " " << count);
	for(size_t i=0; i<count; i++){
		if(blocks_[i]){
			continue;
		}
		BlockHead* head = (BlockHead*)((FeedItem*)(data + i*(Ice::Long)block_size_) + CacheManager::RECORD_PER_BLOCK);
		/*if(head->user_id == 0 && head->pre_block_index==0 && head->next_block_index==0){
			continue;
		}*/
		if(head->user_id == 0){
			continue;	
		}
		BlockCachePtr cache = Locate(i, false);
		memcpy(cache->buf(), data + i*(Ice::Long)block_size_, block_size_);
		/*if(!cache->Valid()){
			MCE_WARN("DataFile::LocateBatch--> " << cache->Head2Str());
		}*/
	}
}


size_t DataFile::loadAllData(char* data){
	//size_t count = Size();
	//count--;//1 file head
	size_t count = file_head_->used_block_count;
	if(count<=0){
		return 0;
	}
	Ice::Long len = count*(Ice::Long)block_size_;
	//char* data;
	//posix_memalign((void**)&data, 512, len);
	try{
		MCE_INFO("DataFile::loadAllData --> read beg f:" << file_id_ << " len:" << len << " c:" << count);
		size_t c = count;
		if(ActualSize() <= 0){
			MCE_INFO("DataFile::loadAllData --> read nothing f:" << file_id_ << " len:" << len << " c:" << count);
			//memset(data, 0, len);
			return 0;
		}else{
			c = Pread(1, data, count);
			if(c!=count){
				MCE_WARN("DataFile::loadAllData --> read err f:" << file_id_ << " c:" << c << " count:" << count);
				return 0;
			}
		}
		MCE_INFO("DataFile::loadAllData --> read end f:" << file_id_ << " len:" << len << " c:" << count);
		return c;
	}catch(...){
		MCE_WARN("DataFile::loadAllData --> err " << file_id_);
	}
	return 0;
}
