#include "Epollor.h"

using namespace MyUtil;

Epollor::Epollor() :
	_fd(-1),
	_size(1000),
	_maxWaits(100),
    _timeout(0),
	_events(NULL),
	_stopFlag(false)
{
    
}

Epollor::~Epollor()
{
	cout << "Epollor::~Epollor" << endl;
    clear();
}


void Epollor::clear()
{
    _channelsAll.clear();
    _channelsDiscarded.clear();
	if (_fd > 0) {
		::close(_fd);
        _fd = -1;
	}
	if (_events) {
		delete [] _events;
        _events = NULL;
	}

}
void
Epollor::start(int size, int maxWaits, int timeout)
{
	if (size > 0) {
		_size = size;
	}
	if (maxWaits > 0) {
		_maxWaits = maxWaits;
	}
    if (timeout > 0) {
        _timeout = timeout;
    }
	if ((_fd = epoll_create(_maxWaits)) < 0) {
        throw EpollCreateException();
    }
    _events = new struct epoll_event [_maxWaits];
    if (!_events) {
        ::close(_fd);
        _fd = -1;
        throw EpollCreateException();
    }    
    Thread::start().detach();
}

void
Epollor::stop()
{
	_stopFlag = true;
	cout << "StopFlag -> " << true << endl;
}

void
Epollor::run()
{
	while (true) {			    
        int nevs = epoll_wait(_fd, _events, _maxWaits, 1);
        if (nevs < 0) { 
            continue;
        }
	    /**
	        Ok, events occured and held in events buffer
	    */
	    struct epoll_event* pev = _events;
	    for (int i = 0; i < nevs;  ++i, ++pev) {
	    	if (!pev->data.ptr) {
	    		epoll_ctl(_fd, EPOLL_CTL_DEL, pev->data.fd, pev);
	    		continue;
	    	}
	        EpollChannel* channel = (EpollChannel*)pev->data.ptr;
	        try {
	        	channel->handleEvents(pev->events);
	        } catch (EpollCtrlException& e) {
	        	cout << "EpollCtrlException" << endl;
	        }
	    }	    
        try {
            checkTimeout();
        } catch (EpollCtrlException& e) {
            cout << "EpollCtrlException" << endl;
        }
	    releaseDiscards();
	    if (_stopFlag) {
	    	cout << "StopFlag got" << endl;
	    	return;
	    }
	}
}

void
Epollor::registerChannel(const EpollChannelPtr& channel)
{
	{
		IceUtil::Mutex::Lock lock(_mutexChannels);
		int fd = channel->getFileDescriptor()->getFD();
		if (_channelsAll.find(fd) != _channelsAll.end()) {
			return;
		}
		if (epoll_ctl(_fd, EPOLL_CTL_ADD, fd, &channel->getEvent()) < 0) {
	        throw EpollCtrlException();
	    }
		_channelsAll[fd] = channel;
	}
	PacketedChannelQueue::instance().hookOpen(channel);
}

void
Epollor::deregisterChannel(const EpollChannelPtr& channel)
{
	{
		IceUtil::Mutex::Lock lock(_mutexChannels);
		int fd = channel->getFileDescriptor()->getFD();
		ChannelMap::iterator pos = _channelsAll.find(fd);
		if (pos == _channelsAll.end()) {
			return;
		}
		_channelsDiscarded.push_back(channel); 
		_channelsAll.erase(fd);    
		if (epoll_ctl(_fd, EPOLL_CTL_DEL, fd, &channel->getEvent()) < 0) {
	//        clear();
	        //_channelsAll.erase(fd);
	        throw EpollCtrlException();
	    }
	}
	PacketedChannelQueue::instance().hookClose(channel);
}

void 
Epollor::setupChannel(const EpollChannelPtr& channel)
{	
	IceUtil::Mutex::Lock lock(_mutexChannels);
	int fd = channel->getFileDescriptor()->getFD();
	ChannelMap::iterator pos = _channelsAll.find(fd);
	if (pos == _channelsAll.end()) {
		return;
	}
	if (epoll_ctl(_fd, EPOLL_CTL_MOD, fd, &channel->getEvent()) < 0) {
//        clear();
    	throw EpollCtrlException();
    }
}

EpollChannelPtr
Epollor::findChannel(int fd)
{
	IceUtil::Mutex::Lock lock(_mutexChannels);
	ChannelMap::iterator pos = _channelsAll.find(fd);
	if (pos != _channelsAll.end()) {
		return pos->second;
	}
	throw ChannelNotExistException();
}

void
Epollor::releaseDiscards()
{	
    IceUtil::Mutex::Lock lock(_mutexChannels);
   	for (ChannelList::iterator it = _channelsDiscarded.begin();
   		it != _channelsDiscarded.end(); ++it) {
   			(*it)->getEvent().data.ptr = 0;
   	}
    _channelsDiscarded.clear();
}
void 
Epollor::checkTimeout()
{
    if(_timeout == 0) {
        return ;
    }
	vector<EpollChannelPtr> channels;
	{
		IceUtil::Mutex::Lock lock(_mutexChannels);
		int now = time(NULL);
		for(ChannelMap::iterator it = _channelsAll.begin();
				it != _channelsAll.end(); it++) {
			if((*it).second->getLastTime() == 0) {
				continue;
			}
			
			if((now - (*it).second->getLastTime()) <=  _timeout){
				continue;
			}
			cout << "Channel Timeout " <<endl;
			//delete channel
			int fd = (*it).second->getFileDescriptor()->getFD();
			_channelsDiscarded.push_back((*it).second); 
			_channelsAll.erase(fd);    
			if (epoll_ctl(_fd, EPOLL_CTL_DEL, fd, &((*it).second)->getEvent()) < 0) {
				throw EpollCtrlException();
			}
			channels.push_back((*it).second);
		}
	}
	for(size_t i = 0; i < channels.size(); i++) {
		PacketedChannelQueue::instance().hookClose(channels[i]);
	}


}
//---------------------------------------------------------------------------
PacketedChannelQueue::PacketedChannelQueue()
{
	
}

PacketedChannelQueue::~PacketedChannelQueue()
{
	
}

PacketPtr
PacketedChannelQueue::getPacket(const BufferDrainerPtr& drainer)
{
	while (true) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutexChannels);
		if (_channelsOpen.size()) {
			EpollChannelPtr channel = _channelsOpen.front();
			_channelsOpen.pop_front();
			if (!channel) {
				continue;
			}
			// OPEN status, notify external application this status
			PacketPtr packet = new Packet;
			packet->fd = channel->getFileDescriptor()->getFD();
			packet->type(CHANNEL_STATUS_OPEN);
			return packet;
		}
		if (_channelsPacket.size()) {
			BufferedEpollChannelPtr channel = 
				BufferedEpollChannelPtr::dynamicCast(_channelsPacket.front());			
			if (!channel) {				
				_channelsPacket.pop_front();
				continue;
			}	
			PacketPtr packet = channel->getPacket(drainer);			
			if (!packet) {
				_channelsPacket.pop_front();
				continue;
			}
			return packet;
		}
		if (_channelsClose.size()) {
			EpollChannelPtr channel = _channelsClose.front();
			_channelsClose.pop_front();
			if (!channel) {
				continue;
			}
			// CLOSE status, notify external application this status
			PacketPtr packet = new Packet;
			packet->fd = channel->getFileDescriptor()->getFD();
			packet->type(CHANNEL_STATUS_CLOSE);				
			return packet;
		}
		try {
			_mutexChannels.wait();
		} catch (Ice::Exception& e) {
			// do nothing
		}
	}
}

void
PacketedChannelQueue::putPacket(const PacketPtr& packet)
{
	EpollChannelPtr channel 
		= Epollor::instance().findChannel(packet->fd);
	BufferedEpollChannelPtr bc = 
        BufferedEpollChannelPtr::dynamicCast(channel);
    if (!bc) {
    	return;
    }
	bc->putPacket(packet); 
}

void
PacketedChannelQueue::hookOpen(const EpollChannelPtr& channel)
{
	hook(channel, _channelsOpen);
}

void
PacketedChannelQueue::hookPacket(const EpollChannelPtr& channel)
{
	hook(channel, _channelsPacket);
}

void
PacketedChannelQueue::hookClose(const EpollChannelPtr& channel)
{
	hook(channel, _channelsClose);
}

void
PacketedChannelQueue::hook(
	const EpollChannelPtr& channel, 
	ChannelQueue& channels)
{	
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutexChannels);
	ChannelQueue::iterator it 
    	= find(channels.begin(), channels.end(), channel);
    if(it != channels.end()) {
        return;
    }
    cout << "$$$$$$ hook " << channel->getFileDescriptor()->getFD() << endl;
    channels.push_back(channel);
    if (channels.size() == 1) {
        _mutexChannels.notify();
	}
}

