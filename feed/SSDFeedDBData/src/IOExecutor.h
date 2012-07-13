#ifndef FILEARRAY_H_
#define FILEARRAY_H_

#include <vector>
#include <string>
#include <libaio.h>
//#include <Singleton.h>

using namespace std;
//#include "Cache.h"
class BlockCache;

//class IOExecutor : public MyUtil::Singleton<IOExecutor>{
//struct Data{
//	char data[BLOCKSIZE];
//};

class IOExecutor{
public:
	static const size_t AIO_MAXIO = 1000;
	static const size_t MAX_BUFFER = 32*1024*1024;
	IOExecutor();
	~IOExecutor();
	void 	init();
	void	execute(int fd, const vector<BlockCache*>& caches, bool write = true);
private:
	io_context_t 	_ctx;
	char*		_data[AIO_MAXIO];
	//char		_buffer[MAX_BUFFER];
	char*		_buffer;
	io_event 	_events[AIO_MAXIO];
        iocb* 		_iocbSeq[AIO_MAXIO];
};

#endif


