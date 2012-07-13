#ifndef _XOA_CLIENT_FACTORY_H_
#define _XOA_CLIENT_FACTORY_H_

#include "xoa2/client/client_factory.h"

#include <iostream>

#include <boost/lexical_cast.hpp>

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include "LogWrapper.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

template <typename T>
class XoaClientWrap : public ClientWrap<T> {
  boost::shared_ptr<TSocket> socket_;
  boost::shared_ptr<TTransport> transport_;
  boost::shared_ptr<TProtocol> protocol_;
 public:
  XoaClientWrap(const char * host, int port, const char * lb_key) 
    : ClientWrap<T>(NULL, lb_key)
    , socket_(new TSocket(host, port))
    , transport_(new TBufferedTransport(socket_))
    , protocol_(new TBinaryProtocol(transport_))
  {
    socket_->setSendTimeout(100);
    socket_->setRecvTimeout(100);
  }

  bool Init() {
    ClientWrap<T>::client_ = new T(protocol_);

    try {
      transport_->open();
    } catch (TTransportException & e) {
      MCE_WARN("Thrift conn open error : " << e.what());
      return false;
    } catch (...) {
      MCE_WARN("Thrift conn open unknown error.");
      return false;
    }

    return true;
  }

  virtual ~XoaClientWrap() {
    transport_->close();
  }
};

template <typename ClientType>
class XoaClientFactory : public ClientFactory<ClientType> {
 private:
  std::string service_name_;

  bool ParseConfig(const std::string & config, std::string * host, unsigned short * port) const {
    size_t pos = config.find_first_of(':');
    if (pos == std::string::npos) {
      return false;
    }
    *host = config.substr(0, pos);

    try {
      *port = boost::lexical_cast<short>(config.substr(pos + 1));
    } catch (boost::bad_lexical_cast &) {
      return false;
    }

    return true;
  }

 public:
  XoaClientFactory(const char * service_name) : service_name_(service_name) {
  }

  virtual ClientWrap<ClientType> * Create(const char * addr) {
    std::string host;
    unsigned short port = 0;
    XoaClientWrap<ClientType> * wrap = NULL;

    if (ParseConfig(addr, &host, &port)) {
      try {
        wrap = new XoaClientWrap<ClientType>(host.c_str(), port, addr);
      } catch (...) {
        MCE_INFO("Create client wrap new() error. " << host << ":" << port);
        wrap = NULL;
      }
      if (wrap && !wrap->Init()) {
        MCE_INFO("Create client wrap Init() error. " << host << ":" << port);
        delete wrap;
        wrap = NULL;
      }
    }
    return wrap;
  }
};

#endif // _XOA_CLIENT_FACTORY_H_

