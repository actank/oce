#ifndef OBJECTCACHE_H_
#define OBJECTCACHE_H_

#include <list>
#include "Singleton.h"
#include "IceLogger.h"
#include "FeedDB.h"
using namespace xce::feed;
class MemPool : public MyUtil::Singleton<MemPool>{
public:
	bool Init(int initSize, int growSize){
		MCE_INFO("MemPool::init --> " << initSize << " " << growSize);
		IceUtil::Mutex::Lock lock(_mutex);
		_initSize = initSize;
		_growSize = growSize;
		_total = 0;
		return Grow(_initSize);
	}

	void* GetObject(){
		IceUtil::Mutex::Lock lock(_mutex);
		if(_freeList.empty()){
			if(!Grow(_growSize)){
				return 0;
			}
		}
		if(_freeList.empty()){
			return 0;
		}
		void* tmp = _freeList.front();
		_freeList.pop_front();
		memset(tmp, 0, UNITSIZE);
		return tmp;
	}

	void ReleaseObject(void* obj){
		IceUtil::Mutex::Lock lock(_mutex);
		_freeList.push_back(obj);
	}

	bool Grow(int size){
		char* tmp;
		if(posix_memalign((void**)&tmp, 512, size*(size_t)UNITSIZE) != 0){
			MCE_WARN("MemPool::Grow --> posix_memalign err, size:" << size*(size_t)UNITSIZE);
		}
		if(!tmp){
			MCE_WARN("MemPool::Grow --> new mem err, " << size*(size_t)UNITSIZE);
			return false;
		}
		for(int i=0; i<size; i++){
			_freeList.push_back(tmp+i*UNITSIZE);
		}
		//_blockList.push_back(tmp);
		/*for(int i=0; i<size; i++){
			//memset(tmp, 0, UNITSIZE);
			char* tmp;
			if(posix_memalign((void**)&tmp, 512, (size_t)UNITSIZE) != 0){
                        	MCE_WARN("MemPool::Grow --> posix_memalign err, size:" << (size_t)UNITSIZE);
                	}
			if(tmp){
				_freeList.push_back(tmp);
			}
		}*/
		_total += size;
		MCE_INFO("MemPool::grow --> grow:" << size << " total:" << _total << " free:" << _freeList.size());
		return true;
	}

	pair<int, int> Size(){
		IceUtil::Mutex::Lock lock(_mutex);
		return make_pair<int, int>(_total, _freeList.size());
	}
	
	void FreeSome(){
		/*const int MAX = 10000000;
		const size_t FREE_MAX = 2000000;
		const int FREE_EACH = 10000;
		{
			IceUtil::Mutex::Lock lock(_mutex);
			if(_total < MAX || _freeList.size() < FREE_MAX){
				return;
			}
			MCE_INFO("MemPool::FreeSome --> total:" << _total << " free:" << _freeList.size());
		}
		for(int i=0; i<FREE_EACH; i++){
			IceUtil::Mutex::Lock lock(_mutex);
			void* p = _freeList.back();
			_total--;
			_freeList.pop_back();
			free(p);
		}*/
	}
private:
	static const int UNITSIZE = BLOCKSIZE;
	//int		_size;
	int 		_initSize;
	int 		_growSize;
	//list<void*> 	_blockList;
	int		_total;
	list<void*> 	_freeList;
	IceUtil::Mutex	_mutex;
};

#endif
