#include "EpollChannel.h"
#include "Epollor.h"
#include "Socket.h"

using namespace MyUtil;
	
EpollChannel::EpollChannel(
	const FileDescriptorPtr& fileDescriptor) :
	_fileDescriptor(fileDescriptor)	
{
	memset(&_event, 0, sizeof(struct epoll_event));
	_event.data.ptr = this;
    _time = 0;
}

EpollChannel::~EpollChannel()
{
	
}

bool
EpollChannel::operator<(const EpollChannel& rhs) const
{
    return getFileDescriptor()->getFD() < rhs.getFileDescriptor()->getFD();
}

bool
EpollChannel::operator==(const EpollChannel& rhs) const
{
    return getFileDescriptor()->getFD() == rhs.getFileDescriptor()->getFD();
}

FileDescriptorPtr
EpollChannel::getFileDescriptor() const
{
	return _fileDescriptor;
}

struct epoll_event&
EpollChannel::getEvent()
{
	return _event;
}

void
EpollChannel::setEvent(short event)
{
	_event.events = event;
}

int 
EpollChannel::getLastTime()
{
    return _time;
}

//---------------------------------------------------------------------------
BufferedEpollChannel::BufferedEpollChannel(
	const FileDescriptorPtr& fileDescriptor) :
	EpollChannel(fileDescriptor)
{
    _bufferInput = new Buffer;	
    _bufferOutput = new Buffer;	
	_ios = new IOStream(fileDescriptor);
    _time = time(NULL);
}

BufferedEpollChannel::~BufferedEpollChannel()
{
	
}

void
BufferedEpollChannel::handleEvents(int events)
{
    _time= time(NULL);
    try{
        if (events & EPOLLIN) {
            _ios->read(_bufferInput);
            cout << "EPOLLIN at " << getFileDescriptor()->getFD() 
            	<< ", size: " << _bufferInput->size() << endl;
            PacketedChannelQueue::instance().hookPacket(this);
        } else if (events & EPOLLOUT) {
            cout << "EPOLLOUT at " << getFileDescriptor()->getFD() << endl;
            _ios->write(_bufferOutput);
            if (!_bufferOutput->size()) {
                _event.events = EPOLLIN|EPOLLERR|EPOLLHUP;
                Epollor::instance().setupChannel(this);
            }
        } else if ((events & EPOLLERR) || (events & EPOLLHUP)) {
            cout << "EPOLLERR|EPOLLHUP at " << getFileDescriptor()->getFD() << endl;
            throw IOBrokeException();
        }
    } catch (IOBrokeException& e) {
        cout << "IOBrokeException at " << getFileDescriptor()->getFD() << endl;
        Epollor::instance().deregisterChannel(this);
    }
}

PacketPtr
BufferedEpollChannel::getPacket(const BufferDrainerPtr& drainer)
{
	string result = _bufferInput->drain(drainer);
	if (result.empty()) {
		return 0;
	}
	PacketPtr packet = new Packet;
	packet->fd = getFileDescriptor()->getFD();
	packet->data = result;
	return packet;
}

void
BufferedEpollChannel::putPacket(const PacketPtr& packet)
{
    _bufferOutput->append(packet->data);
	if (_bufferOutput->size()) {
		try {
			_ios->write(_bufferOutput);
		} catch (IOBrokeException& e) {
			cout << "IOBrokeException at " << getFileDescriptor()->getFD() << endl;
			Epollor::instance().deregisterChannel(this);
			return;
		}
		if (_bufferOutput->size()) {
			_event.events |= EPOLLOUT;
			Epollor::instance().setupChannel(this);
		}
	}
}

//---------------------------------------------------------------------------
ListenedEpollChannel::ListenedEpollChannel(
	const FileDescriptorPtr& fileDescriptor) :
	EpollChannel(fileDescriptor)
{
	ServerSocketPtr acceptor = ServerSocketPtr::dynamicCast(fileDescriptor);
	acceptor->setNonBlock(true);
}

ListenedEpollChannel::~ListenedEpollChannel()
{
	
}

void
ListenedEpollChannel::handleEvents(int events)
{
    if (events & EPOLLIN) {
        ServerSocketPtr acceptor 
            = ServerSocketPtr::dynamicCast(getFileDescriptor());

        SocketPtr socket = acceptor->accept();
        if (!socket) {
            return;
        }
        socket->setNonBlock(true);
        BufferedEpollChannelPtr channel = new BufferedEpollChannel(socket);
        channel->setEvent(EPOLLIN|EPOLLERR|EPOLLHUP);
        Epollor::instance().registerChannel(channel);
        			
    } else if ((events & EPOLLERR) || (events & EPOLLHUP)) {
    	Epollor::instance().deregisterChannel(this);
	} 
}

