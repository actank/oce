#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <limits.h>
#include <fcntl.h>
#include <Ice/Ice.h>

#include "IOStream.h"

namespace MyUtil
{
using namespace std;

class SockAddress : public IceUtil::Shared
{
public:
	static struct sockaddr_in parseAsSockAddrIn(const string& ip, int port);
	static struct sockaddr_in parseAsSockAddrIn(uint32_t ip, int port);
	static struct sockaddr parseAsSockAddr(const string& ip, int port);
	static struct sockaddr parseAsSockAddr(uint32_t ip, int port);
	
	static string parseAsPeerIp(int fd);
	static uint32_t parseAsPeerInetIp(int fd);
	
	static string parseAsSockIp(int fd);	
	static uint32_t parseAsSockInetIp(int fd);
	
	static uint16_t parseAsPeerPort(int fd);
	static uint16_t parseAsSockPort(int fd);	
};
typedef IceUtil::Handle<SockAddress> InteAddressPtr;
	
//---------------------------------------------------------------------------
class SocketBasic : public FileDescriptor
{
public:
    SocketBasic();
	SocketBasic(int fd);
	virtual ~SocketBasic();
		
    int getOption(int key);
	void setOption(int key, int value);
	
	void setNonBlock(bool nonBlock);	
	
	string getSockIp();
    string getPeerIp();
    
    uint16_t getSockPort();
    uint16_t getPeerPort(); 
};
typedef IceUtil::Handle<SocketBasic> SocketBasicPtr;
	
//---------------------------------------------------------------------------
class Socket : public SocketBasic
{
public:
    Socket(int fd);
    Socket(const string& host, int port);
    
    virtual ~Socket();
	  
   	string getSockIp();
    string getPeerIp();
    uint16_t getSockPort();
    uint16_t getPeerPort();  
private:
    //  Disallow to use
    Socket(const Socket& rhs);
    Socket& operator= (const Socket& rhs);    
};
typedef IceUtil::Handle<Socket> SocketPtr;
	
//---------------------------------------------------------------------------
class ServerSocket : public SocketBasic
{
public:
    ServerSocket(int port, int num = 200);
    virtual ~ServerSocket(void);
    
    SocketPtr accept();
    
private:
    //  Disallow to use
    ServerSocket(const ServerSocket& rhs);
    ServerSocket& operator=(const ServerSocket& rhs);
};
typedef IceUtil::Handle<ServerSocket> ServerSocketPtr;

//--------------------------------------------------------------------------
class SocketException : public Ice::Exception
{

};
 
};

#endif                                        // #ifndef
