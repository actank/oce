#include "IOStream.h"

using namespace MyUtil;

FileDescriptor::FileDescriptor() 
{
}


FileDescriptor::FileDescriptor(int fd) :
	_fd(fd)
{
	
}

FileDescriptor::~FileDescriptor()
{
	this->close();
}

int
FileDescriptor::getFD() const
{
	return _fd;
}

void
FileDescriptor::close()
{
	cout << "FileDescriptor::close()" << endl;
	::close(_fd);
}

//---------------------------------------------------------------------------
IOStream::IOStream(const FileDescriptorPtr& fileDescriptor)
    : _fileDescriptor(fileDescriptor)
{
	
}

IOStream::~IOStream()
{
    // No body
}

void 
IOStream::read(const BufferPtr& buffer)
{
	char bufTmp[256];	
    int bytes;
    while (true) {
	    do {
	        bytes = ::read(_fileDescriptor->getFD(), bufTmp, sizeof(bufTmp));
	    } while (bytes <= 0 && errno == EINTR);
	    if (bytes == -1) {
			if (errno == EAGAIN || errno == EINTR) {
				return;
			}
            cout << strerror(errno) << " errno: " << errno << " bytes : " << bytes << endl;
			throw IOBrokeException();
		} else if (bytes == 0) {
            cout << strerror(errno) << " errno:" << errno << " bytes: " << bytes <<endl;
			throw IOBrokeException();
		}
    	buffer->append(bufTmp, bytes);
	}
}

void
IOStream::write(const BufferPtr& buffer)
{
    string data= buffer->drain(NULL);    
    int bytes;
    while (data.size()) {
	    do {
	        bytes = ::write(_fileDescriptor->getFD(), data.c_str(), data.size());
	    } while ((bytes <= 0) && (errno == EINTR));
	    if (bytes == -1) {
	    	if (errno == EAGAIN || errno == EINTR || errno == EINPROGRESS) {				
	    		buffer->insert(data);
	    		return;
    		} else {
    			throw IOBrokeException();
    		}
    	} else if (bytes == 0) {
            cout << strerror(errno) << " errno:" << errno << endl;
    		throw IOBrokeException();
    	}
		cout << "IOStream::write -> " << bytes << " -> " << data << endl;
    	data.erase(0, bytes);
	}
}
