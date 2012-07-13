#ifndef __IO_STREAM_H__
#define __IO_STREAM_H__

#include "Buffer.h"

namespace MyUtil
{
using namespace std;

class FileDescriptor : public IceUtil::Shared
{
public:
    FileDescriptor();
	FileDescriptor(int fd);
	virtual ~FileDescriptor();
	
	int getFD() const;
	
	void close();
	
protected:
	int _fd;
};
typedef IceUtil::Handle<FileDescriptor> FileDescriptorPtr;
	
class IOStream : public IceUtil::Shared
{
public:
	IOStream(const FileDescriptorPtr& fileDescriptor);
	virtual ~IOStream();
	
	void read(const BufferPtr& buffer);	
	void write(const BufferPtr& buffer);
	
private:
	FileDescriptorPtr _fileDescriptor;
};
typedef IceUtil::Handle<IOStream> IOStreamPtr;
	
class IOBlockException : public Ice::Exception
{

};

class IOBrokeException : public Ice::Exception
{
	
};

};

#endif                                        // #ifndef
