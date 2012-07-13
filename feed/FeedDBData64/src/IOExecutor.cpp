#include <cstring>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include "IceLogger.h"
#include "IOExecutor.h"
#include "Cache.h"

/////////////////////////////////////////////////////////////////
IOExecutor::IOExecutor(){
	init();
}

IOExecutor::~IOExecutor(){
	free(_buffer);
}

void IOExecutor::init(){
	memset(&_ctx, 0, sizeof(_ctx));
	io_setup(AIO_MAXIO, &_ctx);
	if(posix_memalign((void**)&_buffer, 512, MAX_BUFFER) != 0){
		MCE_WARN("IOExecutor::init --> posix_memalign err, size:" << MAX_BUFFER);
	}
}

void IOExecutor::execute(int fd, const vector<BlockCache*>& caches, bool write){
	if(caches.empty() || caches.size()*BLOCKSIZE > MAX_BUFFER){
		MCE_WARN("IOExecutor::execute --> empty or too much " << caches.size());
		return;
	}

	//MCE_INFO("IOExecutor::execute --> fid:" << caches.at(0)->file_id() << " fd:" << fd << " size:" << caches.size() << " write:" << write);
	memset(_buffer, 0, MAX_BUFFER);
	if(write){
		for(size_t i=0; i<caches.size(); i++){
			memcpy(_buffer+i*BLOCKSIZE, caches.at(i)->buf(), BLOCKSIZE);	
		}
	}

	//int size = 1;
	int blkCount = 0;
	int totalIO = 0;
	int ioCount = 0;
	//char* loc = _buffer;
	for (size_t i = 0; i < caches.size();) {
		size_t j = i+1;
		for(; j<caches.size(); j++){
			if((caches.at(j)->index() != (caches.at(j-1)->index() + 1))){
				break;
			}
		}
		int count = j-i;
		iocb *io = new iocb;
		memset(io, 0, sizeof(iocb));
		if(write){
			io_prep_pwrite(io, fd, _buffer+i*BLOCKSIZE, count*BLOCKSIZE, (caches.at(i)->index()+1)*(off_t)BLOCKSIZE);
		}else{
			io_prep_pread(io, fd, _buffer+i*BLOCKSIZE, count*BLOCKSIZE, (caches.at(i)->index()+1)*(off_t)BLOCKSIZE);
		}
		_iocbSeq[ioCount] = io;
		ioCount++;
		totalIO++;
		blkCount += count;
		//MCE_INFO("IOExecutor::execute --> io count:" << ioCount << " blk:" << caches.at(i)->blockIndex << " size:" << count << " datapos:"
		//		<< i << " tc:" << blkCount << "," << j << " w:" << write);
		i=j;
		if((ioCount == (int)AIO_MAXIO || j == caches.size()) && ioCount > 0){
			int res = io_submit(_ctx, ioCount, _iocbSeq);
			if (res < 0){
				MCE_WARN("io_submit error, c:" << ioCount << " " << errno << " " << strerror(errno) << " " << _iocbSeq[0]->aio_fildes);
			}
			res = io_getevents(_ctx, ioCount, AIO_MAXIO, _events, NULL);
			if(res < 0){
				MCE_WARN("io_getevents error, c:" << ioCount << " " << errno << " " << strerror(errno));
			}
			for(int n=0; n<ioCount; n++){
				delete _iocbSeq[n];
			}
			//totalIO += ioCount;
			//MCE_INFO("IOExecutor::executor --> submit " << ioCount << " total IO:" << totalIO);
			ioCount = 0;
		}
	}
	//MCE_INFO("IOExecutor::executor --> finish total blk:" << blkCount << " total IO:" << totalIO);
	if(!write){
		for(size_t i=0; i<caches.size(); i++){
			memcpy(caches.at(i)->buf(), _buffer+i*BLOCKSIZE, BLOCKSIZE);	
		}
	}
}

