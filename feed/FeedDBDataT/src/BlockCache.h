#ifndef BLOCK_CACHE_H_
#define BLOCK_CACHE_H_

#include <stdlib.h>
#include "FeedDB.h"
#include "IceLogger.h"


using namespace xce::feed;
using namespace MyUtil;

typedef unsigned long long UINT64;
typedef unsigned int UINT32;

struct BlockHead{
	UINT32 	user_id;
	UINT32 	total_count;
	UINT32 	cur_count;
	int 	pre_file_id;		//指向上一块
	int 	pre_block_index;	//指向上一块
	int		next_file_id;		//指向上一块
	int		next_block_index;	//指向上一块

};

class BlockCache{
public:
	//static const unsigned int RECORD_PER_BLOCK = BLOCKSIZE/sizeof(FeedItem)-1;//保留一个record作为BlockHea

	BlockCache(int f=0, int b=-1/*, UINT32 size=BLOCKSIZE*/){
		file_id_ = f;
		block_index_ = b;
		//dirty = false;
		//evict = false;
		buf_ = 0;
	}
	void SetIndex(int f, int b){
		file_id_ = f;
		block_index_ = b;
	}
	bool Empty(){
		return (GetHead()->cur_count == 0);
	}
	FeedItem* Begin() const{
		BlockHead* head = GetHead();
		return (FeedItem*)head - head->cur_count;
	}
	FeedItem* End() const{
		return (FeedItem*)GetHead();
	}
	BlockHead* GetHead() const;
	UINT64 GetKey(){
		return (((UINT64)file_id_)<<32) | block_index_;
	}
	//bool 		checkHead(){
	FeedItem* 	Find(long long feedId);
	int			GetData(FeedItem* data, UINT32 count);
	bool		PutData(const BlockInfoPtr& blk, const FeedItem* data, bool overwrite=false);
	//void		BlockCopy(const BlockCache* cache);
	//void		BlockCat(const BlockCache* cache);
	bool 		Remove(long long feedId);
	void		RemoveAll();
	bool		Init();
	void		Release();
	vector<FeedItem*> GetFeedItems();
	int file_id() const{
		return file_id_;
	}
	int index() const{
		return block_index_;
	}
	char* buf() const{
		return buf_;
	}
	/*string timestamp(){
		tm* ts = localtime(&timestamp_);
		ostringstream os;
		os << ts->tm_hour << ":" << ts->tm_min << ":" << ts->tm_sec;
		return os.str();
	}*/
	string Head2Str(){
		BlockHead* head = GetHead();
		ostringstream os;	
		os << "f:" << file_id_ << " b:" << block_index_ << " u:" <<  head->user_id
		<< " tc:" << head->total_count << " cc:" << head->cur_count << " preF:" << head->pre_file_id << " preB:" << head->pre_block_index
		<< " nextF:" << head->next_file_id << " nextB:" << head->next_block_index;
		return os.str();
	}
	bool Error(){
		BlockHead* head = GetHead();
		if(head->user_id == 0 && (head->pre_block_index!=0 || head->next_block_index!=0)){
			return true;
		}
		return false;
	}
	bool HasData(){
		BlockHead* head = GetHead();
		if(head->user_id != 0 && head->cur_count>0){
			return true;
		}
		return false;
	}
	bool Zero(){
		BlockHead* head = GetHead();
		if(head->user_id==0 && head->pre_block_index==0 && head->next_block_index==0){
			return true;
		}
		return false;
	}
private:
	//static IceUtil::Mutex mutex;
	int		file_id_;
	int		block_index_;
	char* 	buf_;
	//time_t		timestamp_;
	//bool	dirty_;
	//bool	evict;

	static int new_count;
	static int free_count;
};
typedef BlockCache* BlockCachePtr;

#endif
