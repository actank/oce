// Created: 1/2011
// Author:  xiang.ye@opi-corp.com

#ifndef _LOGGING_SERVER_HEADER_
#define _LOGGING_SERVER_HEADER_

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/socketappender.h>
#include <log4cplus/helpers/socket.h>
#include <log4cplus/spi/loggingevent.h>

#include "boost/thread/thread.hpp"
#include "boost/thread/mutex.hpp"

#include <ostream>

namespace logging {

using namespace std;
using namespace log4cplus::helpers;
using namespace log4cplus::spi;

int hndl_count_ = 0;
boost::mutex tc_mutex_;

int DecHndlCount()
{
	boost::mutex::scoped_lock alock( tc_mutex_ );
	return --hndl_count_;
}
int IncHndlCount()
{
	boost::mutex::scoped_lock alock( tc_mutex_ );
	return ++hndl_count_;
}
int GetHndlCount()
{
	boost::mutex::scoped_lock alock( tc_mutex_ );
	return hndl_count_;
}

class SocketLoggingHandler {
public:
	virtual ~SocketLoggingHandler() {
	}
	void Handle( int handler_id, Socket& sock) const {
		cout << "=> handler_" << handler_id << " starting ..." << " ( " << IncHndlCount() << " handlers running ... )" << endl;
		int done = 0;
		int recv = 0;
		while( true ){
			try{
				if( !sock.isOpen() ) {
					cout << "=> handler_" << handler_id << " sock_not_open! released. ( " << DecHndlCount() << " handlers running )" <<  endl;
					return;
				}

				SocketBuffer msgSizeBuffer(sizeof(unsigned int));
				if(!sock.read(msgSizeBuffer))
				{
					cout << "=> handler_" << handler_id << " bad_msg_size! released. ( " << DecHndlCount() << " handlers running )" << endl;
					sock.close();
					return;
				}
				unsigned int msgSize = msgSizeBuffer.readInt();
				SocketBuffer buffer(msgSize);
				if(!sock.read(buffer))
				{
					cout << "=> handler_" << handler_id << " bad_data! released. ( " << DecHndlCount() << " handlers running )" <<  endl;
					sock.close();
					return;
				}

				InternalLoggingEvent event = readFromBuffer(buffer);

				if( handle( event ) ){
					++done;
				}

				if( ++recv == 10000 ){
					cout << "=> handler_" << handler_id << " : " << done << "/" << recv << " (done/recv)" <<  endl;
					done = 0;
					recv = 0;
				}
			} catch( ... ){
				cout << "=> handler_" << handler_id << " unknown_err! released. ( " << DecHndlCount() << " handlers running )" <<  endl;
				sock.close();
				return;
			}
		}
	}

protected:
	virtual bool handle(InternalLoggingEvent& event) const = 0;
};

class SocketLoggingServer {
public:
	SocketLoggingServer(int port)
		: port_(port) {
	} 
	bool Start(const SocketLoggingHandler& handler) const {
                try{
                        cout << "=> socketserver starting on port: " << port_ << " ..." << endl;
                        ServerSocket serverSocket(port_);
                        cout << "=> socketserver started on port: " << port_ << endl;
                        int count = 0;
                        while( true ){
                                cout << "=> socketserver listioning " << port_ << endl;
                                Socket sock = serverSocket.accept();
                                cout << "=> socketserver recieve a client connection. handler_" << ++count << " starting ..." << endl;
                                boost::thread handle( boost::bind( &SocketLoggingHandler::Handle, &handler, count, sock) );
				count %= 100000000;
                        }
                } catch( ... ){
                        cout << "=> socketserver failed on port: " << port_ << endl;
                        return false;
                }
                return true;
	}
private:
	int port_;
};

}

#endif
