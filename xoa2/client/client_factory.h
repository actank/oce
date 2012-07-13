#ifndef _CLIENT_FACTORY_H_
#define _CLIENT_FACTORY_H_

#include "xoa2/client/xoa_registry_client.h"
#include "LogWrapper.h"

// 需要统一的接口，同时又不能指定统一继承自该接口, 故使用模板
template <typename T>
class ClientWrap : public ZkEventListener {
 protected:
  T * client_;
  std::string load_balance_key_;
  bool disabled_;

  std::string service_name_;
 public:
  ClientWrap(T * client, const char * lb_key) : client_(client)
    , load_balance_key_(lb_key)
    , disabled_(false) {
  }

  bool disabled() const {
    return disabled_;
  }

  virtual void HandleEvent(int type) {
    MCE_DEBUG("ClientWrap::HandleEvent() " << client_ << " event : " << type);
    disabled_ = true;
  }

  const std::string & load_balance_key() const {
    return load_balance_key_;
  }

  T * client() {
    if (disabled_) {
      return NULL;
    }

    return client_;
  }

  virtual ~ClientWrap() {
    MCE_DEBUG("Client " << client_ << " destroyed.");
    delete client_;
  }
};

template <typename ClientType>
class ClientFactory {
 private:
 public:
  virtual ClientWrap<ClientType> * Create(const char * addr) = 0;

  ClientFactory() {} 
  virtual ~ClientFactory() {}
};

#endif // _CLIENT_FACTORY_H_

