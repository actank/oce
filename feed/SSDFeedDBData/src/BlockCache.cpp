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
#include "MemPool.h"

using namespace MyUtil;


bool CmpFunc(const FeedItem& a, const FeedItem& b){
	return a.feed > b.feed;
}

int BlockCache::new_count = 0;
int BlockCache::free_count = 0;

BlockHead* BlockCache::GetHead() const{
	BlockHead* head = (BlockHead*)((FeedItem*)buf_ + CacheManager::RECORD_PER_BLOCK);
	return head;
}

bool BlockCache::Init(){
	if(!buf_){
		buf_ = (char*)MemPool::instance().GetObject();
		new_count++;
	}
	memset(buf_, 0, BLOCKSIZE);
	BlockHead* head = GetHead();
	head->cur_count = 0;
	head->total_count = 0;
	head->pre_file_id = 0;
	head->pre_block_index = -1;
	head->next_file_id = 0;
	head->next_block_index = -1;

	//MCE_INFO("BlockCache::init--> " << head->user_id << " f:" << file_id_ << " b:" << block_index_ << " cc:" << head->cur_count
	//	<< " pf:" << head->pre_file_id << " pb:" <<head->pre_block_index << " nf:" << head->next_file_id << " nb:" << head->next_block_index);
	//timestamp_ = time(NULL);
	return true;
}

void BlockCache::Release(){
	if(buf_){
		free_count++;
		MemPool::instance().ReleaseObject(buf_);
		buf_ = 0;
	}
	delete this;
}

int BlockCache::GetData(FeedItem* data, UINT32 count){
	BlockHead* head = GetHead();
	if(!head){
		return 0;
	}
	FeedItem* off = (FeedItem*)buf_ + CacheManager::RECORD_PER_BLOCK - head->cur_count;
	//if(head->user_id==261947900 || head->user_id==227805680){
	//	MCE_INFO("#get# ##@@@@ " << head->user_id << " f:"<< file_id_ << " b:" << block_index_ << " preF:" << head->pre_file_id << " preB:" << head->pre_block_index
	//	 << " nextF:" << head->next_file_id << "nextB:" << head->next_block_index << " cc:" << head->cur_count);
	//}
	int c = count > head->cur_count ? head->cur_count : count;
	memcpy(data, off, c*sizeof(FeedItem));
	return c;
}

bool BlockCache::PutData(const BlockInfoPtr& blk, const FeedItem* data, bool overwrite){
	BlockHead* head = GetHead();
	if(!head){
		return false;
	}
	if(head->user_id > 0 && (UINT32)blk->userId != head->user_id){
		MCE_WARN("BlockCache::putData --> user id err,  " << head->user_id << " " << blk->userId);
		return false;
	}
	/*if(head->user_id==0 && (head->pre_block_index!=0 || head->next_block_index!=0)){
		MCE_WARN("DataFile::PutData--> invalid userid f:" << file_id_ << " b:" << block_index_ << " userid:" << head->user_id << " cc:" << head->cur_count
		<< " preF:" << head->pre_file_id << " preB:" << head->pre_block_index
		<< " nextF:" << head->next_file_id << " nextB:" << head->next_block_index);
		return;
	}*/

	head->user_id = blk->userId;
	if(blk->curCount != (int)(head->cur_count+1)){
		//MCE_WARN("BlockCache::putData --> count err, ht:" << head->totalCount+1 << " hc:" << head->curCount+1
		//		<< " bt:" << blk->totalCount << " bc:" << blk->curCount);
	}
	if((char*)Begin() == buf_){
		//MCE_WARN("BlockCache::putData --> block full f:" << file_id_ << " b:" << block_index_ << " userid:"
		//<< head->user_id << " tc:" << head->total_count << " cc:" << head->cur_count
		//<< " pf:" << head->pre_file_id << " pb:" <<head->pre_block_index << " nf:" << head->next_file_id << " nb:" << head->next_block_index
		//<< " -- " << " feed:" << data->feed << " merge:" << data->merge << " time:" << data->time);
		return false;
	}
	//MCE_DEBUG("BlockCache::putData --> head user:" << head->userId << " tc:" << head->totalCount << " cc:" << head->curCount
	//	<< " pf:" << head->preFileId << " pb:" <<head->preBlockIndex << " nf:" << head->nextFileId << " nb:" << head->nextBlockIndex);
	//MCE_DEBUG("BlockCache::putData --> blk  user:" << blk->userId << " tc:" << blk->totalCount << " cc:" << blk->curCount
	//	<< " f:" << blk->fileId << " b:" <<blk->blockIndex << " pf:" << blk->preFileId << " pb:" << blk->preBlockIndex);
	FeedItem* pos = Begin();
	//FeedItem* end = end();

	/*if(head->user_id==261947900 || head->user_id==227805680){
		MCE_INFO("BlockCache::putData --> #put# " << head->user_id << " f:" << file_id_ << " b:" << block_index_ << " cc:" << head->cur_count
		<< " pf:" << head->pre_file_id << " pb:" <<head->pre_block_index << " nf:" << head->next_file_id << " nb:" << head->next_block_index);
	}*/
	//数组基本有序，用二分查找更慢
	for(; pos!=End(); pos++){
		if(pos->feed < data->feed){
			break;
		}else if(pos->feed == data->feed){
			if(overwrite){
			  MCE_WARN("BlockCache::PutData --> feed exist overwrite" << blk->userId << " f:" << blk->fileId << " b:" << blk->blockIndex << " feed:" << data->feed);
        *pos = *data;
        return true;
      }
			MCE_WARN("BlockCache::PutData --> feed exist return " << blk->userId << " f:" << blk->fileId << " b:" << blk->blockIndex << " feed:" << data->feed);
      return false;
		}
	}
	//FeedItem* pos = find(data->feed);
	if(pos!=Begin()){
		memmove(Begin()-1, Begin(), (pos-Begin())*sizeof(FeedItem));
	}
	*(pos-1) = *data;
	head->cur_count++;// = blk->curCount;
	head->total_count++;// = blk->totalCount;
	
	if(head->user_id==0){
		MCE_WARN("DataFile::PutData--> after putData invalid userid f:" << file_id_ << " b:" << block_index_ << " userid:" << head->user_id << " cc:" << head->cur_count
		<< " preF:" << head->pre_file_id << " preB:" << head->pre_block_index
		<< " nextF:" << head->next_file_id << " nextB:" << head->next_block_index);
	}
	return true;
	//dirty = true;
	//MCE_DEBUG("BlockCache::putData --> @@@ get:" << " f:" << fileId << " b:" << blockIndex << " buf:" << (Ice::Long)buf << " user:" << head->userId << " tc:" << head->totalCount << " cc:" << head->curCount
         //     << " pf:" << head->preFileId << " pb:" <<head->preBlockIndex << " nf:" << head->nextFileId << " nb:" << head->nextBlockIndex);
}

bool BlockCache::Remove(long long feed){
	FeedItem* pos = Find(feed);
	if(pos!=End() && pos->feed==feed){
		memmove(Begin()+1, Begin(), sizeof(FeedItem)*(pos-Begin()));
		//dirty = true;
		GetHead()->cur_count--;
		return true;
	}
	return false;
}

void BlockCache::RemoveAll(){
	GetHead()->cur_count = 0;
}

FeedItem* BlockCache::Find(long long feed){
	//MCE_DEBUG("find ##@@@@## " << fileId << " " << blockIndex << " " << (long long)buf << " " << (long long)begin() << " " << (long long)end() << " " << getHead()->curCount);
	FeedItem tmp;
	tmp.feed = feed;
	return lower_bound(Begin(), End(), tmp, CmpFunc);
}
vector<FeedItem*> BlockCache::GetFeedItems(){
	vector<FeedItem*> items;
	BlockHead* head = GetHead();
	if(!head){
		return items;
	}
	size_t c = head->cur_count;
	FeedItem* end = End();
	for(size_t i=1; i<=c; i++){
		items.push_back(end-i);
	}
	return items;
}

