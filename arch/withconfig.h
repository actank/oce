#ifndef XCE_ARCH_WITH_CONFIG_TEST_H__
#define XCE_ARCH_WITH_CONFIG_TEST_H__

#include <time.h>
#include <stdlib.h> // for rand/srand
#include <IceUtil/UUID.h>
#include "gtest/gtest.h"
#include "arch/config_adapter.h"
#include "arch/service.h"
#include "base/asyncall.h"
#include "base/network.h"
#include "arch/configserverI.h"

// namespace xce ?

// 用来测试服务的实现
// SetUp: 启动一个Object Adapter
// TearDown: 释放
class WithIceService : public testing::Test {
protected:
  void SetUp() {
    adapter_ = xce::CreateObjectAdapter("withgrid ", xce::Network::local_ip(), port());
    ASSERT(adapter_);

    xce::Service::Adapter() = adapter_;
    xce::Start();

    adapter_->activate();
  }

  void TearDown() {
    xce::Stop();
    adapter_->deactivate();
    adapter_->destroy();
    xce::Service::Adapter() = 0;
    adapter_ = 0;

    // TODO: make sure xx, yy released by reference count?
    // TODO: clear all servant
  }

  virtual int port() const {
    srand(time(0));
    static int r = rand() % 100;
    return 1024 + r;
  }

  // 两个完全一样的实现，后面的稍微好使用一点
  template<class T, class PrxType>
  PrxType AddServant(T* p, Ice::Identity* id) {
    id->name = IceUtil::generateUUID();
    Ice::ObjectPrx o = adapter_->addFacet(p, *id, "");
    return PrxType::uncheckedCast(o);
  }

  template<class T, class PrxType>
  Ice::Identity AddServant(T* p, PrxType& prx) {
    prx = PrxType::uncheckedCast(adapter_->addWithUUID(p));
    Ice::Identity id = prx->ice_getIdentity();
    return id;
  }

  void RemoveServant(const Ice::Identity& id) {
    adapter_->removeFacet(id, "");
  }

  Ice::ObjectAdapterPtr adapter_;
};

// 保证 ConfigServerPrx = CachedConfigPrx() 可访问
class WithRemoteConfig : public WithIceService {
protected:
  void SetUp() {
    WithIceService::SetUp();
    StartConfigServer();
  }

  void TearDown() {
    StopConfigServer();
    WithIceService::TearDown();
  }
  
  void StartConfigServer() {
    ASSERT(config_id_.name.empty());
    bool f = ReplaceCachedConfigPrx();
    ASSERT(f);
    ASSERT(!config_id_.name.empty());
  }

  void StopConfigServer() {
    if (!config_id_.name.empty()) {
      RemoveServant(config_id_);
      config_id_.name.clear();
    }
    config_ = 0;
    xce::CachedConfigPrx() = config_;
  }

private:
  bool ReplaceCachedConfigPrx() {
    using namespace xce;
    // 1 use env first
    const char* proxy = getenv("CONFIG_PROXY");
    if (proxy) {
      // new communicator
      int argc = 0;
      char* argv[] = {0};
      Ice::CommunicatorPtr c = Ice::initialize(argc, argv);

      // ConfigPrx
      Ice::ObjectPrx base = c->stringToProxy(proxy);    
      config_ = ConfigServerPrx::checkedCast(base);
    }
    else {
      // 2 use ConfigServerI
      xce::ConfigServerI *cs = new xce::ConfigServerI();
      // config_ = AddServant<xce::ConfigServerI, xce::ConfigServerPrx>(cs, &config_id_);
      config_id_ = AddServant(cs, config_);
      ASSERT(config_);
    }

    // CAUTION:
    CachedConfigPrx() = ConfigPrx(config_);
    return true;
  }

  xce::ConfigServerPrx config_;
  Ice::Identity config_id_;
};

#endif // XCE_ARCH_WITH_CONFIG_TEST_H__
