#include "Socket.h"

using namespace MyUtil;

struct sockaddr_in
SockAddress::parseAsSockAddrIn(const string& ip, int port)
{
	uint32_t nhost;
    /* First try it as aaa.bbb.ccc.ddd. */
    nhost = inet_addr(ip.c_str());
    if (nhost == UINT_MAX) {        
	    struct hostent *hostt;
	    if ((hostt = gethostbyname(ip.c_str())) == NULL) {
	        nhost = 0;
	    }
    	nhost = (*(struct in_addr *) *hostt->h_addr_list).s_addr;
    }
    return parseAsSockAddrIn(nhost, port);
}

struct sockaddr_in
SockAddress::parseAsSockAddrIn(uint32_t ip, int port)
{
	struct sockaddr_in sockaddrin;
	memset(&sockaddrin, 0, sizeof(struct sockaddr_in));		
	sockaddrin.sin_addr.s_addr = ip;
    sockaddrin.sin_family = AF_INET;
    sockaddrin.sin_port = htons(port);
    
    return sockaddrin;
}

struct sockaddr
SockAddress::parseAsSockAddr(const string& ip, int port)
{
	struct sockaddr_in sockaddrin = parseAsSockAddrIn(ip, port);
	struct sockaddr sockaddr;
	memcpy(&sockaddr, &sockaddrin, sizeof(struct sockaddr));
	
	return sockaddr;
}

struct sockaddr
SockAddress::parseAsSockAddr(uint32_t ip, int port)
{
	struct sockaddr_in sockaddrin = parseAsSockAddrIn(ip, port);
	struct sockaddr sockaddr;
	memcpy(&sockaddr, &sockaddrin, sizeof(struct sockaddr));
	
	return sockaddr;
}

string
SockAddress::parseAsPeerIp(int fd)
{
	struct sockaddr_in in;
	int inlen=sizeof(sockaddr_in);
	getpeername(fd, (sockaddr*)&in, (socklen_t*)&inlen);
	return inet_ntoa(*(in_addr*)(&(in.sin_addr.s_addr)));
}

uint32_t
SockAddress::parseAsPeerInetIp(int fd)
{
	struct sockaddr_in in;
	int inlen=sizeof(sockaddr_in);
	getpeername(fd, (sockaddr*)&in, (socklen_t*)&inlen);
	return in.sin_addr.s_addr;
}

string
SockAddress::parseAsSockIp(int fd)
{
	struct sockaddr_in in;
	int inlen=sizeof(sockaddr_in);
	getsockname(fd, (sockaddr*)&in, (socklen_t*)&inlen);
	return inet_ntoa(*(in_addr*)(&(in.sin_addr.s_addr)));
}

uint32_t
SockAddress::parseAsSockInetIp(int fd)
{
	struct sockaddr_in in;
	int inlen=sizeof(sockaddr_in);
	getsockname(fd, (sockaddr*)&in, (socklen_t*)&inlen);
	return in.sin_addr.s_addr;
}

uint16_t
SockAddress::parseAsPeerPort(int fd)
{
	struct sockaddr addr;
    socklen_t len = sizeof(struct sockaddr);
    getpeername(fd, (struct sockaddr*)&addr, &len);

    return ntohs(((struct sockaddr_in*)&addr)->sin_port);
}

uint16_t
SockAddress::parseAsSockPort(int fd)
{
	struct sockaddr addr;
    socklen_t len = sizeof(struct sockaddr);
    getsockname(fd, (struct sockaddr*)&addr, &len);

    return ntohs(((struct sockaddr_in*)&addr)->sin_port);
}

//---------------------------------------------------------------------------
SocketBasic::SocketBasic() 
{
	
}


SocketBasic::SocketBasic(int fd) :
	FileDescriptor(fd)
{
	
}


SocketBasic::~SocketBasic()
{
	
}

int 
SocketBasic::getOption(int key)
{
    int value;
    socklen_t size = sizeof(value);
    if (getsockopt(getFD(), SOL_SOCKET, key, &value, &size) < 0) {
        return -1;
    }
    return value;
}

void
SocketBasic::setOption(int option, int value)
{
    if (setsockopt(_fd, SOL_SOCKET, option, &value, sizeof(value)) < 0) {
    	cout << "Socket::setOption failed" << endl;
    }
}

void 
SocketBasic::setNonBlock(bool nonBlock)
{
    int flags;
    
    /* Get socket block status */
    if ((flags = fcntl(getFD(), F_GETFL, 0)) < 0) {
        /* Handle error */
        cout << "Socket::setBlock " << strerror(errno) << endl;
        return;
    }
    
    if (nonBlock) {
        /* NoBlocking */
        if ((flags = fcntl(getFD(), F_SETFL, flags | O_NONBLOCK)) < 0) {
	        /* Handle error */
	        cout << "Socket::setBlock " << strerror(errno) << endl;
        }
    } else {        
        /* Blocking */
        if ((flags = fcntl(getFD(), F_SETFL, flags & (~O_NONBLOCK))) < 0) {
	        /* Handle error */
	        cout << "Socket::setBlock " << strerror(errno) << endl;          
        }
    }
}

string 
SocketBasic::getSockIp()
{
    return SockAddress::parseAsSockIp(getFD());
}

string 
SocketBasic::getPeerIp()
{
    return SockAddress::parseAsPeerIp(getFD());
}

uint16_t 
SocketBasic::getSockPort()
{
    return SockAddress::parseAsSockPort(getFD());
}

uint16_t 
SocketBasic::getPeerPort()
{
    return SockAddress::parseAsPeerPort(getFD());
}

//---------------------------------------------------------------------------
Socket::Socket(int fd) :
	SocketBasic(fd)
{
	
}


Socket::Socket(const string& host, int port)
{
    /* step1: create a socket */
    if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    	cout << "Socket::Socket " << strerror(errno) << endl;
        throw;
    }
   
    setOption(SO_REUSEADDR, 1);
  	setOption(SO_KEEPALIVE, 1);
    struct sockaddr sockaddr = SockAddress::parseAsSockAddr(host, port);
    if (::connect(_fd, &sockaddr, sizeof(struct sockaddr)) < 0) {
    	cout << "Socket::Socket " << strerror(errno) << endl;
    	throw;
    }
}

Socket::~Socket()
{
	
}

//---------------------------------------------------------------------------
ServerSocket::ServerSocket(int port, int num)
{
	/* step1: create a socket */
    if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    	cout << "ServerSocket::ServerSocket " << strerror(errno) << endl;
        throw;
    }

    setOption(SO_REUSEADDR, 1);
  	setOption(SO_KEEPALIVE, 1);
  	struct sockaddr sockaddr = SockAddress::parseAsSockAddr(INADDR_ANY, port);
	if (::bind(_fd, &sockaddr, sizeof(struct sockaddr)) < 0) {
		cout << "ServerSocket::ServerSocket " << strerror(errno) << endl;
  		throw;
  	}
    if (::listen (_fd, num) < 0) {
    	cout << "ServerSocket::ServerSocket " << strerror(errno) << endl;
  		throw;
  	}
}

ServerSocket::~ServerSocket(void)
{
  // No body
}

SocketPtr
ServerSocket::accept()
{
	int fd;
    while ((fd = ::accept(_fd, NULL, NULL)) < 0) {
        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {   // Either a real error occured, or block was interrupted for
            // some reason.  Only abort execution if a real error occured.
            // EINTR is 'interruped system call' error
            // don't return - do the accept again
            continue;
        }
        return 0;
    }
    
    return new Socket(fd);
}
