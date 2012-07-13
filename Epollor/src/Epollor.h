#ifndef __EPOLLOR_H__
#define __EPOLLOR_H__

#include <sys/types.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <map>
#include <list>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "EpollChannel.h"
#include "Singleton.h"

namespace MyUtil
{
using namespace std;

class Epollor : public Singleton<Epollor>,
	public IceUtil::Thread
{
friend class Singleton<Epollor>;
public:
	
	void start(int size = 1000, int maxWaits = 100, int timeout = 0);
	
	void stop();
	
	// event_set struct event
	void registerChannel(const EpollChannelPtr& channel);
	
	// remove from event queue
	void deregisterChannel(const EpollChannelPtr& channel);
	
	// schedule a new(exist) channel to the active events queue
	void setupChannel(const EpollChannelPtr& channel);
    
  	// find a channel with fd in channels
  	EpollChannelPtr findChannel(int fd);
  	
private:
	// initialize libevent	
	Epollor();
	// do nothing but clear all channels
	virtual ~Epollor();	
    void clear();
	
    Epollor(const Epollor& rhs);
    Epollor& operator=(const Epollor& rhs);      
  	
  	
	// start a new thread to loop events and handle channels' updating
	virtual void run();
	
	void releaseDiscards();
    void checkTimeout();
	
private:
    typedef map<int, EpollChannelPtr> ChannelMap;
    ChannelMap _channelsAll;
    typedef list<EpollChannelPtr> ChannelList;
    ChannelList _channelsDiscarded;
    IceUtil::Mutex _mutexChannels;
    
    int _fd;
    int _size;
    int _maxWaits;
    int _timeout;
    struct epoll_event* _events;
    bool _stopFlag;
    
};
typedef IceUtil::Handle<Epollor> EpollorPtr;

//---------------------------------------------------------------------------
class PacketedChannelQueue : public Singleton<PacketedChannelQueue>
{
friend class Singleton<PacketedChannelQueue>;
public:
	// pop a packet from this framework
	PacketPtr getPacket(const BufferDrainerPtr& drainer);
	// push a packet to this framework
	void putPacket(const PacketPtr& packet);
	
	void hookOpen(const EpollChannelPtr& channel);
	void hookPacket(const EpollChannelPtr& channel);
	void hookClose(const EpollChannelPtr& channel);
private:
	PacketedChannelQueue();
	~PacketedChannelQueue();
	
	PacketedChannelQueue(const PacketedChannelQueue& rhs);
	PacketedChannelQueue& operator=(const PacketedChannelQueue& rhs);
		
	typedef list<EpollChannelPtr> ChannelQueue;
    ChannelQueue _channelsOpen;
    ChannelQueue _channelsPacket;    
    ChannelQueue _channelsClose;
    IceUtil::Monitor<IceUtil::Mutex> _mutexChannels;    	
    	
   	void hook(const EpollChannelPtr& channel, ChannelQueue& channels);
};

//---------------------------------------------------------------------------
class EpollCtrlException: public Ice::Exception {};
class EpollCreateException: public Ice::Exception {};
class ChannelNotExistException : public Ice::Exception {};


};


#endif
