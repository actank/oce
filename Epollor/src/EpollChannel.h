#ifndef __EPOLL_CHANNEL_H__
#define __EPOLL_CHANNEL_H__

#include <sys/types.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include "IOStream.h"

namespace MyUtil
{
using namespace std;

const static int CHANNEL_STATUS_OPEN = 1;
const static int CHANNEL_STATUS_CLOSE = 2;
const static int CHANNEL_STATUS_PACKET = 3;

//---------------------------------------------------------------------------
// the packet transfered between this framework and application
struct Packet : public IceUtil::Shared
{
public:
	Packet() : _type(CHANNEL_STATUS_PACKET) {};
	int fd;
	string data;
	
	bool eventPacket() { return _type == CHANNEL_STATUS_PACKET;}
	bool eventOpen() { return _type == CHANNEL_STATUS_OPEN;}
	bool eventClose() { return _type == CHANNEL_STATUS_CLOSE;}
	
	int type() { return _type; }
	void type(int type) { _type = type; }	
private:
	int _type;
};
typedef IceUtil::Handle<Packet> PacketPtr;
	
//---------------------------------------------------------------------------
class EpollChannel : public IceUtil::Shared
{
public:
    EpollChannel(const FileDescriptorPtr& fileDescriptor);
    virtual ~EpollChannel();

	bool operator<(const EpollChannel& rhs) const;
    bool operator==(const EpollChannel& rhs) const;
    
	virtual void handleEvents(int events) = 0;
    
    FileDescriptorPtr getFileDescriptor() const;
	
	struct epoll_event& getEvent();
	void setEvent(short event);
    int getLastTime();
		
protected:	
    FileDescriptorPtr _fileDescriptor;    
    struct epoll_event _event;
    int _time;    
};
typedef IceUtil::Handle<EpollChannel> EpollChannelPtr;

//---------------------------------------------------------------------------
class BufferedEpollChannel : public EpollChannel
{
public:
	BufferedEpollChannel(const FileDescriptorPtr& fileDescriptor);
	virtual ~BufferedEpollChannel();
	
	virtual void handleEvents(int events);
	
	PacketPtr getPacket(const BufferDrainerPtr& drainer);
    void putPacket(const PacketPtr& data);
  	
protected:	
	BufferPtr _bufferInput;
    BufferPtr _bufferOutput;    
    
    IOStreamPtr _ios;
};
typedef IceUtil::Handle<BufferedEpollChannel> BufferedEpollChannelPtr;

//---------------------------------------------------------------------------
class ListenedEpollChannel : public EpollChannel
{
public:
	ListenedEpollChannel(const FileDescriptorPtr& fileDescriptor);
	virtual ~ListenedEpollChannel();
	
	virtual void handleEvents(int events);	
};
typedef IceUtil::Handle<ListenedEpollChannel> ListenedEpollChannelPtr;
};

#endif
